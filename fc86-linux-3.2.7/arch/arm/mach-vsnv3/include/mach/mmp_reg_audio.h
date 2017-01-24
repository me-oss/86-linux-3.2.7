//==============================================================================
//
//  File        : mmp_register_dma.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Rogers Chen
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_AUDIO_H_
#define _MMP_REG_AUDIO_H_
#include <linux/platform_device.h>


#include    "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/


// ********************************
//   Audio structure (0x8000 7800)
// ********************************
typedef struct _AITS_AUD {
    AIT_REG_B   I2S_FIFO_CPU_INT_EN;                                     // 0x0
    AIT_REG_B   I2S_FIFO_HOST_INT_EN;      								 // 0x1
        /*-DEFINE-----------------------------------------------------*/
        #define AUD_INT_FIFO_EMPTY             0x0001
        #define AUD_INT_FIFO_FULL              0x0002
        #define AUD_INT_FIFO_REACH_UNRD_TH     0x0004
        #define AUD_INT_FIFO_REACH_UNWR_TH     0x0008
		/*------------------------------------------------------------*/    
	AIT_REG_W   						_x02;
    AIT_REG_B   I2S_FIFO_SR;                                         	 // 0x4
	AIT_REG_B   						_x05[3];
    AIT_REG_B   I2S_FIFO_RST;                                         	 // 0x8
        /*-DEFINE-----------------------------------------------------*/
		#define AUD_FIFO_RST_EN                0x0001
		/*------------------------------------------------------------*/    

	AIT_REG_B   						_x09[3];

    AIT_REG_D   I2S_FIFO_DATA;                                           // 0x0C
	AIT_REG_W   I2S_FIFO_RD_TH;                                          // 0x10
	AIT_REG_W   						_x12;
	AIT_REG_W   I2S_FIFO_WR_TH;                                          // 0x14
	AIT_REG_W   						_x16;
	AIT_REG_W   I2S_FIFO_UNRD_CNT;                                       // 0x18
	AIT_REG_W   						_x1A;
	AIT_REG_W   I2S_FIFO_UNWR_CNT;                                       // 0x1C
	AIT_REG_W   						_x1E[5];                         
	AIT_REG_B   I2S_CTL;                                                 // 0x28
        /*-DEFINE-----------------------------------------------------*/
		#define I2S_SDO_OUT_EN                 0x08
		#define I2S_LRCK_OUT_EN                0x04
		#define I2S_BCK_OUT_EN                 0x02
		#define I2S_HCK_CLK_EN                 0x01
		#define I2S_ALL_DIS                    0x0
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x29[3];
	AIT_REG_B   I2S_CLK_DIV;                                             // 0x2C
	AIT_REG_B   I2S_MCLK_FIXMODE;                                        // 0x2D
	AIT_REG_W   						_x2E;
	AIT_REG_W   I2S_RATIO_N_M;                         					 // 0x30
	AIT_REG_W   						_x32;
	AIT_REG_B   I2S_BIT_CLT;                                             // 0x34
		#define	I2S_OUTPUT_16_BITS		0x01
		#define	I2S_OUTPUT_24_BITS		0x02
		#define	I2S_OUTPUT_32_BITS		0x04
	AIT_REG_B   						_x35[3];
	AIT_REG_B   I2S_LRCK_POL;                                            // 0x38
	AIT_REG_B   						_x39[3];

    AIT_REG_D   I2S_L_CHNL_DATA;                                        // 0x3C
    AIT_REG_D   I2S_R_CHNL_DATA;                                        // 0x40


    AIT_REG_B	I2S_BIT_ALIGN_OUT;										// 0x44

    
	AIT_REG_B   I2S_BIT_ALIGN_IN;      								 	 // 0x45
	AIT_REG_W   						_x46;
	AIT_REG_B   I2S_MODE_CTL;                                           // 0x48
        /*-DEFINE-----------------------------------------------------*/
		#define I2S_MCLK_OUT_EN                 4
		#define I2S_SLAVE                       0   
		#define I2S_MASTER                      1   
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x49[3];
	AIT_REG_B   I2S_MCLK_CTL;	      									// 0x4C
        /*-DEFINE-----------------------------------------------------*/
    	#define I2S_768_FS				 0x20				
    	#define I2S_512_FS				 0x10		
    	#define I2S_384_FS				 0x08
    	#define I2S_256_FS				 0x04
    	#define I2S_192_FS				 0x02								 	 
    	#define I2S_128_FS				 0x01							
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x4D[3];
	AIT_REG_B   I2S_ASPORT_CTL;	      								 	 // 0x50
        /*-DEFINE-----------------------------------------------------*/
    	#define I2S_ASPORT_EN			 0x01							
		/*------------------------------------------------------------*/
	
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	I2S_MSPORT_CH_START_BIT;								// 0x51
	AIT_REG_W							_x52;
	AIT_REG_B	I2S_DATA_OUT_PAD_CTL;									// 0x54
	AIT_REG_B							_x55[3];
	#elif 0//(CHIP == VSN_V2)
	AIT_REG_B							_x51[7];							//0x51
	#endif
	
	AIT_REG_B   I2S_DATA_IN_SEL;										 // 0x58
        /*-DEFINE-----------------------------------------------------*/
        #define I2S_SDO_IN     				    0x01
        #define I2S_SDI_IN    				    0x00
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x59[3];
    AIT_REG_B   I2S_CPU_INT_EN;                                          // 0x5C
    AIT_REG_B   I2S_HOST_INT_EN;                                         // 0x5D
        /*-DEFINE-----------------------------------------------------*/
        #define AUD_INT_EN     				    0x01
        #define AUD_INT_DIS    				    0x00
		/*------------------------------------------------------------*/
	
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B	I2S_INT_CSR;											// 0x5E
	AIT_REG_B							_x5F;
	AIT_REG_B	I2S_OUT_DELAY_MODE;										// 0x60
		#define	I2S_STD_MODE			0x01
		#define	I2S_I2S_MODE			0x00	
	AIT_REG_B							_x61[3];
	AIT_REG_W	I2S_MSPORT_FRAME_CYC_CNT;								// 0x64
	AIT_REG_B	I2S_MSPORT_FRAME_SYNC_LENGTH;							// 0x66
	AIT_REG_B							_x67;
	AIT_REG_B	I2S_SDO_SHF_REG_SW_RESET;								// 0x68
	AIT_REG_B							_x69[7];
	#elif 0//(CHIP == VSN_V2)    
	AIT_REG_B   						_x5E[14];
	
	AIT_REG_B	I2S_INT_CLR;											// 0x6C
	AIT_REG_B							_x6D[3];
	#endif
	
	AIT_REG_B	I2S_MUX_MODE_CTL;										// 0x70
        /*-DEFINE-----------------------------------------------------*/
        #define AUD_MUX_AUTO			    	0x01
        #define AUD_MUX_CPU			    		0x00
		/*------------------------------------------------------------*/
		
	#if 1//(CHIP == VSN_V3)
	AIT_REG_B							_x71[3];
	AIT_REG_B	FULL_DUP_PATH_SEL;										// 0x74
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_CODEC_FULL_PATH				0x01
		#define AUD_I2S_FULL_PATH				0x00
		/*------------------------------------------------------------*/
	AIT_REG_B							_x75[3];
	AIT_REG_B	HOST_I2S_PATH_CTL;										// 0x78
		/*-DEFINE-----------------------------------------------------*/
		#define	EN_HOST_I2S_OUTPUT				0x40
		#define EN_HOST_I2S_INPUT				0x20
		#define AUD_ADC_OUTPUT_HOST_I2S			0x08
		#define I2S_OUTPUT_HOST_I2S				0x04
		#define HOST_I2S_INPUT_DAC				0x02
		#define HOST_I2S_INPUT_I2S				0x01
		/*------------------------------------------------------------*/
	AIT_REG_B	MISC_I2S_CTL;											// 0x79
		/*-DEFINE-----------------------------------------------------*/
		#define I2S_MCLK_INVERT					0x04
		#define AUD_AFE_USE_I2S_MCLK			0x02
		#define HOST_I2S_INPUT_REC_EN			0x01
		/*------------------------------------------------------------*/
	AIT_REG_W							_x7A[3];
	AIT_REG_B	I2S_TIMER_CNT_EN;										// 0x80
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B							_x81[3];
	AIT_REG_B	I2S_TIMER_CNT_SRC;										// 0x84
		/*-DEFINE-----------------------------------------------------*/
		#define SRC_I2S_MCLK					0x01
		#define SRC_BIT_CLK						0x02
		#define SRC_LR_CLK						0x04
		/*------------------------------------------------------------*/
	AIT_REG_B	I2S_TIMER_CNT_ROUNDING;									// 0x85
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_x86;
	AIT_REG_D	I2S_TIMER_TARGET_CNT;									// 0x88
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_D	I2S_TIMER_CUR_CNT;										// 0x8c
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	I2S_TIMER_CPU_INT_EN;									// 0x90
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	I2S_TIMER_HOST_INT_EN;									// 0x91
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_x92;
	AIT_REG_B	I2S_TIMER_INT_CSR;										// 0x94
		/*-DEFINE-----------------------------------------------------*/
		#define I2S_TIMER_CSR_CLR				0x01		//write 1 clear
		/*------------------------------------------------------------*/
	AIT_REG_B							_x95[3];
	AIT_REG_B	I2S_TIMER_SW_RST;										// 0x98
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	#endif
	
} AITS_AUD, *AITPS_AUD;


