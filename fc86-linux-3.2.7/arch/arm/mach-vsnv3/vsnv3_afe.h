#ifndef __VSNV3_AFE_
#define __VSNV3_AFE_

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mfd/core.h>

//#include <mach/edma.h>

/*
 * Register values.
 */
 #if 0
#define AFE_FIFO_CPU_INT_EN 	0x00
#define AFE_FIFO_HOST_INT_EN	0x01
		/*-DEFINE-----------------------------------------------------*/
		#define AFE_INT_FIFO_REACH_UNWR_TH			0x08
		#define AFE_INT_FIFO_REACH_UNRD_TH			0x04
		#define AFE_INT_FIFO_FULL					0x02
		#define AFE_INT_FIFO_EMPTY					0x01
		/*------------------------------------------------------------*/

#define AFE_FIFO_CSR	0x04
		/*-DEFINE-----------------------------------------------------*/
		#define FIFO_UNWR_TH						0x08
		#define FIFO_UNRD_TH						0x04
		#define FIFO_FULL							0x02
		#define FIFO_EMPTY							0x01
		/*------------------------------------------------------------*/

#define AFE_FIFO_RST	0x08
		/*-DEFINE-----------------------------------------------------*/
		#define REST_FIFO 1

#define AFE_FIFO_DATA	0x0C
#define AFE_FIFO_RD_TH	0x10
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/

#define AFE_FIFO_WR_TH	0x14
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/

#define AFE_FIFO_UNRD_CNT	0x18
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/

#define AFE_FIFO_UNWR_CNT	0x1C
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
#define AFE_L_CHANNEL_DATA	0x20
		/*-DEFINE-----------------------------------------------------*/

#define AFE_R_CHANNEL_DATA	0x24
		/*-DEFINE-----------------------------------------------------*/

#define AFE_ADC_CPU_INT_EN	0x28
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_ADC_INT_EN						0x01

#define AFE_HOST_INT_EN	0x29
		/*-DEFINE-----------------------------------------------------*/

#define AFE_INT_CSR	0x2A
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
#define AFE_FIFO_RM	0x2C
		/*-DEFINE-----------------------------------------------------*/
		#define HOST_AFE_ADC_INT_EN					0x01
		/*------------------------------------------------------------*/
#define AFE_ADC_COEF_RM	0x2D
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
#define AFE_GBL_PWR_CTL	0x50
		/*-DEFINE-----------------------------------------------------*/
		#define BYPASS_OP							0x40
		#define PWR_UP_ANALOG						0x20
		#define PWR_UP_VREF							0x10
		#define PWR_UP_ADC_DIGITAL_FILTER			0x01
		/*------------------------------------------------------------*/
#define AFE_CLK_CTL	0x51
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_CODEC_NORMAL_MODE				0x80
		#define ADC_CLK_MODE_USB					0x02
		#define ADC_CLK_INVERT						0x01
		/*------------------------------------------------------------*/

#define AFE_MUX_MODE_CTL	0x54
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_DATA_BIT_20						0x02	//otherwise 16bits
		#define AFE_MUX_AUTO_MODE					0x01
		#define AFE_MUX_CPU_MODE					0x00
		/*------------------------------------------------------------*/
#define SPECIAL_AUD_CODEC_PATH	0x56
		/*-DEFINE-----------------------------------------------------*/
		#define EXT_AUD_CODEC_EN					0x01
		/*------------------------------------------------------------*/

#define AFE_GBL_BIAS_ADJ	0x58
	#define GBL_BIAS_50							0x00
	#define GBL_BIAS_62_5						0x01
	#define GBL_BIAS_75							0x02
	#define GBL_BIAS_87_5						0x03
	#define GBL_BIAS_100						0x04
	#define GBL_BIAS_112_5						0x05
	#define GBL_BIAS_125						0x06
	#define GBL_BIAS_137_5						0x07
		/*------------------------------------------------------------*/

#define AFE_ADC_PWR_CTL	0x60
		/*-DEFINE-----------------------------------------------------*/
	#define ADC_SDM_RCH_POWER_EN				0x08
	#define ADC_SDM_LCH_POWER_EN				0x04
	#define ADC_PGA_RCH_POWER_EN				0x02
	#define ADC_PGA_LCH_POWER_EN				0x01
		/*------------------------------------------------------------*/
