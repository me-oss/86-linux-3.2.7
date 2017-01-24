//==============================================================================
//
//  File        : mmpf_pio.c
//  Description : PIO Pins Control Interface
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file mmpf_pio.c
 *  @brief The PIO pins control functions
 *  @author Ben Lu
 *  @version 1.0
 */
 

#include <linux/module.h>
#include <linux/irq.h>

#include <mach/mmpf_typedef.h>

#include <mach/reg_retina.h>
#include <mach/mmp_err.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmpf_pio.h>
 

#define  MMPF_OS_SEMID  int
#define MMPF_OS_FLAGID int

static MMPF_OS_SEMID  gPIO_GpioModeSemID;
static MMPF_OS_SEMID  gPIO_OutputModeSemID;
static MMPF_OS_SEMID  gPIO_SetDataSemID;
static MMPF_OS_SEMID  gPIO_EnableTrigModeSemID;
static MMPF_OS_SEMID  gPIO_EnableInterruptSemID;
static MMP_ULONG gPIO_BoundingTime[PIO_MAX_PIN_SIZE];
static PioCallBackFunc *gPIO_CallBackFunc[PIO_MAX_PIN_SIZE];


extern MMP_UBYTE		m_gbKeypadCheck[];

static DEFINE_SPINLOCK(mmpf_pio_lock);

