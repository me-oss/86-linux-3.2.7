/*
 * ait8455evb_i2s_wm8731  --  SoC audio for AT8455-based
 * 			ATMEL AT91SAM9G20ek board.
 *
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2008 Atmel
 *  Copyright (C) 2014 Alpha Image Technology Corp.
 *
 * Authors: Sedji Gaouaou <sedji.gaouaou@atmel.com>
 *
 * Based on ati_b1_wm8731.c by:
 * Frank Mandarino <fmandarino@endrelia.com>
 * Copyright 2006 Endrelia Technologies Inc.
 * Based on corgi.c by:
 * Copyright 2005 Wolfson Microelectronics PLC.
 * Copyright 2005 Openedhand Ltd.
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
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>

#include <linux/atmel-ssc.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <asm/mach-types.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <mach/mmp_reg_audio.h>

#include "ait-pcm.h"
#include "ait_ssc_dai.h"

//#define MCLK_RATE 12288000	//16K
#define MCLK_RATE 12000000

/*
 * As shipped the board does not have inputs.  However, it is relatively
 * straightforward to modify the board to hook them up so support is left
 * in the driver.
 */
#undef ENABLE_MIC_INPUT

static int ait8455evb_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret;

	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM);//SND_SOC_DAIFMT_CBS_CFS);//);

	if (ret < 0)
	{
		printk(KERN_ERR "snd_soc_dai_set_fmt(%s):%d\n",codec_dai->name,ret);
		return ret;
	}
	else
	{
		printk(KERN_DEBUG "%s:snd_soc_dai_set_fmt(%s) OK\n",__FUNCTION__,codec_dai->name);
	}
	
	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_LEFT_J  |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFS);//);
	if (ret < 0)
	{
			printk(KERN_ERR "%s: snd_soc_dai_set_fmt(%s):%d\n",__FUNCTION__,cpu_dai->name,ret);
		return ret;
	}
	else
		printk(KERN_DEBUG "%s: snd_soc_dai_set_fmt(%s) OK\n",__FUNCTION__,cpu_dai->name);
	return 0;
}

static struct snd_soc_ops ait8455evb_snd_i2s_ops = {
	.hw_params = ait8455evb_hw_params,
};

static int ait8455evb_i2s_set_bias_level(struct snd_soc_card *card,
					struct snd_soc_dapm_context *dapm,
					enum snd_soc_bias_level level)
{
	static int mclk_on;
	int ret = 0;
	pr_debug("level = %d\n",level);
#if 0
	switch (level) {
	case SND_SOC_BIAS_ON:
	case SND_SOC_BIAS_PREPARE:
		if (!mclk_on)
			ret = clk_enable(mclk);
		if (ret == 0)
			mclk_on = 1;
		break;

	case SND_SOC_BIAS_OFF:
	case SND_SOC_BIAS_STANDBY:
		if (mclk_on)
			clk_disable(mclk);
		mclk_on = 0;
		break;
	}
#endif
	mclk_on = 1;
	return ret;
}

static const struct snd_soc_dapm_widget at91sam9g20ek_dapm_widgets[] = {
	SND_SOC_DAPM_MIC("Int Mic", NULL),
	SND_SOC_DAPM_SPK("Ext Spk", NULL),
};

static const struct snd_soc_dapm_route intercon[] = {

	/* speaker connected to LHPOUT */
	{"Ext Spk", NULL, "LHPOUT"},

	/* mic is connected to Mic Jack, with WM8731 Mic Bias */
	{"MICIN", NULL, "Mic Bias"},
	{"Mic Bias", NULL, "Int Mic"},
};

/*
 * Logic for a wm8731 as connected on a at91sam9g20ek board.
 */
static int ait8455evb_wm8973_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_codec *codec = rtd->codec;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dapm_context *dapm = &codec->dapm;
	int ret;

	printk(KERN_DEBUG
			"ait8455evb_wm8973 "
			": ait8455evb_wm8973_init() called\n");

	ret = snd_soc_dai_set_sysclk(codec_dai, 0/*WM8731_SYSCLK_MCLK*/,
		MCLK_RATE, SND_SOC_CLOCK_IN);
	if (ret < 0) {
		printk(KERN_ERR "Failed to set WM8731 SYSCLK: %d\n", ret);
		return ret;
	}

	/* Add specific widgets */
	snd_soc_dapm_new_controls(dapm, at91sam9g20ek_dapm_widgets,
				  ARRAY_SIZE(at91sam9g20ek_dapm_widgets));
	/* Set up specific audio path interconnects */
	snd_soc_dapm_add_routes(dapm, intercon, ARRAY_SIZE(intercon));

	/* not connected */
	snd_soc_dapm_nc_pin(dapm, "RLINEIN");
	snd_soc_dapm_nc_pin(dapm, "LLINEIN");

