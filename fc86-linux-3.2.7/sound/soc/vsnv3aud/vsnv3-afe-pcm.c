/*
 * vsnv3-afe-pcm.c  --  ALSA PCM interface for the AIT VSNV3 SoC.
 *
 *  Copyright (C) 2013 Alpha Image Tech.
 * Authors: Vincent Chen <vincent_chen@a-i-t.com.tw>
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
//#define DEBUG

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
/* TODO: These values were taken from the AT91 platform driver, check
 *	 them against real values for AT32
 */
static const struct snd_pcm_hardware vsnv3_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_MMAP |
				  SNDRV_PCM_INFO_MMAP_VALID |
				  SNDRV_PCM_INFO_INTERLEAVED |
				  SNDRV_PCM_INFO_PAUSE,
	.formats		= SNDRV_PCM_FMTBIT_S16_LE,
	.period_bytes_min	= 160,
	.period_bytes_max	= 8192,
	.periods_min		= 100,
	.periods_max		= 1024,
	.buffer_bytes_max	= 64 * 1024,
};


/*--------------------------------------------------------------------------*\
 * Data types
\*--------------------------------------------------------------------------*/
struct vsnv3_runtime_data {
	struct ait_pcm_params *params;
	dma_addr_t dma_buffer;		/* physical address of dma buffer */
	dma_addr_t dma_buffer_end;	/* first address beyond DMA buffer */
	size_t period_size;

	dma_addr_t period_ptr;		/* physical address of next period */

	u8   channels;

	u32 afe_fifo_read_threshold;
};


/*--------------------------------------------------------------------------*\
 * Helper functions
\*--------------------------------------------------------------------------*/
static int vsnv3_pcm_preallocate_dma_buffer(struct snd_pcm *pcm,
	int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = vsnv3_pcm_hardware.buffer_bytes_max;
	pr_debug("%s\r\n",__FUNCTION__);

	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	buf->area = dma_alloc_coherent(pcm->card->dev, size,
					  &buf->addr, GFP_KERNEL);
	pr_debug("vsnv3-pcm:"
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
static int AFESentCntInPeriod;
static void vsnv3_pcm_dma_irq(u32 afe_sr,
	struct snd_pcm_substream *substream)
{
	struct vsnv3_runtime_data *prtd = substream->runtime->private_data;
	struct ait_pcm_params *params = prtd->params;

	int unReadCntInPeriod;
	struct snd_dma_buffer *buf = &substream->dma_buffer;

	AITPS_AFE   pAFE    = AITC_BASE_AFE;

	if (afe_sr & AFE_INT_FIFO_REACH_UNRD_TH){
		int i;
		unsigned short* pcmPtr;
		unsigned short pcmPtrDummy;

		unReadCntInPeriod = prtd->afe_fifo_read_threshold/params->pdc_xfer_size;

		pcmPtr = ((unsigned short*)buf->area+(prtd->period_ptr-prtd->dma_buffer)/params->pdc_xfer_size+AFESentCntInPeriod);

		if((AFESentCntInPeriod+unReadCntInPeriod)>= (prtd->period_size / params->pdc_xfer_size))
		{
			for(i=0;i<((prtd->period_size/ params->pdc_xfer_size)-AFESentCntInPeriod);++i)
			{
				pcmPtr[i] = pAFE->AFE_FIFO_DATA;
				if(prtd->channels==1)
					pcmPtrDummy = pAFE->AFE_FIFO_DATA;
			}
		
			unReadCntInPeriod -= ((prtd->period_size/ params->pdc_xfer_size)-AFESentCntInPeriod);
			AFESentCntInPeriod = 0;

			prtd->period_ptr += prtd->period_size;			

			if (prtd->period_ptr >= prtd->dma_buffer_end)
				prtd->period_ptr = prtd->dma_buffer;

			pcmPtr = ((unsigned short*)buf->area+(prtd->period_ptr-prtd->dma_buffer)/ params->pdc_xfer_size);
			params->xfer_ptr= prtd->period_ptr; 

			
			snd_pcm_period_elapsed(substream);
		}

		for(i=0;i<unReadCntInPeriod;++i)
		{
			pcmPtr[i] = pAFE->AFE_FIFO_DATA;
			if(prtd->channels==1)
				pcmPtrDummy = pAFE->AFE_FIFO_DATA;
			
		}

		AFESentCntInPeriod+=unReadCntInPeriod;
		
		params->xfer_ptr= prtd->period_ptr+AFESentCntInPeriod*params->pdc_xfer_size;
	}else if(afe_sr &AFE_INT_FIFO_FULL)
	{

		pr_warn("AFE FIFO FULL\n");

	}
	
}


/*--------------------------------------------------------------------------*\
 * PCM operations
\*--------------------------------------------------------------------------*/
static int vsnv3_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct vsnv3_runtime_data *prtd = runtime->private_data;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;

	AITPS_AFE   pAFE    = AITC_BASE_AFE;


	pr_debug("atmel-pcm: " "%s\r\n",__FUNCTION__);
	/* this may get called several times by oss emulation
	 * with different params */

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);
	runtime->dma_bytes = params_buffer_bytes(params);

	prtd->params = snd_soc_dai_get_dma_data(rtd->cpu_dai, substream);
	prtd->params->dma_intr_handler = vsnv3_pcm_dma_irq;

	prtd->dma_buffer = runtime->dma_addr;
	prtd->dma_buffer_end = runtime->dma_addr + runtime->dma_bytes;
	prtd->period_size = params_period_bytes(params);
	prtd->channels = params_channels(params);

	pAFE->AFE_FIFO_RD_TH = 256;

	prtd->afe_fifo_read_threshold = 256;

	return 0;
}