#define AFE_ADC_SAMPLE_RATE_CTL	0x61
		/*-DEFINE-----------------------------------------------------*/
	#define ADC_SRATE_MASK                  0x0F
	#define SRATE_48000HZ                   0x0A
        #define SRATE_44100HZ                   0x09
        #define SRATE_32000HZ                   0x08
        #define SRATE_24000HZ                   0x06
        #define SRATE_22050HZ                   0x05
        #define SRATE_16000HZ                   0x04
        #define SRATE_12000HZ                   0x02
        #define SRATE_11025HZ                   0x01
        #define SRATE_8000HZ                    0x00

#define AFE_ADC_INPUT_SEL	0x62
		/*-DEFINE-----------------------------------------------------*/
        #define ADC_AUX_IN	    	               0x10
        #define ADC_MIC_IN		                   0x04
        #define ADC_MIC_DIFF2SINGLE				   0x02
        #define ADC_MIC_DIFF					   0x00
        #define ADC_CTL_MASK					   ~(ADC_AUX_IN|ADC_MIC_IN)

#define AFE_ADC_BOOST_CTL;										// 0x63
		/*-DEFINE-----------------------------------------------------*/
		#define MIC_NO_BOOST						0x00
        #define MIC_BOOST_20DB						0x01
        #define MIC_BOOST_30DB						0x02
        #define MIC_BOOST_40DB						0x03
        #define MIC_LCH_BOOST(_a)					(_a<<2)
        #define MIC_RCH_BOOST(_a)					(_a)
		/*------------------------------------------------------------*/
#define AFE_ADC_HPF_CTL;										// 0x64
		/*-DEFINE-----------------------------------------------------*/
		#define ADC_HPF_EN							0x01

#define AFE_ADC_LCH_PGA_GAIN_CTL	0x68
		/*-DEFINE-----------------------------------------------------*/

#define AFE_ADC_RCH_PGA_GAIN_CTL	0x69
		/*-DEFINE-----------------------------------------------------*/

#define AFE_ADC_LCH_DIGITAL_VOL	0x6A
		/*-DEFINE-----------------------------------------------------*/

#define AFE_ADC_RCH_DIGITAL_VOL	0x6B
		/*-DEFINE-----------------------------------------------------*/

#define FIX_AFE_ADC_OVERFLOW	0x6C
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
#define AFE_ADC_DIGITAL_GAIN_MUTE_STEP	 0x6D
		/*-DEFINE-----------------------------------------------------*/

#define AFE_ADC_DATA_BIT_WIDTH	0x6E
		/*-DEFINE-----------------------------------------------------*/
#define AFE_ADC_BIAS_ADJ	0x70
		#define ANA_ADC_DISC_OP_MASK            0x0C
        #define ANA_ADC_DISC_OP(_a)             (_a<<2)
        #define ANA_ADC_CONT_OP_MASk            0x03
        #define ANA_ADC_CONT_OP(_a)             (_a)

#define AFE_ADC_CTL_REG1	0x71
		/*-DEFINE-----------------------------------------------------*/
		#define AFE_ZERO_CROSS_DET				0x10

#define AFE_ADC_CTL_REG2	0x72
		/*-DEFINE-----------------------------------------------------*/

#define AFE_ADC_CTL_REG3	0x73
		/*-DEFINE-----------------------------------------------------*/
		
#define AFE_ADC_CTL_REG4	0x74
		/*-DEFINE-----------------------------------------------------*/
		#define ADC_MIC_BIAS_ON                 0x04
		#define ADC_MIC_BIAS_OFF                 0x00
		#define ADC_MIC_BIAS_VOLTAGE0_90AVDD     0x03
		#define ADC_MIC_BIAS_VOLTAGE0_80AVDD     0x02
		#define ADC_MIC_BIAS_VOLTAGE0_75AVDD     0x01		
		#define ADC_MIC_BIAS_VOLTAGE0_65AVDD     0x00
