//==============================================================================
//
//  File        : mmpf_uart.h
//  Description : INCLUDE File for the Firmware Timer Control Driver
//  Author      : Jerry Lai
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMPF_TIMER_H_
#define _MMPF_TIMER_H_

//#include    "includes.h"
#include <mach/mmp_err.h>
#include <mach/mmpf_typedef.h>

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

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================


//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

typedef enum _MMPF_TIMER_ID
{
	MMPF_TIMER_0 =0,
	MMPF_TIMER_1,
	MMPF_TIMER_2,
	#if (CHIP == MERCURY)
    MMPF_TIMER_3,
    MMPF_TIMER_4,
    MMPF_TIMER_5,
    #endif
	MMPF_TIMER_MAX
} MMPF_TIMER_ID;

typedef enum _MMPF_TIMER_EVT_MODE {
    MMPF_TIMER_EVT_PERIODIC = 0,
    MMPF_TIMER_EVT_ONESHOT,
    MMPF_TIMER_EVT_NONE
} MMPF_TIMER_EVT_MODE;

typedef enum _MMPF_TIMER_MCLK_DIV {
    MMPF_TIMER_MCLK_D2 = 0,
    MMPF_TIMER_MCLK_D8,
    MMPF_TIMER_MCLK_D32,
    MMPF_TIMER_MCLK_D128,
    MMPF_TIMER_MCLK_D1024,
    MMPF_TIMER_MCLK_D4,
    MMPF_TIMER_MCLK_D16,
    MMPF_TIMER_MCLK_D1,
    MMPF_TIMER_MCLK_DIV_NUM
} MMPF_TIMER_MCLK_DIV;

typedef enum _MMPF_TIMER_PRCN {
    MMPF_TIMER_PRCN_MSEC = 0,
    MMPF_TIMER_PRCN_USEC,
    MMPF_TIMER_PRCN_TICK
} MMPF_TIMER_PRCN;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef void TimerCallBackFunc(void);              //New for callback function

typedef struct _MMPF_TIMER_ATTRIBUTE {
    TimerCallBackFunc       *Callback;
    MMPF_TIMER_EVT_MODE     EventMode;
    MMPF_TIMER_MCLK_DIV     MClkDiv;
    MMPF_TIMER_PRCN         TimeUnit;
    MMP_ULONG               ulDelayCnt;
    MMP_BOOL                bEnableTick;
} MMPF_TIMER_ATTRIBUTE;

//==============================================================================
//

#define MMPF_Timer_ReadCount(_id)           (((AITPS_TC)(&(AITC_BASE_TCB->TC[_id])))->TC_CVR)
#define MMPF_Timer_ReadCompareValue(_id)    (((AITPS_TC)(&(AITC_BASE_TCB->TC[_id])))->TC_RC)
#define MMPF_Timer_IsOnTime(_id)            (!!((((AITPS_TC)(&(AITC_BASE_TCB->TC[_id])))->TC_SR)&TC_CPCS))

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//New API: To set AIC source and assign callback function. It will call SetAICISR()
//call RTNA_AIC.... to set the interrupt source.
MMP_ERR    MMPF_Timer_OpenInterrupt(MMPF_TIMER_ID id);             

//New API: To replace following 2 APIs with this one
//RTNA_AIC_IRQ_En or RTNA_AIC_IRQ_Dis to turn on the AIC interrupt status
// and then turn on Timer interrupt in Timer module. (Merge MMPF_TimerEnableIRQ function)
MMP_ERR    MMPF_Timer_EnableInterrupt(MMPF_TIMER_ID id, MMP_BOOL bEnable);
//--MMP_ERR    MMPF_Timer_EnableAICTimerSRC(MMPF_TIMER_TC tc);
//--MMP_ERR    MMPF_Timer_DisableAICTimerSRC(MMPF_TIMER_TC tc);

void MMPF_TIMER1_ISR(void);    //The ISR. It will call callback function.
void MMPF_TIMER2_ISR(void);    

//FW is urge to use MMPF_Timer_OpenMS insted of MMPF_Timer_Open. 
//OpenMS will get CPU frequency and calculate the delay
MMP_ERR MMPF_Timer_Open(MMPF_TIMER_ID id, MMP_ULONG ulMs, TimerCallBackFunc *Func);
MMP_ERR MMPF_Timer_Close(MMPF_TIMER_ID id);
MMP_ERR MMPF_Timer_OpenUs(MMPF_TIMER_ID id, MMP_ULONG ulUs, TimerCallBackFunc *Func);
//MMP_ERR MMPF_Timer_TCSync(void);

//New API: To replace following 2 APIs with this one
// void MMPF_Timer_EnableIRQ(MMPF_TIMER_ID id, MMP_ULONG ulType, MMP_BOOL bEnable);
//--void MMPF_Timer_EnableIRQ(MMPF_TIMER_TC tc, MMP_ULONG type);
//--void MMPF_Timer_DisableIRQ(MMPF_TIMER_TC tc, MMP_ULONG type);



//New API: To get the status of each timer. In case some FW need more then 1 timer
//and need to check which timer triggered the ISR. 
//MMP_UBYTE  MMPF_Timer_GetSR(MMPF_TIMER_TC tc);   


// Please do not use RTNA_TC.... again. RTNA_AIC can be used.
// All of the register access, please enter from AITPS_TCB not AITPS_TC, and remove the
// AITC_BASE_TC0, AITC_BASE_TC1, AITC_BASE_TC2 base declaration
// Please use the newest VSS version to modify the codes.

//==============================================================================
//
//                              MACRO FUNCTIONS
//
//==============================================================================

 MMP_ULONG MMPF_Timer_ReadCount(MMPF_TIMER_ID id);


#endif // _INCLUDES_H_