typedef struct _AITS_AUD_I2SFIFO{


    AIT_REG_B   I2S_FIFO_CPU_INT_EN;                                     // 0x0
    AIT_REG_B   I2S_FIFO_HOST_INT_EN;      								 // 0x1
        /*-DEFINE-----------------------------------------------------*/
        #define AUD_INT_FIFO_EMPTY             0x0001
        #define AUD_INT_FIFO_FULL              0x0002
        #define AUD_INT_FIFO_REACH_UNRD_TH     0x0004
        #define AUD_INT_FIFO_REACH_UNWR_TH     0x0008
		/*------------------------------------------------------------*/    
	AIT_REG_W   						_x02;
    AIT_REG_B   I2S_FIFO_SR;                                         	 // 0x4
	AIT_REG_B   						_x05[3];
    AIT_REG_B   I2S_FIFO_RST;                                         	 // 0x8
        /*-DEFINE-----------------------------------------------------*/
		#define AUD_FIFO_RST_EN                0x0001
		/*------------------------------------------------------------*/    

	AIT_REG_B   						_x09[3];

    AIT_REG_D   I2S_FIFO_DATA;                                           // 0x0C
	AIT_REG_W   I2S_FIFO_RD_TH;                                          // 0x10
	AIT_REG_W   						_x12;
	AIT_REG_W   I2S_FIFO_WR_TH;                                          // 0x14
	AIT_REG_W   						_x16;
	AIT_REG_W   I2S_FIFO_UNRD_CNT;                                       // 0x18
	AIT_REG_W   						_x1A;
	AIT_REG_W   I2S_FIFO_UNWR_CNT;                                       // 0x1C
	AIT_REG_W   						_x1E[5];                         
}AITS_AUD_I2SFIFO, *AITPS_AUD_I2SFIFO;
	
