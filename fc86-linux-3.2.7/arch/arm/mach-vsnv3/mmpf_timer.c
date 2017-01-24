//==============================================================================
//
//  File        : mmpf_timer.c
//  Description : Firmware Timer Control Function
//  Author      : Jerry Lai
//  Revision    : 1.0
//
//==============================================================================

//#include "includes_fw.h"
#include <linux/kernel.h>
#include <stddef.h>
#include <mach/mmpf_timer.h>

#include <mach/lib_retina.h>
#include <mach/mmpf_typedef.h>
#include <mach/mmp_err.h>
//#include "mmpf_hif.h"
//#include "mmpf_pll.h"

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
static MMP_BOOL  gbTimerEnabled[MMPF_TIMER_2+1] = {0};

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
static MMP_ERR MMPF_TimerInternal_EnableAICTimerSRC(MMPF_TIMER_ID id, MMP_BOOL bEnable);

static TimerCallBackFunc *TimerCallBacks[3] = {0,0,0};//{NULL, NULL, NULL};

static __inline AITPS_TC GetpTC(MMPF_TIMER_ID id);



/**
@brief  To set AIC interrupt source
@param[in]  id   The timer you want to use as interrupt source
*/
MMP_ERR    MMPF_Timer_OpenInterrupt(MMPF_TIMER_ID id)
{
//    AITPS_AIC pAIC = AITC_BASE_AIC;
    // FH
    static MMP_BOOL bTimerIntOpened[3] = {MMP_FALSE, MMP_FALSE, MMP_FALSE};
    
    if (bTimerIntOpened[id] == MMP_TRUE) {
        return MMP_ERR_NONE;
    }

	switch(id)
	{
		case MMPF_TIMER_1:
//			RTNA_AIC_Open(pAIC, AIC_SRC_TC1, tc1_isr_a, AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 2);
		break;
		case MMPF_TIMER_2:
//			RTNA_AIC_Open(pAIC, AIC_SRC_TC2, tc2_isr_a, AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 2);
		break;
		default:
//			RTNA_DBG_Str(0, "ERROR: Invalid timer id @ MMPF_Timer_OpenInterrupt\r\n");
		break;
	}

	bTimerIntOpened[id] = MMP_TRUE;

	return MMP_ERR_NONE;
}

