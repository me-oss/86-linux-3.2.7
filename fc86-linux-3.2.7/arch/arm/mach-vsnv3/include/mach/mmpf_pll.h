//==============================================================================
//
//  File        : mmpf_pll.h
//  Description : INCLUDE File for the Firmware PLL Driver.
//  Author      : Rogers Chen
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMPF_PLL_H_
#define _MMPF_PLL_H_

//#include    "includes_fw.h"
#include    "mmp_err.h"


//==============================================================================
//
//                              COMPILER OPTION
//
//==============================================================================


//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#define EXT_PMCLK_CKL 12000
#define MMPF_PLL_GROUP0     0x01
#define MMPF_PLL_GROUP1     0x02
#define MMPF_PLL_GROUPNULL  0x00
#if 0//(CHIP == VSN_V2)
#define MAX_GROUP_NUM 	5 	//Group 0 ==> GBL_CLK
							//Group 1 ==> DRAM_CLK
							//Group 2 ==> USBPHY_CLK
							//Group 3 ==> RX_BIST_CLK
							//Group 4 ==> SENSOR_CLK
#endif
#if 1//(CHIP == VSN_V3)
#define MAX_GROUP_NUM 	7 	//Group 0 ==> GBL_CLK
							//Group 1 ==> DRAM_CLK
							//Group 2 ==> USBPHY_CLK
							//Group 3 ==> RX_BIST_CLK
							//Group 4 ==> SENSOR_CLK
							//Group 5 ==> AUDIO_CLK
							//Group 6 ==> COLOR_CLK(ISP)
#endif
//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================


//Group 0 ==> GBL_CLK
//Group 1 ==> DRAM_CLK
//Group 2 ==> USBPHY_CLK
//Group 3 ==> RX_BIST_CLK
//Group 4 ==> SENSOR_CLK
typedef enum _MMPF_PLL_MODE {
	MMPF_PLL_24CPU_24G0134_X = 0x0,
	MMPF_PLL_48CPU_48G0134_X,
	MMPF_PLL_96CPU_96G0134_X,
	MMPF_PLL_120CPU_120G0134_X,
	MMPF_PLL_132CPU_132G0134_X,
	MMPF_PLL_133CPU_133G0134_X,
	MMPF_PLL_144CPU_144G0134_X,
	MMPF_PLL_166CPU_166G0134_X,
	MMPF_PLL_266CPU_144G0134_X,
	MMPF_PLL_300CPU_144G0134_X,
	MMPF_PLL_332CPU_192G034_166G1_X,
	MMPF_PLL_332CPU_200G034_166G1_X,
	MMPF_PLL_168CPU_133G0134_X,
	MMPF_PLL_192CPU_133G0134_X,
	MMPF_PLL_400CPU_144G0134_X,
	MMPF_PLL_400CPU_166G0134_X,
	MMPF_PLL_400CPU_192G034_200G1_X,
	MMPF_PLL_400CPU_240G034_192G1_X,
	MMPF_PLL_498CPU_166G0134_X,
	MMPF_PLL_498CPU_192G034_200G1_X,
	MMPF_PLL_498CPU_216G034_192G1_X,
	MMPF_PLL_498CPU_264G034_192G1_X,
	MMPF_PLL_600CPU_166G0134_X,
	MMPF_PLL_600CPU_192G0134_X,
	MMPF_PLL_600CPU_216G0_192G134_X,
	MMPF_PLL_600CPU_240G0_192G134_X,
	MMPF_PLL_600CPU_264G0_192G134_X,
	MMPF_PLL_600CPU_192G034_166G1_X,
	MMPF_PLL_600CPU_192G034_200G1_X,
	MMPF_PLL_AUDIO_48K,
	MMPF_PLL_AUDIO_44d1K,
	MMPF_PLL_AUDIO_32K,
	MMPF_PLL_AUDIO_24K,
	MMPF_PLL_AUDIO_22d05K,
	MMPF_PLL_AUDIO_16K,
	MMPF_PLL_AUDIO_12K,
	MMPF_PLL_AUDIO_11d025K,
	MMPF_PLL_AUDIO_8K,
    MMPF_PLL_MODE_NUMBER
} MMPF_PLL_MODE;

