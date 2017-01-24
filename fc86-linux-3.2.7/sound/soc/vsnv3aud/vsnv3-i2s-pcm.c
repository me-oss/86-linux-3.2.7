/*
 * atmel-pcm.c  --  ALSA PCM interface for the Atmel atmel SoC.
 *
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2008 Atmel
 *
 * Authors: Sedji Gaouaou <sedji.gaouaou@atmel.com>
 *
 * Based on at91-pcm. by:
 * Frank Mandarino <fmandarino@endrelia.com>
 * Copyright 2006 Endrelia Technologies Inc.
 *
 * Based on pxa2xx-pcm.c by:
 *
 * Author:	Nicolas Pitre
 * Created:	Nov 30, 2004
 * Copyright:	(C) 2004 MontaVista Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/atmel_pdc.h>
#include <linux/atmel-ssc.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include "ait-pcm.h"

#include <mach/mmpf_typedef.h>
#include <mach/mmp_err.h>
#include <mach/mmp_reg_audio.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmpf_audio_ctl.h>

/*--------------------------------------------------------------------------*\
 * Hardware definition
\*--------------------------------------------------------------------------*/
static const struct snd_pcm_hardware ait_i2s_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_MMAP |
				  SNDRV_PCM_INFO_MMAP_VALID |
				  SNDRV_PCM_INFO_INTERLEAVED |
				  SNDRV_PCM_INFO_PAUSE,
	.formats		= SNDRV_PCM_FMTBIT_S16_LE,
	.period_bytes_min	= 192,	
	.period_bytes_max	= 8192,
	.periods_min		= 2,
	.periods_max		= 1024,
	.buffer_bytes_max	= 20* 512,
};


/*--------------------------------------------------------------------------*\
 * Data types
\*--------------------------------------------------------------------------*/
struct atmel_runtime_data {
	struct ait_pcm_params *params;
	dma_addr_t dma_buffer;		/* physical address of dma buffer */
	dma_addr_t dma_buffer_end;	/* first address beyond DMA buffer */
	size_t period_size;

	dma_addr_t period_ptr;		/* physical address of next period */

	int channels;
};


/*--------------------------------------------------------------------------*\
 * Helper functions
\*--------------------------------------------------------------------------*/
static int ait_i2s_pcm_preallocate_dma_buffer(struct snd_pcm *pcm,
	int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = ait_i2s_pcm_hardware.buffer_bytes_max;

	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_coherent(pcm->card->dev, size,
					  &buf->addr, GFP_KERNEL);
	pr_debug("ait-i2s-pcm:"
		"preallocate_dma_buffer: area=%p, addr=%p, size=%d\n",
		(void *) buf->area,
		(void *) buf->addr,
		size);

	if (!buf->area)
		return -ENOMEM;

	buf->bytes = size;
	return 0;
}
/*--------------------------------------------------------------------------*\
 * ISR
\*--------------------------------------------------------------------------*/
static int I2SSentCntInPeriod;
static int I2SRcvCntInPeriod;

int k = 0;
u32 TotalTxCnt = 0;

static void atmel_pcm_dma_irq(u32 i2s_sr,
	struct snd_pcm_substream *substream)
{

	AITPS_AUD_I2SFIFO   pAUD    = AITC_BASE_I2S_FIFO;
	static int count;	
	int unWriteCnt;	
	struct atmel_runtime_data *prtd = substream->runtime->private_data;
	struct ait_pcm_params *params = prtd->params;


	struct snd_dma_buffer *buf = &substream->dma_buffer;

	count++;