/**
@brief  Start to recieve timer IRQ 
@param[in]  id   The timer you want to use as interrupt source
@param[in]  bEnable  1 start the timer IRQ. 0 stop the timer IRQ
*/
MMP_ERR	   MMPF_Timer_EnableInterrupt(MMPF_TIMER_ID id, MMP_BOOL bEnable)
{
	AITPS_TC pTC = GetpTC(id);
	static MMP_UBYTE intEnableCount = 0; // FH

	if(bEnable == MMP_TRUE)
	{
		//original MMPF_TimerInternal_EnableAICTimerSRC(id, MMP_TRUE);

		// MODIFIED BEGIN: for multiple timer enables/disables
		// timer was disabled?
		if (gbTimerEnabled[id] == MMP_FALSE)
		{
			//Enable AIC's Timer Source
			if (intEnableCount++ == 0)
			{
				MMPF_TimerInternal_EnableAICTimerSRC(id, MMP_TRUE);
			}

			gbTimerEnabled[id] = MMP_TRUE;
		}
		// MODIFIED END: for multiple timer enables/disables

		//clear counter to 0
		pTC->TC_CVR = 0;

		//Enable Timer IRQ
		pTC->TC_SR = pTC->TC_SR;  //clear SR
		pTC->TC_IER = TC_CPCS;       //IER : interrupt enable register
		pTC->TC_IMR = TC_CPCS;  //Compare Status Interrupt Mask

	}
	else
	{
		//original MMPF_TimerInternal_EnableAICTimerSRC(id, MMP_FALSE);

		// MODIFIED BEGIN: for multiple timer enables/disables
		// timer was enabled?
		if (gbTimerEnabled[id] == MMP_TRUE)
		{
			if (--intEnableCount == 0) // FH
			{
				//Disable AIC's Timer Source
				MMPF_TimerInternal_EnableAICTimerSRC(id, MMP_FALSE);
			}

			gbTimerEnabled[id] = MMP_FALSE;
		}

		//Disable Timer IRQ
		pTC->TC_IDR = TC_CPCS;      //IDR : interrupt disable register
		pTC->TC_SR = pTC->TC_SR;  //clear SR

	}

	return MMP_ERR_NONE;
}
MMP_ERR MMPF_Timer_OpenUs(MMPF_TIMER_ID id, MMP_ULONG ulUs, TimerCallBackFunc *Func)
{
	MMP_ULONG count;
	MMP_ULONG mode;
	MMP_ULONG cpuFreq;
	AITPS_TC  pTC = GetpTC(id); //Get correct Timer
    
	//Check from Boot Code
	cpuFreq = 0x40740;	// 264M
	//cpuFreq /= 1000;

	//Not a so good implement. 	
	//Use highest timer clock = CPU clock	
	count = (ulUs * (cpuFreq/2));  //(1/Timer FREQ) * count = n * (1/1000)
	mode = TC_CPCTRG | TC_CLK_MCK;
	//should check if the delay is not too long for a timer

    // Start the Timer and set compare value
    pTC->TC_CCR = TC_CLKDIS;  //Stop Timer
    pTC->TC_IDR = TC_CPCS;    //Disable Interrupt mode

	pTC->TC_SR =  pTC->TC_SR; //Clear Status Register
	pTC->TC_CCR |= 0x04;      //Reset Timer
    pTC->TC_CMR = mode;       //Set mode

    pTC->TC_RC = count;       //Set Compare value
    pTC->TC_CCR = TC_CLKEN;   //Start Timer again
    	
	//Save the callback
	TimerCallBacks[id] = Func;
	
	return MMP_ERR_NONE;
    
}
/**
@brief  Set Timer triggered time in ms, and set timer callback function

@param[in]  id   The timer you want to use as interrupt source
@param[in]  ulMs The interval for timer to trigger interrupt in ms
@param[in]  Func The call back function. The function is bind to the id.
*/
MMP_ERR MMPF_Timer_Open(MMPF_TIMER_ID id, MMP_ULONG ulMs, TimerCallBackFunc *Func)
{
//	MMP_ULONG count;
//	MMP_ULONG mode;
//	MMP_ULONG cpuFreq;
	AITPS_TC  pTC = GetpTC(id); //Get correct Timer
#if 0
	#if 0//(CHIP == VSN_V2)
	if(MMPF_PLL_GetCPUFreq(&cpuFreq) != MMP_ERR_NONE) {
		return MMP_SYSTEM_ERR_SETPLL;
	}
	#endif
	#if 1//(CHIP == VSN_V3)
	//if(MMPF_PLL_GetGroupFreq(0, &cpuFreq) != MMP_ERR_NONE) { //In VSN_V3, Timer clock is produced by group 0
//		return MMP_SYSTEM_ERR_SETPLL;
//	}
cpuFreq = 0x40740;	// 264M
cpuFreq >>=9;
	#endif
	
    //cpuFreq = 144000;  //AIT_PMP,20100324
    #if 0//(CHIP == P_V2)
    cpuFreq = 12000 * 2;  //AIT_PYTHONV2, CPU Periphial always 12MHz
    #endif
	if(cpuFreq == 0) {
//		RTNA_DBG_Str(0, "ERROR : FW PLL not set.");
		return MMP_SYSTEM_ERR_SETPLL;
	}
	
	
	//Not a so good implement. 	
	//Use highest timer clock = CPU clock	
//	count = ulMs * ((cpuFreq/2));  //(1/Timer FREQ) * count = n * (1/1000)
	count = ulMs * ((cpuFreq));  //(1/Timer FREQ) * count = n * (1/1000)
	
	mode = TC_CPCTRG | TC_CLK_MCK;
	//should check if the delay is not too long for a timer
#endif
	// Start the Timer and set compare value
	pTC->TC_CCR = TC_CLKDIS;  //Stop Timer
	pTC->TC_IDR = TC_CPCS;    //Disable Interrupt mode

	pTC->TC_SR =  pTC->TC_SR; //Clear Status Register
	pTC->TC_CCR |= 0x04;      //Reset Timer
	pTC->TC_CMR = TC_CPCTRG | TC_CLK_MCK_D1024;//(0x40<<8)|mode;       //Set mode

	pTC->TC_RC = 132*ulMs;//1320>>1;//count;       //Set Compare value
	pTC->TC_CCR = TC_CLKEN;   //Start Timer again


printk("pTC->TC_CCR = 0x%x\r\n",pTC->TC_CCR );		
printk("pTC->TC_IDR = 0x%x\r\n",pTC->TC_IDR );		
printk("pTC->TC_SR= 0x%x\r\n",pTC->TC_SR);		
printk("pTC->TC_CMR = 0x%x\r\n",pTC->TC_CMR );		
printk("pTC->TC_RC = 0x%x\r\n",pTC->TC_RC );		




	//Save the callback
	//TimerCallBacks[id] = Func;
	
	return MMP_ERR_NONE;
}

/**
@brief  Stop timer triggered time in ms, and clear timer callback function

@param[in]  id   The timer you want to use as interrupt source
*/
MMP_ERR MMPF_Timer_Close(MMPF_TIMER_ID id)
{
	AITPS_TC pTC = GetpTC(id);
printk("MMPF_Timer_Close+\r\n");	
	//Stop timer
	pTC->TC_CCR = TC_CLKDIS;
	pTC->TC_IDR = TC_CPCS;
	
	//clear callback
	TimerCallBacks[id] = NULL;
printk("MMPF_Timer_Close-\r\n");
	return MMP_ERR_NONE;

}