typedef struct _AITS_AUD_I2SCTRL{
	AIT_REG_B   I2S_CTL;                                                 // 0x28
        /*-DEFINE-----------------------------------------------------*/
		#define I2S_SDO_OUT_EN                 0x08
		#define I2S_LRCK_OUT_EN                0x04
		#define I2S_BCK_OUT_EN                 0x02
		#define I2S_HCK_CLK_EN                 0x01
		#define I2S_ALL_DIS                    0x0
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x29[3];
	AIT_REG_B   I2S_CLK_DIV;                                             // 0x2C
	AIT_REG_B   I2S_MCLK_FIXMODE;                                        // 0x2D
	AIT_REG_W   						_x2E;
	AIT_REG_W   I2S_RATIO_N_M;                         					 // 0x30
	AIT_REG_W   						_x32;
	AIT_REG_B   I2S_BIT_CLT;                                             // 0x34
		#define	I2S_OUTPUT_16_BITS		0x01
		#define	I2S_OUTPUT_24_BITS		0x02
		#define	I2S_OUTPUT_32_BITS		0x04
	AIT_REG_B   						_x35[3];
	AIT_REG_B   I2S_LRCK_POL;                                            // 0x38
	AIT_REG_B   						_x39[3];

	AIT_REG_D   I2S_L_CHNL_DATA;                                        // 0x3C
	AIT_REG_D   I2S_R_CHNL_DATA;                                        // 0x40


	AIT_REG_B	I2S_BIT_ALIGN_OUT;										// 0x44

    
	AIT_REG_B   I2S_BIT_ALIGN_IN;      								 	 // 0x45
	AIT_REG_W   						_x46;
	AIT_REG_B   I2S_MODE_CTL;                                           // 0x48
        	/*-DEFINE-----------------------------------------------------*/
		#define I2S_MCLK_OUT_EN                 4
		#define I2S_SLAVE                       0   
		#define I2S_MASTER                      1   
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x49[3];
	AIT_REG_B   I2S_MCLK_CTL;	      									// 0x4C
        	/*-DEFINE-----------------------------------------------------*/
	    	#define I2S_768_FS				 0x20				
	    	#define I2S_512_FS				 0x10		
	    	#define I2S_384_FS				 0x08
	    	#define I2S_256_FS				 0x04
	    	#define I2S_192_FS				 0x02								 	 
	    	#define I2S_128_FS				 0x01							
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x4D[3];
	AIT_REG_B   I2S_ASPORT_CTL;	      								 	 // 0x50
        /*-DEFINE-----------------------------------------------------*/
    	#define I2S_ASPORT_EN			 0x01							
		/*------------------------------------------------------------*/
	
	AIT_REG_B	I2S_MSPORT_CH_START_BIT;								// 0x51
	AIT_REG_W							_x52;
	AIT_REG_B	I2S_DATA_OUT_PAD_CTL;									// 0x54
	AIT_REG_B							_x55[3];
	
	AIT_REG_B   I2S_DATA_IN_SEL;										 // 0x58
		/*-DEFINE-----------------------------------------------------*/
		#define I2S_SDO_IN     				    0x01
		#define I2S_SDI_IN    				    0x00
		/*------------------------------------------------------------*/    
	AIT_REG_B   						_x59[3];
	AIT_REG_B   I2S_CPU_INT_EN;                                          // 0x5C
	AIT_REG_B   I2S_HOST_INT_EN;                                         // 0x5D
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_INT_EN     				    0x01
		#define AUD_INT_DIS    				    0x00
		/*------------------------------------------------------------*/
	
	AIT_REG_B	I2S_INT_CSR;											// 0x5E
	AIT_REG_B							_x5F;
	AIT_REG_B	I2S_OUT_DELAY_MODE;										// 0x60
		#define	I2S_STD_MODE			0x01
		#define	I2S_I2S_MODE			0x00	
	AIT_REG_B							_x61[3];
	AIT_REG_W	I2S_MSPORT_FRAME_CYC_CNT;								// 0x64
	AIT_REG_B	I2S_MSPORT_FRAME_SYNC_LENGTH;							// 0x66
	AIT_REG_B							_x67;
	AIT_REG_B	I2S_SDO_SHF_REG_SW_RESET;								// 0x68
	AIT_REG_B							_x69[7];

}AITS_AUD_I2SCTRL, *AITPS_AUD_I2SCTRL;

