//Vin:Todo
//GPL

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <linux/clk.h>


#include <mach/mmp_err.h>
#include <mach/mmpf_typedef.h>
#include <mach/mmp_reg_audio.h>

#include <mach/vsnv3_afe.h>


static struct workqueue_struct *vsnv3_workq = NULL;
#if 0
/* codec private data */
struct wm8971_priv {
	enum snd_soc_control_type control_type;
	unsigned int sysclk;
};


static int wm8971_add_widgets(struct snd_soc_codec *codec)
{
	struct snd_soc_dapm_context *dapm = &codec->dapm;

//	snd_soc_dapm_new_controls(dapm, wm8971_dapm_widgets,
//				  ARRAY_SIZE(wm8971_dapm_widgets));
//g	snd_soc_dapm_add_routes(dapm, audio_map, ARRAY_SIZE(audio_map));

	return 0;
}

struct _coeff_div {
	u32 mclk;
	u32 rate;
	u16 fs;
	u8 sr:5;
	u8 usb:1;
};


#endif
static int vsnv3_afe_set_dai_sysclk(struct snd_soc_dai *codec_dai,
		int clk_id, unsigned int freq, int dir)
{
#if 0
	struct snd_soc_codec *codec = codec_dai->codec;
	struct wm8971_priv *wm8971 = snd_soc_codec_get_drvdata(codec);

	switch (freq) {
	case 11289600:
	case 12000000:
	case 12288000:
	case 16934400:
	case 18432000:
		wm8971->sysclk = freq;
		return 0;
	}
	return -EINVAL;
#endif
	return 0;
}

static int vsnv3_afe_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{

	return 0;
}

static int vsnv3_afe_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params,
	struct snd_soc_dai *dai)
{

	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_codec *codec = rtd->codec;

	u16 srate = snd_soc_read(codec, AFE_REG_OFFSET(AFE_ADC_SAMPLE_RATE_CTL)) & 0xFFF0;

	switch (params_rate(params)) {
		case 8000:	srate |=SRATE_8000HZ;	break;
		case 11025: 	srate |=SRATE_11025HZ;	break;
		case 12000: 	srate |=SRATE_12000HZ;	break;
		case 16000: 	srate |=SRATE_16000HZ;	break;
		case 22050: 	srate |=SRATE_22050HZ;	break;
		case 24000: 	srate |=SRATE_24000HZ;	break;
		case 32000:	srate |=SRATE_32000HZ;	break;
		case 44100: 	srate |=SRATE_44100HZ;	break;
		case 48000: 	srate |=SRATE_48000HZ;	break;
		default:
			pr_debug("atmel_ssc_hw_params: bad rate %d\n",params_rate(params)); return -EINVAL;
	}

	snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_SAMPLE_RATE_CTL), srate);
	
	return 0;
}

static int vsnv3_afe_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	struct vsnv3_afe_data *vsnv3_afe_data= codec->control_data;


	if (mute)
	{
		vsnv3_afe_data->mute = 1;
		snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL), 0x0);	//Init Audio Digital gain: 0dB
	}
	else
	{
		pr_debug("restore digital gain 0x%.4X\r\n",vsnv3_afe_data->digital_gain);
		vsnv3_afe_data->mute = 0;
		snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL), vsnv3_afe_data->digital_gain);	//Init Audio Digital gain: 0dB
	}
	return 0;
}

static int vsnv3_afe_set_bias_level(struct snd_soc_codec *codec,
	enum snd_soc_bias_level level)
{

