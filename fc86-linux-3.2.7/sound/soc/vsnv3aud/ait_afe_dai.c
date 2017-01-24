/*
 * atmel_ssc_dai.c  --  ALSA SoC ATMEL SSC Audio Layer Platform driver
 *
 * Copyright (C) 2005 SAN People
 * Copyright (C) 2008 Atmel
 *
 * Author: Sedji Gaouaou <sedji.gaouaou@atmel.com>
 *         ATMEL CORP.
 *
 * Based on at91-ssc.c by
 * Frank Mandarino <fmandarino@endrelia.com>
 * Based on pxa2xx Platform drivers by
 * Liam Girdwood <lrg@slimlogic.co.uk>
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
 //Vin
//#define DEBUG

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/slab.h>

#include <linux/atmel-ssc.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>

#include <mach/hardware.h>

#include "ait-pcm.h"
#include "ait_ssc_dai.h"

#include <mach/mmpf_typedef.h>
#include <mach/mmp_reg_audio.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_audio.h>
#include <mach/vsnv3_afe.h>


/*
 * DMA parameters.
 */
static struct ait_pcm_params ssc_dma_params[2][2] = {
	{{
	.name		= "SSC0 PCM out",	//I2S FIFO out
	},
	{
	.name		= "SSC0 PCM in",//AFE FIFO in
	} },
	{{
	.name		= "Audio AFE out(Unsuuport)",
	},
	{
	.name		= "SSC1 AFE in",
	} },

};


static struct ait_afe_dai_ssc_info ssc_info[2] = {
	{
	.name		= "ssc0",
	.lock		= __SPIN_LOCK_UNLOCKED(ssc_info[0].lock),
	.dir_mask	= SSC_DIR_MASK_UNUSED,
	.initialized	= 0,
	},
	{
	.name		= "ssc1",
	.lock		= __SPIN_LOCK_UNLOCKED(ssc_info[1].lock),
	.dir_mask	= SSC_DIR_MASK_UNUSED,
	.initialized	= 0,
	},

};


/*
 * SSC interrupt handler.  Passes PDC interrupts to the DMA
 * interrupt handler in the PCM driver.
 */
static irqreturn_t ait_afe_interrupt(int irq, void *dev_id)
{
	struct ait_afe_dai_ssc_info *ssc_p = dev_id;
	struct ait_pcm_params *dma_params;
	u32 ssc_sr;
	u32 ssc_substream_mask,afe_substream_mask;
	int i;
	u32 afe_sr,afe_mask;
	
	if(ssc_p->ssc->regs==AITC_BASE_AFE)
	{
		afe_sr = afereg_readb(ssc_p->ssc->regs,AFE_FIFO_CSR)&0x0f;
		afe_mask = afereg_readb(ssc_p->ssc->regs,AFE_FIFO_CPU_INT_EN)&0x0f;

		afe_sr = afe_sr&afe_mask;
		

		for (i = 0; i < ARRAY_SIZE(ssc_p->dma_params); i++) {
			dma_params = ssc_p->dma_params[i];
			if ((dma_params != NULL) &&
				(dma_params->dma_intr_handler != NULL)) {
				if (afe_sr & afe_mask) {
//			afereg_writeb(ssc_p->ssc->regs,AFE_FIFO_CPU_INT_EN,afereg_readb(ssc_p->ssc->regs,AFE_FIFO_CPU_INT_EN)&(~AFE_INT_FIFO_REACH_UNRD_TH));
					dma_params->dma_intr_handler(afe_sr,dma_params->substream);
//					afereg_writeb(ssc_p->ssc->regs,AFE_FIFO_CPU_INT_EN,afereg_readb(ssc_p->ssc->regs,AFE_FIFO_CPU_INT_EN)|(AFE_INT_FIFO_REACH_UNRD_TH));			
				}				
			}
		}

	}

	return IRQ_HANDLED;
}


/*-------------------------------------------------------------------------*\
 * DAI functions
\*-------------------------------------------------------------------------*/
/*
 * Startup.  Only that one substream allowed in each direction.
 */