static int vsnv3_pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct vsnv3_runtime_data *prtd = substream->runtime->private_data;
	struct ait_pcm_params *params = prtd->params;
	pr_debug("%s\r\n",__FUNCTION__);
	if (params != NULL) {
		AITPS_AFE   pAFE    = AITC_BASE_AFE;
		pr_info("%s:	pAFE->AFE_FIFO_CPU_INT_EN = 0x%x\n",	__func__,pAFE->AFE_FIFO_CPU_INT_EN);
		pAFE->AFE_FIFO_CPU_INT_EN = 0;

		prtd->params->dma_intr_handler = NULL;
	}

	return 0;
}

static int vsnv3_pcm_prepare(struct snd_pcm_substream *substream)
{

	AITPS_AFE   pAFE    = AITC_BASE_AFE;
	pAFE->AFE_FIFO_CPU_INT_EN = 0;
	pAFE->AFE_MUX_MODE_CTL = AFE_MUX_CPU_MODE;

	pAFE->AFE_FIFO_RST = REST_FIFO;
	pAFE->AFE_FIFO_RST = 0;

	pAFE->AFE_FIFO_UNRD_CNT = 0;

	pAFE->AFE_FIFO_WR_TH = 0xFF;
				
	return 0;
}



static int vsnv3_pcm_trigger(struct snd_pcm_substream *substream,
	int cmd)
{
	struct snd_pcm_runtime *rtd = substream->runtime;
	struct vsnv3_runtime_data *prtd = rtd->private_data;
	struct ait_pcm_params *params = prtd->params;
	int ret = 0;
	AITPS_AFE   pAFE    = AITC_BASE_AFE;
	static int start =0;
	
	pr_debug("vsnv3_pcm_trigger: cmd = %d\r\n",cmd);	
	pr_debug("atmel-pcm:buffer_size = %ld,"
		"dma_area = %p, dma_bytes = %u\n",
		rtd->buffer_size, rtd->dma_area, rtd->dma_bytes);
	
	switch (cmd) {
		case SNDRV_PCM_TRIGGER_START:
			if(start ==0)
			{
					prtd->period_ptr = prtd->dma_buffer;

					params->xfer_ptr = prtd->period_ptr;

					AFESentCntInPeriod = 0;

					start = 1;
			}
			BUG_ON(!prtd->period_ptr);

			pAFE->AFE_FIFO_CSR = AFE_INT_FIFO_REACH_UNRD_TH |AFE_INT_FIFO_FULL;
			
			pAFE->AFE_FIFO_CPU_INT_EN = 	AFE_INT_FIFO_REACH_UNRD_TH |
										AFE_INT_FIFO_FULL;

			pAFE->AFE_MUX_MODE_CTL = AFE_MUX_AUTO_MODE;
					
			break;		/* SNDRV_PCM_TRIGGER_START */

		case SNDRV_PCM_TRIGGER_STOP:
		case SNDRV_PCM_TRIGGER_SUSPEND:
		case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
				pAFE->AFE_FIFO_CPU_INT_EN = 0;
				pAFE->AFE_MUX_MODE_CTL = AFE_MUX_AUTO_MODE;
				pAFE->AFE_FIFO_RST = REST_FIFO;
				pAFE->AFE_FIFO_RST = 0;
				start =0;
			break;

		case SNDRV_PCM_TRIGGER_RESUME:
		case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
			pAFE->AFE_FIFO_CPU_INT_EN |= AFE_INT_FIFO_REACH_UNRD_TH;
			pAFE->AFE_FIFO_CPU_INT_EN |= AUD_ADC_INT_EN;
			break;

		default:
			ret = -EINVAL;
	}

	return ret;
}

static snd_pcm_uframes_t vsnv3_pcm_pointer(
	struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct vsnv3_runtime_data *prtd = runtime->private_data;
	struct ait_pcm_params *params = prtd->params;
	dma_addr_t ptr;
	snd_pcm_uframes_t x;

	ptr = params->xfer_ptr;
	x = bytes_to_frames(runtime, ptr - prtd->dma_buffer);

	if (x == runtime->buffer_size)
		x = 0;
	return x;
}

