/**
 @file mmpf_system.h
 @brief Header File for the mmpf system.
 @author Truman Yang
 @version 1.0
*/

/** @addtogroup MMPF_SYS

These declaration should be sync with host mmp_system.h
@{
*/

#ifndef _MMPF_SYSTEM_H_
#define _MMPF_SYSTEM_H_

#include    "includes_fw.h"
#include "mmpf_pll.h"
#if (CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
#include 	"mmpf_pio.h"
#endif
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
#define COMPILER_DATE_LEN (12) /// "mmm dd yyyy"
#define COMPILER_TIME_LEN (10) /// "hh:mm:ss" 9 bytes but word alignment

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
typedef struct MMP_CUSTOMER {
    MMP_UBYTE   customer;
    MMP_UBYTE   project;
    MMP_UBYTE   hardware;
} MMP_CUSTOMER;

typedef struct MMP_RELEASE_VERSION {
    MMP_UBYTE   major;
    MMP_UBYTE   minor;
    MMP_USHORT  build;
} MMP_RELEASE_VERSION;

typedef struct MMP_RELEASE_DATE {
    MMP_UBYTE   year;
    MMP_UBYTE   month;
    MMP_UBYTE   day;
} MMP_RELEASE_DATE;

typedef struct MMP_SYSTEM_BUILD_VERSION {
    MMP_UBYTE szDate[COMPILER_DATE_LEN]; /// "mmm dd yyyy"
    MMP_UBYTE szTime[COMPILER_TIME_LEN]; /// "hh:mm:ss" 9 bytes
} MMP_SYSTEM_BUILD_VERSION;

#if (SUPPORT_CPU_CLOCK_SW == 1)||(SUPPORT_G0_CLOCK_SW == 1)
typedef struct _MMP_SYSTEM_POWER_FREQ {
    MMPF_PLL_FREQ CPUFreq;              ///< CPU clock frequency
    MMPF_PLL_FREQ SYSFreq;              ///< Group 0 clock frequency
    MMPF_PLL_FREQ DRAMFreq;             ///< DRAM clock frequency
} MMP_SYSTEM_POWER_FREQ;
#endif

typedef enum _MMPF_SYSTEM_POWER_LEVEL
{
    MMPF_POWER_LEVEL_0 = 0,             // power level 0 (lowest power)
    MMPF_POWER_LEVEL_1,                 // power level 1
    MMPF_POWER_LEVEL_2,                 // power level 2
    MMPF_POWER_LEVEL_3,                 // power level 3
    MMPF_POWER_LEVEL_4,                 // power level 4
    MMPF_POWER_LEVEL_5,                 // power level 5 (highest power)
    MMPF_POWER_LEVEL_MAX
} MMPF_SYSTEM_POWER_LEVEL;

#if (CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
typedef enum
{
	MMPF_WAKEUP_NONE = 0x0,
	MMPF_WAKEUP_GPIO,
	MMPF_WAKEUP_USB_RESUME,
	MMPF_WAKEUP_MAX
} MMPF_WAKEUP_EVNET;
#endif


typedef enum _MMPF_SYS_HMODULETYPE
{
	MMPF_SYS_HMODULETYPE_CPU_PERIF = 0x0,
    MMPF_SYS_HMODULETYPE_VI = 0x1,
    MMPF_SYS_HMODULETYPE_ISP = 0x2,
    MMPF_SYS_HMODULETYPE_JPG = 0x3,
    MMPF_SYS_HMODULETYPE_SCAL = 0x4,
    MMPF_SYS_HMODULETYPE_GPIO = 0x5,
    MMPF_SYS_HMODULETYPE_AUD = 0x6,
    MMPF_SYS_HMODULETYPE_DRAM = 0x7,
     
    MMPF_SYS_HMODULETYPE_MCI = 0x8,
    MMPF_SYS_HMODULETYPE_RAW = 0x9,
    MMPF_SYS_HMODULETYPE_DMA = 0xA,
    MMPF_SYS_HMODULETYPE_I2C = 0xB,
    MMPF_SYS_HMODULETYPE_USB = 0xC,
    MMPF_SYS_HMODULETYPE_H264 = 0xD,
    MMPF_SYS_HMODULETYPE_IBC = 0xE,
    MMPF_SYS_HMODULETYPE_GRA = 0xF,
     
    MMPF_SYS_HMODULETYPE_CPU_SRAM = 0x10,
    MMPF_SYS_HMODULETYPE_SD = 0x11,
    MMPF_SYS_HMODULETYPE_PWM = 0x12,
    MMPF_SYS_HMODULETYPE_PSPI = 0x13,
    MMPF_SYS_HMODULETYPE_USB_PHY = 0x14
} MMPF_SYS_HMODULETYPE;