//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_Initialize
//  Description :
//------------------------------------------------------------------------------
/** @brief The function registers the interrupt and create related semaphore for PIO pins.

The function registers the interrupt and create related semaphore for PIO pins. 
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PIO_Initialize(void)
{
	MMP_USHORT i = 0;
//	AITPS_AIC   pAIC = AITC_BASE_AIC;
	static MMP_BOOL m_bInitialFlag = MMP_FALSE;
    
	if (m_bInitialFlag == MMP_FALSE) {
#if 0		
//	    RTNA_AIC_Open(pAIC, AIC_SRC_GPIO, gpio_isr_a,
//	                AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 3);
//	    RTNA_AIC_IRQ_En(pAIC, AIC_SRC_GPIO);

		gPIO_GpioModeSemID =  MMPF_OS_CreateSem(1);
		gPIO_OutputModeSemID =  MMPF_OS_CreateSem(1);
		gPIO_SetDataSemID =  MMPF_OS_CreateSem(1);
		gPIO_EnableTrigModeSemID = MMPF_OS_CreateSem(1);
		gPIO_EnableInterruptSemID = MMPF_OS_CreateSem(1);
#endif		
//		MEMSET0(gPIO_BoundingTime);

//		for ( i = 0; i < PIO_MAX_PIN_SIZE ; i++) {
//			gPIO_CallBackFunc[i] = NULL;
//		}
		for ( i = 0; i < PIO_MAX_PIN_SIZE ; i++) {
			MMPF_PIO_EnableInterrupt(i, 0, 0, 0);
			}
		m_bInitialFlag = MMP_TRUE;
	}	
	return MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_EnableOutputMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set the PIN as GPIO mode (pin should be set as GPIO mode before using GPIO module to control signals)

The function set the PIN as GPIO mode (pin should be set as GPIO mode before using GPIO module to control signals) 
@param[in] piopin is the PIO number, please reference the data structure of MMPF_PIO_REG
@param[in] bEnable is the choice of output mode or input mode
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PIO_EnableGpioMode(MMPF_PIO_REG piopin, MMP_BOOL bEnable)
{
	MMP_UBYTE		ret;
	AITPS_GBL   pGBL = AITC_BASE_GBL;
	unsigned long	spinlock_flags;
#ifdef DEBUG
#endif
	spin_lock_irqsave(&mmpf_pio_lock, spinlock_flags);

	if(bEnable == MMP_TRUE) {
		if (piopin < MMPF_PIO_REG_GPIO8) {
			pGBL->GBL_IO_CTL5 |= (0x1 << piopin);
		}
		else if (piopin < MMPF_PIO_REG_GPIO16) {
			pGBL->GBL_IO_CTL6 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO8)); 
		}
		else if (piopin < MMPF_PIO_REG_GPIO24) {
			pGBL->GBL_IO_CTL7 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO16)); 
		}
		else if (piopin < MMPF_PIO_REG_GPIO32) {
			pGBL->GBL_IO_CTL8 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO24));
		}
		else if (piopin < MMPF_PIO_REG_GPIO40) {	
			pGBL->GBL_IO_CTL9 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO32));
		}
		#if 0//(CHIP == VSN_V2)
		else {
			pGBL->GBL_IO_CTL10 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO40));
		}
		#endif
		#if 1//(CHIP == VSN_V3)
		else if (piopin < MMPF_PIO_REG_GPIO48) {	
			pGBL->GBL_IO_CTL10 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO40));
		}
		else if (piopin < MMPF_PIO_REG_GPIO56) {	
			pGBL->GBL_IO_CTL11 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO48));
		}
		else if (piopin < MMPF_PIO_REG_GPIO64) {	
			pGBL->GBL_IO_CTL12 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO56));
		}
		else {
			pGBL->GBL_IO_CTL13 |= (0x1 << (piopin - MMPF_PIO_REG_GPIO64));
		}
		#endif
	} else {
		if (piopin < MMPF_PIO_REG_GPIO8) {
			pGBL->GBL_IO_CTL5 &= ~(0x1 << piopin);
		}
		else if (piopin < MMPF_PIO_REG_GPIO16) {
			pGBL->GBL_IO_CTL6 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO8)); 
		}
		else if (piopin < MMPF_PIO_REG_GPIO24) {
			pGBL->GBL_IO_CTL7 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO16)); 
		}
		else if (piopin < MMPF_PIO_REG_GPIO32) {
			pGBL->GBL_IO_CTL8 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO24));
		}
		else if (piopin < MMPF_PIO_REG_GPIO40) {
			pGBL->GBL_IO_CTL9 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO32));
		}
		#if 0//(CHIP == VSN_V2)
		else {
			pGBL->GBL_IO_CTL10 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO40));
		}
		#endif
		#if 1//(CHIP == VSN_V3)
		else if (piopin < MMPF_PIO_REG_GPIO48) {
			pGBL->GBL_IO_CTL10 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO40));
		}
		else if (piopin < MMPF_PIO_REG_GPIO56) {
			pGBL->GBL_IO_CTL11 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO48));
		}
		else if (piopin < MMPF_PIO_REG_GPIO64) {
			pGBL->GBL_IO_CTL12 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO56));
		}
		else {
			pGBL->GBL_IO_CTL13 &= ~(0x1 << (piopin - MMPF_PIO_REG_GPIO64));
		}
		#endif
	
	}

	spin_unlock_irqrestore(&mmpf_pio_lock, spinlock_flags);
	
//	ret = MMPF_OS_ReleaseSem(gPIO_GpioModeSemID);
	return MMP_ERR_NONE;	
}
//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_EnableOutputMode
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set the PIN as Output mode (bEnable = MMP_TRUE) or Input mode.

The function set the PIN as Output mode (bEnable = MMP_TRUE) or Input mode. 
@param[in] piopin is the PIO number, please reference the data structure of MMPF_PIO_REG
@param[in] bEnable is the choice of output mode or input mode
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PIO_EnableOutputMode(MMPF_PIO_REG piopin, MMP_BOOL bEnable)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	MMP_UBYTE		ret;  
	unsigned long	spinlock_flags;
	spin_lock_irqsave(&mmpf_pio_lock, spinlock_flags);		
	if(bEnable) {
		pGPIO->GPIO_EN[IndexAddressInfo] |= (0x1 << bitPostionInfo);

	}
	else {
		pGPIO->GPIO_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
	}	
	spin_unlock_irqrestore(&mmpf_pio_lock, spinlock_flags);
	return MMP_ERR_NONE;
}

MMP_ERR MMPF_PIO_GetOutputMode(MMPF_PIO_REG piopin, MMP_UBYTE* returnValue)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	MMP_UBYTE		ret;  
	unsigned long	spinlock_flags;
	spin_lock_irqsave(&mmpf_pio_lock, spinlock_flags);		
	
	*returnValue = (pGPIO->GPIO_EN[IndexAddressInfo] >> bitPostionInfo) & 0x01;

	spin_unlock_irqrestore(&mmpf_pio_lock, spinlock_flags);
	return MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_SetData
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set the PIO pin as High or Low (When the pin is at output mode).

The function set the PIO pin as High or Low (When the pin is at output mode).
@param[in] piopin is the PIO number, please reference the data structure of MMPF_PIO_REG
@param[in] outputValue is 0 stands for Low, otherwise it stands for High
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PIO_SetData(MMPF_PIO_REG piopin, MMP_UBYTE outputValue)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	MMP_UBYTE		ret; 
	unsigned long	spinlock_flags;
	spin_lock_irqsave(&mmpf_pio_lock, spinlock_flags);	
	if((pGPIO->GPIO_EN[IndexAddressInfo] & (0x1 << bitPostionInfo)) == 0x0){
		//"Error !!! PIO Input Mode to call  MMPF_PIO_SetData
		return MMP_PIO_ERR_INPUTMODESETDATA;
	}
	
	if(outputValue) {
		pGPIO->GPIO_DATA[IndexAddressInfo] |= (0x1 << bitPostionInfo);
	}
	else {
		pGPIO->GPIO_DATA[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
	}	
	spin_unlock_irqrestore(&mmpf_pio_lock, spinlock_flags);
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_GetData
//  Description :
//------------------------------------------------------------------------------
/** @brief The function get the PIO pin's singal. (When the pin is at input mode).

The function get the PIO pin's singal. (When the pin is at input mode).
@param[in] piopin is the PIO number, please reference the data structure of MMPF_PIO_REG
@param[out] returnValue is standing for the High or Low signal.
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PIO_GetData(MMPF_PIO_REG piopin, MMP_UBYTE* returnValue)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	

	unsigned long	spinlock_flags;
	spin_lock_irqsave(&mmpf_pio_lock, spinlock_flags);		
	
	if((pGPIO->GPIO_EN[IndexAddressInfo] & (0x1 << bitPostionInfo)) != 0x0){
		//Error !!! PIO Output Mode to call MMPF_PIO_GetData
//		return MMP_PIO_ERR_OUTPUTMODEGETDATA;
	}
	
	*returnValue = ((pGPIO->GPIO_DATA[IndexAddressInfo] >> bitPostionInfo) & 0x1);  //AIT_PMP,fix bug,201003
	spin_unlock_irqrestore(&mmpf_pio_lock, spinlock_flags);
	return MMP_ERR_NONE;
}



//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_GetData
//  Description :
//------------------------------------------------------------------------------
/** @brief The function get the PIO pin's singal. (When the pin is at input mode).

The function get the PIO pin's singal. (When the pin is at input mode).
@param[in] piopin is the PIO number, please reference the data structure of MMPF_PIO_REG
@param[in] trigmode set the pio pin as edge trigger (H2L or L2H) or level trigger (H or L)
@param[out] bEnable make the tirgger settings work or not.
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PIO_EnableTrigMode(MMPF_PIO_REG piopin, MMPF_PIO_TRIGMODE  trigmode, MMP_BOOL bEnable)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	MMP_UBYTE ret; 
	unsigned long	spinlock_flags;
	
	spin_lock_irqsave(&mmpf_pio_lock, spinlock_flags);	
	
	if(bEnable) {
		switch(trigmode){
			case MMPF_PIO_TRIGMODE_EDGE_H2L:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				//pGPIO->GPIO_INT_L2H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));	
				pGPIO->GPIO_INT_H2L_EN[IndexAddressInfo] |= (0x1 << bitPostionInfo);
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
			case MMPF_PIO_TRIGMODE_EDGE_L2H:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				//pGPIO->GPIO_INT_H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_H2L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				pGPIO->GPIO_INT_L2H_EN[IndexAddressInfo] |= (0x1 << bitPostionInfo);
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
			case MMPF_PIO_TRIGMODE_LEVEL_H:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				//pGPIO->GPIO_INT_H2L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_L2H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				pGPIO->GPIO_INT_H_EN[IndexAddressInfo] |= (0x1 << bitPostionInfo);
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
			case MMPF_PIO_TRIGMODE_LEVEL_L:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				//pGPIO->GPIO_INT_H2L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_L2H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				//pGPIO->GPIO_INT_H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
				pGPIO->GPIO_INT_L_EN[IndexAddressInfo] |= (0x1 << bitPostionInfo);
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
		}
	}
	else {
		switch(trigmode){
			case MMPF_PIO_TRIGMODE_EDGE_H2L:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				pGPIO->GPIO_INT_H2L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
			case MMPF_PIO_TRIGMODE_EDGE_L2H:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				pGPIO->GPIO_INT_L2H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
			case MMPF_PIO_TRIGMODE_LEVEL_H:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				pGPIO->GPIO_INT_H_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
			case MMPF_PIO_TRIGMODE_LEVEL_L:
//				ret = MMPF_OS_AcquireSem(gPIO_EnableTrigModeSemID, PIO_SEM_TIMEOUT);
				pGPIO->GPIO_INT_L_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
//				ret = MMPF_OS_ReleaseSem(gPIO_EnableTrigModeSemID);
				break;
		}
	}	
	spin_unlock_irqrestore(&mmpf_pio_lock, spinlock_flags);
	return MMP_ERR_NONE;
}

/*MMP_ERR MMPF_PIO_GetInterruptStatus(MMPF_PIO_REG piopin, MMP_UBYTE *returnStatus)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	
	*returnStatus = (pGPIO->GPIO_INT_CPU_SR[IndexAddressInfo] & (0x1 << bitPostionInfo));
	return MMP_ERR_NONE;
}

MMP_ERR MMPF_PIO_CleanInterruptStatus(MMPF_PIO_REG piopin)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	
	pGPIO->GPIO_INT_CPU_SR[IndexAddressInfo] &= (0x1 << bitPostionInfo);
	return MMP_ERR_NONE;
}*/



