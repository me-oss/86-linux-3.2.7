/*
 * sam9g20_wm8731  --  SoC audio for AT91SAM9G20-based
 * 			ATMEL AT91SAM9G20ek board.
 *
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2008 Atmel
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

//#include "../codecs/wm8731.h"
#include "ait-pcm.h"
#include "ait_afe_dai.h"

#define MCLK_RATE 12000000

/*
 * As shipped the board does not have inputs.  However, it is relatively
 * straightforward to modify the board to hook them up so support is left
 * in the driver.
 */
#undef ENABLE_MIC_INPUT

static struct clk *mclk;

static int ait8455evb_afe_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	int ret;

	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBM_CFM);
	if (ret < 0)
	{
			printk(KERN_ERR "%s: snd_soc_dai_set_fmt(%s):%d\n",__FUNCTION__,cpu_dai->name,ret);
		return ret;
	}
	else
		printk(KERN_DEBUG "%s: snd_soc_dai_set_fmt(%s) OK\n",__FUNCTION__,cpu_dai->name);
	return 0;
}

static struct snd_soc_ops ait8455evb_afe_ops = {
	.hw_params = ait8455evb_afe_hw_params,
};

static int ait8455evb_set_bias_level(struct snd_soc_card *card,
					struct snd_soc_dapm_context *dapm,
					enum snd_soc_bias_level level)
{
	static int mclk_on;
	int ret = 0;
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
//Vin: add
	mclk_on = 1;
	return ret;
}
#if 0
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
#endif
static int ait8455_audio_afe_init(struct snd_soc_pcm_runtime *rtd)
{

	struct snd_soc_codec *codec = rtd->codec;
//	struct snd_soc_dai *codec_dai = rtd->codec_dai;
//	struct snd_soc_dapm_context *dapm = &codec->dapm;
//	int ret;

	printk(KERN_DEBUG
			"ait8455_audio_afe "
			": ait8455_audio_afe_init() called\n");

	return 0;
}

static struct snd_soc_dai_link ait8455evb_dai= {
	.name = "AIT8455-AFE",

	.stream_name = "AIT8455 AFE",

	.cpu_dai_name = "ait-afe-dai",

	.init = ait8455_audio_afe_init,
	.platform_name = "vsnv3afe-pcm-audio",
	
	.codec_name = "vsnv3-afe-codec",
	.codec_dai_name = "vsnv3-afe-hifi",
	
	.ops = &ait8455evb_afe_ops,
};

static struct snd_soc_card snd_soc_vsnv3_afe = {
	.name = "AIT8455-AFE",
	.dai_link = &ait8455evb_dai,
	.num_links = 1,
	.set_bias_level = ait8455evb_set_bias_level,
};

static struct platform_device *vsnv3_afe_snd_device;

#include <mach/mmp_register.h>
static struct resource afe_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_AFE,
		.end		= AITC_BASE_PHY_AFE+SZ_128 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_AFE_FIFO,
		.end		= AITVSNV3_ID_AFE_FIFO,
		.flags	= IORESOURCE_IRQ,
	},
	
};
#if 0
int ait_afe_set_audio(int reserve)
{
//	struct ssc_device *ssc;
	static struct platform_device *vsnv3_dma_pdev;
	struct platform_device *ssc_pdev;
	int ret;


	vsnv3_dma_pdev = platform_device_alloc(ait8455evb_dai.platform_name, -1);
	if (!vsnv3_dma_pdev)
		return -ENOMEM;

	ret = platform_device_add(vsnv3_dma_pdev);
	if (ret < 0) {
		platform_device_put(vsnv3_dma_pdev);
		vsnv3_dma_pdev = NULL;
		return ret;
	}

	ssc_pdev = platform_device_alloc(ait8455evb_dai.cpu_dai_name, -1);
	if (!ssc_pdev)
		return -ENOMEM;

	ssc_pdev->resource = afe_resources;
	ssc_pdev->num_resources	= ARRAY_SIZE(afe_resources);

	ret = platform_device_add(ssc_pdev);
	if (ret < 0)
	{
		pr_err( "%s: %d ret = %d\n",__FUNCTION__,__LINE__,ret);
		platform_device_put(ssc_pdev);
	}
	return ret;
}