//=====================================================================




//------------------------------------------------------------------------------
//  Function    : MMPF_TimerInternal_EnableAICTimerSRC
//  Description :
//  Note        :
//------------------------------------------------------------------------------
static MMP_ERR MMPF_TimerInternal_EnableAICTimerSRC(MMPF_TIMER_ID id, MMP_BOOL bEnable)
{
#if 0
    AITPS_AIC pAIC = AITC_BASE_AIC;

	if(bEnable==MMP_TRUE) {
		//wilson@120406
		switch(id) {
			case MMPF_TIMER_1:
				RTNA_AIC_IRQ_En(pAIC, AIC_SRC_TC1);
			break;
			case MMPF_TIMER_2:
				RTNA_AIC_IRQ_En(pAIC, AIC_SRC_TC2);
			break;
			default:
//				RTNA_DBG_Str(0, "ERROR: invalid timer id\r\n");
			break;
    	
    	}
    } else {
    	//wilson@120406
		switch(id) {
			case MMPF_TIMER_1:
				RTNA_AIC_IRQ_Dis(pAIC, AIC_SRC_TC1);
			break;
			case MMPF_TIMER_2:
				RTNA_AIC_IRQ_Dis(pAIC, AIC_SRC_TC2);
			break;
			default:
//				RTNA_DBG_Str(0, "ERROR: invalid timer id\r\n");
			break;
    	
    	}
    }
#endif
	return MMP_ERR_NONE;
}


////------------------------------------------------------------------------------
//  Function    : Return the AITPS_TC of given id
//  Description : Helper function to get correct pTC from ID
//  Note        : Because timer hardware is split into 2 group. The ID
//                to pTC is handled here 
//  Return      : 
//------------------------------------------------------------------------------
static __inline AITPS_TC GetpTC(MMPF_TIMER_ID id)
{
	AITPS_TCB pTCB = AITC_BASE_TCB;
	printk("GetpTC+\r\n");
	if(id <= MMPF_TIMER_2) {
		return ((AITPS_TC)(&(pTCB->TC[id])));
	}
	else {
//		RTNA_DBG_Str(0, "ERROR: TIMER ID IS INVALID\r\n");
		return 0;
	}
}


//Only VOIP_FW, BROGENT_FW have their own TimerISR 
//Should be corrected by firmware owner
#if defined(VOIP_FW) 

#else //Rest FW are ok 
////------------------------------------------------------------------------------
//  Function    : RTNA_TIMER1_ISR
//  Description : The ISR operation. Do the job of clear AIC. And call the callback
//  Note        : 
//  Return      : 
//------------------------------------------------------------------------------
void MMPF_TIMER1_ISR(void)
{
#if 0
	AITPS_TC  pTC;
    AITPS_AIC pAIC = AITC_BASE_AIC;
	MMP_UBYTE i;
	MMP_UBYTE TimerTriggered[3] = {0,0,0};
	
	//Check All Timer 1~2
	for(i = 1; i < 3; i++) {
		if(TimerCallBacks[i] != NULL) {//Do following check only if callback is set
			pTC = GetpTC(i);
			if(pTC->TC_SR & 0x10) {  //Timer i is triggered
			    if (gbTimerEnabled[i])  // FH
					TimerTriggered[i] = 1; //Set flag. Execute later
					
				pTC->TC_SR = pTC->TC_SR; //Done, clear the SR
				
				switch(i)
				{
					case 1:
						pAIC->AIC_ICCR |= 0x1 << AIC_SRC_TC1;    // Clear all TC1 Interrupt on AIC
					break;
					case 2:
						pAIC->AIC_ICCR |= 0x1 << AIC_SRC_TC2;    // Clear all TC2 Interrupt on AIC
					break;
					default:
//						RTNA_DBG_Str(0, "ERROR: TIMER ID IS INVALID\r\n");
					break;
				}
				
			}
		}
	}

	//Execute the Timer callback
	for(i=1;i<3;++i) {
		if(TimerTriggered[i]==1) {
			TimerCallBacks[i]();
		}
	}
#endif	
}


////------------------------------------------------------------------------------
//  Function    : RTNA_TIMER2_ISR
//  Description : The ISR operation. Do the job of clear AIC. And call the callback
//  Note        : This is needed for compatable for RETINA
//  Return      : 
//------------------------------------------------------------------------------

void MMPF_TIMER2_ISR(void)
{
	
}

#endif  //VOIP_FW need some effort to move to new
        //TimerISR calling method
////--------- Following code are no more needed ------------------------------
////--------- Leave the define of certain FW that still use old method -------
////--------- Please FW owner use new method to do timer call ----------------
 MMP_ULONG MMPF_Timer_ReadCount(MMPF_TIMER_ID id)
{
	AITPS_TC pTC = GetpTC(id);	

	return pTC->TC_CVR;
}