//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_EnableInterrupt
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set the PIO pin's interrupt actions.

The function set the PIO pin's interrupt actions.
@param[in] piopin is the PIO number, please reference the data structure of MMPF_PIO_REG
@param[in] bEnable stands for enable interrupt or not.
@param[in] boundingTime is used for advanced interrupt settings.
@param[in] CallBackFunc is used by interrupt handler.
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PIO_EnableInterrupt(MMPF_PIO_REG piopin, MMP_BOOL bEnable, MMP_ULONG boundingTime, PioCallBackFunc *CallBackFunc)
{
	MMP_UBYTE bitPostionInfo = piopin & PIO_BITPOSITION_INFO;
	MMP_UBYTE IndexAddressInfo = PIO_GET_INDEX(piopin);
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	MMP_UBYTE ret;

	unsigned long	spinlock_flags;
	spin_lock_irqsave(&mmpf_pio_lock, spinlock_flags);	
	
	if(bEnable) {
		
		gPIO_BoundingTime[piopin] = boundingTime;
		gPIO_CallBackFunc[piopin] = CallBackFunc;	
		
		pGPIO->GPIO_INT_CPU_EN[IndexAddressInfo] |= (0x1 << bitPostionInfo);
	}
	else {
		pGPIO->GPIO_INT_CPU_EN[IndexAddressInfo] &= (~(0x1 << bitPostionInfo));
		gPIO_BoundingTime[piopin] = 0;
		gPIO_CallBackFunc[piopin] = NULL;
	}
	
	spin_unlock_irqrestore(&mmpf_pio_lock, spinlock_flags);
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_PIO_ISR
//  Description : 
//------------------------------------------------------------------------------
/** @brief PIO pin's interrupt handler function.

PIO pin's interrupt handler function.
@return It reports the status of the operation.
*/

extern  MMP_ULONG   glSystemEvent;
extern MMPF_OS_FLAGID SYS_Flag_Hif;

#include <linux/interrupt.h>
#include <linux/gpio_keys.h>
struct gpio_button_data {
	struct gpio_keys_button *button;
	struct input_dev *input;
	struct timer_list timer;
	struct work_struct work;
	int timer_debounce;	/* in msecs */
	bool disabled;
};

extern irqreturn_t vsnv3_sd_det_irq(int irq, void *_host);
void MMPF_PIO_ISR(unsigned int irq,struct irq_desc *desc)
{
	MMP_ULONG i = 0, j = 0;
	MMP_ULONG  intsrc_H = 0, intsrc_L = 0, intsrc_H2L = 0, intsrc_L2H = 0;
	MMPF_PIO_REG piopin = MMPF_PIO_REG_UNKNOWN;
	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	struct irq_desc * gpio_irq_desc;
	struct irqaction *action;// = desc->action;
	struct gpio_button_data *bdata;// = (struct gpio_button_data *)(action->dev_id) ;

	//To find out the GPIO number and clean the interrupt
	for (i = 0; i < 4; i++) {
		
		intsrc_H = pGPIO->GPIO_INT_CPU_EN[i] & pGPIO->GPIO_INT_H_SR[i];
		intsrc_L = pGPIO->GPIO_INT_CPU_EN[i] & pGPIO->GPIO_INT_L_SR[i];
		intsrc_H2L = pGPIO->GPIO_INT_CPU_EN[i] & pGPIO->GPIO_INT_H2L_SR[i];
		intsrc_L2H = pGPIO->GPIO_INT_CPU_EN[i] & pGPIO->GPIO_INT_L2H_SR[i];

		if(intsrc_H2L != 0x0){
			int k;
			//pGPIO->GPIO_INT_H2L_SR[i] = intsrc_H2L;  //clean interrupt
#if 0			
			for (j = 1; j <= 0x20; j++){
				if((intsrc_H2L >> j) == 0){
					pGPIO->GPIO_INT_H2L_SR[i] = intsrc_H2L&(1<<(j-1));					
					break; 
				}					
			}
#endif
			k = ffs(intsrc_H2L);
			pGPIO->GPIO_INT_H2L_SR[i] = intsrc_H2L&(1<<(ffs(intsrc_H2L)-1));
			piopin = i*0x20 + ffs(intsrc_H2L)-1;//(j-1);
			//break;
		}

		
		if(intsrc_L2H != 0x0&&(piopin == MMPF_PIO_REG_UNKNOWN)){
			//pGPIO->GPIO_INT_L2H_SR[i] = intsrc_L2H;  //clean interrupt
#if 0			
			for (j = 1; j <= 0x20; j++){
				if((intsrc_L2H >> j) == 0){
					pGPIO->GPIO_INT_L2H_SR[i] = intsrc_L2H&(1<<(j-1));					
					break; 
				}					
			}
#endif			
			pGPIO->GPIO_INT_L2H_SR[i] = intsrc_L2H&(1<<(ffs(intsrc_L2H)-1));				
			
			piopin = i*0x20 + ffs(intsrc_L2H)-1;// (j-1);
			//break;
		}
		
		if(intsrc_H != 0x0 &&(piopin == MMPF_PIO_REG_UNKNOWN)){
			//pGPIO->GPIO_INT_H_SR[i] = intsrc_H;  //clean interrupt
#if 0			
			for (j = 1; j <= 0x20; j++){
				if((intsrc_H >> j) == 0) 
				{
					pGPIO->GPIO_INT_H_SR[i] = intsrc_H&(1<<(j-1));
					break; 
				}					
			}
#endif			
			pGPIO->GPIO_INT_H_SR[i] = intsrc_H&(1<<(ffs(intsrc_H)-1));					
			
			piopin = i*0x20 + ffs(intsrc_H)-1;// (j-1);
			//break;
		}
		if(intsrc_L != 0x0&&(piopin == MMPF_PIO_REG_UNKNOWN)){
#if 0			
			//pGPIO->GPIO_INT_L_SR[i] = intsrc_L;  //clean interrupt
			for (j = 1; j <= 0x20; j++){
				if((intsrc_L >> j) == 0){
					pGPIO->GPIO_INT_L_SR[i] = intsrc_L&(1<<(j-1));					
					break; 
				}
			}
#endif			
			pGPIO->GPIO_INT_L_SR[i] = intsrc_L&(1<<(ffs(intsrc_L)-1));					
			
			piopin = i*0x20 + ffs(intsrc_L)-1;// (j-1);
			//break;
		}
		



		if(piopin != MMPF_PIO_REG_UNKNOWN)
		{
			gpio_irq_desc = irq_to_desc(NR_AIC_IRQS+piopin);

			action = gpio_irq_desc->action;
			bdata = (struct gpio_button_data *)(action->dev_id) ;

			//bdata->button->value=(int)piopin;

			pr_debug("%s:piopin = %d\n",__func__,piopin);	

			handle_simple_irq(NR_AIC_IRQS+piopin, gpio_irq_desc);

			piopin = MMPF_PIO_REG_UNKNOWN;
		}
	}
#if 0	
		
	if (piopin != MMPF_PIO_REG_UNKNOWN) {
		if(gPIO_CallBackFunc[piopin] != NULL) {
			(*gPIO_CallBackFunc[piopin]) (piopin);
		}
	
	}
    RTNA_DBG_Str3("GPIO INT : ");
    RTNA_DBG_Long3(piopin);
    RTNA_DBG_Str3("\r\n");

#if (CUSTOMER == NMG)  
	if (piopin != MMPF_PIO_REG_UNKNOWN) {
	    if(glSystemEvent == 0){
		    if(piopin == SNAP_PIN)
                glSystemEvent |= SNAP_BUTTON_PRESSED;
            else if(piopin == FLIP_PIN)
                glSystemEvent |= FLIP_BUTTON_PRESSED;
            else if(piopin == IRHID_PIN)
                glSystemEvent |= IRHID_BUTTON_PRESSED;

//            MMPF_OS_SetFlags(SYS_Flag_Hif,SYS_FLAG_USBOP, MMPF_OS_FLAG_SET);
			MMPF_USBCTL_PostMsg(2,TASK_USB_PRIO,USBCTL_MSG_OTHERS,0,(void *)NULL);
        }
    }
#endif
	gpio_irq_desc = irq_to_desc(NR_AIC_IRQS+piopin);

	action = gpio_irq_desc->action;
	bdata = (struct gpio_button_data *)(action->dev_id) ;

	//bdata->button->value=(int)piopin;

	pr_debug("%s:piopin = %d\n",__func__,piopin);	

	handle_simple_irq(NR_AIC_IRQS+piopin, gpio_irq_desc);
//printk("%s:%d\n",__func__,__LINE__);	

#endif

	return;
	
}