#ifdef ENABLE_MIC_INPUT
	snd_soc_dapm_enable_pin(dapm, "Int Mic");
#else
	snd_soc_dapm_nc_pin(dapm, "Int Mic");
#endif

	/* always connected */
	snd_soc_dapm_enable_pin(dapm, "Ext Spk");

	return 0;
}

extern int ait8455evb_rt5627_init(struct snd_soc_pcm_runtime *rtd);
extern 	struct snd_soc_ops ait8455evb_snd_i2s_rt5627_ops;

static struct snd_soc_dai_link ait8455evb_i2s_dai_link= {
	.name = "AIT8455 I2S WM8973",

	.stream_name = "WM8973 PCM",

	.cpu_dai_name = "ait-ssc-dai.0",

	.init = ait8455evb_wm8973_init,
	.platform_name = "ait-pcm-audio",
	
	.codec_name = "wm8973-codec.1-001a",
	.codec_dai_name = "wm8973-hifi",
	
	.ops = &ait8455evb_snd_i2s_ops,
	
};

static struct snd_soc_dai_link ait8455evb_i2s_rt5627_dai_link= {
	.name = "RT5627",

	.stream_name = "RT5627 PCM",

	.cpu_dai_name = "ait-ssc-dai.0",

    .init = ait8455evb_rt5627_init,

	.platform_name = "ait-pcm-audio",
	
	.codec_name = "rt5627-codec.1-0019",

	.codec_dai_name = "rt5627_hifi",
	
	.ops = &ait8455evb_snd_i2s_rt5627_ops ,//&ait8455evb_snd_i2s_ops,
};

static struct snd_soc_dai_link ait8455evb_i2s_rt5628_dai_link= {
	.name = "RT5628",

	.stream_name = "RT5628 PCM",

	.cpu_dai_name = "ait-ssc-dai.0",

	.init = ait8455evb_rt5627_init,

	.platform_name = "ait-pcm-audio",
	
	.codec_name = "rt5627-codec.1-0018",

	.codec_dai_name = "rt5627_hifi",
	
	.ops = &ait8455evb_snd_i2s_rt5627_ops ,//&ait8455evb_snd_i2s_ops,
};

int ait8455evb_tc358743_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret;
#if 0
	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM);//SND_SOC_DAIFMT_CBS_CFS);//);

	if (ret < 0)
	{
		printk(KERN_ERR "snd_soc_dai_set_fmt(%s):%d\n",codec_dai->name,ret);
		return ret;
	}
	else
	{
		printk(KERN_DEBUG "%s:snd_soc_dai_set_fmt(%s) OK\n",__FUNCTION__,codec_dai->name);
	}
#endif	
	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_LEFT_J  |
		SND_SOC_DAIFMT_NB_NF |SND_SOC_DAIFMT_CBS_CFS);// SND_SOC_DAIFMT_CBM_CFS);//);
	if (ret < 0)
	{
			printk(KERN_ERR "%s: snd_soc_dai_set_fmt(%s):%d\n",__FUNCTION__,cpu_dai->name,ret);
		return ret;
	}
	else
		printk(KERN_DEBUG "%s: snd_soc_dai_set_fmt(%s) OK\n",__FUNCTION__,cpu_dai->name);
	return 0;
	
	
	return 0;
}


//static 
struct snd_soc_ops ait8455evb_snd_i2s_tc358743_ops = {
	.hw_params = ait8455evb_tc358743_hw_params,
};


int ait8455evb_tc358743_init(struct snd_soc_pcm_runtime *rtd)
{

	return 0;
}

static struct snd_soc_dai_link ait8455evb_i2s_tc358743_dai_link= {
	.name = "TC358743XBG H2C (Audio)",

	.stream_name = "TC358743XBG PCM IN",

	.cpu_dai_name = "ait-ssc-dai.1",

	.init = ait8455evb_tc358743_init,

	.platform_name = "ait-pcm-audio",
	
	.codec_name = "tc358743-codec",//tc358743.1-0018",

	.codec_dai_name = "tc358743-hifi",
	
	.ops = &ait8455evb_snd_i2s_tc358743_ops ,//&ait8455evb_snd_i2s_ops,
};