static int ait_afe_dai_startup(struct snd_pcm_substream *substream,
			     struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[dai->id];
	int dir_mask;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
	{		
		dir_mask = SSC_DIR_MASK_PLAYBACK;
	}
	else
	{
		pr_debug("Start capture\n");
		dir_mask = SSC_DIR_MASK_CAPTURE;
	}

	spin_lock_irq(&ssc_p->lock);
	if (ssc_p->dir_mask & dir_mask) {
		spin_unlock_irq(&ssc_p->lock);
		return -EBUSY;
	}
	ssc_p->dir_mask |= dir_mask;
	spin_unlock_irq(&ssc_p->lock);

	return 0;
}

/*
 * Shutdown.  Clear DMA parameters and shutdown the SSC if there
 * are no other substreams open.
 */
static void ait_afe_dai_shutdown(struct snd_pcm_substream *substream,
			       struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[dai->id];
	struct ait_pcm_params *dma_params;
	int dir, dir_mask;
pr_debug("%s\r\n",__FUNCTION__);

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dir = 0;
	else
		dir = 1;

	dma_params = ssc_p->dma_params[dir];

	if (dma_params != NULL) {

		dma_params->ssc = NULL;
		dma_params->substream = NULL;
		ssc_p->dma_params[dir] = NULL;
	}

	dir_mask = 1 << dir;

	spin_lock_irq(&ssc_p->lock);
	ssc_p->dir_mask &= ~dir_mask;
	if (!ssc_p->dir_mask) {
		if (ssc_p->initialized) {
			/* Shutdown the SSC clock. */
			pr_debug("atmel_ssc_dau: Stopping clock\n");
			clk_disable(ssc_p->ssc->clk);

			free_irq(ssc_p->ssc->irq, ssc_p);
			ssc_p->initialized = 0;
		}

		ssc_p->cmr_div = ssc_p->tcmr_period = ssc_p->rcmr_period = 0;
	}
	spin_unlock_irq(&ssc_p->lock);
}


/*
 * Record the DAI format for use in hw_params().
 */
static int ait_afe_dai_set_dai_fmt(struct snd_soc_dai *cpu_dai,
		unsigned int fmt)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[cpu_dai->id];
	ssc_p->daifmt = fmt;
	return 0;
}

/*
 * Record SSC clock dividers for use in hw_params().
 */
static int ait_afe_dai_set_dai_clkdiv(struct snd_soc_dai *cpu_dai,
	int div_id, int div)
{

	return 0;
}

/*
 * Configure the SSC.
 */