	if (i2s_sr & AUD_INT_FIFO_REACH_UNWR_TH){
		int i;
		unsigned short* pcmPtr;

		unWriteCnt =(int)pAUD->I2S_FIFO_WR_TH;

		if(prtd->channels==1)
			unWriteCnt >>=1;
		
		BUG_ON(!prtd->period_ptr );
		pcmPtr = (unsigned short*)buf->area+(prtd->period_ptr-prtd->dma_buffer)/params->pdc_xfer_size+I2SSentCntInPeriod;

		if((I2SSentCntInPeriod+unWriteCnt)>= (prtd->period_size / params->pdc_xfer_size))
		{

			for(i=0;i<((prtd->period_size/ params->pdc_xfer_size)-I2SSentCntInPeriod);++i)
			{
				pAUD->I2S_FIFO_DATA =pcmPtr[i];			
				if(prtd->channels==1)
					pAUD->I2S_FIFO_DATA = pcmPtr[i];
			}
			
			unWriteCnt -= (prtd->period_size/ params->pdc_xfer_size)-I2SSentCntInPeriod;

			I2SSentCntInPeriod = 0;

			prtd->period_ptr += prtd->period_size;			

			if (prtd->period_ptr >= prtd->dma_buffer_end)
				prtd->period_ptr = prtd->dma_buffer;
			BUG_ON(!prtd->period_ptr );
			pcmPtr = ((unsigned short*)buf->area+(prtd->period_ptr-prtd->dma_buffer)/ params->pdc_xfer_size);
			params->xfer_ptr = prtd->period_ptr;

			snd_pcm_period_elapsed(substream);			
		}

		for(i=0;i<unWriteCnt;++i)
		{
			pAUD->I2S_FIFO_DATA =pcmPtr[i];
			if(prtd->channels==1)
				pAUD->I2S_FIFO_DATA = pcmPtr[i];
		}

		I2SSentCntInPeriod+=unWriteCnt;
		
		params->xfer_ptr = prtd->period_ptr+I2SSentCntInPeriod*params->pdc_xfer_size;		

	}
	else if(i2s_sr &AUD_INT_FIFO_EMPTY)
	{
		int i;
		for (i = 0; i< 512; i++)
			pAUD->I2S_FIFO_DATA = 0;

		pr_warn("I2S FIFO EMPTY\n");

	}

	
}
static void ait_i2s_pcm_in_dma_irq(u32 i2s_sr,struct snd_pcm_substream *substream)
{

	struct atmel_runtime_data *prtd = substream->runtime->private_data;
	
	struct ait_pcm_params *params = prtd->params;

	int unReadCntInPeriod;
	struct snd_dma_buffer *buf = &substream->dma_buffer;

	AITPS_AUD_I2SFIFO   pAUD    = AITC_BASE_I2S_FIFO;

	
	if (i2s_sr & AUD_INT_FIFO_REACH_UNRD_TH){
		int i;
		unsigned short* pcmPtr;
		unsigned short pcmPtrDummy;

//		unReadCntInPeriod = prtd->afe_fifo_read_threshold/params->pdc_xfer_size;
		unReadCntInPeriod =(int)pAUD->I2S_FIFO_RD_TH;

		pcmPtr = ((unsigned short*)buf->area+(prtd->period_ptr-prtd->dma_buffer)/params->pdc_xfer_size+I2SRcvCntInPeriod);

		if((I2SRcvCntInPeriod+unReadCntInPeriod)>= (prtd->period_size / params->pdc_xfer_size))
		{
			for(i=0;i<((prtd->period_size/ params->pdc_xfer_size)-I2SRcvCntInPeriod);++i)
			{
				pcmPtr[i] =  pAUD->I2S_FIFO_DATA;
				if(prtd->channels==1)
					pcmPtrDummy = pAUD->I2S_FIFO_DATA;
			}
		
			unReadCntInPeriod -= ((prtd->period_size/ params->pdc_xfer_size)-I2SRcvCntInPeriod);
			I2SRcvCntInPeriod = 0;

			prtd->period_ptr += prtd->period_size;			

			if (prtd->period_ptr >= prtd->dma_buffer_end)
				prtd->period_ptr = prtd->dma_buffer;

			pcmPtr = ((unsigned short*)buf->area+(prtd->period_ptr-prtd->dma_buffer)/ params->pdc_xfer_size);
			params->xfer_ptr= prtd->period_ptr; 

			
			snd_pcm_period_elapsed(substream);
		}

		for(i=0;i<unReadCntInPeriod;++i)
		{
			pcmPtr[i] = pAUD->I2S_FIFO_DATA;
			if(prtd->channels==1)
				pcmPtrDummy = pAUD->I2S_FIFO_DATA;
			
		}

		I2SRcvCntInPeriod+=unReadCntInPeriod;
		
		params->xfer_ptr= prtd->period_ptr+I2SRcvCntInPeriod*params->pdc_xfer_size;
	}else if(i2s_sr &AUD_INT_FIFO_FULL){

		pr_warn("I2S FIFO FULL\n");

	}
	
	
}