#endif		
#define DAVINCI_VC_PID			0x00
#define DAVINCI_VC_CTRL			0x04
#define DAVINCI_VC_INTEN		0x08
#define DAVINCI_VC_INTSTATUS		0x0c
#define DAVINCI_VC_INTCLR		0x10
#define DAVINCI_VC_EMUL_CTRL		0x14
#define DAVINCI_VC_RFIFO		0x20
#define DAVINCI_VC_WFIFO		0x24
#define DAVINCI_VC_FIFOSTAT		0x28
#define DAVINCI_VC_TST_CTRL		0x2C
#define DAVINCI_VC_REG05		0x94
#define DAVINCI_VC_REG09		0xA4
#define DAVINCI_VC_REG12		0xB0

/* DAVINCI_VC_CTRL bit fields */
#define DAVINCI_VC_CTRL_MASK		0x5500
#define DAVINCI_VC_CTRL_RSTADC		BIT(0)
#define DAVINCI_VC_CTRL_RSTDAC		BIT(1)
#define DAVINCI_VC_CTRL_RD_BITS_8	BIT(4)
#define DAVINCI_VC_CTRL_RD_UNSIGNED	BIT(5)
#define DAVINCI_VC_CTRL_WD_BITS_8	BIT(6)
#define DAVINCI_VC_CTRL_WD_UNSIGNED	BIT(7)
#define DAVINCI_VC_CTRL_RFIFOEN		BIT(8)
#define DAVINCI_VC_CTRL_RFIFOCL		BIT(9)
#define DAVINCI_VC_CTRL_RFIFOMD_WORD_1	BIT(10)
#define DAVINCI_VC_CTRL_WFIFOEN		BIT(12)
#define DAVINCI_VC_CTRL_WFIFOCL		BIT(13)
#define DAVINCI_VC_CTRL_WFIFOMD_WORD_1	BIT(14)

/* DAVINCI_VC_INT bit fields */
#define DAVINCI_VC_INT_MASK		0x3F
#define DAVINCI_VC_INT_RDRDY_MASK	BIT(0)
#define DAVINCI_VC_INT_RERROVF_MASK	BIT(1)
#define DAVINCI_VC_INT_RERRUDR_MASK	BIT(2)
#define DAVINCI_VC_INT_WDREQ_MASK	BIT(3)
#define DAVINCI_VC_INT_WERROVF_MASKBIT	BIT(4)
#define DAVINCI_VC_INT_WERRUDR_MASK	BIT(5)

/* DAVINCI_VC_REG05 bit fields */
#define DAVINCI_VC_REG05_PGA_GAIN	0x07

/* DAVINCI_VC_REG09 bit fields */
#define DAVINCI_VC_REG09_MUTE		0x40
#define DAVINCI_VC_REG09_DIG_ATTEN	0x3F

/* DAVINCI_VC_REG12 bit fields */
#define DAVINCI_VC_REG12_POWER_ALL_ON	0xFD
#define DAVINCI_VC_REG12_POWER_ALL_OFF	0x00

#define DAVINCI_VC_CELLS		2

enum davinci_vc_cells {
	DAVINCI_VC_VCIF_CELL,
	DAVINCI_VC_CQ93VC_CELL,
};

struct davinci_vcif {
	struct platform_device	*pdev;
	u32 dma_tx_channel;
	u32 dma_rx_channel;
	dma_addr_t dma_tx_addr;
	dma_addr_t dma_rx_addr;
};

struct vsnv3afe{
	struct platform_device *pdev;
	struct snd_soc_codec *codec;
	u32 sysclk;
};

struct vsnv3_afe_data{
	/* Device data */
	struct device *dev;
	struct platform_device *pdev;
	struct clk *clk;

	/* Memory resources */
	void __iomem *base;
	resource_size_t pbase;
	size_t base_size;

	/* MFD cells */
//	struct mfd_cell cells[DAVINCI_VC_CELLS];

	/* Client devices */
//	struct davinci_vcif davinci_vcif;
	struct vsnv3afe vsnv3afe;
};



/* Register access macros */
//#define vsnv3afe_readl( reg)		(AITC_BASE_AFE ->##reg)//__raw_readl(AITC_BASE_AFE + reg)
//#define vsnv3afe_writel( reg, value)	__raw_writel((value), AITC_BASE_AFE + reg)

#endif