typedef enum _MMPF_SYS_CLK
{

    MMPF_SYS_CLK_MCI  = 0,
    MMPF_SYS_CLK_CPU  = 1,
    MMPF_SYS_CLK_SCAL = 2,
    MMPF_SYS_CLK_JPG  = 3,
    MMPF_SYS_CLK_AUD  = 4,
    MMPF_SYS_CLK_VIF  = 5,
    MMPF_SYS_CLK_ISP  = 6,
    MMPF_SYS_CLK_GPIO = 7,
    MMPF_SYS_CLK_GRP0_NUM = 8,

    MMPF_SYS_CLK_DRAM = 8,
    MMPF_SYS_CLK_BS_SPI = 9,
    MMPF_SYS_CLK_RAWPROC = 10,
    MMPF_SYS_CLK_DMA  = 11,
    MMPF_SYS_CLK_I2CM = 12,
    MMPF_SYS_CLK_H264 = 13,
    MMPF_SYS_CLK_ICON = 14,
    MMPF_SYS_CLK_GRA  = 15,
    MMPF_SYS_CLK_GRP1_NUM = 16,

    MMPF_SYS_CLK_USB  = 16,
    MMPF_SYS_CLK_IBC  = 17,
    MMPF_SYS_CLK_PWM  = 18,
    MMPF_SYS_CLK_SD1  = 19,
    MMPF_SYS_CLK_SD   = 20,
    MMPF_SYS_CLK_SPI  = 21,
    MMPF_SYS_CLK_MIPI = 22,
    MMPF_SYS_CLK_CPU_PHL  = 23,
    MMPF_SYS_CLK_GRP2_NUM = 24,

    MMPF_SYS_CLK_AUD_CODEC = 24,
    MMPF_SYS_CLK_CIDC = 25,
    MMPF_SYS_CLK_GNR = 26,
    MMPF_SYS_CLK_SM   = 27,
    MMPF_SYS_CLK_COLR  = 28,
    MMPF_SYS_CLK_GRP3_NUM = 29,

    MMPF_SYS_CLK_MDL_NUM = 29,

} MMPF_SYS_CLK;
    
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
MMP_ERR MMPF_SYS_EnableClock(MMPF_SYS_CLK clocktype, MMP_BOOL bEnableclock);
MMP_ERR MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE moduletype, MMP_BOOL bResetRegister);
MMP_ERR MMPF_SYS_ResetSystem(MMP_UBYTE ubSpecialCase);

#if (OS_TYPE == OS_UCOSII)
MMP_ERR MMPF_SYS_EnterPSMode(MMP_BOOL bEnterPSMode);
MMP_ERR MMPF_SYS_EnterBypassMode(MMP_BOOL bEnterBypassMode);
MMP_ERR MMPF_SYS_InitializeHIF(void);
MMP_ERR MMPF_SYS_ProcessCmd(void);
MMP_ERR MMPF_SYS_GetFWEndAddr(MMP_ULONG *ulEndAddr);
MMP_ERR MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE moduletype, MMP_BOOL bResetRegister);
MMP_ERR	MMPF_SYS_ResetSystem(MMP_UBYTE ubSpecialCase);
void MMPF_SYS_InitFB(void) ;
void *MMPF_SYS_AllocFB(char *tag,MMP_ULONG size, MMP_USHORT align);
void MMPF_SYS_SetCurFBAddr(char *ptr);
void *MMPF_SYS_GetCurFBAddr(void) ;
MMP_BOOL MMPF_SYS_GetGNRClockStatus(void);
MMP_ERR MMPF_SYS_EnableGNRClock(MMP_BOOL bEnable);
MMP_ERR MMPF_SYS_EnterSelfSleepMode(void);
MMP_ERR MMPF_Self_Rom_Boot(void);
#endif

#endif	//_MMPF_SYSTEM_H_

/** @}*/ //end of MMPF_SYS