typedef struct{
	AIT_REG_B	I2S_MUX_MODE_CTL;										// 0x70
        /*-DEFINE-----------------------------------------------------*/
        #define AUD_MUX_AUTO			    	0x01
        #define AUD_MUX_CPU			    		0x00
		/*------------------------------------------------------------*/
		
	AIT_REG_B							_x71[3];
	AIT_REG_B	FULL_DUP_PATH_SEL;										// 0x74
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_CODEC_FULL_PATH				0x01
		#define AUD_I2S_FULL_PATH				0x00
		/*------------------------------------------------------------*/
	AIT_REG_B							_x75[3];
	AIT_REG_B	HOST_I2S_PATH_CTL;										// 0x78
		/*-DEFINE-----------------------------------------------------*/
		#define	EN_HOST_I2S_OUTPUT				0x40
		#define EN_HOST_I2S_INPUT				0x20
		#define AUD_ADC_OUTPUT_HOST_I2S			0x08
		#define I2S_OUTPUT_HOST_I2S				0x04
		#define HOST_I2S_INPUT_DAC				0x02
		#define HOST_I2S_INPUT_I2S				0x01
		/*------------------------------------------------------------*/
	AIT_REG_B	MISC_I2S_CTL;											// 0x79
		/*-DEFINE-----------------------------------------------------*/
		#define I2S_MCLK_INVERT					0x04
		#define AUD_AFE_USE_I2S_MCLK			0x02
		#define HOST_I2S_INPUT_REC_EN			0x01
		/*------------------------------------------------------------*/
	AIT_REG_W							_x7A[3];
}AITS_AUD_I2S_MUX, *AITPS_AUD_I2S_MUX;
		