static int ait_afe_dai_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params,
	struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = snd_pcm_substream_chip(substream);
	int id = dai->id;
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[id];
	struct ait_pcm_params *dma_params;
	int dir, channels, bits;
	u32 tfmr, rfmr, tcmr, rcmr;
	int start_event;
	int ret;
	pr_debug("%s: para \r\n",__func__,params_format(params));


	AITPS_GBL pGBL = AITC_BASE_GBL;

	/*
	 * Currently, there is only one set of dma params for
	 * each direction.  If more are added, this code will
	 * have to be changed to select the proper set.
	 */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dir = 0;
	else
		dir = 1;

	dma_params = &ssc_dma_params[id][dir];
	dma_params->ssc = ssc_p->ssc;
	dma_params->substream = substream;

	ssc_p->dma_params[dir] = dma_params;

	/*
	 * The snd_soc_pcm_stream->dma_data field is only used to communicate
	 * the appropriate DMA parameters to the pcm driver hw_params()
	 * function.  It should not be used for other purposes
	 * as it is common to all substreams.
	 */
	snd_soc_dai_set_dma_data(rtd->cpu_dai, substream, dma_params);

	channels = params_channels(params);

	/*
	 * Determine sample size in bits and the PDC increment.
	 */
	switch (params_format(params)) {
		
	case SNDRV_PCM_FORMAT_S16_LE:
		bits = 16;
		dma_params->pdc_xfer_size = 2;
		break;
	case SNDRV_PCM_FORMAT_S8:
	case SNDRV_PCM_FORMAT_S24_LE:
	case SNDRV_PCM_FORMAT_S32_LE:
	default:
		printk(KERN_WARNING "atmel_ssc_dai: unsupported PCM format");
		return -EINVAL;
	}

	/*
	 * The SSC only supports up to 16-bit samples in I2S format, due
	 * to the size of the Frame Mode Register FSLEN field.
	 */
	if ((ssc_p->daifmt & SND_SOC_DAIFMT_FORMAT_MASK) == SND_SOC_DAIFMT_I2S
		&& bits > 16) {
		printk(KERN_WARNING
				"atmel_ssc_dai: sample size %d "
				"is too large for I2S\n", bits);
		return -EINVAL;
	}

	/*
	 * Compute SSC register settings.
	 */
	switch (ssc_p->daifmt
		& (SND_SOC_DAIFMT_FORMAT_MASK | SND_SOC_DAIFMT_MASTER_MASK)) {

	case SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBS_CFS:
	case SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_CBM_CFM:
	case SND_SOC_DAIFMT_DSP_A | SND_SOC_DAIFMT_CBS_CFS:
		break;

	case SND_SOC_DAIFMT_DSP_A | SND_SOC_DAIFMT_CBM_CFM:
	default:
		printk(KERN_WARNING "atmel_ssc_dai: unsupported DAI format 0x%x\n",
			ssc_p->daifmt);
		return -EINVAL;
	}


	pGBL->GBL_AUDIO_CLK_DIV = GBL_AUDIO_CLK_SRC_DPLL2|GBL_AUDIO_CLK_ON|GBL_AUDIO_CLK_OFF|(GBL_AUDIO_CLK_DIV_MASK&0x0A);
	if (params_rate(params) > 24000)
		pGBL->GBL_ADC_CLK_DIV = 0x0331;
	else if (params_rate(params) > 12000)
		pGBL->GBL_ADC_CLK_DIV = 0x0773;
	else	
		pGBL->GBL_ADC_CLK_DIV = 0x0FF7;

	pr_info("%s:%d pGBL->GBL_ADC_CLK_DIV = 0x%x\n",__func__,__LINE__,pGBL->GBL_ADC_CLK_DIV);

	pr_debug("atmel_ssc_hw_params: "
				"sample rate = %d\r\n",params_rate(params));

	if (!ssc_p->initialized) {

		/* Enable PMC peripheral clock for this SSC */
		pr_debug("atmel_ssc_dai: Starting clock\n");
		clk_enable(ssc_p->ssc->clk);

		afereg_writeb(ssc_p->ssc->regs,FIX_AFE_ADC_OVERFLOW, 3);

		afereg_writeb(ssc_p->ssc->regs,SPECIAL_AUD_CODEC_PATH, 0);
		afereg_writeb(ssc_p->ssc->regs,AFE_GBL_BIAS_ADJ, GBL_BIAS_100);

		afereg_writeb(ssc_p->ssc->regs,AFE_ADC_BIAS_ADJ, ANA_ADC_CONT_OP(1)|ANA_ADC_DISC_OP(1));
		afereg_writeb(ssc_p->ssc->regs,AFE_ADC_CTL_REG1, AFE_ZERO_CROSS_DET);

		
		afereg_writeb(ssc_p->ssc->regs,AFE_GBL_PWR_CTL, afereg_readb(ssc_p->ssc->regs,AFE_GBL_PWR_CTL) | PWR_UP_ANALOG);
		msleep_interruptible(20);
		afereg_writeb(ssc_p->ssc->regs,AFE_GBL_PWR_CTL, afereg_readb(ssc_p->ssc->regs, AFE_GBL_PWR_CTL) | PWR_UP_VREF);
		msleep_interruptible(1);
		afereg_writeb(ssc_p->ssc->regs,AFE_ADC_PWR_CTL, afereg_readb(ssc_p->ssc->regs, AFE_ADC_PWR_CTL) | ADC_SDM_LCH_POWER_EN |
																										ADC_SDM_RCH_POWER_EN|
																										ADC_PGA_LCH_POWER_EN|
																										ADC_PGA_RCH_POWER_EN);
		afereg_writeb(ssc_p->ssc->regs,AFE_GBL_PWR_CTL, afereg_readb(ssc_p->ssc->regs, AFE_GBL_PWR_CTL)  |PWR_UP_ADC_DIGITAL_FILTER);

		afereg_writeb(ssc_p->ssc->regs,AFE_ADC_HPF_CTL, ADC_HPF_EN);
		
		afereg_writeb(ssc_p->ssc->regs,AFE_ADC_INPUT_SEL, ADC_MIC_IN|ADC_MIC_DIFF2SINGLE);

		afereg_writeb(ssc_p->ssc->regs,AFE_CLK_CTL, ADC_CLK_MODE_USB);
		afereg_writeb(ssc_p->ssc->regs,AFE_CLK_CTL, afereg_readb(ssc_p->ssc->regs, AFE_CLK_CTL) | (ADC_CLK_INVERT | AUD_CODEC_NORMAL_MODE));
		
		pr_info("ssc irq = 0x%x\r\n",ssc_p->ssc->irq);
		ret = request_irq(ssc_p->ssc->irq, ait_afe_interrupt, 0,
				ssc_p->name, ssc_p);
		if (ret < 0) {
			printk(KERN_WARNING
					"atmel_ssc_dai: request_irq failure\n");
			pr_debug("Atmel_ssc_dai: Stoping clock\n");
			clk_disable(ssc_p->ssc->clk);
			return ret;
		}

		ssc_p->initialized = 1;
	}

	pr_debug("atmel_ssc_dai,hw_params: SSC initialized\n");
	return 0;
}