static int vsnv3_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct vsnv3_runtime_data *prtd;
	int ret = 0;

	snd_soc_set_runtime_hwparams(substream, &vsnv3_pcm_hardware);

	/* ensure that buffer size is a multiple of period size */
	ret = snd_pcm_hw_constraint_integer(runtime,
						SNDRV_PCM_HW_PARAM_PERIODS);
	
	if (ret < 0)
		goto out;

	prtd = kzalloc(sizeof(struct vsnv3_runtime_data), GFP_KERNEL);
	if (prtd == NULL) {
		pr_err("%s: prtd == NULL\r\n",__FUNCTION__);		
		ret = -ENOMEM;
		goto out;
	}
	runtime->private_data = prtd;

 out:
	return ret;
}

static int vsnv3_pcm_close(struct snd_pcm_substream *substream)
{
	struct vsnv3_runtime_data *prtd = substream->runtime->private_data;
	AITPS_AFE   pAFE    = AITC_BASE_AFE;
	pAFE->AFE_FIFO_CPU_INT_EN = 0;

	kfree(prtd);
	return 0;
}

static int vsnv3_pcm_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma)
{

	return remap_pfn_range(vma, vma->vm_start,
		       substream->dma_buffer.addr >> PAGE_SHIFT,
		       vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static struct snd_pcm_ops atmel_pcm_ops = {
	.open		= vsnv3_pcm_open,
	.close		= vsnv3_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= vsnv3_pcm_hw_params,
	.hw_free	= vsnv3_pcm_hw_free,
	.prepare	= vsnv3_pcm_prepare,
	.trigger	= vsnv3_pcm_trigger,
	.pointer	= vsnv3_pcm_pointer,
	.mmap		= vsnv3_pcm_mmap,
};


/*--------------------------------------------------------------------------*\
 * ASoC platform driver
\*--------------------------------------------------------------------------*/
//static u64 atmel_pcm_dmamask = 0xffffffff;


static int vsnv3_pcm_new(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_card *card = rtd->card->snd_card;
	struct snd_soc_dai *dai = rtd->cpu_dai;
	struct snd_pcm *pcm = rtd->pcm;	//created by soc_new_pcm->snd_pcm_new
	int ret = 0;

	if (!card->dev->dma_mask)
		card->dev->dma_mask = (u64*)0xffffffff;//&atmel_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = (u64)0xffffffff;
	

	if (dai->driver->playback.channels_min) {
		ret = vsnv3_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}
	

	if (dai->driver->capture.channels_min) {
		pr_debug("vsnv3-pcm:"
				"Allocating PCM capture DMA buffer\n");
		ret = vsnv3_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
 out:
	return ret;
}

static void vsnv3_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;
	pr_debug("%s\r\n",__FUNCTION__);

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
static int vsnv3_pcm_suspend(struct snd_soc_dai *dai)
{
	struct snd_pcm_runtime *runtime = dai->runtime;
	struct vsnv3_runtime_data *prtd;
	struct ait_pcm_params *params;

	if (!runtime)
		return 0;

	prtd = runtime->private_data;
	params = prtd->params;

	return 0;
}

static int vsnv3_pcm_resume(struct snd_soc_dai *dai)
{
	struct snd_pcm_runtime *runtime = dai->runtime;
	struct vsnv3_runtime_data *prtd;
	struct ait_pcm_params *params;

	if (!runtime)
		return 0;

	prtd = runtime->private_data;
	params = prtd->params;

	return 0;
}
#else
#define atmel_pcm_suspend	NULL
#define atmel_pcm_resume	NULL
#endif

static struct snd_soc_platform_driver atmel_soc_platform = {
	.ops		= &atmel_pcm_ops,
	.pcm_new	= vsnv3_pcm_new,
	.pcm_free	= vsnv3_pcm_free_dma_buffers,
	.suspend	= vsnv3_pcm_suspend,
	.resume		= vsnv3_pcm_resume,
};

static int __devinit vsnv3_soc_platform_probe(struct platform_device *pdev)
{

	return snd_soc_register_platform(&pdev->dev, &atmel_soc_platform);
}

static int __devexit vsnv3_soc_platform_remove(struct platform_device *pdev)
{
	snd_soc_unregister_platform(&pdev->dev);
	return 0;
}

static struct platform_driver vsnv3_pcm_driver = {
	.driver = {
			.name = "vsnv3afe-pcm-audio",
			.owner = THIS_MODULE,
	},

	.probe = vsnv3_soc_platform_probe,
	.remove = __devexit_p(vsnv3_soc_platform_remove),
};

module_platform_driver(vsnv3_pcm_driver)

MODULE_AUTHOR("Vincent Chen <vincent_chen@a-i-t.com.tw>");
MODULE_DESCRIPTION("AIT PCM module");
MODULE_LICENSE("GPL");
