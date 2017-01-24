//==============================================================================
//
//  File        : mmph_reg_gbl.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_GBL_H_
#define _MMP_REG_GBL_H_


#include    "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/
// **********************************
//   GLOBAL structure (0x8000 6900 for VSN_V2), (0x8000 5D00 for VSN_V2)
// **********************************

typedef struct _AITS_GBL {
	#if 1//(CHIP == VSN_V3)
	AIT_REG_D	GBL_CPU_SMOOTH_DIV_CTL;						//Register Space 2, 0x00
	AIT_REG_B			_0x04[0xC];							//Register Space 2, 0x04~0x0F reserved
	AIT_REG_B	GBL_DPLL2_M;								//Register Space 2, 0x10
	AIT_REG_B	GBL_DPLL2_N;								//Register Space 2, 0x11
	AIT_REG_B	GBL_DPLL2_PWR;								//Register Space 2, 0x12
	AIT_REG_B	GBL_DPLL2_CFG2;								//Register Space 2, 0x13
	AIT_REG_B	GBL_DPLL2_CFG3;								//Register Space 2, 0x14
	AIT_REG_B	GBL_DPLL2_CFG4;								//Register Space 2, 0x15
	AIT_REG_B	GBL_DPLL2_SRC_SEL;							//Register Space 2, 0x16
	AIT_REG_B	GBL_DPLL2_CFG5;								//Register Space 2, 0x17
	AIT_REG_B	GBL_IO_CTL14;								//Register Space 2, 0x18
	AIT_REG_B			_0x19[0x27];						//Register Space 2, 0x19~0x3F reserved
	AIT_REG_B	GBL_IO_CFG_PSTM_T32[0xB];					//Register Space 2, 0x40
	AIT_REG_B			_0x4B[0x85];						//Register Space 2, 0x4B~0xCF reserved
	AIT_REG_B	GBL_IO_CFG_PCGPIO6[0xC];					//Register Space 2, 0xD0
	AIT_REG_B			_0xDC[0x4];							//Register Space 2, 0xDC~0xDF reserved
	AIT_REG_B	GBL_IO_CFG_PDGPIO[0x5];						//Register Space 2, 0xE0~0xE4
	
	AIT_REG_B			_0x5E00[0xB1B];						//Register Space offset
	#endif
	AIT_REG_B   GBL_CHIP_CFG;								//0x00
		/*-DEFINE-----------------------------------------------------*/
		#if 0//(CHIP == VSN_V2)
		#define	ARM_JTAG_MODE_EN		   	0x3F
		#endif
		#if 1//(CHIP == VSN_V3)
		#define	ARM_JTAG_MODE_EN		   	0x80
		#endif
		#define ROM_BOOT_MODE				0x40
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_CHIP_CTL;								//0x01
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_WD_RST_ALL_MODULE		0x01
		#define	MOD_BOOT_STRAP_EN		   	0x80
		
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_SYS_CLK_CTL;							//0x02
		/*-DEFINE-----------------------------------------------------*/
		#define	USB_CLK_SEL_EXT0		0x80
		#define DRAM_ASYNC_EN			0x40
        /*------------------------------------------------------------*/
	AIT_REG_B   GBL_CLK_DIS0; 								//0x03
		/*-DEFINE-----------------------------------------------------*/
		#define	CLK_EN_ALL		   				0x0000
		#define GBL_CLK_MCI_DIS					0x01
		#define GBL_CLK_CPU_DIS                 0x02
		#define GBL_CLK_SCAL_DIS                0x04
		#define GBL_CLK_JPG_DIS                 0x08
		#define GBL_CLK_AUD_DIS                 0x10
		#define GBL_CLK_VI_DIS                  0x20
		#define GBL_CLK_ISP_DIS                 0x40
		#define GBL_CLK_GPIO_DIS                0x80
        /*------------------------------------------------------------*/		
	AIT_REG_B   GBL_CLK_PATH_CTL;							//0x04
		/*-DEFINE-----------------------------------------------------*/
		#define	BYPASS_DPLL0		   	0x04
		#define	BYPASS_DPLL1		   	0x08
		#if 1 //(CHIP == VSN_V3)
		#define	BYPASS_DPLL2		   	0x10
		#endif
		#define USB_PHY_PLL_INPUT_EN	0x20
		#define	USB_CLK_SEL_EXT1		0x40
        /*------------------------------------------------------------*/
	AIT_REG_B	GBL_CLK_DIV;								//0x05
		/*-DEFINE-----------------------------------------------------*/
		#define PLL_SEL_PLL_OFFSET			0x06
		#define PLL_POST_DIV_EN				0x20
		#define PLL_CLK_MUX					0xC0
		#if 0//(CHIP == VSN_V2)
		#define PLL_CLK_DIV_MCLK			0x80
		#endif
		#if 1//(CHIP == VSN_V3)
		#define PLL_CLK_DIV_MCLK			0xC0
		#endif
		#define PLL_DIVIDE_MASK				0x1f
		#define	GROUP_CLK_DIV(_a)			(_a)
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_USBPHY_CLK_DIV;							//0x06	
	AIT_REG_B   GBL_PLL_WAIT_CNT;							//0x07
	AIT_REG_W	GBL_RST_REG_EN;								//0x08
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_REG_CPU_PEREF_RST       0x0001
		#define GBL_REG_VI_RST            	0x0002
		#define GBL_REG_ISP_RST            	0x0004
		#define GBL_REG_JPG_RST             0x0008
		#define GBL_REG_SCAL_RST          	0x0010
		#define GBL_REG_GPIO_RST            0x0020
		#define GBL_REG_AUDIO_RST           0x0040
		#define GBL_REG_DRAM_RST            0x0080
		
		#define GBL_REG_MCI_RST            	0x0100
		#define GBL_REG_RAW_RST            	0x0200
		#define GBL_REG_DMA_RST            	0x0400
		#define GBL_REG_USB_RST            	0x1000
		#define GBL_REG_H264_RST            0x2000
		#define GBL_REG_ICON_IBC_RST        0x4000
		#define GBL_REG_GRA_RST            	0x8000
		#define GBL_REG_RST_ALL				0xFFFF
		/*------------------------------------------------------------*/
	AIT_REG_W	GBL_RST_CTL01;								//0x0A
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_CPU_PEREF_RST           0x0001
		#define GBL_VI_RST           		0x0002
		#define GBL_ISP_RST           		0x0004
		#define GBL_JPG_RST           		0x0008
		#if 0//(CHIP == VSN_V2)
		#define GBL_SCAL_RST           		0x0010
		#endif
		#if 1//(CHIP == VSN_V3)
		#define GBL_SCAL0_RST           	0x0010
		#endif
		#define GBL_GPIO_RST           		0x0020
		#define GBL_AUD_PEREF_RST           0x0040
		#define GBL_DRAM_RST           		0x0080
		
		#define GBL_MCI_RST           		0x0100
		#define GBL_RAW_RST           		0x0200
		#define GBL_DMA_RST           		0x0400
		#define GBL_I2C_RST           		0x0800
		#define GBL_USB_RST           		0x1000
		#define GBL_H264_RST           		0x2000
		#define GBL_ICON_IBC_RST           	0x4000
		#define GBL_GRA_RST           		0x8000
		#define GBL_MODULE_RST_ALL			0xFFFF
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_RST_CTL2;								//0x0C
		/*-DEFINE-----------------------------------------------------*/
		#define	GBL_CPU_SRAM_RST			0x01
		#define	GBL_SD0_RST					0x02
		#define	GBL_PWM_RST					0x04
		#define	GBL_PSPI_RST				0x08
		#define GBL_USB_PHY_RST				0x10
		#define	GBL_SD0_SW_RST_EN				0x20
		#define	GBL_REG_PWM_RST				0x40
		#define	GBL_REG_PSPI_RST			0x80
		/*------------------------------------------------------------*/
	AIT_REG_B   GBL_DRAM_CLK_DIV;							//0x0D
	AIT_REG_W   GBL_CLK_DIS1; 								//0x0E
		/*-DEFINE-----------------------------------------------------*/
        #define GBL_CLK_DRAM_DIS            0x0001
        #define GBL_CLK_BS_SPI_DIS			0x0002
        #define GBL_CLK_RAW_DIS             0x0004
        #define GBL_CLK_DMA_DIS             0x0008
        #define GBL_CLK_I2C_DIS             0x0010
        #define GBL_CLK_H264_DIS            0x0020
        #define GBL_CLK_ICON_DIS            0x0040
        #define GBL_CLK_GRA_DIS             0x0080
        #define GBL_CLK_USB_DIS             0x0100
        #define GBL_CLK_IBC_DIS             0x0200
        #define GBL_CLK_PWM_DIS             0x0400
        
        #define GBL_CLK_SD1_DIS		0x0800        
        #define GBL_CLK_SD0_DIS		0x1000

        #define GBL_CLK_PSPI_DIS            0x2000
        #define GBL_CLK_MIPI_DIS            0x4000
        #define GBL_CLK_CPU_PHL_DIS         0x8000
        
		
		/*------------------------------------------------------------*/
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x10[0x1];							//0x10 reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_DPLL0_M;								//0x10
	#endif
	
	AIT_REG_B	GBL_DPLL0_N;								//0x11
	#if 0//(CHIP == VSN_V2)	
	AIT_REG_B	GBL_DPLL_PWR;								//0x12
	#endif
	#if 1//(CHIP == VSN_V3)	
	AIT_REG_B	GBL_DPLL01_PWR;								//0x12
	#endif
		/*-DEFINE-----------------------------------------------*/
		#define DPLL_PWR_DOWN_EN		0x01
		/*-------------------------------------------------------*/
	
	AIT_REG_B	GBL_DPLL_CFG3;								//0x13
		/*-DEFINE-----------------------------------------------*/
		#define DPLL0_UPDATE_EN         0x01
		#define DPLL1_UPDATE_EN         0x02
		#if 1//(CHIP == VSN_V3)
		#define DPLL2_UPDATE_EN         0x04
		#endif
		/*-------------------------------------------------------*/
	AIT_REG_B	GBL_DPLL0_CFG4;								//0x14
		/*-DEFINE-----------------------------------------------*/
		#define PLL_500_1500_1000MHZ	0x0 
		/*-------------------------------------------------------*/
	AIT_REG_B   		_0x15[0x1];							//0x15 reserved
	AIT_REG_B	GBL_DPLL0_K;								//0x16
		/*-DEFINE-----------------------------------------------------*/
		#define	PLL0_CLK_DIV(_a)		(_a - 1)
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_DPLL_SOURCE_SEL;						//0x17
	AIT_REG_B	GBL_DPLL1_M;								//0x18
	AIT_REG_B	GBL_DPLL1_N;								//0x19
		/*-DEFINE-----------------------------------------------------*/
		#define	PLL1_CLK_N(_a)			(_a - 2)
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_CPU_CLK_DIV;							//0x1A
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x1B[0x1];							//0x1B reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_COLOR_CLK_DIV;							//0x1B
	#endif
	AIT_REG_B	GBL_HOST_BANK_SEL0;							//0x1C
	AIT_REG_B	GBL_HOST_BANK_SEL1;							//0x1D
	AIT_REG_B	GBL_HOST_BANK_SEL2;							//0x1E
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x1F[0x1];							//0x1F reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B 	GBL_CLK_DIS2;								//0x1F
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_CLK_AUD_CODEC_DIS			0x01
		#define GBL_CLK_CIDC_DIS                0x02
		#define GBL_CLK_GNR_DIS                	0x04
		#define GBL_CLK_SM_DIS                 	0x08
		#define GBL_CLK_COLOR_DIS               0x10
        /*------------------------------------------------------------*/		
	#endif
	AIT_REG_B	GBL_SOFT_RESOL_SEL;							//0x20
        /*-DEFINE-----------------------------------------------------*/
        #define GBL_SNR_RESOL_SEL_3M            0xF3
        #define GBL_SNR_RESOL_SEL_5M            0xF5
        #define GBL_SNR_RESOL_SEL_8M            0xF8
        #define GBL_SNR_RESOL_SEL_DEF           0x00
        /*------------------------------------------------------------*/
	AIT_REG_B	GBL_CPU_AUTO_BOOT_CTL;						//0x21
	AIT_REG_B	GBL_LCD_BYPASS_CTL0;						//0x22
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_LCD_BYPASS_CLK_ACTIVE		0x01
		#define GBL_LCD_BYPASS_PWN_DPLL			0x02
		#define GBL_ENTER_SELF_SLEEP_MODE		0x04
		#define GBL_XTAL_OFF_BYPASS				0x20
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_LCD_BYPASS_CTL1;						//0x23
		/*-DEFINE-----------------------------------------------------*/
		#if 1//(CHIP == VSN_V3)
		#define GBL_UART1_TX_PAD0			0x10 // PSNR_D4 as Tx
		#define GBL_UART1_RX_PAD0			0x20 // PSNR_D5 as Rx
		#define GBL_UART1_TX_PAD1			0x40 // PCGPIO2 as Tx
		#define GBL_UART1_RX_PAD1			0x80 // PCGPIO3 as Rx
		#endif
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_VI_CLK_DIV;								//0x24
	AIT_REG_B	GBL_IO_CTL0;								//0x25
		/*-DEFINE-----------------------------------------------------*/
		#if 1//(CHIP == VSN_V3)
		#define GBL_I2C_1_PIN_SEL			0x40
		#define GBL_SD1_IO_PAD0_EN			0x80
		#endif
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_IO_CTL1;								//0x26
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_UART_TX_PAD0			0x01 // PAGPIO0 as Tx
		#define GBL_UART_TX_PAD1			0x02 // PSNR_D8 as Tx
        #define GBL_SIF_PAD_EN              0x04 // PB0~PB3
        #define GBL_I2C_0_PAD_EN            0x08 // For I2CM0 download function used only !!
        #define GBL_I2C_1_PAD_EN            0x10 // 
        #define GBL_I2C_0_PAD_SEL_SNR       0x20 // sensor pad
        #if 1//(CHIP == VSN_V3)
        #define GBL_PSPI_IO_PAD1_EN			0x40
        #endif
        #define GBL_I2C_0_PAD_SEL_HOS       0x00 // host pad
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_IO_CTL2;								//0x27
	AIT_REG_B	GBL_IO_CTL3;								//0x28
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_UART_RX_PAD0			0x01 // PAGPIO6  as Rx
		#define GBL_UART_RX_PAD1			0x02 // PSNR_D9  as Rx
		#define GBL_UART_RX_PAD2			0x08 // PBGPIO12 as Rx
		#define GBL_UART_TX_PAD2			0x04 // PBGPIO14 as Tx
		#define GBL_PSPI_IO_PAD0_EN		(1<<4)
		#define GBL_SD0_IO_PAD0_EN			0x20
		#define GBL_SD0_IO_PAD1_EN			0x80		
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_IO_CTL4;								//0x29
		/*-DEFINE-----------------------------------------------------*/
		#if 1//(CHIP == VSN_V3)
		#define GBL_SD1_IO_PAD1_EN			0x80
		#endif
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_IO_CTL5;								//0x2A
	AIT_REG_B	GBL_IO_CTL6;								//0x2B
	AIT_REG_B	GBL_IO_CTL7;								//0x2C
	AIT_REG_B	GBL_AUDIO_CLK_DIV;							//0x2D
		#define GBL_AUDIO_CLK_SRC_DPLL2						2<<6
		#define GBL_AUDIO_CLK_ON								1<<5
		#define GBL_AUDIO_CLK_OFF								0<<5		
		#define GBL_AUDIO_CLK_DIV_MASK						0x1F
		
	AIT_REG_B	GBL_TCM_SIZE_SEL;							//0x2E
	AIT_REG_B	GBL_PROBE_CTL;								//0x2F
		/*-DEFINE-----------------------------------------------------*/
		#if 1//(CHIP == VSN_V3)
		#define GBL_JTAG_PDGPIO_SEL			0x10
		#endif
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_BIST_CTL;								//0x30
	AIT_REG_B	GBL_BIST_DBG_CTL;							//0x31
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x32[0x1];							//0x32 reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_BIST_DBG_CTL1;
	#endif
	AIT_REG_B	GBL_CPU_CFG;								//0x33
		/*-DEFINE-----------------------------------------------------*/
		#define	GBL_CPU_ASYNC_EN		0x01
		/*------------------------------------------------------------*/
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x34[0x2];							//0x34~0x35 reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_W	GBL_ADC_CLK_DIV;							//0x34	audio clock div [3:0] = 512fs	audio clock div [11:8] = audio clock div [7:4] = 256fs
	#endif
	AIT_REG_B	GBL_MACRO_TEST_MODE_CTL;					//0x36
	AIT_REG_B	GBL_ROM_MARGIN_CTL;							//0x37
	AIT_REG_D	GBL_BIST_RESULT_0;							//0x38
	AIT_REG_D	GBL_BIST_RESULT_1;							//0x3C
	
	AIT_REG_B	GBL_IO_CFG_PSTM_T[0x20];					//0x40~0x5F
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_IO_SCHMITT_TRIG_EN			0x01
		#define	GBL_IO_PULL_DOWN_RESIS_EN		0x02
		#define	GBL_IO_PULL_UP_RESIS_EN			0x04
		#define GBL_IO_SLOW_SLEW_EN				0x08
		#define GBL_IO_OUT_DRIVING(_a)			(_a << 0x5)
		/*------------------------------------------------------------*/
	
	AIT_REG_D	GBL_DRAM_BIST_ST_ADDR;						//0x60
	AIT_REG_D	GBL_DRAM_BIST_END_ADDR;						//0x64
	AIT_REG_B	GBL_GPIO_WAKE_EDGE_EN;						//0x68
	AIT_REG_B	GBL_GPIO_WAKE_LEVEL_EN;						//0x69
	AIT_REG_B	GBL_WAKE_CTL;								//0x6A
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_WAKE_GPIO0								0x01
		#define GBL_WAKE_GPIO6								0x02
		#define GBL_WAKE_USB_RESUME							0x04
		#if 1//(CHIP == VSN_V3)
		#define GBL_WAKE_GPIO23								0x08
		#define GBL_WAKE_GPIO50								0x10
		#define GBL_WAKE_GPIO63								0x20
		#endif
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_GPIO_WAKE_INT_POLARITY;					//0x6B
	AIT_REG_B	GBL_GPIO_WAKE_HOST_SR;						//0x6C
	AIT_REG_B	GBL_GPIO_WAKE_HOST_EN;						//0x6D
	AIT_REG_B	GBL_GPIO_WAKE_CPU_SR;						//0x6E
	AIT_REG_B	GBL_GPIO_WAKE_CPU_EN;						//0x6F
	
	AIT_REG_B	GBL_IO_CTL8;								//0x70
	AIT_REG_B	GBL_IO_CTL9;								//0x71
	AIT_REG_B	GBL_IO_CTL10;								//0x72
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x73[0x1];							//0x73 reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_IO_CTL11;								//0x73
	#endif
	AIT_REG_B	GBL_SRAM_MARGIN_CTL0;						//0x74
	AIT_REG_B	GBL_SRAM_MARGIN_CTL1;						//0x75
	AIT_REG_B	GBL_SRAM_MARGIN_CTL2;						//0x76
	AIT_REG_B	GBL_MIPI_RX_CLK_SEL;						//0x77
		/*-DEFINE-----------------------------------------------------*/
		#define	PLL_MIPI_MCLK_EN		0x02
		/*------------------------------------------------------------*/
	AIT_REG_B	GBL_I2CS_HOST_INT_EN;						//0x78
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x79[0x1];							//0x79 reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_RST_CTL3;								//0x79
		/*-DEFINE-----------------------------------------------------*/
		#define	GBL_SD1_SW_RST_EN		0x80
		#define	GBL_REG_SCAL2_RST	0x40
		#define	GBL_REG_SCAL1_RST	0x20
		#define	GBL_REG_SM_RST		0x10
		#define	GBL_SD1_RST			0x08
		#define	GBL_SCAL2_RST		0x04
		#define	GBL_SCAL1_RST		0x02
		#define	GBL_SM_RST			0x01
		/*------------------------------------------------------------*/
	#endif
	AIT_REG_B	GBL_I2CS_HOST_INT_SR;						//0x7A
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x7B[0x1];							//0x7B reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_IO_CTL12;								//0x7B
	#endif
	AIT_REG_B	GBL_I2CS_CPU_INT_EN;						//0x7C
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0x7D[0x1];							//0x7D reserved
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_IO_CTL13;								//0x7D
	#endif
	AIT_REG_B	GBL_I2CS_CPU_INT_SR;						//0x7E
	AIT_REG_B   		_0x7F[0x1];							//0x7F reserved
	
	AIT_REG_B	GBL_BIST_SEL[0x10];							//0x80~0x8F
	
	AIT_REG_B	GBL_I2CS_CTL;								//0x90
	AIT_REG_B	GBL_I2CS_PROBE_SEL;							//0x91
	AIT_REG_B	GBL_I2CS_HOLD_TIME_CNT;						//0x92
	AIT_REG_B	GBL_I2CS_ST_CHECK_CNT;						//0x93
	AIT_REG_B	GBL_I2CS_ERR_TIMER;							//0x94
	AIT_REG_B   		_0x95[0x1];							//0x95 reserved
	AIT_REG_B	GBL_I2CS_FSM_STATE;							//0x96
	AIT_REG_B	GBL_I2CS_SR;								//0x97
	AIT_REG_B	GBL_IO_CFG_PSTM_B24[0x8];					//0x98~0x9F
	
	AIT_REG_B	GBL_XTAL_IO_CFG;							//0xA0
	AIT_REG_B	GBL_PI2C_AGPIO_CFG[0xE];					//0xA1
	AIT_REG_B   		_0xAF[0x1];							//0xAF reserved
	
	
	AIT_REG_B	GBL_IO_CFG_PBGPIO[0x10];					//0xB0~0xBF
	
	AIT_REG_B	GBL_IO_CFG_PSNR[0x6];						//0xC0~0xC5
	AIT_REG_B   		_0xC6[0x4];							//0xC6~0xC9 reserved
	AIT_REG_B	GBL_IO_CFG_PSNR2[0x3];						//0xC0~0xC5
	AIT_REG_B   		_0xCD[0x4];							//0xCD~0xD0 reserved
	AIT_REG_B	GBL_IO_CFG_PCGPIO[0x6];						//0xD1~0xD6
	AIT_REG_B   		_0xD7[0x1];							//0xD7 reserved
	AIT_REG_B	GBL_IO_CFG_PSTM_B[0x18];					//0xD8~0xEF
	
	AIT_REG_B   		_0xF0[0x4];							//0xF0~0xF3 reserved
	#if 0//(CHIP == VSN_V2)
	AIT_REG_D   GBL_DPLL1_CFG;								//0xF4
		/*-DEFINE-----------------------------------------------*/
		#define PLL_DIV_BIT_MASK			0x7
		/*-------------------------------------------------------*/
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_W   GBL_DPLL1_CFG01;							//0xF4
		/*-DEFINE-----------------------------------------------*/
		#define PLL_DIV_BIT_MASK			0x7
		/*-------------------------------------------------------*/
	AIT_REG_B   GBL_DPLL1_CFG2;								//0xF6
	AIT_REG_B	GBL_CLK_SEL;								//0xF7
	#endif
		/*-DEFINE-----------------------------------------------*/
		#define PLL_200_500_350MHZ			0x00 
		#define PLL_300_700_500MHZ			0x40
		#define PLL1_CLK_DIV_8           	0x00
		#define PLL1_CLK_DIV_4           	0x01
		#define PLL1_CLK_DIV_2           	0x02
		#define PLL1_CLK_DIV_1           	0x03
		#if 0//(CHIP == VSN_V2)
		#define PLL1_V2I_HIGH_GAIN_EN		0x80
		#define PLL1_V2I_GAIN_ADJUST_DIS	0x10000
		#endif
		#if 1//(CHIP == VSN_V3)
		#define PLL_V2I_HIGH_GAIN_EN		0x80
		#define PLL_V2I_GAIN_ADJUST_DIS	0x01
		#define GBL_CLK_SEL_MUX0			0x01
		#define PLL_SEL_PLL_OFFSET1		0x01
		#endif
		/*-------------------------------------------------------*/
		
	AIT_REG_B	GBL_STACK_MEM_TYPE;							//0xF8
		/*-DEFINE-----------------------------------------------*/
		#if 0//(CHIP == VSN_V2)
		#define MEM_ERTON_16Mb_SDR			0x00 
		#define MEM_WINBO_16Mb_SDR			0x00
		#define MEM_ISSI_16Mb_SDR			0x00
		
		#define MEM_WINBO_32Mb_SDR			0x01 
		#define MEM_WINBO_64Mb_SDR			0x01
		#define MEM_WINBO_32Mb_DDR			0x01 
		#define MEM_WINBO_64Mb_DDR			0x01
		#define MEM_ERTON_64Mb_DDR			0x01
		
		#define MEM_WINBO_128Mb_DDR			0x02
		#define MEM_WINBO_256Mb_DDR			0x03
		#define MEM_PIECE_512Mb_DDR			0x04
		#endif
		#if 1//(CHIP == VSN_V3)
		#define MEM_WINBO_256Mb_DDR			0x02
		#endif
		#define GBL_PROJECT_ID_RD_EN		0x80
		/*-------------------------------------------------------*/
	#if 0//(CHIP == VSN_V2)
	AIT_REG_B   		_0xF9[0x1];							//0xF9
	#endif
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	GBL_CPU_CLK_SEL;							//0xF9
		/*-DEFINE-----------------------------------------------*/
		#define CPU_CLK_SEL_MUX0			0x01
		/*-------------------------------------------------------*/
	#endif
	AIT_REG_B   GBL_CHIP_VER; 								//0xFA
	AIT_REG_B   GBL_SIF_REBOOT_EN; 							//0xFB
	AIT_REG_W   GBL_SECO_VER; 								//0xFC
	AIT_REG_W   GBL_PROJETCT_ID; 							//0xFE
} AITS_GBL, *AITPS_GBL;


#endif // _MMPH_REG_GBL_H_