typedef struct{
	AIT_REG_B	I2S_TIMER_CNT_EN;										// 0x80
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B							_x81[3];
	AIT_REG_B	I2S_TIMER_CNT_SRC;										// 0x84
		/*-DEFINE-----------------------------------------------------*/
		#define SRC_I2S_MCLK					0x01
		#define SRC_BIT_CLK						0x02
		#define SRC_LR_CLK						0x04
		/*------------------------------------------------------------*/
	AIT_REG_B	I2S_TIMER_CNT_ROUNDING;									// 0x85
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_x86;
	AIT_REG_D	I2S_TIMER_TARGET_CNT;									// 0x88
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_D	I2S_TIMER_CUR_CNT;										// 0x8c
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	I2S_TIMER_CPU_INT_EN;									// 0x90
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	I2S_TIMER_HOST_INT_EN;									// 0x91
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_x92;
	AIT_REG_B	I2S_TIMER_INT_CSR;										// 0x94
		/*-DEFINE-----------------------------------------------------*/
		#define I2S_TIMER_CSR_CLR				0x01		//write 1 clear
		/*------------------------------------------------------------*/
	AIT_REG_B							_x95[3];
	AIT_REG_B	I2S_TIMER_SW_RST;										// 0x98
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/

}AITS_AUD_I2STIMER, *AITPS_AUD_I2STIMER;

#if 1//(CHIP == VSN_V3)
// ********************************
//   AFE structure (0x8000 7f00)
// ********************************