/*--------------------------------------------------------------------------*\
 * PCM operations
\*--------------------------------------------------------------------------*/
static int vsnv3_i2s_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct atmel_runtime_data *prtd = runtime->private_data;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;

	/* this may get called several times by oss emulation
	 * with different params */

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);
	runtime->dma_bytes = params_buffer_bytes(params);

	prtd->params = snd_soc_dai_get_dma_data(rtd->cpu_dai, substream);
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		prtd->params->dma_intr_handler = atmel_pcm_dma_irq;
	else
		prtd->params->dma_intr_handler = ait_i2s_pcm_in_dma_irq;
		

	prtd->dma_buffer = runtime->dma_addr;
	prtd->dma_buffer_end = runtime->dma_addr + runtime->dma_bytes;
	prtd->period_size = params_period_bytes(params);
	prtd->channels = params_channels(params);

	pr_debug("atmel-pcm: "
		"hw_params: DMA for %s initialized "
		"(dma_bytes=%u, period_size=%u)\n",
		prtd->params->name,
		runtime->dma_bytes,
		prtd->period_size);
	return 0;
}

static int vsnv3_i2s_pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct atmel_runtime_data *prtd = substream->runtime->private_data;
	struct ait_pcm_params *params = prtd->params;

	if (params != NULL) {
		prtd->params->dma_intr_handler = NULL;
	}

	return 0;
}

static int vsnv3_i2s_pcm_prepare(struct snd_pcm_substream *substream)
{
	return 0;
}