static struct snd_soc_card snd_card_ait8455evb_i2s[] = {
	{
		.name = "AIT8455-I2S-WM8973",
		.dai_link = &ait8455evb_i2s_dai_link,
		.num_links = 1,
		.set_bias_level = ait8455evb_i2s_set_bias_level,
	},
	{
		.name = "AIT8455-I2S-RT5628",
		.dai_link = &ait8455evb_i2s_rt5628_dai_link,
		.num_links = 1,
		.set_bias_level = ait8455evb_i2s_set_bias_level,
	},
	{
		.name = "AIT8455-I2S-RT5627",
		.dai_link = &ait8455evb_i2s_rt5627_dai_link,
		.num_links = 1,
		.set_bias_level = ait8455evb_i2s_set_bias_level,
	},
	{
		.name = "AIT8455-I2S-TC358743XBG(Audio)",
		.dai_link = &ait8455evb_i2s_tc358743_dai_link,
		.num_links = 1,
		.set_bias_level = ait8455evb_i2s_set_bias_level,
	}

	
		
};


static struct platform_device *ait8455_i2s_snd_device[ARRAY_SIZE(snd_card_ait8455evb_i2s)];


static struct resource ait_i2s_resources[] __initdata= {
	[0] = {
		.start	= AITC_BASE_PHY_AUD,
		.end		= AITC_BASE_PHY_AUD+SZ_256 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_I2S_FIFO,
		.end		= AITVSNV3_ID_I2S_FIFO,
		.flags	= IORESOURCE_IRQ,
	},
	
};


static int __init ait8455evb_i2s_snd_card_init(void)
{
	int ret;
	int i;	

	static struct platform_device *dma_pdev;
	static struct platform_device *ssc_pdev;


	/* Allocate a dummy device for DMA if we don't have one already */
	dma_pdev = platform_device_alloc("ait-pcm-audio", -1);
	if (!dma_pdev)
		return -ENOMEM;

	ret = platform_device_add(dma_pdev);
	if (ret < 0) {
		platform_device_put(dma_pdev);
		dma_pdev = NULL;
		return ret;
	}

	ssc_pdev = platform_device_alloc("ait-ssc-dai", 0);
	if (!ssc_pdev)
		return -ENOMEM;

	ret = platform_device_add_resources(ssc_pdev,
				  ait_i2s_resources, ARRAY_SIZE(ait_i2s_resources));
	if (ret < 0)
	{
		pr_err( "%s: %d ret = %d\n",__FUNCTION__,__LINE__,ret);
		return ret;
	}
	
	ret = platform_device_add(ssc_pdev);
	if (ret < 0)
	{
		pr_err( "%s: %d ret = %d\n",__FUNCTION__,__LINE__,ret);
		platform_device_put(ssc_pdev);
	}

	ssc_pdev = platform_device_alloc("ait-ssc-dai", 1);
	if (!ssc_pdev)
		return -ENOMEM;

	ret = platform_device_add_resources(ssc_pdev,
				  ait_i2s_resources, ARRAY_SIZE(ait_i2s_resources));
	if (ret < 0)
	{
		pr_err( "%s: %d ret = %d\n",__FUNCTION__,__LINE__,ret);
		return ret;
	}
	
	ret = platform_device_add(ssc_pdev);
	if (ret < 0)
	{
		pr_err( "%s: %d ret = %d\n",__FUNCTION__,__LINE__,ret);
		platform_device_put(ssc_pdev);
	}

		
	for(i=0;i<ARRAY_SIZE(snd_card_ait8455evb_i2s);++i)
	{
		ait8455_i2s_snd_device[i] = platform_device_alloc("soc-audio", 10+i);
		if (!ait8455_i2s_snd_device[i]) {
			printk(KERN_ERR "ASoC: Platform device allocation failed\n");
			ret = -ENOMEM;
			goto err_mclk;
		}

		platform_set_drvdata(ait8455_i2s_snd_device[i],
				&snd_card_ait8455evb_i2s[i]);
	

	
		ret = platform_device_add(ait8455_i2s_snd_device[i]);
		if (ret) {
			printk(KERN_ERR "ASoC: Platform device allocation failed\n");
			platform_device_put(ait8455_i2s_snd_device[i]);
			
			goto err_device_add;
		}
	}

	return ret;
err_device_add:

err_mclk:
	return ret;
	
}

static void __exit ait8455evb_i2s_snd_card_exit(void)
{
	int i;
	for(i=0;i<ARRAY_SIZE(snd_card_ait8455evb_i2s);++i)
	{
		if(ait8455_i2s_snd_device[i])
		{
			platform_device_unregister(ait8455_i2s_snd_device[i]);
			ait8455_i2s_snd_device[i] = NULL;
		}
	}
}

module_init(ait8455evb_i2s_snd_card_init);
module_exit(ait8455evb_i2s_snd_card_exit);

/* Module information */
MODULE_AUTHOR("Vincent Chen  vincent_chen@a-i-t.com.tw");
MODULE_DESCRIPTION("ALSA SoC AIT8455evb_WM8973");
MODULE_LICENSE("GPL");