static int ait_afe_dai_prepare(struct snd_pcm_substream *substream,
			     struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[dai->id];
	struct ait_pcm_params *dma_params;
	int dir;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dir = 0;
	else
		dir = 1;

	dma_params = ssc_p->dma_params[dir];

	return 0;
}


#ifdef CONFIG_PM
static int ait_afe_dai_suspend(struct snd_soc_dai *cpu_dai)
{
	struct ait_afe_dai_ssc_info *ssc_p;

	if (!cpu_dai->active)
		return 0;

	ssc_p = &ssc_info[cpu_dai->id];
		
	clk_disable(ssc_p->ssc->clk);

	return 0;
}



static int ait_afe_dai_resume(struct snd_soc_dai *cpu_dai)
{
	struct ait_afe_dai_ssc_info *ssc_p;
	u32 cr;

	if (!cpu_dai->active)
		return 0;

	ssc_p = &ssc_info[cpu_dai->id];
	clk_enable(ssc_p->ssc->clk);


	return 0;
}
#else /* CONFIG_PM */
#define ait_afe_dai_suspend	NULL
#define ait_afe_dai_resume	NULL
#endif /* CONFIG_PM */

static int ait_afe_dai_probe(struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[dai->id];
	int ret = 0;
	struct ssc_device *ssc = snd_soc_dai_get_drvdata(dai);
	
	snd_soc_dai_set_drvdata(dai, ssc_p);

	ssc_p->ssc = ssc;
	if (IS_ERR(ssc_p->ssc)) {
		printk(KERN_ERR "ASoC: Failed to request SSC %d\n", dai->id);
		ret = PTR_ERR(ssc_p->ssc);
	}
	return ret;
}

static int ait_afe_dai_remove(struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = snd_soc_dai_get_drvdata(dai);

//	ssc_free(ssc_p->ssc);
	return 0;
}

#define AIT_AFE_RATES SNDRV_PCM_RATE_8000_48000
#define AIT_AFE_FORMATS (SNDRV_PCM_FMTBIT_S16_LE )