static int start =0;
static int vsnv3_i2s_pcm_trigger(struct snd_pcm_substream *substream,
	int cmd)
{
	struct snd_pcm_runtime *rtd = substream->runtime;
	struct atmel_runtime_data *prtd = rtd->private_data;
	struct ait_pcm_params *params = prtd->params;
	int ret = 0;
	AITPS_AUD_I2SFIFO   pAUD    = AITC_BASE_I2S_FIFO;

	pr_err("vsnv3_i2s_pcm_trigger: cmd = %d\r\n",cmd);
	pr_debug("vsnv3_i2s_pcm_trigger:buffer_size = %ld,"
		"dma_area = %p, dma_bytes = %u\n",
		rtd->buffer_size, rtd->dma_area, rtd->dma_bytes);

	switch (cmd) {
		case SNDRV_PCM_TRIGGER_START:
			if(start ==0)
			{
					int i;
					prtd->period_ptr = prtd->dma_buffer;

					params->xfer_ptr = prtd->period_ptr;
					I2SSentCntInPeriod = 0;
					I2SRcvCntInPeriod = 0;

					pAUD->I2S_FIFO_UNRD_CNT = 0;
					pAUD->I2S_FIFO_UNWR_CNT = 0;	
					pAUD->I2S_FIFO_SR = pAUD->I2S_FIFO_SR;
					pAUD->I2S_FIFO_RST = AUD_FIFO_RST_EN;
					pAUD->I2S_FIFO_RST = 0;
					pAUD->I2S_FIFO_WR_TH = 128;
					pAUD->I2S_FIFO_RD_TH = 128;					

					for (i = 0; i< 256; i++)
						pAUD->I2S_FIFO_DATA = 0;

					start = 1;
			}

			pAUD->I2S_FIFO_SR = pAUD->I2S_FIFO_SR;
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
				pAUD->I2S_FIFO_CPU_INT_EN = AUD_INT_FIFO_REACH_UNWR_TH|AUD_INT_FIFO_EMPTY;
			else
				pAUD->I2S_FIFO_CPU_INT_EN = AUD_INT_FIFO_REACH_UNRD_TH|AUD_INT_FIFO_FULL;
			
			BUG_ON(!prtd->period_ptr );			
	
			break;

		case SNDRV_PCM_TRIGGER_STOP:
		case SNDRV_PCM_TRIGGER_SUSPEND:
		case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			pAUD->I2S_FIFO_CPU_INT_EN &= ~(AUD_INT_FIFO_REACH_UNWR_TH|AUD_INT_FIFO_EMPTY);
			else
				pAUD->I2S_FIFO_CPU_INT_EN &= ~(AUD_INT_FIFO_REACH_UNRD_TH|AUD_INT_FIFO_FULL);

			pAUD->I2S_FIFO_SR = pAUD->I2S_FIFO_SR;
			pAUD->I2S_FIFO_RST = AUD_FIFO_RST_EN;
			pAUD->I2S_FIFO_RST = 0;
			{
				int i;
				for (i = 0; i< 512; i++)
					pAUD->I2S_FIFO_DATA = 0;
			}
			start =0;
			break;

		case SNDRV_PCM_TRIGGER_RESUME:
		case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
				pAUD->I2S_FIFO_CPU_INT_EN = AUD_INT_FIFO_REACH_UNWR_TH|AUD_INT_FIFO_EMPTY;
			else
				pAUD->I2S_FIFO_CPU_INT_EN = AUD_INT_FIFO_REACH_UNRD_TH|AUD_INT_FIFO_FULL;
			break;

		default:
			ret = -EINVAL;
	}

	return ret;
}

static snd_pcm_uframes_t vsnv3_i2s_pcm_pointer(
	struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct atmel_runtime_data *prtd = runtime->private_data;
	struct ait_pcm_params *params = prtd->params;
	dma_addr_t ptr;
	snd_pcm_uframes_t x;

	ptr = params->xfer_ptr;	
	x = bytes_to_frames(runtime, ptr - prtd->dma_buffer);

	if (x == runtime->buffer_size)
		x = 0;
	return x;
}

static int vsnv3_i2s_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct atmel_runtime_data *prtd;
	int ret = 0;

	snd_soc_set_runtime_hwparams(substream, &ait_i2s_pcm_hardware);

	/* ensure that buffer size is a multiple of period size */
	ret = snd_pcm_hw_constraint_integer(runtime,SNDRV_PCM_HW_PARAM_PERIODS);
	
	if (ret < 0)
		goto out;

	prtd = kzalloc(sizeof(struct atmel_runtime_data), GFP_KERNEL);
	if (prtd == NULL) {	
		ret = -ENOMEM;
		goto out;
	}
	runtime->private_data = prtd;

 out:
	return ret;
}

static int vsnv3_i2s_pcm_close(struct snd_pcm_substream *substream)
{
	struct atmel_runtime_data *prtd = substream->runtime->private_data;

	kfree(prtd);
	return 0;
}