typedef enum _MMPF_PLL_FREQ {
    MMPF_PLL_FREQ_600MHz = 0,
    MMPF_PLL_FREQ_498MHz,
    MMPF_PLL_FREQ_400MHz,
    MMPF_PLL_FREQ_333MHz,
    MMPF_PLL_FREQ_332MHz,
    MMPF_PLL_FREQ_300MHz,
	MMPF_PLL_FREQ_266MHz,
	MMPF_PLL_FREQ_264MHz_PLL1,
	MMPF_PLL_FREQ_240MHz_PLL0,
	MMPF_PLL_FREQ_240MHz_PLL1,
	MMPF_PLL_FREQ_216MHz_PLL0,
	MMPF_PLL_FREQ_216MHz_PLL1,
	MMPF_PLL_FREQ_200MHz_PLL0,
	MMPF_PLL_FREQ_200MHz_PLL1,
	MMPF_PLL_FREQ_192MHz_PLL0,
	MMPF_PLL_FREQ_192MHz_PLL1,
	MMPF_PLL_FREQ_168MHz,
    MMPF_PLL_FREQ_166MHz_PLL0,
    MMPF_PLL_FREQ_166MHz_PLL1,
    MMPF_PLL_FREQ_162MHz,
    MMPF_PLL_FREQ_156MHz,
    MMPF_PLL_FREQ_144MHz,
    MMPF_PLL_FREQ_133MHz,
    MMPF_PLL_FREQ_132MHz,
    MMPF_PLL_FREQ_120MHz,
	MMPF_PLL_FREQ_96MHz,
    MMPF_PLL_FREQ_60MHz,
    MMPF_PLL_FREQ_54MHz,
    MMPF_PLL_FREQ_48MHz,
    MMPF_PLL_FREQ_39MHz,
    MMPF_PLL_FREQ_24MHz, 
    MMPF_PLL_FREQ_EXT_CLK,
    MMPF_PLL_FREQ_TOTAL_NO
} MMPF_PLL_FREQ;
typedef enum _MMPF_PLL_MIPI_FREQ {
   
    MMPF_PLL_MIPI_FREQ_TOTAL_NO
} MMPF_PLL_MIPI_FREQ;

typedef enum _MMPF_PLL_ID {
    MMPF_PLL_ID_0 = 0,
    MMPF_PLL_ID_1,
    #if 1//(CHIP == VSN_V3)
	MMPF_PLL_ID_2,
    #endif
    MMPF_PLL_ID_PMCLK,
    MMPF_PLL_ID_MAX
} MMPF_PLL_ID;

typedef enum _MMPF_GROUP_SRC {
    MMPF_GUP_SRC_PLL_0 = 0,
    MMPF_GUP_SRC_PLL_1,
    #if 1//(CHIP == VSN_V3)
    MMPF_GUP_SRC_PLL_2,
    #endif
    MMPF_GUP_SRC_PMCLK,
    MMPF_GUP_SRC_ZERO,
    MMPF_GUP_SRC_NULL
} MMPF_GROUP_SRC;

typedef enum _MMPF_PLL_SRC {
	MMPF_PLL_SRC_PMCLK = 0x0,
    MMPF_PLL_SRC_DPLL0,
    #if 0//(CHIP == VSN_V2)
    MMPF_PLL_SRC_DPLL1
    #endif
    #if 1//(CHIP == VSN_V3)
    MMPF_PLL_SRC_DPLL1,
    MMPF_PLL_SRC_DPLL2
    #endif
} MMPF_PLL_SRC;

typedef enum _MMPF_PLL_HDMI_MODE {
    MMPF_PLL_HDMI_27MHZ = 0,
    MMPF_PLL_HDMI_74_25MHZ,
    MMPF_PLL_HDMI_SYNC_DISPLAY
} MMPF_PLL_HDMI_MODE;


//==============================================================================
//
//                              VARIABLES
//
//==============================================================================


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR MMPF_PLL_Setting(MMPF_PLL_MODE PLLMode, MMP_BOOL KeepG0);
MMP_ERR MMPF_PLL_SetVSNV2PLL(MMPF_PLL_MODE target_pll_mode, MMP_BOOL KeepG0);
MMP_ERR MMPF_PLL_SetVSNV3PLL(MMPF_PLL_MODE target_pll_mode, MMP_BOOL KeepG0);
void MMPF_PLL_WaitCount(MMP_ULONG count);
MMP_ERR MMPF_PLL_GetCPUFreq(MMP_ULONG *ulCPUFreq);
MMP_ERR MMPF_PLL_GetGroupFreq(MMP_UBYTE ubGroupNum, MMP_ULONG *ulGroupFreq);
//==============================================================================
//
//                              MACRO FUNCTIONS
//
//==============================================================================


#endif // _INCLUDES_H_
