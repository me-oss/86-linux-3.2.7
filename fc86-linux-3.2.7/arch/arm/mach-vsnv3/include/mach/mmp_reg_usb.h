//==============================================================================
//
//  File        : mmp_register_usb.h
//  Description : INCLUDE File for the USB register map.
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_USB_H_
#define _MMP_REG_USB_H_
#include "includes_fw.h"
#if (CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
#include    "mmp_register.h"
/*
#define     USB_DMA_BASE_ADDR		(0x80001400)  //USB DMA registers base address
#define 	USB_CTL_BASE_ADDR		(0x80001000)  //USB control registers base address
*/
/** @addtogroup MMPH_reg
@{
*/

// *****************************************************************************
//   USB base control register structure (0x8000 6000)
// *****************************************************************************
typedef struct _AITS_USB_DMA {
    AIT_REG_B   USB_CTL_SEL;                                          	// 0x00
   	AIT_REG_B   USB_SIDDQ;                                          	// 0x01
   	AIT_REG_B   USB_LOP_CTL;                                          	// 0x02
   	AIT_REG_B   USB_UTMI_WRA_CTL;                                       // 0x03
   	AIT_REG_B   USB_PLL_CTL;                                          	// 0x04
   	AIT_REG_B   USB_XO_CTL;                                          	// 0x05
   	AIT_REG_B   USB_PHY_PROBE_SEL;                                      // 0x06
   	AIT_REG_B   USB_TRCV_TM_CTL;                                        // 0x07
   	AIT_REG_B   USB_POR_BYPASS;                                 		// 0x08
   	AIT_REG_B   USB_MODE_CTL;                                      		// 0x09
   	AIT_REG_B   USB_BIST_EN;                                          	// 0x0A
   	AIT_REG_B   USB_BIST_SR;                                          	// 0x0B
   	AIT_REG_B                   _0x0C[0x4];								//0x0C~0x0F reserved
   	
   	AIT_REG_B   USB_PHY_SPI_CTL0;                                       // 0x10
   	AIT_REG_B                   _0x11[0x1];								// 0x11
   	AIT_REG_B   USB_PHY_SPI_CTL1;                                       // 0x12
   	AIT_REG_B   USB_PHY_SPI_CTL2;                                       // 0x13
   	AIT_REG_B                   _0x14[0x28];							//0x14~0x3B reserved
   	AIT_REG_B   USB_OTG_TM;                                          	// 0x3C
   	AIT_REG_B   USB_OTG_CTL;                                          	// 0x3D
   	AIT_REG_B   USB_OTG_SRAM_CTL;                                       // 0x3E
   	AIT_REG_B                   _0x3F[0x1];								// 0x3F
   	
   	
   	AIT_REG_B   USB_UTMI_CTL0;                                         	// 0x40
   	AIT_REG_B   USB_UTMI_CTL1;                                          // 0x41
   	AIT_REG_B   USB_UTMI_CTL2;                                          // 0x42
   	AIT_REG_B   USB_UTMI_CTL3;                                          // 0x43
   	AIT_REG_B   USB_UTMI_CTL4;                                         	// 0x44
   	AIT_REG_B                   _0x45[0xB];
   	
   	
   	AIT_REG_B   USB_FARADAY_PHY_CTL0;                                   // 0x50
   	AIT_REG_B   USB_FARADAY_PHY_CTL1;                                   // 0x51
   	AIT_REG_B   USB_FARADAY_PHY_CTL2;                                   // 0x52
   	AIT_REG_B                   _0x53[0x11];							// 0x53~0x63 reserved
   	AIT_REG_D	USB_SOF_CNT;
   	#if (CHIP == VSN_V2)
   	AIT_REG_B                   _0x68[0x18];							// 0x68~0x7F reserved
   	AIT_REG_B	USB_DMA_CTL;											// 0x80
   	AIT_REG_B	USB_DMA_DESC_CNT;										// 0x81
   	#endif
   	#if (CHIP == VSN_V3)
   	AIT_REG_B	USB_CLK_GATED_SEL;										// 0x68
   	AIT_REG_B                   _0x69[0x17];							// 0x69~0x7F reserved
   	AIT_REG_B	USB_DMA1_CTL1;											// 0x80
   	AIT_REG_B	USB_DMA1_CTL2;											// 0x81
   	#endif
   	
   	AIT_REG_B	USB_DMA_INT_EN;											// 0x82
   		/*-DEFINE-----------------------------------------------------*/
   		#define USB_INT_DMA1_DONE_EN		0x01
   		#define USB_INT_DMA1_SET_TXPKTRDY	0x02
   		#if (CHIP == VSN_V3)
   		#define USB_INT_DMA1_DESC_CNT_EN	0x04
   		#define USB_INT_DMA2_DONE_EN		0x08
   		#define USB_INT_DMA2_SET_TXPKTRDY	0x10
   		#define USB_INT_DMA2_DESC_CNT_EN	0x20
   		#define USB_INT_DMA3_DONE_EN		0x40
   		#endif
   		/*------------------------------------------------------------*/
   	AIT_REG_B	USB_DMA_INT_SR;											// 0x83
   	
   	#if (CHIP == VSN_V2)
   	AIT_REG_D	USB_DMA_FB_ST_ADDR;										// 0x84
   	AIT_REG_B	USB_DMA_FIXED_DATA;										// 0x88
   	AIT_REG_B					_0x89[0x1];								// 0x89 reserved
   	AIT_REG_W	USB_DMA_CMD_ADDR;										// 0x8A
   	AIT_REG_B                   _0x8C[0x8];								// 0x8C~0x93 reserved
   	AIT_REG_D	USB_DMA_TAR_AND_VAL;									// 0x94
   	AIT_REG_D	USB_DMA_TAR_OR_VAL;										// 0x98
   	AIT_REG_B                   _0x9C[0x14];							// 0x9C~0xAF reserved
   	AIT_REG_B	USB_CLK_GATED_SEL;										// 0xB0
   	AIT_REG_B                   _0xB1[0x0F];							// 0xB1~0xBF reserved
   	#endif
   	#if (CHIP == VSN_V3)
   	AIT_REG_D	USB_DMA1_FB_ST_ADDR;									// 0x84
   	AIT_REG_B	USB_DMA1_FIXED_DATA;									// 0x88
   	AIT_REG_B					_0x89[0x1];								// 0x89 reserved
   	AIT_REG_W	USB_DMA1_CMD_ADDR;										// 0x8A
   	AIT_REG_W	USB_DMA1_DESC_CNT;										// 0x8C
   	AIT_REG_W	USB_DMA1_DESC_CNT_NUM;									// 0x8E
   	AIT_REG_W	USB_DMA1_PKT_BYTE_CNT;									// 0x90
   	AIT_REG_B                   _0x92[0x2];								// 0x92~0x93 reserved
   	AIT_REG_D	USB_DMA1_TAR_AND_VAL;									// 0x94
   	AIT_REG_D	USB_DMA1_TAR_OR_VAL;									// 0x98
   	AIT_REG_B                   _0x9C[0x4];								// 0x9C~0x9F reserved
   	AIT_REG_B	USB_DMA2_CTL1;											// 0xA0
   	AIT_REG_B	USB_DMA2_CTL2;											// 0xA1
   	AIT_REG_B                   _0xA2[0x2];								// 0xA2~0xA3 reserved
   	AIT_REG_D	USB_DMA2_FB_ST_ADDR;									// 0xA4
   	AIT_REG_B	USB_DMA2_FIXED_DATA;									// 0xA8
   	AIT_REG_B					_0xA9[0x1];								// 0xA9 reserved
   	AIT_REG_W	USB_DMA2_CMD_ADDR;										// 0xAA
   	AIT_REG_W	USB_DMA2_DESC_CNT;										// 0xAC
   	AIT_REG_W	USB_DMA2_DESC_CNT_NUM;									// 0xAE
   	AIT_REG_W	USB_DMA2_PKT_BYTE_CNT;									// 0xB0
   	AIT_REG_B                   _0xB2[0x2];								// 0xB2~0xB3 reserved
   	AIT_REG_D	USB_DMA2_TAR_AND_VAL;									// 0xB4
   	AIT_REG_D	USB_DMA2_TAR_OR_VAL;									// 0xB8
   	AIT_REG_B                   _0xBC[0x4];								// 0xBC~0xBF reserved
   	#endif
   	
   	AIT_REG_B	USB_INCOMPTX_CTL;										// 0xC0
   	AIT_REG_B					_0xC1[0x1];								// 0xC1 reserved
   	AIT_REG_B	USB_EP_DISABLE;											// 0xC2
   	AIT_REG_B                   _0xC3[0xD];								// 0xC3~0xCF reserved
   	AIT_REG_B	USB_DMA_INT2_EN;
   	AIT_REG_B	USB_DMA_INT2_SR;
   	#if (CHIP == VSN_V2)
   	AIT_REG_B					_0xD2[0x2];								// 0xD2~0xD3 reserved
   	AIT_REG_W	USB_PKT_BYTE_CNT;
   	#endif
   	#if (CHIP == VSN_V3)
   	AIT_REG_B					_0xD2[0xE];								// 0xD2~0xDF reserved
   	AIT_REG_B	USB_DMA3_CTL1;											// 0xE0
   	AIT_REG_B	USB_DMA3_POST_WAIT;										// 0xE1
   	AIT_REG_B	USB_DMA3_CTL2;											// 0xE2
   	AIT_REG_B					_0xE3[0x1];								// 0xE3 reserved
   	AIT_REG_D	USB_DMA3_FB_ST_ADDR;									// 0xE4
   	AIT_REG_W	USB_DMA3_FIFO_ADDR;										// 0xE8
   	AIT_REG_W	USB_DMA3_CMD_ADDR;										// 0xEA
   	AIT_REG_W	USB_DMA3_PKT_BYTE;										// 0xEC
   	AIT_REG_W	USB_DMA3_PKT_BYTE_LAST;									// 0xEE
   	AIT_REG_D	USB_DMA3_PKT_SUM;										// 0xF0
   	AIT_REG_D	USB_DMA3_TAR_AND_VAL;									// 0xF4
   	AIT_REG_D	USB_DMA3_TAR_OR_VAL;									// 0xF8
   	#endif
} AITS_USB_DMA, *AITPS_USB_DMA;