static int vsnv3_i2s_pcm_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma)
{

	return remap_pfn_range(vma, vma->vm_start,
		       substream->dma_buffer.addr >> PAGE_SHIFT,
		       vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static struct snd_pcm_ops ait_i2s_pcm_ops = {
	.open		= vsnv3_i2s_pcm_open,
	.close		= vsnv3_i2s_pcm_close,
	.ioctl			= snd_pcm_lib_ioctl,
	.hw_params	= vsnv3_i2s_pcm_hw_params,
	.hw_free		=vsnv3_i2s_pcm_hw_free,
	.prepare		= vsnv3_i2s_pcm_prepare,
	.trigger		= vsnv3_i2s_pcm_trigger,
	.pointer		= vsnv3_i2s_pcm_pointer,
	.mmap		= vsnv3_i2s_pcm_mmap,
};


/*--------------------------------------------------------------------------*\
 * ASoC platform driver
\*--------------------------------------------------------------------------*/
//static u64 atmel_pcm_dmamask = 0xffffffff;


static int vsnv3_i2s_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_soc_dai *dai = rtd->cpu_dai;
	struct snd_pcm *pcm = rtd->pcm;
	int ret = 0;
	
	if (!card->dev->dma_mask)
		card->dev->dma_mask = 0xffffffff;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = 0xffffffff;

	if (dai->driver->playback.channels_min) {
		ret = ait_i2s_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (dai->driver->capture.channels_min) {
		pr_debug("ait-pcm:"
				"Allocating PCM capture DMA buffer\n");
		ret = ait_i2s_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
 out:
	return ret;
}

static void vsnv3_i2s_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;
		dma_free_coherent(pcm->card->dev, buf->bytes,
				  buf->area, buf->addr);
		buf->area = NULL;
	}
}

#ifdef CONFIG_PM
static int vsnv3_i2s_pcm_suspend(struct snd_soc_dai *dai)
{
	struct snd_pcm_runtime *runtime = dai->runtime;
	struct atmel_runtime_data *prtd;
	struct ait_pcm_params *params;


	if (!runtime)
		return 0;

	prtd = runtime->private_data;
	params = prtd->params;

	return 0;
}

static int vsnv3_i2s_pcm_resume(struct snd_soc_dai *dai)
{
	struct snd_pcm_runtime *runtime = dai->runtime;
	struct atmel_runtime_data *prtd;
	struct ait_pcm_params *params;

	if (!runtime)
		return 0;

	prtd = runtime->private_data;
	params = prtd->params;

	return 0;
}
#else
#define vsnv3_i2s_pcm_suspend	NULL
#define vsnv3_i2s_pcm_resume	NULL
#endif

static struct snd_soc_platform_driver vsnv3_i2s_soc_platform = {
	.ops			= &ait_i2s_pcm_ops,
	.pcm_new	= vsnv3_i2s_pcm_new,
	.pcm_free	= vsnv3_i2s_pcm_free_dma_buffers,
	.suspend		= vsnv3_i2s_pcm_suspend,
	.resume		= vsnv3_i2s_pcm_resume,
};

static int __devinit vsnv3_i2s_soc_platform_probe(struct platform_device *pdev)
{

	return snd_soc_register_platform(&pdev->dev, &vsnv3_i2s_soc_platform);
}

static int __devexit vsnv3_i2s_soc_platform_remove(struct platform_device *pdev)
{
	snd_soc_unregister_platform(&pdev->dev);
	return 0;
}

static struct platform_driver vsnv3_i2s_pcm_driver = {
	.driver = {
			.name = "ait-pcm-audio",
			.owner = THIS_MODULE,
	},

	.probe = vsnv3_i2s_soc_platform_probe,
	.remove = __devexit_p(vsnv3_i2s_soc_platform_remove),
};

static int __init snd_atmel_pcm_init(void)
{
	return platform_driver_register(&vsnv3_i2s_pcm_driver);
}
module_init(snd_atmel_pcm_init);

static void __exit snd_atmel_pcm_exit(void)
{
	platform_driver_unregister(&vsnv3_i2s_pcm_driver);
}
module_exit(snd_atmel_pcm_exit);

MODULE_AUTHOR("Vincent Chen <vincent_chen@a-i-t.com>");
MODULE_DESCRIPTION("AIT8455 PCM module");
MODULE_LICENSE("GPL");