	u16 bias = snd_soc_read(codec, AFE_REG_OFFSET(AFE_ADC_CTL_REG4))&0xff00;//, ADC_MIC_BIAS_ON | ADC_MIC_BIAS_VOLTAGE075AVDD);
	switch (level) {
	case SND_SOC_BIAS_ON:
		/* set vmid to 50k and unmute dac */
		bias|=ADC_MIC_BIAS_ON | ADC_MIC_BIAS_VOLTAGE075AVDD;
		break;
	case SND_SOC_BIAS_PREPARE:
		bias|=ADC_MIC_BIAS_ON | ADC_MIC_BIAS_VOLTAGE090AVDD;	
		break;
	case SND_SOC_BIAS_STANDBY:
		if (codec->dapm.bias_level == SND_SOC_BIAS_OFF)
			snd_soc_cache_sync(codec);

		/* mute dac and set vmid to 500k, enable VREF */
		bias|=ADC_MIC_BIAS_ON | ADC_MIC_BIAS_VOLTAGE065AVDD;
	
		break;
	case SND_SOC_BIAS_OFF:
		bias|=ADC_MIC_BIAS_OFF;
		break;
	}

	snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_CTL_REG4), bias);
		
	codec->dapm.bias_level = level;
	return 0;
}

#define VSNV3_AFE_RATES SNDRV_PCM_RATE_8000_48000// (SNDRV_PCM_RATE_8000|SNDRV_PCM_RATE_11025|SNDRV_PCM_RATE_16000 )

#define VSNV3_AFE_FORMATS (SNDRV_PCM_FMTBIT_S16_LE)

static struct snd_soc_dai_ops wm8971_dai_ops = {
	.hw_params	= vsnv3_afe_pcm_hw_params,
	.digital_mute	= vsnv3_afe_mute,
	.set_fmt	= vsnv3_afe_set_dai_fmt,
	.set_sysclk	= vsnv3_afe_set_dai_sysclk,
};

static struct snd_soc_dai_driver vsnv3_afe_dai = {
	.name = "vsnv3-afe-hifi",
#if 0		
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM8971_RATES,
		.formats = WM8971_FORMATS,},
#endif		
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = VSNV3_AFE_RATES,
		.formats = VSNV3_AFE_FORMATS,},
	.ops = &wm8971_dai_ops,
};

static void vsnv3_afe_work(struct work_struct *work)
{

	struct snd_soc_dapm_context *dapm =
		container_of(work, struct snd_soc_dapm_context,
			     delayed_work.work);
	struct snd_soc_codec *codec = dapm->codec;
	vsnv3_afe_set_bias_level(codec, codec->dapm.bias_level);
}

static int vsnv3_afe_suspend(struct snd_soc_codec *codec, pm_message_t state)
{
	vsnv3_afe_set_bias_level(codec, SND_SOC_BIAS_OFF);
	return 0;
}

static int vsnv3_afe_resume(struct snd_soc_codec *codec)
{
	vsnv3_afe_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	if (codec->dapm.suspend_bias_level == SND_SOC_BIAS_ON) {
		codec->dapm.bias_level = SND_SOC_BIAS_ON;
		queue_delayed_work(vsnv3_workq , &codec->dapm.delayed_work,
			msecs_to_jiffies(1000));
	}

	return 0;
}

static const struct snd_kcontrol_new vsnv3_afe_controls[] = {
	SOC_DOUBLE("Mic PGA Capture Volume", AFE_REG_OFFSET(AFE_ADC_LCH_PGA_GAIN_CTL), 0, 8,31, 0),
	SOC_DOUBLE("Mic Boost", AFE_REG_OFFSET(AFE_ADC_BOOST_CTL), 2, 0, 3, 0),
	SOC_DOUBLE("AFE Digital Gain", AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL), 0, 8,87, 0),
};

static inline unsigned int vsnv3_afe_read(struct snd_soc_codec *codec,
                                                  unsigned int reg)
{
	struct vsnv3_afe_data *vsnv3_afe_data= codec->control_data;
	unsigned int value;

	if(vsnv3_afe_data->mute && reg==AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL))
		value = vsnv3_afe_data->digital_gain;
	else
		value = readw(vsnv3_afe_data->base + reg);

	pr_debug( "%s: Reg[0x%x] = 0x%x \n",__FUNCTION__,(int)(vsnv3_afe_data->base + reg),value);
	return value;
}
  
