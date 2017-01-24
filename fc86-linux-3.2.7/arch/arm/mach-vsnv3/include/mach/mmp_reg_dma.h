//==============================================================================
//
//  File        : mmp_register_dma.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_DMA_H_
#define _MMP_REG_DMA_H_

#include    "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#define DMA_M_NUM 1
#define DMA_R_NUM 1

// ********************************
//   DMA_M structure (0x8000 7610)
// ********************************
typedef struct _AITS_DMA_M {
    AIT_REG_D   DMA_M_SRC_ADDR;                                          // 0x10
    AIT_REG_D   DMA_M_BYTE_CNT;                                          // 0x14
    /*-DEFINE-----------------------------------------------------*/  
        #define DMA_M_MAX_BYTE_CNT       0xFFFFFF
    /*------------------------------------------------------------*/    
    AIT_REG_D   DMA_M_DST_ADDR;                                          // 0x18
    AIT_REG_D   DMA_M_LEFT_BYTE;                                         // 0x1C
} AITS_DMA_M, *AITPS_DMA_M;



// ********************************
//   DMA_M_LOFFS structure (0x8000 7620)
// ********************************
typedef struct _AITS_DMA_M_OFFL {
    AIT_REG_D   DMA_M_SRC_LOFFS_W;                                          // 0x70
    AIT_REG_D   DMA_M_SRC_LOFFS_OFFS;                                       // 0x74
    AIT_REG_D   DMA_M_DST_LOFFS_W;                                          // 0x78
    AIT_REG_D   DMA_M_DST_LOFFS_OFFS;                                       // 0x7C
} AITS_DMA_M_LOFFS, *AITPS_DMA_M_LOFFS;


// ********************************
//   DMA_R structure (0x8000 7630)
// ********************************
typedef struct _AITS_DMA_R {
    AIT_REG_D   DMA_R_SRC_ADDR;                                          // 0x30
    AIT_REG_W   DMA_R_SRC_OFST;                                          // 0x34
    /*-DEFINE-----------------------------------------------------*/
        #define DMA_R_MAX_OFST          0x7FFF
    /*------------------------------------------------------------*/
    AIT_REG_W                           _x06;   
    AIT_REG_D   DMA_R_DST_ADDR;                                          // 0x38
    AIT_REG_W   DMA_R_DST_OFST;                                          // 0x3C
    AIT_REG_W                           _x3E;
    AIT_REG_W   DMA_R_PIX_W;                                             // 0x40
    AIT_REG_W   DMA_R_PIX_H;                                             // 0x42
    /*-DEFINE-----------------------------------------------------*/
        #define DMA_R_MAX_PIX_W_H       0x2000
    /*------------------------------------------------------------*/
    AIT_REG_B   DMA_R_CTL;                                              // 0x44
    /*-DEFINE-----------------------------------------------------*/
        #define DMA_R_NO                0x00
        #define DMA_R_90            	0x01
        #define DMA_R_270           	0x02
        #define DMA_R_180           	0x03
        #define DMA_R_BLK_16X16         0x04
        #define DMA_R_BLK_8X8           0x00
        #define DMA_R_BPP_24            0x10
        #define DMA_R_BPP_16            0x08
        #define DMA_R_BPP_8             0x00
    /*------------------------------------------------------------*/ 
    AIT_REG_B	DMA_R_MIRROR_EN;										// 0x45
    /*-DEFINE-----------------------------------------------------*/
    	#define DMA_R_MIRROR_DISABLE    0x00
        #define DMA_R_H_ENABLE          0x01
        #define DMA_R_V_ENABLE          0x02
    /*------------------------------------------------------------*/ 		   
    AIT_REG_W                           _x46[5];     
} AITS_DMA_R, *AITPS_DMA_R;

// ********************************
//   DMA structure (0x8000 7600)
// ********************************
typedef struct _AITS_DMA {
    AIT_REG_B   DMA_EN;                                         		// 0x00
    	/*-DEFINE-----------------------------------------------------*/
    	#if (CHIP == VSN_V2)
        #define DMA_M_ENABLE               0x01
        #define DMA_R_ENABLE               0x04
        #endif
        #if (CHIP == VSN_V3)
        #define DMA_M0_ENABLE               0x01
        #define DMA_M1_ENABLE               0x02
        #define DMA_R0_ENABLE               0x04
        #endif
    	/*------------------------------------------------------------*/
    #if (CHIP == VSN_V2)
    AIT_REG_B   DMA_M0_LOFFS_EN;                                         // 0x01
    #endif
    #if (CHIP == VSN_V3)
    AIT_REG_B   DMA_M_LOFFS_EN;                                          // 0x01
    #endif
    	/*-DEFINE-----------------------------------------------------*/
    	#if (CHIP == VSN_V2)
        #define DMA_LOFFS_ENABLE         0x01
        #endif
        #if (CHIP == VSN_V3)
        #define DMA_LOFFS_ENABLE_M0      0x01
        #define DMA_LOFFS_ENABLE_M1      0x02
        #endif
        /*------------------------------------------------------------*/
    AIT_REG_B   						_x02[0x2];                              
    AIT_REG_B   DMA_INT_CPU_EN;                                         // 0x04
    AIT_REG_B   						_x05;        
    AIT_REG_B   DMA_INT_CPU_SR;                                       	// 0x06
    AIT_REG_B   						_x07;
    AIT_REG_B   DMA_INT_HOST_EN;                                        // 0x08        
    AIT_REG_B   						_x09;
    AIT_REG_B   DMA_INT_HOST_SR;                                      	// 0x0A
        /*-DEFINE-----------------------------------------------------*/
        #define DMA_INT_M0               0x01
        #if (CHIP == VSN_V3)
        #define DMA_INT_M1               0x02
        #endif
        #define DMA_INT_R0               0x04      
        /*------------------------------------------------------------*/
    AIT_REG_B   						_x0B[0x5];
    #if (CHIP == VSN_V2)
    AITS_DMA_M   		DMA_M[DMA_M_NUM];
    AITS_DMA_M_LOFFS 	DMA_M_LOFFS[DMA_M_NUM];
    AITS_DMA_R   		DMA_R[DMA_R_NUM];
    #endif
    #if (CHIP == VSN_V3)
    AITS_DMA_M   		DMA_M0;
    AITS_DMA_M_LOFFS 	DMA_M_LOFFS0;
    AITS_DMA_R   		DMA_R[DMA_R_NUM];
    AITS_DMA_M   		DMA_M1;
    AITS_DMA_M_LOFFS 	DMA_M_LOFFS1;
    #endif
} AITS_DMA, *AITPS_DMA;
////////////////////////////////////
// Register definition
//

#if !defined(BUILD_FW)


#endif

#if (CHIP == VSN_V2)
//Temp Code for porting P_V2 to VSN_V2
#define DMA_INT_M1               DMA_INT_M0
#define DMA_INT_R1               DMA_INT_R0       
#endif
/// @}

#endif // _MMPH_REG_DMA_H_