#endif
static int __init ait8455evb_afe_init(void)
{
#if 0

	int ret;
	ret = ait_afe_set_audio(-1);
	if (ret != 0) {
		pr_err("Failed to set SSC 0 for audio: %d\n", ret);
		return ret;
	}
#endif	
	static struct platform_device *vsnv3_dma_pdev;
	struct platform_device *ssc_pdev;
	int ret;

/* Add Platform Device*/
	vsnv3_dma_pdev = platform_device_alloc(ait8455evb_dai.platform_name, -1);
	if (!vsnv3_dma_pdev)
		return -ENOMEM;

	ret = platform_device_add(vsnv3_dma_pdev);
	if (ret < 0) {
		platform_device_put(vsnv3_dma_pdev);
		vsnv3_dma_pdev = NULL;
		return ret;
	}

/* Add CPU DAI device*/
	ssc_pdev = platform_device_alloc(ait8455evb_dai.cpu_dai_name, -1);
	if (!ssc_pdev)
		return -ENOMEM;

	ssc_pdev->resource = afe_resources;
	ssc_pdev->num_resources	= ARRAY_SIZE(afe_resources);

	ret = platform_device_add(ssc_pdev);
	if (ret < 0)
	{
		pr_err( "%s: %d ret = %d\n",__FUNCTION__,__LINE__,ret);
		platform_device_put(ssc_pdev);
	}

	
#if 0
	/*
	 * Codec MCLK is supplied by PCK0 - set it up.
	 */
	mclk = clk_get(NULL, "pck0");
	if (IS_ERR(mclk)) {
		printk(KERN_ERR "ASoC: Failed to get MCLK\n");
		ret = PTR_ERR(mclk);
		goto err;
	}

	pllb = clk_get(NULL, "pllb");
	if (IS_ERR(pllb)) {
		printk(KERN_ERR "ASoC: Failed to get PLLB\n");
		ret = PTR_ERR(pllb);
		goto err_mclk;
	}
	ret = clk_set_parent(mclk, pllb);
	clk_put(pllb);
	if (ret != 0) {
		printk(KERN_ERR "ASoC: Failed to set MCLK parent\n");
		goto err_mclk;
	}

	clk_set_rate(mclk, MCLK_RATE);
#endif
/* Add soc-audio device it will creat a sound card */

	vsnv3_afe_snd_device = platform_device_alloc("soc-audio", 1);
	if (!vsnv3_afe_snd_device) {
		printk(KERN_ERR "ASoC: Platform device allocation failed\n");
		ret = -ENOMEM;
		goto err_mclk;
	}

	platform_set_drvdata(vsnv3_afe_snd_device,
			&snd_soc_vsnv3_afe);

	ret = platform_device_add(vsnv3_afe_snd_device);
	if (ret) {
		printk(KERN_ERR "ASoC: Platform device allocation failed\n");
		goto err_device_add;
	}

	return ret;

err_device_add:
	platform_device_put(vsnv3_afe_snd_device);
err_mclk:
	clk_put(mclk);
	mclk = NULL;
//err:
	return ret;
}

static void __exit ait8455evb_afe_exit(void)
{
	platform_device_unregister(vsnv3_afe_snd_device);
	vsnv3_afe_snd_device = NULL;
	clk_put(mclk);
	mclk = NULL;
}

module_init(ait8455evb_afe_init);
module_exit(ait8455evb_afe_exit);

/* Module information */
MODULE_AUTHOR("Vincent");
MODULE_DESCRIPTION("ALSA SoC AIT8455evb_WM8973");
//MODULE_LICENSE("GPL");