static inline int vsnv3_afe_write(struct snd_soc_codec *codec, unsigned int reg,
                         unsigned int value)
{
	struct vsnv3_afe_data *vsnv3_afe_data = codec->control_data;

	pr_debug( "%s: Reg[0x%x] = 0x%x \n",__FUNCTION__,(int)(vsnv3_afe_data->base + reg),value);

	if(reg==AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL))
		vsnv3_afe_data->digital_gain = value;

	if(!vsnv3_afe_data->mute || reg!=AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL))
		writew(value, vsnv3_afe_data->base + reg);

	return 0;
}
  
static int vsnv3_afe_probe(struct snd_soc_codec *codec)
{

	struct vsnv3_afe_data *vsnv3_afe_data = codec->dev->platform_data;
	u16 reg;

	vsnv3_afe_data->vsnv3afe.codec = codec;
	codec->control_data = vsnv3_afe_data;
		
	INIT_DELAYED_WORK(&codec->dapm.delayed_work, vsnv3_afe_work );
	vsnv3_workq  = create_workqueue("vsnv3afe");
	if (vsnv3_workq  == NULL)
		return -ENOMEM;

	//clk_enable(vsnv3_afe_data->clk);
	reg = snd_soc_read(codec, AFE_REG_OFFSET(AFE_FIFO_RST));
	snd_soc_write(codec, AFE_REG_OFFSET(AFE_FIFO_RST), reg|REST_FIFO);

	reg = snd_soc_read(codec, AFE_REG_OFFSET(AFE_ADC_BOOST_CTL));
	snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_BOOST_CTL), reg|MIC_LCH_BOOST(MIC_BOOST_40DB) |MIC_RCH_BOOST(MIC_BOOST_40DB));	
	
#if 0	
	/* charge output caps - set vmid to 5k for quick power up */
	codec->dapm.bias_level = SND_SOC_BIAS_STANDBY;
	queue_delayed_work(vsnv3_workq , &codec->dapm.delayed_work,
		msecs_to_jiffies(1000));

	/*Disable ISR*/
	snd_soc_write(codec, AFE_FIFO_CPU_INT_EN , 0);
	snd_soc_write(codec, AFE_ADC_CPU_INT_EN , 0);	

	/*Clear ISR Status*/
	snd_soc_write(codec, AFE_FIFO_CSR , 0xff);
	snd_soc_write(codec, AFE_ADC_CPU_INT_EN , 0xff);	
#endif

#if 0
	snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_LCH_PGA_GAIN_CTL), 0x1f1f);	//Init Audio and Digital gain
	snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL), 0x5757);	//Init Audio and Digital gain	
#else
	snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_LCH_PGA_GAIN_CTL), 0x1414);	//Init Audio and Digital gain
	snd_soc_write(codec, AFE_REG_OFFSET(AFE_ADC_LCH_DIGITAL_VOL), 0x4C4C);	//Init Audio and Digital gain	
#endif

	//clk_disable(vsnv3_afe_data->clk);

	/* Set controls */
	snd_soc_add_controls(codec, vsnv3_afe_controls,
										ARRAY_SIZE(vsnv3_afe_controls));

	/* Off, with power on */
	vsnv3_afe_set_bias_level(codec, SND_SOC_BIAS_STANDBY);
	return 0;
}


/* power down chip */
static int vsnv3_afe_remove(struct snd_soc_codec *codec)
{
	vsnv3_afe_set_bias_level(codec, SND_SOC_BIAS_OFF);

	if (vsnv3_workq)
		destroy_workqueue(vsnv3_workq);
	return 0;
}


  
static struct snd_soc_codec_driver soc_codec_dev_afe = {
	 .read = vsnv3_afe_read,
	.write = vsnv3_afe_write,
	.set_bias_level = vsnv3_afe_set_bias_level,
	.probe = vsnv3_afe_probe,
 	.remove =vsnv3_afe_remove,
 	.resume = vsnv3_afe_resume,
 	.suspend = vsnv3_afe_suspend
 
};

