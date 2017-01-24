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

#include "../codecs/rt5627.h"
#include "ait-pcm.h"
#include "ait_ssc_dai.h"

#define MCLK_RATE 12000000

/*
 * As shipped the board does not have inputs.  However, it is relatively
 * straightforward to modify the board to hook them up so support is left
 * in the driver.
 */
#undef ENABLE_MIC_INPUT

//static struct clk *mclk;

static int ait8455evb_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
	 unsigned int freq_out;
	int ret;

	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_LEFT_J|
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


	switch (params_rate(params)) {
	case 16000:
	case 32000:
	case 48000:		
		freq_out = 16384000;
		break;

	case 44100:
		freq_out = 22579200;
		break;
		
	case 8000:
		freq_out = 8192000;
		break;
		
	case 11025:
	case 22050:		
		freq_out = 11289600;
		break;

	case 88200:
	case 96000:
	case 64000:
	default:
		return -EINVAL;
	}

	ret = snd_soc_dai_set_pll(codec_dai, RT5627_PLL_FR_MCLK, 0, MCLK_RATE, freq_out);	

	if (ret < 0) {
		printk(KERN_ERR "%s: Failed to set ALC5627 PLL: %d\n", __func__,ret);
		return ret;
	}	

		
	return 0;
}

//static 
struct snd_soc_ops ait8455evb_snd_i2s_rt5627_ops = {
	.hw_params = ait8455evb_hw_params,
};

/*
 * Logic for a rt5627 as connected on a at91sam9g20ek board.
 */
//static 
int ait8455evb_rt5627_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_dai *codec_dai = rtd->codec_dai;
	int ret;

	ret = snd_soc_dai_set_pll(codec_dai, RT5627_PLL_FR_MCLK, 0, MCLK_RATE, 16384000);
	if (ret < 0) {
		printk(KERN_ERR "Failed to set ALC5627 PLL: %d\n", ret);
		return ret;
	}

	return 0;
}

/* Module information */
MODULE_AUTHOR("Vincent Chen");
MODULE_DESCRIPTION("ALSA SoC AIT8455evb_RT5627");
//MODULE_LICENSE("GPL");
