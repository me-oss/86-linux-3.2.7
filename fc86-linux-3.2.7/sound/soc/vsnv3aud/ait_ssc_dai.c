/*
 * ait_i2s_dai_dai.c  --  ALSA SoC ATMEL SSC Audio Layer Platform driver
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
//#define DEBUG

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/atmel_pdc.h>
#include <linux/slab.h>

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
#include <mach/mmpf_i2s_ctl.h>
#include <mach/mmp_reg_audio.h>
#include <mach/vsnv3_afe.h>

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
	.name		= "I2S in",
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


static irqreturn_t ait_audio_i2s_interrupt(int irq, void *dev_id)
{
	struct ait_afe_dai_ssc_info *ssc_p = dev_id;
	struct ait_pcm_params *dma_params;
	u32 ssc_sr;
	u32 afe_substream_mask;
	int i;

	ssc_sr =(unsigned long)((AITPS_AUD_I2SFIFO)AITC_BASE_I2S_FIFO)->I2S_FIFO_SR&(unsigned long)((AITPS_AUD_I2SFIFO)AITC_BASE_I2S_FIFO)->I2S_FIFO_CPU_INT_EN;
		
	/*
	 * Loop through the substreams attached to this SSC.  If
	 * a DMA-related interrupt occurred on that substream, call
	 * the DMA interrupt handler function, if one has been
	 * registered in the dma_params structure by the PCM driver.
	 */
	for (i = 0; i < ARRAY_SIZE(ssc_p->dma_params); i++) {
		dma_params = ssc_p->dma_params[i];
		if ((dma_params != NULL) &&
			(dma_params->dma_intr_handler != NULL)) {
				afe_substream_mask = AUD_INT_FIFO_EMPTY|
											AUD_INT_FIFO_REACH_UNWR_TH|
											AUD_INT_FIFO_FULL|
											AUD_INT_FIFO_REACH_UNRD_TH;


				if (ssc_sr & afe_substream_mask) {
						dma_params->dma_intr_handler(ssc_sr,
								dma_params->substream);
				}else
				{
	//				printk(KERN_ERR"(unsigned long)((AITPS_AUD)AITC_BASE_AUD)->I2S_FIFO_SR = %x\n",(unsigned long)((AITPS_AUD)AITC_BASE_AUD)->I2S_FIFO_SR);		
	//				printk(KERN_ERR"(unsigned long)((AITPS_AUD)AITC_BASE_AUD)->I2S_FIFO_CPU_INT_EN = %x\n",(unsigned long)((AITPS_AUD)AITC_BASE_AUD)->I2S_FIFO_CPU_INT_EN);		
				
	//				printk(KERN_ERR"ssc_sr = %x\n",ssc_sr);		
				
	//				WARN_ON(1);
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
static int ait_i2s_dai_startup(struct snd_pcm_substream *substream,
			     struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[dai->id];
	int dir_mask;

	clk_enable(ssc_p->ssc->clk);

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
static void ait_i2s_dai_shutdown(struct snd_pcm_substream *substream,
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
//Review this code
	
	ssc_p->dir_mask &= ~dir_mask;
	if (!ssc_p->dir_mask) {
		if (ssc_p->initialized) {
			AITPS_AUD_I2SCTRL   pAUD  = AITC_BASE_I2S_CTRL;
				
			/* Shutdown the SSC clock. */
			pr_debug("atmel_ssc_dau: Stopping clock\n");
			pAUD->I2S_CTL &= ~I2S_SDO_OUT_EN;			

//Review this code
			clk_disable(ssc_p->ssc->clk);				// don't turn off MCLK

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
static int ait_i2s_dai_set_dai_fmt(struct snd_soc_dai *cpu_dai,
		unsigned int fmt)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[cpu_dai->id];

	ssc_p->daifmt = fmt;
	return 0;
}

/*
 * Record SSC clock dividers for use in hw_params().
 */
static int ait_i2s_dai_set_dai_clkdiv(struct snd_soc_dai *cpu_dai,
	int div_id, int div)
{
	pr_debug("%s:  \r\n",__func__);

	return 0;
}


static int ait_i2s_dai_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params,
	struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = snd_pcm_substream_chip(substream);
	int id = dai->id;
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[id];
	struct ait_pcm_params *dma_params;
	int dir, channels, bits;
	int ret;

	AITPS_GBL pGBL = AITC_BASE_GBL;
	AITPS_AUD_I2SCTRL   pAUD    = AITC_BASE_I2S_CTRL;//AITPS_AUD   pAUD    = AITC_BASE_AUD;
	AITPS_AFE   pAFE    = AITC_BASE_AFE;

	BUG_ON(pGBL->GBL_CLK_DIS2 & GBL_CLK_AUD_CODEC_DIS);
	BUG_ON(pGBL->GBL_CLK_DIS0 & GBL_CLK_AUD_DIS);

	pr_debug("%s: para =0x%x \r\n",__func__,params_format(params));
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
	case SNDRV_PCM_FORMAT_S8:
		bits = 8;
		dma_params->pdc_xfer_size = 1;
		break;
	case SNDRV_PCM_FORMAT_S16_LE:
		bits = 16;
		dma_params->pdc_xfer_size = 2;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		bits = 24;
		dma_params->pdc_xfer_size = 4;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		bits = 32;
		dma_params->pdc_xfer_size = 4;
		break;
	default:
		printk(KERN_WARNING "ait_ssc_dai: unsupported PCM format");
		return -EINVAL;
	}

	pAUD->I2S_BIT_CLT = I2S_OUTPUT_32_BITS;
	pAUD->I2S_BIT_ALIGN_OUT = 0;	
	pAUD->I2S_BIT_ALIGN_IN = 0;
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

	switch (ssc_p->daifmt	& SND_SOC_DAIFMT_FORMAT_MASK)
	{
//		case SND_SOC_DAIFMT_I2S:
//			pAUD->I2S_OUT_DELAY_MODE = I2S_I2S_MODE;//pAUD->I2S_OUT_MODE_CTL = i2s_mode;
//			break;
		case SND_SOC_DAIFMT_RIGHT_J:
		case SND_SOC_DAIFMT_LEFT_J:			
			pAUD->I2S_OUT_DELAY_MODE = I2S_STD_MODE;
			break;
		case SND_SOC_DAIFMT_I2S:			
		default:
			printk(KERN_WARNING "atmel_ssc_dai: unsupported DAI format 0x%x\n",
				ssc_p->daifmt);
			return -EINVAL;			

	}
	pAUD->I2S_OUT_DELAY_MODE = I2S_STD_MODE;

	switch (ssc_p->daifmt
		& (SND_SOC_DAIFMT_MASTER_MASK)) {

	case SND_SOC_DAIFMT_CBS_CFS:		/* codec clk & FRM slave */
		pAUD->I2S_MODE_CTL = I2S_SLAVE;
		pAUD->I2S_CTL = I2S_ALL_DIS;//I2S_SDO_OUT_EN  | I2S_HCK_CLK_EN;	
		pAFE->AFE_MUX_MODE_CTL = AFE_MUX_AUTO_MODE;

		break;
	case SND_SOC_DAIFMT_CBM_CFS:

		pAUD->I2S_MODE_CTL = I2S_SLAVE|I2S_MCLK_OUT_EN;
		pAUD->I2S_CTL = I2S_SDO_OUT_EN  | I2S_HCK_CLK_EN;			
		/*
		 * I2S format, SSC provides BCLK and LRC clocks.
		 *
		 * The SSC transmit and receive clocks are generated
		 * from the MCK divider, and the BCLK signal
		 * is output on the SSC TK line.
		 */
	
		break;

	case SND_SOC_DAIFMT_CBM_CFM:

		pAUD->I2S_MODE_CTL = I2S_MASTER|I2S_MCLK_OUT_EN;

		
		pAUD->I2S_CTL = (I2S_SDO_OUT_EN | I2S_LRCK_OUT_EN | I2S_BCK_OUT_EN | I2S_HCK_CLK_EN);
		
	
		/*
		 * I2S format, CODEC supplies BCLK and LRC clocks.
		 *
		 * The SSC transmit clock is obtained from the BCLK signal on
		 * on the TK line, and the SSC receive clock is
		 * generated from the transmit clock.
		 *
		 *  For single channel data, one sample is transferred
		 * on the falling edge of the LRC clock.
		 * For two channel data, one sample is
		 * transferred on both edges of the LRC clock.
		 */
	
		break;

	case SND_SOC_DAIFMT_DSP_A | SND_SOC_DAIFMT_CBM_CFS:
	case SND_SOC_DAIFMT_DSP_A | SND_SOC_DAIFMT_CBM_CFM:
	default:
		printk(KERN_WARNING "atmel_ssc_dai: unsupported DAI format 0x%x\n",
			ssc_p->daifmt);
		return -EINVAL;
	}

	pAUD->I2S_DATA_IN_SEL = I2S_SDI_IN;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		pAUD->I2S_DATA_OUT_PAD_CTL = 0x1;	// Enable serial data output
	else
		pAUD->I2S_DATA_OUT_PAD_CTL = 0x0;
	


	pGBL->GBL_AUDIO_CLK_DIV = GBL_AUDIO_CLK_SRC_DPLL2|GBL_AUDIO_CLK_ON|/*GBL_AUDIO_CLK_OFF|*/(GBL_AUDIO_CLK_DIV_MASK&0x0A);

	
	pAUD->I2S_LRCK_POL = LEFT_CHANNEL_LOW;
			
	if (!ssc_p->initialized) {
		ret = request_irq(ssc_p->ssc->irq, ait_audio_i2s_interrupt, 0,
				ssc_p->name, ssc_p);
		if (ret < 0) {
			printk(KERN_WARNING
					"ait_i2s_dai_hw_params: request_irq failure\n");
			pr_debug("ait_i2s_dai_hw_params: Stoping clock\n");

			return ret;
		}

		ssc_p->initialized = 1;
	}
	printk("%s %d -\n",__func__,__LINE__);

	return 0;
}


static int ait_i2s_dai_prepare(struct snd_pcm_substream *substream,
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
static int ait_i2s_dai_suspend(struct snd_soc_dai *cpu_dai)
{
	struct ait_afe_dai_ssc_info *ssc_p;

	if (!cpu_dai->active)
		return 0;

	ssc_p = &ssc_info[cpu_dai->id];

	return 0;
}


static int ait_i2s_dai_resume(struct snd_soc_dai *cpu_dai)
{
	struct ait_afe_dai_ssc_info *ssc_p;

	if (!cpu_dai->active)
		return 0;

	ssc_p = &ssc_info[cpu_dai->id];

	return 0;
}
#else /* CONFIG_PM */
#  define ait_i2s_dai_suspend	NULL
#  define ait_i2s_dai_resume	NULL
#endif /* CONFIG_PM */

static int ait_i2s_dai_probe(struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[dai->id];
	int ret = 0;
	AITPS_AUD_I2SCTRL   pAUD    = AITC_BASE_I2S_CTRL;//AITPS_AUD   pAUD    = AITC_BASE_AUD;
	AITPS_AUD_I2S_MUX   pAUD_MUX    = AITC_BASE_I2S_MUX;//  ((AITPS_AUD_I2SCTRL)		AIT8455_OPR_P2V(AITC_BASE_PHY_I2S_CTRL))         // AUDIO    BASE Address

	snd_soc_dai_set_drvdata(dai, ssc_p);

	//ssc_p->ssc = ssc_request(dai->id);
	if (IS_ERR(ssc_p->ssc)) {
		printk(KERN_ERR "ASoC: Failed to request SSC %d\n", dai->id);
		ret = PTR_ERR(ssc_p->ssc);
	}
	clk_enable(ssc_p->ssc->clk);
	
	//+++turn on MCLK at initial driver period, ALC5627 need MCLK to R/W register
	pAUD->I2S_CTL = I2S_HCK_CLK_EN;

	{

		MMP_USHORT /*mn_value,*/ ratioM, ratioN;
		MMP_ULONG audClk;
		MMP_UBYTE clkDiv/*, cuer_div*/;
		MMP_ULONG freq;	    

		freq = 16000;

		audClk = 48000;
		clkDiv =(MMP_UBYTE)((audClk * 1000)/(256*freq));
		ratioM = (MMP_UBYTE)((audClk * 1000)/freq - clkDiv * 256);
		ratioN = 256 - ratioM;

		pAUD->I2S_MCLK_FIXMODE = 1;
		pAUD->I2S_CLK_DIV = 4;//clkDiv;		//	48M
		pAUD->I2S_RATIO_N_M =  ratioM << 8 | ratioN;
		pAUD->I2S_MCLK_CTL = I2S_256_FS;
		pAUD_MUX->I2S_MUX_MODE_CTL = AUD_MUX_AUTO;
	}
	pAUD->I2S_MODE_CTL |= I2S_MCLK_OUT_EN;

	return ret;
}

static int ait_i2s_dai_remove(struct snd_soc_dai *dai)
{
	struct ait_afe_dai_ssc_info *ssc_p = snd_soc_dai_get_drvdata(dai);
	clk_disable(ssc_p->ssc->clk);	
	clk_put(ssc_p->ssc->clk);
	
	return 0;
}

#define AIT_AUDIO_I2S_RATES SNDRV_PCM_RATE_8000_48000
#define AIT_AUDIO_I2S_FORMATS (SNDRV_PCM_FMTBIT_S16_LE )


static struct snd_soc_dai_ops ait_i2s_dai_ops = {
	.startup	= ait_i2s_dai_startup,
	.shutdown	= ait_i2s_dai_shutdown,
	.prepare	= ait_i2s_dai_prepare,
	.hw_params	= ait_i2s_dai_hw_params,
	.set_fmt	= ait_i2s_dai_set_dai_fmt,
	.set_clkdiv	= ait_i2s_dai_set_dai_clkdiv,
};

static struct snd_soc_dai_driver ait_ssc_dai[2] = {
	{
		.name = "ait-ssc-dai.0",
		.probe = ait_i2s_dai_probe,
		.remove = ait_i2s_dai_remove,
		.suspend = ait_i2s_dai_suspend,
		.resume = ait_i2s_dai_resume,
		.playback = {
			.channels_min = 1,
			.channels_max = 2,
			.rates = AIT_AUDIO_I2S_RATES,
			.formats = AIT_AUDIO_I2S_FORMATS,},
				
		.ops = &ait_i2s_dai_ops,

	},
	{
		.name = "ait-ssc-dai.1",
		.probe = ait_i2s_dai_probe,
		.remove = ait_i2s_dai_remove,
		.suspend = ait_i2s_dai_suspend,
		.resume = ait_i2s_dai_resume,
			
		.capture = {
			.channels_min = 1,
			.channels_max = 2,
			.rates = AIT_AUDIO_I2S_RATES,// SNDRV_PCM_RATE_8000|SNDRV_PCM_RATE_11025|SNDRV_PCM_RATE_16000,
			.formats = SNDRV_PCM_FMTBIT_S16_LE|SNDRV_PCM_FMTBIT_S32_LE},
		.ops = &ait_i2s_dai_ops,
	},

};

static __devinit int asoc_ssc_probe(struct platform_device *pdev)
{
	int retval = 0;

	BUG_ON(pdev->id < 0);
	BUG_ON(pdev->id >= ARRAY_SIZE(ait_ssc_dai));

	struct ait_afe_dai_ssc_info *ssc_p = &ssc_info[pdev->id];

	struct ssc_device *ssc;
	struct resource *regs;

	ssc = kzalloc(sizeof(struct ssc_device), GFP_KERNEL);
	if (!ssc) {
		dev_err(&pdev->dev, "out of memory\n");
		retval = -ENOMEM;
		goto out;
	}

	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) {
		dev_err(&pdev->dev, "no mmio resource defined\n");
		retval = -ENXIO;
		goto out_free;
	}
	ssc->clk = clk_get(&pdev->dev, "pclk");
	if (IS_ERR(ssc->clk)) {
		dev_err(&pdev->dev, "no pclk clock defined\n");
		retval = -ENXIO;
		goto out_free;
	}
	ssc->pdev = pdev;
	
	ssc->regs = ioremap(regs->start, resource_size(regs));
	if (!ssc->regs) {
		dev_err(&pdev->dev, "ioremap failed\n");
		retval = -EINVAL;
		goto out_clk;
	}

	dev_dbg(&pdev->dev, "reg addr = 0x%x\n",ssc->regs);
	/* disable all interrupts */
	clk_enable(ssc->clk);

	ssc->irq = platform_get_irq(pdev, 0);
	if (!ssc->irq) {
		dev_dbg(&pdev->dev, "could not get irq\n");
		retval = -ENXIO;
		goto out_unmap;
	}

	ssc_p->ssc = ssc;
	
	return snd_soc_register_dai(&pdev->dev, &ait_ssc_dai[pdev->id]);
	
out_unmap:
	iounmap(ssc->regs);
	clk_disable(ssc->clk);	
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

static struct platform_driver asoc_ssc_driver = {
	.driver = {
			.name = "ait-ssc-dai",
			.owner = THIS_MODULE,
	},

	.probe = asoc_ssc_probe,
	.remove = __devexit_p(asoc_ssc_remove),
};

static int __init snd_atmel_ssc_init(void)
{
	return platform_driver_register(&asoc_ssc_driver);
}
module_init(snd_atmel_ssc_init);

static void __exit snd_atmel_ssc_exit(void)
{
	platform_driver_unregister(&asoc_ssc_driver);
}
module_exit(snd_atmel_ssc_exit);

/* Module information */
MODULE_AUTHOR("Vincent Chen, vincent_chen@a-i-t.com.tw");
MODULE_DESCRIPTION("AIT I2S ASoC Interface");
MODULE_LICENSE("GPL");