static struct snd_soc_dai_ops ait_afe_dai_dai_ops = {
	.startup	= ait_afe_dai_startup,
	.shutdown	= ait_afe_dai_shutdown,
	.prepare	= ait_afe_dai_prepare,
	.hw_params	= ait_afe_dai_hw_params,
	.set_fmt	= ait_afe_dai_set_dai_fmt,
	.set_clkdiv	= ait_afe_dai_set_dai_clkdiv,
};

static struct snd_soc_dai_driver ait_afe_dai= {

		.name = "ait-afe-dai.0",
		.probe = ait_afe_dai_probe,
		.remove = ait_afe_dai_remove,
		.suspend = ait_afe_dai_suspend,
		.resume = ait_afe_dai_resume,
#if 0		
		.playback = {
			.channels_min = 1,
			.channels_max = 2,
			.rates = ATMEL_SSC_RATES,
			.formats = ATMEL_SSC_FORMATS,},
#endif			
		.capture = {
			.channels_min = 1,
			.channels_max = 2,
			.rates = AIT_AFE_RATES,
			.formats = AIT_AFE_FORMATS},
		.ops = &ait_afe_dai_dai_ops,


};

static __devinit int ait_afe_probe(struct platform_device *pdev)
{

	int retval = 0;
	struct resource *regs;
	struct ssc_device *ssc;

	ssc = kzalloc(sizeof(struct ssc_device), GFP_KERNEL);
	if (!ssc) {
		dev_dbg(&pdev->dev, "out of memory\n");
		retval = -ENOMEM;
		goto out;
	}
	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) {
		dev_warn(&pdev->dev, "no mmio resource defined\n");
		retval = -ENXIO;
		goto out_free;
	}

	ssc->pdev = pdev;
	
	ssc->regs = ioremap(regs->start, resource_size(regs));
	if (!ssc->regs) {
		dev_warn(&pdev->dev, "ioremap failed\n");
		retval = -EINVAL;
		goto out_clk;
	}

	dev_dbg(&pdev->dev, "reg addr = 0x%x\n",ssc->regs);

	ssc->clk = clk_get(&pdev->dev, "afe_clk");		

	if (IS_ERR(ssc->clk)) {
		dev_warn(&pdev->dev, "no pclk clock defined\n");
		retval = -ENXIO;
		goto out_free;
	}
	ssc->irq = platform_get_irq(pdev, 0);
	if (!ssc->irq) {
		dev_warn(&pdev->dev, "could not get irq\n");
		retval = -ENXIO;
		goto out_unmap;
	}

	platform_set_drvdata(pdev, ssc);

	dev_info(&pdev->dev, "AIT Audio device at 0x%p (irq %d)\n",
			ssc->regs, ssc->irq);
#if 1
	return snd_soc_register_dai(&pdev->dev, &ait_afe_dai);
#else
	BUG_ON(pdev->id < 0);
	BUG_ON(pdev->id >= ARRAY_SIZE(atmel_ssc_dai));
	return snd_soc_register_dai(&pdev->dev, &atmel_ssc_dai[pdev->id]);
#endif	

out_unmap:
	iounmap(ssc->regs);
out_clk:
	clk_put(ssc->clk);
out_free:
	kfree(ssc);
out:
	return retval;
}

static int __devexit asoc_ssc_remove(struct platform_device *pdev)
{

	snd_soc_unregister_dai(&pdev->dev);
	return 0;
}

static struct platform_driver ait_afe_driver = {
	.driver = {
			.name = "ait-afe-dai",
			.owner = THIS_MODULE,
	},

	.probe = ait_afe_probe,
	.remove = __devexit_p(asoc_ssc_remove),
};

module_platform_driver(ait_afe_driver)
	
/* Module information */
MODULE_AUTHOR("Sedji Gaouaou, sedji.gaouaou@atmel.com, www.atmel.com");
MODULE_DESCRIPTION("ATMEL SSC ASoC Interface");
MODULE_LICENSE("GPL");
