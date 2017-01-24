//==============================================================================
//
//  File        : mmp_reg_i2cm.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_I2CM_H_
#define _MMP_REG_I2CM_H_

#include    "mmp_register.h"


//--------------------------------
// I2CM structure (0x8000 C000)
//--------------------------------
typedef struct _AITS_I2CMS {
    AIT_REG_B   I2CM_CTL;                                               // 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define     I2CM_MASTER_EN          0x01
        #define     I2CM_REG_16_MODE        0x02
        #define     I2CM_REG_8_MODE         0x00
        #define     I2CM_SDA_OH_MODE        0x04
        #define     I2CM_SDA_OD_MODE        0x00
        #define     I2CM_SCK_OH_MODE        0x00
        #define     I2CM_SCK_OD_MODE        0x08
        #define     I2CM_STOP_2_NOACK       0x00
        #define     I2CM_CONTI_2_NOACK      0x10
        #define     I2CM_S_ACK_MODE_EN		0x20
        #define     I2CM_RFCL_MODE_EN		0x40
        #define     I2CM_REPEAT_MODE_EN		0x80
         /*------------------------------------------------------------*/
    AIT_REG_B	I2CM_SLAV_ADDR;											// 0x01
    	/*-DEFINE-----------------------------------------------------*/
        #define     I2CM_WRITE_MODE         0x80
        #define     I2CM_READ_MODE          0x00
        /*------------------------------------------------------------*/
    #if 0//(CHIP == P_V2)
    AIT_REG_B   I2CM_SET_CNT;                                           // 0x02
    #endif
    #if 1//(CHIP == VSN_V2)||(CHIP == VSN_V3)
    AIT_REG_B	_x02;
    #endif
    
    #if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
    AIT_REG_B	I2CM_SLAV_ADDR1;										// 0x03
    #endif
    AIT_REG_W   I2CM_SCK_DIVIDER;                                       // 0x04
    AIT_REG_B   I2CM_DATA_HOLD_CNT;                                     // 0x06
    AIT_REG_B   I2CM_CMDSET_WAIT_CNT;                                   // 0x07
    #if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
    	/*-DEFINE-----------------------------------------------------*/
    	#define I2CM_DELAY_WAIT_EN			0x20
    	#define I2CM_INPUT_FILTERN_EN		0x40
    	#define I2CM_10BITMODE_EN			0x80	
    	/*------------------------------------------------------------*/
    #endif
    AIT_REG_W   I2CM_INT_HOST_EN;                                       // 0x08
    AIT_REG_W   I2CM_INT_HOST_SR;                                       // 0x0A
    AIT_REG_W   I2CM_INT_CPU_EN;                                        // 0x0C
    	/*-DEFINE-----------------------------------------------------*/
    	#define I2CM_SLAVE_NOACK_EN			0x04
    	#define I2CM_TX_ONE_SET_DONE_EN		0x02
    	#define I2CM_TX_FINISH_EN			0x01
    	/*------------------------------------------------------------*/
    AIT_REG_W   I2CM_INT_CPU_SR;                                        // 0x0E
        /*-DEFINE-----------------------------------------------------*/
        #define     I2CM_TX_DONE             0x0001
        #define     I2CM_SET_TX_DONE         0x0002
        #define     I2CM_SLAVE_NO_ACK        0x0004
        #define     I2CM_TXFIFO_EMPTY        0x0100
        #define     I2CM_TXFIFO_LE_THD       0x0200
        #define     I2CM_TXFIFO_FULL         0x0400
        #define     I2CM_RXFIFO_EMPTY        0x0800
        #define     I2CM_RXFIFO_GE_THD       0x1000
        #define     I2CM_RXFIFO_FULL         0x2000
        #define     I2CM_RXFIFO_OF           0x4000
        /*------------------------------------------------------------*/
    union {                                                             // 0x10
    AIT_REG_B   B[4];
    AIT_REG_W   W[2];
    AIT_REG_D   D[1];
    }           I2CM_TXFIFO_DATA;
    union {                                                             // 0x14
    AIT_REG_B   B[4];
    AIT_REG_W   W[2];
    AIT_REG_D   D[1];
    }           I2CM_RXFIFO_DATA;
    AIT_REG_B   I2CM_TXFIFO_THD;										// 0x18
    AIT_REG_B   I2CM_TXFIFO_SPC;										// 0x19
    AIT_REG_B   I2CM_RXFIFO_THD;	    								// 0x1A
    AIT_REG_B   I2CM_RXFIFO_SPC;    									// 0x1B
    AIT_REG_B   I2CM_RST_FIFO_SW;                                       // 0x1C
        /*-DEFINE-----------------------------------------------------*/
        #define     I2CM_FIFO_RST          0x01
        /*------------------------------------------------------------*/
    #if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
    AIT_REG_B   I2CM_WAIT_DELAY_CYC;									// 0x1D									
    #endif
    #if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
    AIT_REG_B   I2CM_CLK_STRETCH_EN;									// 0x1E
    	/*-DEFINE-----------------------------------------------------*/
        #define     I2CM_STRETCH_ENABLE    0x01
        /*------------------------------------------------------------*/
    #endif
    AIT_REG_B	_x1F;
    #if 1//(CHIP == VSN_V2)||(CHIP == VSN_V3)
    AIT_REG_W   I2CM_SCL_DUTY_CNT;                                      // 0x20
    AIT_REG_W   I2CM_SET_CNT;                                           // 0x22
    AIT_REG_B   _X24[0xDC];                                             // 0x24~0xFF
    #endif
    #if 0//(CHIP == P_V2)
    AIT_REG_D   _X20[56];												// 0x20~0xFF
    #endif
} AITS_I2CMS, *AITPS_I2CMS;

typedef struct _AITS_I2CM {
	#if 0//(CHIP == P_V2)
    AITS_I2CMS    I2CMS[3];
	#endif
	#if 1//(CHIP == VSN_V2)||(CHIP == VSN_V3)
	AITS_I2CMS    I2CMS[2];
	#endif
} AITS_I2CM, *AITPS_I2CM;
#if 0//	!defined(BUILD_FW)
// I2CM OPR

#define I2CM_CTL                    (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_CTL           )))
#define I2CM_SLAV_ADDR				(I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_SLAV_ADDR     )))
#if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
#define I2CM_SLAV_ADDR1				(I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_SLAV_ADDR1    )))
#endif
#define I2CM_SET_CNT                (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_SET_CNT       )))
#define I2CM_SCK_DIVIDER            (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_SCK_DIVIDER   )))
#define I2CM_DATA_HOLD_CNT          (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_DATA_HOLD_CNT )))

#define I2CM_INT_HOST_EN            (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_INT_HOST_EN   )))
#define I2CM_INT_HOST_SR            (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_INT_HOST_SR   )))

#define I2CM_TXFIFO_DATA            (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_TXFIFO_DATA   )))
#define I2CM_RXFIFO_DATA            (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_RXFIFO_DATA   )))
#define I2CM_RST_FIFO_SW            (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_RST_FIFO_SW   )))
#if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
#define I2CM_WAIT_DELAY_CYC         (I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_WAIT_DELAY_CYC   )))
#endif
#if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
#define I2CM_CLK_STRETCH_EN			(I2CM_BASE+(MMP_ULONG)(&(((AITPS_I2CM)0)->I2CMS[0].I2CM_CLK_STRETCH_EN	)))
#endif
#endif
#endif // _MMP_REG_I2CM_H_