union USB_FIFO_RW{
	AIT_REG_B FIFO_B;
	AIT_REG_W FIFO_W;
	AIT_REG_D FIFO_D;
};

typedef struct _AITS_USB_EP {
   	AIT_REG_W   USB_EP_TX_MAXP;
   	AIT_REG_W   USB_EP_TX_CSR; 
  	AIT_REG_W   USB_EP_RX_MAXP; 
   	AIT_REG_W   USB_EP_RX_CSR; 
   	AIT_REG_W   USB_EP_COUNT;
   	AIT_REG_B                   _0x9[0x5]; 
   	AIT_REG_B   USB_CFG_DATA; 
} USB_EP_CTL, *USBS_EP_CTL;

// *****************************************************************************
//   USB DMA register structure (0x8000 A800)
// *****************************************************************************
typedef struct _AITS_USB_CTL {
   	AIT_REG_B   USB_FADDR;                                          		// 0x00
   	AIT_REG_B   USB_POWER;                                          		// 0x01
   	AIT_REG_W   USB_TX_INT_SR;                                          	// 0x02
   	AIT_REG_W   USB_RX_INT_SR;                                          	// 0x04
   	AIT_REG_W   USB_TX_INT_EN;                                          	// 0x06
   	AIT_REG_W   USB_RX_INT_EN;                                          	// 0x08
   	AIT_REG_B   USB_INT_EVENT_SR;                                          // 0x0A
   	AIT_REG_B   USB_INT_EVENT_EN;                                          	// 0x0B
   	AIT_REG_W   USB_FRAME_NUM;                                          	// 0x0C
   	AIT_REG_B   USB_INDEX_EP_SEL;                                          	// 0x0E
   	AIT_REG_B   USB_TESTMODE;                                          		// 0x0F
   	USB_EP_CTL	USB_INDEX_EP[0x1];  	   									// 0x10
   	//#if (CHIP == VSN_V2)
   	//union USB_FIFO_RW   USB_FIFO_EP[0x6];                                   // 0x20
   	//AIT_REG_B                   _0x38[0x34]; 								// 0x38~0x6B
   	//#endif
   //	#if (CHIP == VSN_V3)
   	union USB_FIFO_RW   USB_FIFO_EP[0x8];                                   // 0x20
   	AIT_REG_B                   _0x40[0x2C]; 								// 0x40~0x6B
   //	#endif
   	AIT_REG_W   USB_ADT_HW_VER;                                     		// 0x6C
   	AIT_REG_B                   _0x6E[0xA]; 
   	AIT_REG_B   USB_ADT_EP_INFO;                                          	// 0x78
   	AIT_REG_B   USB_ADT_RAM_INFO;                                          	// 0x79
   	AIT_REG_B   USB_ADT_LINK_INFO;                                          // 0x7A
   	AIT_REG_B   USB_ADT_VP_LEN;                                          	// 0x7B
   	AIT_REG_B   USB_ADT_HS_EOF;                                          	// 0x7C
   	AIT_REG_B   USB_ADT_FS_EOF;                                          	// 0x7D
   	AIT_REG_B   USB_ADT_LS_EOF;                                          	// 0x7E
   	AIT_REG_B   USB_ADT_SOFT_RST;                                          	// 0x7F
   	AIT_REG_B                   _0x80[0x80];
   	//#if (CHIP == VSN_V2)
   	//USB_EP_CTL	USB_EP[6];													// 0x100
   	//AIT_REG_B                   _0x140[0x1E0]; 
   	//#endif
   	//#if (CHIP == VSN_V3)
   	USB_EP_CTL	USB_EP[8];													// 0x100
   	AIT_REG_B                   _0x160[0x1C0];
   	//#endif
   	AIT_REG_W   USB_RX_DPKBUFDIS;                                          	// 0x340
   	AIT_REG_W   USB_TX_DPKBUFDIS;                                          	// 0x342
   	AIT_REG_W   USB_UCH;                                          			// 0x344
   	AIT_REG_W   USB_HSRTN;                                          		// 0x346
  
} AITS_USB_CTL, *AITPS_USB_CTL;
#endif

////////////////////////////////////
// Register definition
//

#if !defined(BUILD_FW)
// DMA OPR

#if (CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)

#endif

#endif
/// @}

#endif // _MMPH_REG_USB_H_