typedef struct _AITS_AFE {
	AIT_REG_B	AFE_FIFO_CPU_INT_EN;									// 0x00
	AIT_REG_B	AFE_FIFO_HOST_INT_EN;									// 0x01
		/*-DEFINE-----------------------------------------------------*/
		#define AFE_INT_FIFO_REACH_UNWR_TH			0x08
		#define AFE_INT_FIFO_REACH_UNRD_TH			0x04
		#define AFE_INT_FIFO_FULL					0x02
		#define AFE_INT_FIFO_EMPTY					0x01
		/*------------------------------------------------------------*/
	AIT_REG_W							_x02;
	AIT_REG_B	AFE_FIFO_CSR;											// 0x04
		/*-DEFINE-----------------------------------------------------*/
		#define FIFO_UNWR_TH						0x08
		#define FIFO_UNRD_TH						0x04
		#define FIFO_FULL							0x02
		#define FIFO_EMPTY							0x01
		/*------------------------------------------------------------*/
	AIT_REG_B							_x05[3];
	AIT_REG_B	AFE_FIFO_RST;											// 0x08
		/*-DEFINE-----------------------------------------------------*/
		#define REST_FIFO	1
		/*------------------------------------------------------------*/
	AIT_REG_B							_x09[3];
	AIT_REG_D	AFE_FIFO_DATA;											// 0x0C
	AIT_REG_W	AFE_FIFO_RD_TH;											// 0x10
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_0x12;
	AIT_REG_W	AFE_FIFO_WR_TH;											// 0x14
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_0x16;
	AIT_REG_W	AFE_FIFO_UNRD_CNT;										// 0x18
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_0x1A;
	AIT_REG_W	AFE_FIFO_UNWR_CNT;										// 0x1C
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_0x1E;
	AIT_REG_D	AFE_L_CHANNEL_DATA;										// 0x20
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_D	AFE_R_CHANNEL_DATA;										// 0x24
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_CPU_INT_EN;										// 0x28
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_ADC_INT_EN						0x01
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_HOST_INT_EN;										// 0x29
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_INT_CSR;											// 0x2A
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B							_0x2B;
	AIT_REG_B	AFE_FIFO_RM;											// 0x2C
		/*-DEFINE-----------------------------------------------------*/
		#define HOST_AFE_ADC_INT_EN					0x01
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_COEF_RM;										// 0x2D
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_W							_0x2E[17];
	AIT_REG_B	AFE_GBL_PWR_CTL;										// 0x50
		/*-DEFINE-----------------------------------------------------*/
		#define BYPASS_OP							0x40
		#define PWR_UP_ANALOG						0x20
		#define PWR_UP_VREF							0x10
		#define PWR_UP_ADC_DIGITAL_FILTER			0x01
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_CLK_CTL;											// 0x51
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_CODEC_NORMAL_MODE				0x80
		#define ADC_CLK_MODE_USB					0x02
		#define ADC_CLK_INVERT						0x01
		/*------------------------------------------------------------*/
	AIT_REG_W							_0x52;
	AIT_REG_B	AFE_MUX_MODE_CTL;										// 0x54
		/*-DEFINE-----------------------------------------------------*/
		#define AUD_DATA_BIT_20						0x02	//otherwise 16bits
		#define AFE_MUX_AUTO_MODE					0x01
		#define AFE_MUX_CPU_MODE					0x00
		/*------------------------------------------------------------*/
	AIT_REG_B							_0x55;
	AIT_REG_B	SPECIAL_AUD_CODEC_PATH;									// 0x56
		/*-DEFINE-----------------------------------------------------*/
		#define EXT_AUD_CODEC_EN					0x01
		/*------------------------------------------------------------*/
	AIT_REG_B							_0x57;
	AIT_REG_B	AFE_GBL_BIAS_ADJ;									// 0x58
		/*-DEFINE-----------------------------------------------------*/
		#define GBL_BIAS_50							0x00
		#define GBL_BIAS_62_5						0x01
		#define GBL_BIAS_75							0x02
		#define GBL_BIAS_87_5						0x03
		#define GBL_BIAS_100						0x04
		#define GBL_BIAS_112_5						0x05
		#define GBL_BIAS_125						0x06
		#define GBL_BIAS_137_5						0x07
		/*------------------------------------------------------------*/
	AIT_REG_B							_0x59[7];
	AIT_REG_B	AFE_ADC_PWR_CTL;										// 0x60
		/*-DEFINE-----------------------------------------------------*/
		#define ADC_SDM_RCH_POWER_EN				0x08
		#define ADC_SDM_LCH_POWER_EN				0x04
		#define ADC_PGA_RCH_POWER_EN				0x02
		#define ADC_PGA_LCH_POWER_EN				0x01
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_SAMPLE_RATE_CTL;											// 0x61
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
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_INPUT_SEL;										// 0x62
		/*-DEFINE-----------------------------------------------------*/
        #define ADC_AUX_IN	    	               0x10
        #define ADC_MIC_IN		                   0x04
        #define ADC_MIC_DIFF2SINGLE				   0x02
        #define ADC_MIC_DIFF					   0x00
        #define ADC_CTL_MASK					   ~(ADC_AUX_IN|ADC_MIC_IN)
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_BOOST_CTL;										// 0x63
		/*-DEFINE-----------------------------------------------------*/
		#define MIC_NO_BOOST						0x00
        #define MIC_BOOST_20DB						0x01
        #define MIC_BOOST_30DB						0x02
        #define MIC_BOOST_40DB						0x03
        #define MIC_LCH_BOOST(_a)					(_a<<2)
        #define MIC_RCH_BOOST(_a)					(_a)
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_HPF_CTL;										// 0x64
		/*-DEFINE-----------------------------------------------------*/
		#define ADC_HPF_EN							0x01
		/*------------------------------------------------------------*/
	AIT_REG_B							_0x65[3];
	AIT_REG_B	AFE_ADC_LCH_PGA_GAIN_CTL;							// 0x68
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_RCH_PGA_GAIN_CTL;							// 0x69
		/*-DEFINE-----------------------------------------------------*/
		/*0~1F: -11dB ~ +20dB*/
		
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_LCH_DIGITAL_VOL;								// 0x6A
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_RCH_DIGITAL_VOL;								// 0x6B
		/*-DEFINE-----------------------------------------------------*/
		/*64~87: +0.5dB~+12dB*/
		/*------------------------------------------------------------*/
	AIT_REG_B	FIX_AFE_ADC_OVERFLOW;									// 0x6C
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_DIGITAL_GAIN_MUTE_STEP;							// 0x6D
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_DATA_BIT_WIDTH;									// 0x6E
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B							_0x6F;
	AIT_REG_B	AFE_ADC_BIAS_ADJ;									// 0x70
		/*-DEFINE-----------------------------------------------------*/
		#define ANA_ADC_DISC_OP_MASK            0x0C
        #define ANA_ADC_DISC_OP(_a)             (_a<<2)
        #define ANA_ADC_CONT_OP_MASk            0x03
        #define ANA_ADC_CONT_OP(_a)             (_a)
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_CTL_REG1;									// 0x71
		/*-DEFINE-----------------------------------------------------*/
		#define AFE_ZERO_CROSS_DET				0x10
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_CTL_REG2;									// 0x72
		/*-DEFINE-----------------------------------------------------*/
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_CTL_REG3;									// 0x73
		/*-DEFINE-----------------------------------------------------*/
		
		/*------------------------------------------------------------*/
	AIT_REG_B	AFE_ADC_CTL_REG4;									// 0x74
		/*-DEFINE-----------------------------------------------------*/
		#define ADC_MIC_BIAS_ON                 0x04
		#define ADC_MIC_BIAS_OFF                 0x00		
		#define ADC_MIC_BIAS_VOLTAGE090AVDD     0x03
		#define ADC_MIC_BIAS_VOLTAGE080AVDD     0x02
		#define ADC_MIC_BIAS_VOLTAGE075AVDD     0x01		
		#define ADC_MIC_BIAS_VOLTAGE065AVDD     0x00
		/*------------------------------------------------------------*/
} AITS_AFE, *AITPS_AFE;

#define I2S_REG_OFFSET(reg) offsetof(AITS_AUD,reg)//(((AITPS_AFE)0)->reg-(AITPS_AFE)0)
#define AFE_REG_OFFSET(reg) offsetof(AITS_AFE,reg)//(((AITPS_AFE)0)->reg-(AITPS_AFE)0)
#endif
////////////////////////////////////
// Register definition
//

#define FIFO_CPU_INT_EN_OFFSET 0
#define CPU_INT_EN 

//aud_writeb(base)	__raw_writeb(base+reg)

/* Register access macros */
#define i2sreg_readb(base, reg)		__raw_readb(base + I2S_REG_OFFSET(reg))
#define i2sreg_writeb(base, reg, value)	__raw_writeb((value), base + I2S_REG_OFFSET(reg))

#define afereg_readb(base, reg)		__raw_readb(base + AFE_REG_OFFSET(reg))
#define afereg_writeb(base, reg, value)	__raw_writeb((value), base + AFE_REG_OFFSET(reg))

/// @}

#endif // _MMP_REG_AUDIO_H_