static int vsnv3_afe_platform_probe(struct platform_device *pdev)
{
	struct resource *res, *mem;
	int ret;

	struct vsnv3_afe_data *vsnv3_afe_data; 
	pr_debug( "%s: %d \n",__FUNCTION__,__LINE__);

	vsnv3_afe_data = kzalloc(sizeof(struct vsnv3_afe_data), GFP_KERNEL);
	if (!vsnv3_afe_data) {
		dev_err(&pdev->dev,
			    "could not allocate memory for private data\n");
		return -ENOMEM;
	}

	vsnv3_afe_data->clk = clk_get(&pdev->dev, "afe_clk");
	if (IS_ERR(vsnv3_afe_data->clk)) {
		dev_err(&pdev->dev,
			    "could not get the clock for voice codec\n");
		ret = -ENODEV;
		goto fail1;
	}
	clk_enable(vsnv3_afe_data->clk);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "no mem resource\n");
		ret = -ENODEV;
		goto fail2;
	}

	vsnv3_afe_data->pbase = res->start;
	vsnv3_afe_data->base_size = resource_size(res);

	mem = request_mem_region(vsnv3_afe_data->pbase, vsnv3_afe_data->base_size,
				 pdev->name);
	if (!mem) {
		dev_err(&pdev->dev, "VCIF region already claimed\n");
		ret = -EBUSY;
		goto fail2;
	}

	vsnv3_afe_data->base = ioremap(vsnv3_afe_data->pbase, vsnv3_afe_data->base_size);
	if (!vsnv3_afe_data->base) {
		dev_err(&pdev->dev, "can't ioremap mem resource.\n");
		ret = -ENOMEM;
		goto fail3;
	}

	vsnv3_afe_data->mute = 0;
	vsnv3_afe_data->digital_gain = 0x3f3f;

#if 0
	res = platform_get_resource(pdev, IORESOURCE_DMA, 0);
	if (!res) {
		dev_err(&pdev->dev, "no DMA resource\n");
		ret = -ENXIO;
		goto fail4;
	}
#endif	

	pdev->dev.platform_data = (void*)vsnv3_afe_data;
	return snd_soc_register_codec(&pdev->dev,
			&soc_codec_dev_afe, &vsnv3_afe_dai, 1);


//fail4:
//	iounmap(vsnv3_afe_data->base);
fail3:
	release_mem_region(vsnv3_afe_data->pbase, vsnv3_afe_data->base_size);
fail2:
	clk_disable(vsnv3_afe_data->clk);
	clk_put(vsnv3_afe_data->clk);
	vsnv3_afe_data->clk = NULL;
fail1:
	kfree(vsnv3_afe_data);
	return ret;	
}

static int vsnv3_afe_platform_remove(struct platform_device *pdev)
{

	struct vsnv3_afe_data *vsnv3_afe_data = platform_get_drvdata(pdev);
	printk("%s\r\n",__FUNCTION__);
	snd_soc_unregister_codec(&pdev->dev);
	
	//mfd_remove_devices(&pdev->dev);

	iounmap(vsnv3_afe_data->base);
	release_mem_region(vsnv3_afe_data->pbase, vsnv3_afe_data->base_size);

	clk_disable(vsnv3_afe_data->clk);
	clk_put(vsnv3_afe_data->clk);
	vsnv3_afe_data->clk = NULL;

	kfree(vsnv3_afe_data);

	return 0;
}


static struct platform_driver vsnv3_afe_driver = {
	.driver = {
		.name = "vsnv3-afe-codec",
		.owner = THIS_MODULE,
	},
	.probe = vsnv3_afe_platform_probe,
	.remove =  vsnv3_afe_platform_remove
};

module_platform_driver(vsnv3_afe_driver)
	
MODULE_DESCRIPTION("ASoC WM8971 driver");
MODULE_AUTHOR("");
MODULE_LICENSE("GPL");
