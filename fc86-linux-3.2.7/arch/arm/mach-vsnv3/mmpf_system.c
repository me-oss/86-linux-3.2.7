//==============================================================================
//
//  File        : mmpf_system.c
//  Description : MMPF_SYS functions
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================

#include <mach/mmpf_system.h>
#include <mach/mmp_reg_gbl.h>


extern void MMPF_PLL_WaitCount(MMP_ULONG count);
#if 0
#include "mmpf_hif.h"
#include "mmpf_pll.h"
#include "mmpf_dma.h"
#include "mmpf_display.h"
#include "mmpf_system.h"
#if (defined(ALL_FW)||defined(UPDATER_FW))&&(SYS_SELF_SLEEP_ENABLE == 0x1)
#include "mmpf_audio_ctl.h"
#include "mmp_reg_audio.h"
#include "mmp_reg_vif.h"
#include "mmpf_pio.h"
#include "mmp_reg_usb.h"
#include "mmpf_dram.h"
#endif
#include "mmp_reg_display.h"
#include "mmpf_msg.h"
#include "mmpf_i2cm.h"
#include "mmpf_usbvend.h"

/** @addtogroup MMPF_SYS
@{
*/
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
OS_STK                  SYS_Task_Stk[TASK_SYS_STK_SIZE]; // put into sram
OS_STK                  LTASK_Task_Stk[LTASK_STK_SIZE];

extern MMPF_OS_FLAGID   SYS_Flag_Hif;
extern MMPF_PLL_MODE    gPLLMode;
/// @deprecated Customer and project ID
extern MMP_CUSTOMER  gbCustomer;
/// @brief Human-maintained release version
extern MMP_RELEASE_VERSION gbFwVersion;

extern MMP_BOOL    gbHasInitAitDAC;

MMP_UBYTE	m_gbSystemCoreID;
MMP_USHORT	m_gsISPCoreID;
#if ((DSC_R_EN)&&(FDTC_SUPPORT == 1))
extern MMPF_OS_FLAGID   FDTC_Flag;
#include "mmpf_fdtc.h"
#endif
#if (APP_EN)
extern MMPF_OS_FLAGID   APP_Flag;
#include "mmpf_appapi.h"
#endif

extern MMP_USHORT  gsCurrentSensor;
//==============================================================================
//
//                         MODULE VARIABLES
//
//==============================================================================
/// @brief Compiler build version
MMP_SYSTEM_BUILD_VERSION mmpf_buildVersion = {__DATE__, __TIME__};

/// @brief Define the firmware name to be printed for debugging
#if defined (ALL_FW)
	#define FIRMWARE_NAME "ALL"
#else
    #define FIRMWARE_NAME "UNKNOWN FW (Modify mmpf_system.c)"
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
#if defined(ALL_FW)||defined(UPDATER_FW)
extern void SPI_Write(MMP_UBYTE addr, MMP_USHORT data);
extern MMP_ERR MMPF_Sensor_PowerDown(MMP_USHORT usSensorID);
#endif

extern void MMPF_MMU_FlushDCache(void);
extern void AT91F_DisableDCache(void);
extern void AT91F_DisableICache(void);
extern void AT91F_DisableMMU(void);
//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================
//Gason@20120112, for div 0 case, system will output message. 
extern void $Super$$__rt_div0(void);
extern long SYS_FLOAT ;
void $Sub$$__rt_div0(void){
    RTNA_DBG_Str(0, "Div 0\r\n");
    $Super$$__rt_div0();
    // Force Watch dog timeout
    //dump_div0_info();
    while(1); 
}

#if defined(ALL_FW)
#include "mmpf_vif.h"
MMP_ERR MMPF_Self_Rom_Boot(void){
	
	AITPS_GBL pGBL = AITC_BASE_GBL;
	//Make sure that VI Clock is ON
	pGBL->GBL_CLK_DIS0 &= (MMP_UBYTE)(~(GBL_CLK_VI_DIS)); 
	MMPF_VIF_SetPIODir(VIF_SIF_RST, 0x01);  
	MMPF_VIF_SetPIOOutput(VIF_SIF_RST, 0x01);  // pull high this pin to force ROM boot download(MSDC) mode, 
	                                       // if the pin is high => MSDC mode, if the pin is low => normal boot mode.
	MMPF_SYS_ResetSystem(0x1);  // 1: re-boot FW to MSDC mode,
	
	return MMP_ERR_NONE;

}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_ResetSystem
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to reset system and system re-run from ROM code.

 The function is used to reset system and system re-run from ROM code.
@return It reports the status of the operation.
*/
#include "mmpf_wd.h"
MMP_ERR	MMPF_SYS_ResetSystem(MMP_UBYTE ubSpecialCase)
{
#if !defined(UPDATER_FW)
	//AITPS_GBL pGBL = AITC_BASE_GBL;
	#if (OS_CRITICAL_METHOD == 3)
	OS_CPU_SR   cpu_sr = 0;
	#endif
	
	#if (CHIP == VSN_V2)
	MMP_ULONG i;
	void (*FW_Entry)(void) = NULL;
	volatile 	MMP_UBYTE *ROM_BASE_B =   (volatile MMP_UBYTE *)0x80002000; //used for copy ROM
	volatile 	MMP_UBYTE *ROM_BASE_B_2 = (volatile MMP_UBYTE *)0x80003000;	//used for copy ROM
	MMP_ULONG ulTcmSize = 0x1000, ulSramSize = 0x3000;
	volatile 	MMP_UBYTE *TCM_BASE_B = (volatile MMP_UBYTE *)0x0;			
	volatile 	MMP_UBYTE *SRAM_BASE_B = (volatile MMP_UBYTE *)0x100000;
	#endif
	#if (CHIP == VSN_V3)
	void (*FW_Entry)(void) = NULL;
	AITPS_WD 	pWD = AITC_BASE_WD;
	MMP_ULONG counter = 0x0;
	#endif
	
	AITPS_GBL   pGBL = AITC_BASE_GBL;
	
	OS_ENTER_CRITICAL();
	
	#if (SYS_WD_ENABLE == 0x1)
	MMPF_WD_Kick();
	#endif
	
  	if(ubSpecialCase != 0x1) {
  		MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_VI, MMP_TRUE);
  	}
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_ISP, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_JPG, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_SCAL, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_GPIO, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_AUD, MMP_TRUE);
    
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_RAW, MMP_TRUE);
  	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_DMA, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_I2C, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_USB, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_H264, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_IBC, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_GRA, MMP_TRUE);
    
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_SD, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_PWM, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_PSPI, MMP_TRUE);
    MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_USB_PHY, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_CPU_SRAM, MMP_TRUE);
	
	MMPF_MMU_FlushDCache();
	AT91F_DisableDCache();
	AT91F_DisableICache();
	AT91F_DisableMMU();
	
	
	pGBL->GBL_CLK_DIS0 = (MMP_UBYTE)(~(GBL_CLK_MCI_DIS | GBL_CLK_VI_DIS| GBL_CLK_JPG_DIS | GBL_CLK_CPU_DIS | GBL_CLK_GPIO_DIS));
	pGBL->GBL_CLK_DIS1 = (MMP_USHORT)(~(GBL_CLK_DRAM_DIS | GBL_CLK_PWM_DIS | GBL_CLK_I2C_DIS | GBL_CLK_DMA_DIS | GBL_CLK_USB_DIS | GBL_CLK_CPU_PHL_DIS));
#if (CHIP == VSN_V3)
	pGBL->GBL_CLK_DIS2 = (MMP_UBYTE)(~(GBL_CLK_CIDC_DIS | GBL_CLK_GNR_DIS | GBL_CLK_COLOR_DIS)) ;
#endif
	
	
	pGBL->GBL_CLK_DIS1 &= ~GBL_CLK_BS_SPI_DIS;
#if (CHIP == VSN_V2)
	for(i = 0; i < ulTcmSize; i++) {
		TCM_BASE_B[i] = ROM_BASE_B[i];
	}
	
	for(i = 0; i < ulSramSize; i++) {
		SRAM_BASE_B[i] = ROM_BASE_B_2[i];
	}
	
	FW_Entry = (void (*)(void))(0x0);
	FW_Entry();	//enter the firmware entry
	while(1) {
	
	}
#endif
	
#if (CHIP == VSN_V3)
	//Change the Boot Strapping as ROM boot
	pGBL->GBL_CHIP_CTL |= MOD_BOOT_STRAP_EN;
	pGBL->GBL_CHIP_CFG = ROM_BOOT_MODE;
	pGBL->GBL_CHIP_CTL &= ~MOD_BOOT_STRAP_EN;
	
	//Turn-off watch dog
 	if((pWD->WD_MODE_CTL0 & WD_EN)!= 0x0) {
 		RTNA_DBG_Str(0, "\r\nTurn-off WD !!\r\n");
  	    pWD->WD_MODE_CTL0 = 0x2340;
 	}
	
	//VSN_V3, CPU access ROM code have HW bug, so we use reset ROM controller to archieve this purpose. 
	//Note: The busy waiting is necessary !!!  ROM controller need some time to re-load ROM code.
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_CPU_PERIF, MMP_FALSE);
	#pragma O0
	for(counter = 0x3FFFF; counter > 0 ; counter --) {
	}
	#pragma
	
	if(ubSpecialCase == 0x1) {
		FW_Entry = (void (*)(void))(0x0);
		FW_Entry();	//enter the firmware entry
		while(1);
	}
	
	
	//Finally, use watch-dog do timeout-reset, this purpose is to reset PLL as normal speed for serial-flash acessing issue.
	MMPF_WD_SetTimeOut(31, 128);
	MMPF_WD_EnableWD(MMP_TRUE, MMP_TRUE, MMP_FALSE, NULL, MMP_TRUE);
#endif
	

	#if 0  //The following test code is used CPU watch dog to archieve system reset
	//Change the Boot Strapping as ROM boot
	pGBL->GBL_CHIP_CTL |= MOD_BOOT_STRAP_EN;
    pGBL->GBL_CHIP_CFG = ROM_BOOT_MODE;
    pGBL->GBL_CHIP_CTL &= ~MOD_BOOT_STRAP_EN;
    
	MMPF_WD_Initialize();
	
	MMPF_WD_SetTimeOut(31, 128);
	
	MMPF_WD_EnableWD(MMP_TRUE, MMP_TRUE, MMP_FALSE, NULL, MMP_TRUE);
	#endif
#endif //#if !defined(UPDATER_FW)
	return MMP_ERR_NONE;
}
#endif


//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_ResetHModule
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to reset HW modules.

The function is used to reset HW modules.
@param[in] moduletype is used to select which modules to reset.
@param[in] bResetRegister is used enable/disable reset module's registers. 
@return It reports the status of the operation.
*/
MMP_ERR MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE moduletype, MMP_BOOL bResetRegister)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;
	
	if(moduletype <= MMPF_SYS_HMODULETYPE_GRA) {
		if(bResetRegister) {
			pGBL->GBL_RST_REG_EN |= (0x1 << moduletype);
		}
		pGBL->GBL_RST_CTL01 |= (0x1 << moduletype);
		MMPF_PLL_WaitCount(0x20);  //Note: 0x20 is just the test value used in PCAM project
		pGBL->GBL_RST_CTL01 &= ~(0x1 << moduletype);
		if(bResetRegister) {
			pGBL->GBL_RST_REG_EN &= ~(0x1 << moduletype);
		}
	}
	else {
		MMP_UBYTE	ubModuleType1 = (moduletype - MMPF_SYS_HMODULETYPE_CPU_SRAM);
		if((bResetRegister) && (ubModuleType1 > 0) && (ubModuleType1 > 0) && (ubModuleType1 < 4)) {
			pGBL->GBL_RST_CTL2 |= (0x1 << (ubModuleType1 + 4));
		}
		pGBL->GBL_RST_CTL2 |= (0x1 << ubModuleType1);
		MMPF_PLL_WaitCount(0x20);
		pGBL->GBL_RST_CTL2 &= ~(0x1 << ubModuleType1);
		if((bResetRegister) && (ubModuleType1 > 0) && (ubModuleType1 > 0) && (ubModuleType1 < 4)) {
			pGBL->GBL_RST_CTL2 &= ~(0x1 << (ubModuleType1 + 4));
		}
	}
	
	return MMP_ERR_NONE;
}
#if 0
//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_SetWakeUpEvent
//  Description :
//------------------------------------------------------------------------------
/** @brief The function set the wake up event when CPU enter sleep

The function set the PIN as Output mode (bEnable = MMP_TRUE) or Input mode.
@param[in] bEnable is used to turn on/off wake up event.
@param[in] event is used to select wake up event type. 
@param[in] piopin is the PIO number, please reference the data structure of MMPF_PIO_REG
@param[in] polarity is used for GPIO event wake up. To set high(0)/low(1) level wake up.
@return It reports the status of the operation.
*/
MMP_ERR MMPF_SYS_SetWakeUpEvent(MMP_BOOL bEnable, MMPF_WAKEUP_EVNET event, MMPF_PIO_REG piopin, MMP_UBYTE polarity)
{
#if (defined(ALL_FW)||defined(UPDATER_FW))&&(SYS_SELF_SLEEP_ENABLE == 0x1)
	MMP_UBYTE ubShiftBit = 0x0;
	AITPS_GBL pGBL = AITC_BASE_GBL;
	AITPS_USB_CTL pUSB_CTL = AITC_BASE_USBCTL;
	
	if(bEnable == MMP_TRUE) {
		if(event == MMPF_WAKEUP_GPIO) {	
			switch(piopin) {
				case MMPF_PIO_REG_GPIO0:
					ubShiftBit = GBL_WAKE_GPIO0;
					break;
				case MMPF_PIO_REG_GPIO6:
					ubShiftBit = GBL_WAKE_GPIO6;
					break;
				#if (CHIP == VSN_V3)
				case MMPF_PIO_REG_GPIO23:
					ubShiftBit = GBL_WAKE_GPIO23;
					break;
				case MMPF_PIO_REG_GPIO50:
					ubShiftBit = GBL_WAKE_GPIO50;
					break;
				case MMPF_PIO_REG_GPIO63:
					ubShiftBit = GBL_WAKE_GPIO63;
					break;
				#endif
				default:
					return MMP_SYSTEM_ERR_FORMAT;
					break; 
			}
			
			MMPF_PIO_Initialize();
			MMPF_PIO_EnableGpioMode(piopin, MMP_TRUE);	
			MMPF_PIO_EnableOutputMode(piopin, MMP_FALSE);
				
			if(polarity == 0x1) { //High level wake up
				pGBL->GBL_GPIO_WAKE_INT_POLARITY &= (~ubShiftBit);
			}
			else { //Low level wake up
				pGBL->GBL_GPIO_WAKE_INT_POLARITY |= (ubShiftBit);
			}
			
			pGBL->GBL_WAKE_CTL |= ubShiftBit;
		}
		else if(event == MMPF_WAKEUP_USB_RESUME) {
			if((pUSB_CTL->USB_POWER & 0x1) == 0x0) {
				pUSB_CTL->USB_POWER |= 0x1;
			}
			pGBL->GBL_WAKE_CTL |= GBL_WAKE_USB_RESUME;
		}
		else {
			return MMP_SYSTEM_ERR_FORMAT;
		} 
	}
	else {
		if(event == MMPF_WAKEUP_GPIO) {
			switch(piopin) {
				case MMPF_PIO_REG_GPIO0:
					ubShiftBit = GBL_WAKE_GPIO0;
					break;
				case MMPF_PIO_REG_GPIO6:
					ubShiftBit = GBL_WAKE_GPIO6;
					break;
				#if (CHIP == VSN_V3)
				case MMPF_PIO_REG_GPIO23:
					ubShiftBit = GBL_WAKE_GPIO23;
					break;
				case MMPF_PIO_REG_GPIO50:
					ubShiftBit = GBL_WAKE_GPIO50;
					break;
				case MMPF_PIO_REG_GPIO63:
					ubShiftBit = GBL_WAKE_GPIO63;
					break;
				#endif
				default:
					return MMP_SYSTEM_ERR_FORMAT;
					break; 
			}
			
			MMPF_PIO_EnableGpioMode(piopin, MMP_FALSE);
			pGBL->GBL_WAKE_CTL &= ~ubShiftBit;
		}
		else if(event == MMPF_WAKEUP_USB_RESUME) {
			pGBL->GBL_WAKE_CTL &= ~GBL_WAKE_USB_RESUME;
		}
		else {
			return MMP_SYSTEM_ERR_FORMAT;
		} 
	}
#else
	RTNA_DBG_Str(0, "CPU wake up event un-support ! \r\n");
#endif
	return MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_EnterSelfSleepMode
//  Description :
//------------------------------------------------------------------------------
/** @brief This function is used to do some operations before CPU sleep and make CPU sleep.
           After CPU wake up, the CPU start to run the code after it enters sleep.
           
    NOTE1: This function should co-work with MMPF_SYS_EnterPSMode !!!!
    NOTE2: It can not be placed into DRAM because DRAM will enter slef refresh mode !!
        
@return It reports the status of the operation.
*/
#pragma arm section code = "EnterSelfSleepMode", rwdata = "EnterSelfSleepMode",  zidata = "EnterSelfSleepMode"
MMP_ERR MMPF_SYS_EnterSelfSleepMode(void)
{
	
#if (defined(ALL_FW)||defined(UPDATER_FW))&&(SYS_SELF_SLEEP_ENABLE == 0x1)
	#if (OS_CRITICAL_METHOD == 3)
	static OS_CPU_SR   cpu_sr = 0;
	#endif
	static MMP_UBYTE    reset_sig;
	static MMP_UBYTE    ubClkDis0 = 0x0, ubClkDis2 = 0x0;
	static MMP_USHORT   usClkDis1 = 0x0;
	static MMP_ULONG    counter = 0, ulTempISR = 0x0;
	static AITPS_AIC 	pAIC = AITC_BASE_AIC;
	static AITPS_GBL    pGBL = AITC_BASE_GBL;
	static AITPS_USB_CTL pUSB = AITC_BASE_USBCTL ;
	//RTNA_DBG_Str(0, "Enter Sleep mode \r\n");
	static MMP_USHORT  usb_int ;// = pUSB_CTL->USB_INT_EVENT_SR & pUSB_CTL->USB_INT_EVENT_EN ;
	OS_ENTER_CRITICAL();
	ulTempISR = pAIC->AIC_IMR;
	pAIC->AIC_IDCR = 0xFFFFFFFF;
	
	#if (CHIP == VSN_V2)
	MMPF_SYS_SetWakeUpEvent(MMP_TRUE, MMPF_WAKEUP_GPIO, MMPF_PIO_REG_GPIO6, 0x0);
	#endif
	
	//MMPF_SYS_SetWakeUpEvent(MMP_TRUE, MMPF_WAKEUP_USB_RESUME, 0x0, 0x0);// Gason@remove for GPIO suspend
	
	ubClkDis0 = pGBL->GBL_CLK_DIS0;
	usClkDis1 = pGBL->GBL_CLK_DIS1;
	#if (CHIP == VSN_V3)
	ubClkDis2 = pGBL->GBL_CLK_DIS2;
	#endif
	
	MMPF_DRAM_SetSelfRefresh(MMP_TRUE);
	// ?????
	//if( (pUSB->USB_POWER & 0x02) == 0 ) {
	//    reset_sig = 1 ;
	//    goto exit_ps ;    
	//}
	
	usb_int = pUSB->USB_INT_EVENT_SR & pUSB->USB_INT_EVENT_EN ;
	
	if( usb_int & (RESUME_INT_BIT | RESET_INT_BIT)) {
	    reset_sig = 1 ;
	    goto exit_ps ;
	}
	
	pGBL->GBL_CLK_DIS0 = (MMP_UBYTE)(~(GBL_CLK_MCI_DIS | GBL_CLK_CPU_DIS | GBL_CLK_GPIO_DIS));
//	pGBL->GBL_CLK_DIS1 = (MMP_USHORT)(~(GBL_CLK_DRAM_DIS | GBL_CLK_USB_DIS | GBL_CLK_CPU_PHL_DIS));
	pGBL->GBL_CLK_DIS1 = (MMP_USHORT)(~(GBL_CLK_DRAM_DIS | GBL_CLK_USB_DIS ));
	#if (CHIP == VSN_V3)
	pGBL->GBL_CLK_DIS2 = (MMP_UBYTE)(GBL_CLK_AUD_CODEC_DIS | GBL_CLK_CIDC_DIS | GBL_CLK_GNR_DIS | GBL_CLK_SM_DIS | GBL_CLK_COLOR_DIS); 
	#endif
	
	pGBL->GBL_LCD_BYPASS_CTL0 |= GBL_ENTER_SELF_SLEEP_MODE;

	//When set 0x8F88 enter sleep mode, CPU still alive in a short time
	//So, need to insert some useless commands before setting 0x8F88 as zero
	#pragma O0
	for(counter = 0x1000; counter > 0 ; counter --) {
	}
	#pragma

	pGBL->GBL_LCD_BYPASS_CTL0 &= (~GBL_ENTER_SELF_SLEEP_MODE);
	pGBL->GBL_CLK_DIS0 = ubClkDis0;
	pGBL->GBL_CLK_DIS1 = usClkDis1;
	#if (CHIP == VSN_V3)
	pGBL->GBL_CLK_DIS2 = ubClkDis2; 
	#endif
	
exit_ps:
	
	MMPF_DRAM_SetSelfRefresh(MMP_FALSE);

	#if (CHIP == VSN_V2)
	MMPF_SYS_SetWakeUpEvent(MMP_FALSE, MMPF_WAKEUP_GPIO, MMPF_PIO_REG_GPIO6, 0x0);
	#endif
	//MMPF_SYS_SetWakeUpEvent(MMP_FALSE, MMPF_WAKEUP_USB_RESUME, 0x0, 0x0); // Gason@remove for GPIO suspend
	
	pAIC->AIC_IECR = ulTempISR;
	OS_EXIT_CRITICAL();
	
	if(reset_sig) {
        //VAR_B(0,USB_REG_BASE_B[USB_POWER_B] );
	    RTNA_DBG_Str(0, "-ExPS.Reset-\r\n");
	} else {
	    RTNA_DBG_Str(0, "-ExPS-\r\n");
	}
	
	#else
	RTNA_DBG_Str(0, "CPU slef sleep mode un-support ! \r\n");
#endif
	return MMP_ERR_NONE;

}
#pragma arm section code, rwdata,  zidata
//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_EnterPSMode
//  Description : called by the task with host command queue to process
//                command for System group
//------------------------------------------------------------------------------
// 0 : Exit Power Saving Mode
// 1 : Enter Power Saving Mode
// 2 : Power On Initial the unused Moudle
MMP_ERR MMPF_SYS_EnterPSMode(MMP_BOOL bEnterPSMode)
{
#define DISABLE_USB_PHY (1) // Enable for more power consumption
extern MMP_UBYTE  gbUsbHighSpeed;
	#if (defined(ALL_FW)||defined(UPDATER_FW))&&(SYS_SELF_SLEEP_ENABLE == 0x1)
	AITPS_GBL   pGBL = AITC_BASE_GBL;
//	AITPS_USB_DMA   pUSBDMA = AITC_BASE_USBDMA;
	//AITPS_MIPI pMIPI = AITC_BASE_MIPI;
	volatile MMP_UBYTE* REG_BASE_B = (volatile MMP_UBYTE*)0x80000000;
//	static MMP_BOOL ubMipiUseLan0 = MMP_FALSE, ubMipiUseLan1 = MMP_FALSE;
	
	pGBL->GBL_CLK_DIS0 &= ~GBL_CLK_VI_DIS  ;
	 
	if(bEnterPSMode == MMP_TRUE) {
		//MIPI RX
		REG_BASE_B[0x6160] &= 0xFB; // MIPI DPHY0 use HW design OPR to control Power-down
		REG_BASE_B[0x6170] &= 0xFB;  // MIPI DPHY1 use HW design OPR to control Power-down
        #if DISABLE_USB_PHY==1
        //USB PHY
        //sean@2012_09_12,for FS suspend/resume test 
        if(gbUsbHighSpeed) {
            SPI_Write(0x00, 0x0B00);
            SPI_Write(0x0A, 0x8520); 
            SPI_Write(0x0C, 0xff40); // 0xff40
        }
        #endif
        
		pGBL->GBL_LCD_BYPASS_CTL0 |= GBL_LCD_BYPASS_PWN_DPLL;
		pGBL->GBL_LCD_BYPASS_CTL0 &= (~ GBL_LCD_BYPASS_CLK_ACTIVE);
		pGBL->GBL_LCD_BYPASS_CTL0 |= GBL_XTAL_OFF_BYPASS;
	}
	else {
	
		pGBL->GBL_LCD_BYPASS_CTL0 &= (~GBL_LCD_BYPASS_PWN_DPLL);
		pGBL->GBL_LCD_BYPASS_CTL0 |= GBL_LCD_BYPASS_CLK_ACTIVE;
		pGBL->GBL_LCD_BYPASS_CTL0 &= (~GBL_XTAL_OFF_BYPASS);
		//USB PHY
        #if DISABLE_USB_PHY==1
        //USB PHY
        if(gbUsbHighSpeed) {
            SPI_Write(0x0C, 0x0000);
            SPI_Write(0x0A, 0x0020);
            SPI_Write(0x00, 0x0300);
            RTNA_WAIT_US(100); // wait 100 us for 0x0B
        }
        #endif
          
	}
	pGBL->GBL_CLK_DIS0 |= GBL_CLK_VI_DIS ;
	
	#endif //#if defined(ALL_FW)
	return  MMP_ERR_NONE;

}

MMP_ERR MMPF_SYS_ConfigIOPad(void)
{
    //T.B.D for VSN_V2
    return MMP_ERR_NONE ;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_EnterBypassMode
//  Description : called by the task with host command queue to process
//                command for System group
//------------------------------------------------------------------------------
MMP_ERR MMPF_SYS_EnterBypassMode(MMP_BOOL bEnterBypassMode)
{
    AITPS_GBL   pGBL = AITC_BASE_GBL;

    if (bEnterBypassMode) {
      	pGBL->GBL_LCD_BYPASS_CTL0 &= (~GBL_LCD_BYPASS_PWN_DPLL);
		pGBL->GBL_LCD_BYPASS_CTL0 |= GBL_LCD_BYPASS_CLK_ACTIVE;
    }
    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_SuspendCPU
//  Description : called by the task with host command input to suspend
//                the OS timer
//------------------------------------------------------------------------------
MMP_ERR MMPF_SYS_SuspendCPU(MMP_BOOL bSuspend)
{
    AITPS_AIC   pAIC = AITC_BASE_AIC;

    if (bSuspend) {
		RTNA_AIC_IRQ_Dis(pAIC, AIC_SRC_TC0);
    }
    else {
		RTNA_AIC_IRQ_En(pAIC, AIC_SRC_TC0);
    }

    return  MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_InitializeHIF
//  Description : Initialize host command ISR
//------------------------------------------------------------------------------
#if (defined(ALL_FW)&&(PCAM_EN == 0))

MMP_ERR MMPF_SYS_InitializeHIF(void)
{
    AITPS_AIC   pAIC = AITC_BASE_AIC;
    AITPS_GBL   pGBL = AITC_BASE_GBL;

    pGBL->GBL_HOST2CPU_INT_SR = GBL_HOST2CPU_INT;
    pGBL->GBL_HOST2CPU_INT_EN |= GBL_HOST2CPU_INT;

    RTNA_AIC_Open(pAIC, AIC_SRC_HOST, hif_isr_a,
                AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE    | 3);

    RTNA_AIC_IRQ_En(pAIC, AIC_SRC_HOST);

    return MMP_ERR_NONE;
}
/** @}*/ //end of MMPF_SYS



/** @addtogroup MMPF_HIF
@{
*/

/** @brief Host command interrupt service routine
*/
void MMPF_HIF_HifISR(void)
{
    MMP_USHORT  command;

    AITPS_GBL   pGBL = AITC_BASE_GBL;

    pGBL->GBL_HOST2CPU_INT_SR = GBL_HOST2CPU_INT;

    command = MMPF_HIF_GetCmd();

    switch (command & GRP_MASK) {
    case GRP_SYS:
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_SYS, MMPF_OS_FLAG_SET);
        break;
    case GRP_SENSOR:
	    #if (((DSC_R_EN))&&(FDTC_SUPPORT == 1))
    	if ((command & FUNC_MASK) == 0x0E) {
        	MMPF_OS_SetFlags(FDTC_Flag, SYS_FLAG_FDTC, MMPF_OS_FLAG_SET);
        }
        else {
        #endif
        	MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_SENSOR, MMPF_OS_FLAG_SET);
		#if (((DSC_R_EN))&&(FDTC_SUPPORT == 1))
        }
        #endif
        break;
    case GRP_DSC:
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_DSC, MMPF_OS_FLAG_SET);
        break;
    case GRP_VID:
        if ((command & FUNC_MASK) < 0x10) {
            MMP_USHORT usFunc = command & (GRP_MASK | FUNC_MASK);
            if (usFunc == HIF_CMD_PARSER) {
                MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_PSR3GP, MMPF_OS_FLAG_SET);
            }
            else if (usFunc == HIF_CMD_VIDEO_DECODER){
                MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_VID_DEC, MMPF_OS_FLAG_SET);
            }
            else if (usFunc == HIF_CMD_MFD){
                MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_VID_DEC, MMPF_OS_FLAG_SET);
            }
            else {
                MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_VIDPLAY, MMPF_OS_FLAG_SET);
            }
        }
        // Will's code
        else if ((command & FUNC_MASK) == 0x13){
            MMP_USHORT usFunc = command & (GRP_MASK | FUNC_MASK);
            if (usFunc == HIF_CMD_H264_DEC){
                MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_VID_DEC, MMPF_OS_FLAG_SET);
            }
        }
        //end
        else {
            if ((command & FUNC_MASK) & 0x08) { // 0x18 & 0x19
                MMPF_OS_SetFlags(SYS_Flag_Hif, CMD_FLAG_VIDRECD, MMPF_OS_FLAG_SET);
            }
            else { // 0x10 & 0x11
                MMPF_OS_SetFlags(SYS_Flag_Hif, CMD_FLAG_MGR3GP, MMPF_OS_FLAG_SET);
            }
        }
        break;
    case GRP_AUD:
        if ((command & FUNC_MASK) < 0x10) {
            MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOPLAY, MMPF_OS_FLAG_SET);
        }
        else {
            MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_AUDIOREC, MMPF_OS_FLAG_SET);
        }
        break;
    case GRP_STG:
        break;
    case GRP_USB:
        if((command & (GRP_MASK|FUNC_MASK)) == HIF_CMD_MSDC_ACK){
            //RTNA_DBG_Str0("get ack\r\n");
            MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_MEMDEV_ACK, MMPF_OS_FLAG_SET);
            MMPF_HIF_ClearCmd();
        }
        else
        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_USB, MMPF_OS_FLAG_SET);
        break;
    case GRP_USR:
        if ((command & FUNC_MASK) <= 0x02) {
            MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_FS, MMPF_OS_FLAG_SET);
        }
        else {
#if defined(BROGENT_FW)
            if (((command & FUNC_MASK) >= 0x03) && ((command & FUNC_MASK) <= 0x08)) {
            MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_BROGENT1, MMPF_OS_FLAG_SET);
            }
#endif /* defined(BROGENT_FW) */
            #if (APP_EN)
    	    if ((command & FUNC_MASK) == 0x09) {
        	    MMPF_OS_SetFlags(APP_Flag, SYS_FLAG_APP, MMPF_OS_FLAG_SET);
            }
            #endif
        }
        break;
    }

}
#endif
/** @} */ // end of MMPF_HIF

//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_GetFWEndAddr
//  Description : called by the task with host command queue to process
//                command for System group
//------------------------------------------------------------------------------

#if defined(ALL_FW)
extern unsigned int Image$$ALL_DRAM$$ZI$$Limit;
#endif
MMP_ERR MMPF_SYS_GetFWEndAddr(MMP_ULONG *ulEndAddr)
{
	#if defined(ALL_FW)
	*ulEndAddr = ((MMP_ULONG)&Image$$ALL_DRAM$$ZI$$Limit + 0xFFF) & (~0xFFF);
	#endif

    return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_GetAudio1StartAddr
//  Description : called by the task with host command queue to process
//                command for System group
//------------------------------------------------------------------------------
MMP_ERR MMPF_SYS_GetAudioStartAddr(MMP_ULONG coderange, MMP_ULONG *ulEndAddr)
{
    return MMP_ERR_NONE;
}
/** @addtogroup MMPF_SYS
@{
*/

//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_ProcessCmd
//  Description : called by the task with host command queue to process
//                command for System group
//------------------------------------------------------------------------------
#if (defined(ALL_FW)&&(PCAM_EN == 0))
MMP_ERR MMPF_SYS_ProcessCmd(void)
{
    MMP_ULONG   ulParameter[6], i;
    MMP_USHORT  usCommand;

	MMP_ULONG	ulData;
	MMP_ULONG   ulGroupFreq = 0;
    usCommand = MMPF_HIF_GetCmd();
    //GBL_HOST_PARM only has 24 bytes (12 words or 6 ulong int)
    for (i = 0; i < 24; i += 4)
        ulParameter[i >> 2] = MMPF_HIF_CmdGetParameterL(i);

    MMPF_HIF_SetCmdStatus(SYSTEM_CMD_IN_EXEC);
    MMPF_HIF_ClearCmd();

    switch (usCommand & (GRP_MASK|FUNC_MASK)) {
    #if (defined(ALL_FW)&&(PCAM_EN == 0))
    case HIF_CMD_VERSION_CONTROL:
        switch (usCommand & SUB_MASK) {
        case CUSTOMER_ID:
            MMPF_HIF_CmdSetParameterW(0, gbCustomer.customer);
            MMPF_HIF_CmdSetParameterW(2, gbCustomer.project);
            MMPF_HIF_CmdSetParameterW(4, gbCustomer.hardware);
            break;
        case FW_ID:
            MMPF_HIF_CmdSetParameterW(0, gbFwVersion.major);
            MMPF_HIF_CmdSetParameterW(2, gbFwVersion.minor);
            MMPF_HIF_CmdSetParameterW(4, gbFwVersion.build);
            break;
        case FW_RELEASE_TIME:
            // append the whole structure of the compiler build version
            for (i = 0; i <= COMPILER_TIME_LEN; i++) {
                MMPF_HIF_CmdSetParameterB(i, mmpf_buildVersion.szTime[i]);
            }
            RTNA_DBG_Str1("===== " FIRMWARE_NAME " Build: " __TIME__ " " __DATE__ " =====\r\n");
            break;
        case FW_RELEASE_DATE:
            // append the whole structure of the compiler build version
            for (i = 0; i <= COMPILER_DATE_LEN; i++) {
                MMPF_HIF_CmdSetParameterB(i, mmpf_buildVersion.szDate[i]);
            }
            break;
        }
        break;
    case HIF_CMD_SET_BYPASS_MODE:
        MMPF_SYS_EnterBypassMode(((usCommand & SUB_MASK) == ENTER_BYPASS_MODE) ? MMP_TRUE : MMP_FALSE);
        break;
    #endif    

    case HIF_CMD_SET_PS_MODE:
        MMPF_SYS_EnterPSMode(((usCommand & SUB_MASK) == ENTER_PS_MODE) ? MMP_TRUE : MMP_FALSE);
        break;

    case HIF_CMD_SYSTEM_SET_PLL:
        switch (usCommand & SUB_MASK) {
        case PLL_SETTING:
            MMPF_PLL_Setting(ulParameter[0], (MMP_BOOL)ulParameter[1]);
            break;
        case PLL_POWER_UP:
            MMPF_PLL_PowerUp(ulParameter[0], ulParameter[1]);
            break;
        }
        break;

#if (TV_EN)
    case HIF_CMD_CONFIG_TV:
        switch (usCommand & SUB_MASK) {
        case TV_COLORBAR:
            MMPF_TV_ColorBar((MMPF_TV_TYPE)ulParameter[0]);
            break;
        case TVENC_INIT:
            MMPF_TV_Initialize();
            break;
        case TVENC_UNINIT:
            MMPF_TV_Uninitialize();
            break;
        case TVENC_REG_SET:
            MMPF_TV_EncRegSet(ulParameter[0], ulParameter[1]);
            break;
        case TVENC_REG_GET:
            MMPF_TV_EncRegGet(ulParameter[0], &ulData);
            MMPF_HIF_CmdSetParameterL(4, ulData);
            break;
        }
        break;
#endif
    case HIF_CMD_CONFIG_HINT:
        switch (usCommand & SUB_MASK) {
        case FLAG_ENABLE:
        case FLAG_DISABLE:
            MMPF_HIF_SetCpu2HostIntFlag(
                ((usCommand & SUB_MASK) == FLAG_ENABLE) ? MMP_TRUE : MMP_FALSE,
                (MMPF_HIF_INT_FLAG)ulParameter[0]);
            break;
        case FLAG_CLEAR:
            MMPF_HIF_ClearCpu2HostIntFlag(ulParameter[0]);
            break;
        }
        break;
    case HIF_CMD_GET_FW_ADDR:
		switch (usCommand & SUB_MASK) {
		case FW_END:
	       	MMPF_SYS_GetFWEndAddr(&ulData);
	       	break;
		case AUDIO_START:
	       	MMPF_SYS_GetAudioStartAddr(ulParameter[0], &ulData);
	       	break;
		}
       	MMPF_HIF_CmdSetParameterL(0, ulData);
        break;
    case HIF_CMD_ECHO:
    	switch (usCommand & SUB_MASK) {
		case GET_INTERNAL_STATUS:
			RTNA_DBG_Str(0, "HIF_CMD_ECHO\r\n");
    		MMPF_HIF_CmdSetParameterW(0, usCommand);
    		break;
    	}
    	break;
    case HIF_CMD_SYSTEM_GET_GROUP_FREQ:
		MMPF_PLL_GetGroupFreq(ulParameter[0], &ulGroupFreq);
		MMPF_HIF_CmdSetParameterL(4, ulGroupFreq);
    	break;
    case HIF_CMD_SYSTEM_SUSPEND_CPU:
		MMPF_SYS_SuspendCPU(ulParameter[0]);
    	break;
    }

    MMPF_HIF_ClearCmdStatus(SYSTEM_CMD_IN_EXEC);

    return MMP_ERR_NONE;
}

#endif
//------------------------------------------------------------------------------
//  Function    : SYS_Task
//  Description : SYS Task Function
//------------------------------------------------------------------------------
//#ifdef BUILD_CE
//#include "mmph_hif.h"
//extern MMP_BOOL bWaitForSysCommandDone;
//#endif


MMP_USHORT MMPF_SYS_PostOverWrMsg(MMP_ULONG src_id,MMP_ULONG dst_id,MMP_ULONG msg_id,MMP_ULONG msg_sub_id,void *data)
{
	
    MMP_USHORT err = MSG_ERROR_NONE ;
    #if defined(ALL_FW)
    msg_t *msg;

    msg = get_overwr_msg(src_id);
    if(!msg ){
        return MSG_INIT_ERR ;
    }
    msg->src_id = src_id ; /*CallerID*/
    msg->dst_id = dst_id ; /*CalledID*/
    msg->msg_id = msg_id ; /*MSG ID*/
    msg->msg_sub_id = msg_sub_id ; /* MSG Sub Id */
    msg->msg_data = data ; /*API parameter*/
    err = MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_SYS, MMPF_OS_FLAG_SET);
    #endif
    return err;
}


msg_t *MMPF_SYS_MakeMsg(MMP_ULONG src_id,MMP_ULONG dst_id,MMP_ULONG msg_id,MMP_ULONG msg_sub_id,void *data)
{
	#if defined(ALL_FW)
    msg_t *msg;
    msg = allocate_msg(src_id);
    if(!msg ){
        return (msg_t *)0 ;
    }
    msg->src_id = src_id ; /*CallerID*/
    msg->dst_id = dst_id ; /*CalledID*/
    msg->msg_id = msg_id ; /*MSG ID*/
    msg->msg_sub_id = msg_sub_id ; /* MSG Sub Id */
    msg->msg_data = data ; /*API parameter*/
   
    return msg ;
    #else //#if defind(ALL_FW)
    return (msg_t *)0;
    #endif
}

msg_t *MMPF_SYS_GetMsg(void)
{
    /*
    Get message first in case there has multiple messages
    */
    #if defined(ALL_FW)  
    msg_t *msg=0;
    MMPF_OS_FLAGS flags;
    msg = (msg_t *)get_msg(1);
    if(msg) {
        return msg ;
    }
    
    /*
    Wait mesage event
    */
    MMPF_OS_WaitFlags(SYS_Flag_Hif, SYS_FLAG_SYS,
                    MMPF_OS_FLAG_WAIT_SET_ANY | MMPF_OS_FLAG_CONSUME, 0, &flags);

    /*if(flags & PCAM_FLAG_OVERWR_MSG) {
        msg = (pcam_msg_t *)get_overwr_msg(1);
        if(msg) {
            return msg ;
        }
    }     */                
    if(flags & SYS_FLAG_SYS) {
        msg = (msg_t *)get_msg(1);
        if(msg) {
            return msg ;
        }
    }
    
    #endif
    return (msg_t *)0;
}

#if defined(ALL_FW)
extern void UsbEp0IntHandler(void);
extern  MMP_UBYTE   USBDevAddr;
MMPF_OS_SEMID  usb_ep_protect_sem = 0xFF;
extern MMPF_OS_SEMID           gUSBDMASem ;
extern MMP_UBYTE gbUSBSuspendFlag ;
#if USB_SUSPEND_TEST
extern volatile MMP_UBYTE gbUSBSuspendEvent ;
#endif
extern void UsbRestIntHandler(void);
extern void UsbEp3TxIntHandler(void);
extern void UsbEp1RxIntHandler(void);
extern void UsbEp2TxIntHandler(void);
extern void UsbEp1TxIntHandler(void);
extern void UsbEp3RxIntHandler(void);
extern void UsbEp4TxIntHandler(void);
extern void UsbEp4RxIntHandler(void);
#endif
extern void USB_SuspendProcess(void);
void SYS_Task(void)
{
	AITPS_GBL   pGBL = AITC_BASE_GBL;
   	#if !defined(ALL_FW)
    MMPF_OS_FLAGS   flags;
    #endif 
    
    #if defined(ALL_FW)
    msg_t *msg=0 ;
	MMP_USHORT err = MSG_ERROR_NONE ;
	#endif
    	
    RTNA_DBG_Str(0, "SYS_Tack() \r\n");	
    #if defined(ALL_FW)
	usb_ep_protect_sem = MMPF_OS_CreateSem(1);
 	gUSBDMASem = MMPF_OS_CreateSem(1);
	RTNA_DBG_PrintLong(0, gUSBDMASem);
	
	if(usb_ep_protect_sem==0xFF) {
        RTNA_DBG_Str(3,"<<usb_ep_protect_sem create sem err>>\r\n");
    }
	
	MMPF_I2cm_InitializeDriver();
	
	RTNA_DBG_PrintLong(0, usb_ep_protect_sem);
	init_msg_queue(1, SYS_Flag_Hif, SYS_FLAG_SYS);
	#endif
	
	
	/*while(1) {
		MMPF_OS_Sleep(1000);
		RTNA_DBG_Str(0, "!");
	}*/
	m_gbSystemCoreID = pGBL->GBL_CHIP_VER;
    m_gsISPCoreID = 989; //modify from 988
 
    //dbg_printf(0,"Core Id:%x,ISP Id:%d\r\n",m_gbSystemCoreID,m_gsISPCoreID);
    
	#if (DSC_R_EN)||(VIDEO_R_EN)||(VIDEO_P_EN)
    MMPF_DMA_Initialize();
   
    #endif
    
    #if (defined(ALL_FW)&&(PCAM_EN == 0))
	#if (VIDEO_P_EN)
    MMPF_Graphics_Initialize();
    #elif (VIDEO_R_EN)
    MMPF_Graphics_Initialize();
	#endif
	
	#endif
	
	
	RTNA_DBG_Str(0,"System Task Gason start.\r\n");
    while (TRUE) {
    #if defined(MBOOT_FW)||defined(UPDATER_FW)
        MMPF_OS_WaitFlags(SYS_Flag_Hif, SYS_FLAG_SYS,
                    MMPF_OS_FLAG_WAIT_SET_ANY | MMPF_OS_FLAG_CONSUME, 0, &flags);
    #endif
// add task event for usb interrupt here.
	#if defined(ALL_FW)
	msg = MMPF_SYS_GetMsg();
	if(!msg) {
	  continue ;
	}

    switch(msg->msg_id) {
		case SYS_MSG_USB_EP0_TX_INT:
			UsbEp0IntHandler();
			break;
		case SYS_MSG_USB_RESET_INT:
            USBDevAddr=0;
            UsbRestIntHandler();
            //RTNA_DBG_Str(0,"UsbRestIntHandler\r\n");
			break;
		case SYS_MSG_USB_SUSPEND_INT:
		#if USB_SUSPEND_TEST==1
            if(gbUSBSuspendEvent){
                gbUSBSuspendEvent = 0;
                if(gbUSBSuspendFlag == 0){
                    RTNA_DBG_Str(0, "-S3 SysTask-\r\n");
                    gbUSBSuspendFlag = 1;
                    USB_SuspendProcess() ;
                }
            }
        #endif

		    break ;
		case SYS_MSG_USB_EP3_RX_INT:
		     UsbEp3RxIntHandler();
			break;	
		case SYS_MSG_USB_EP3_TX_INT:
			 //UsbEp3TxIntHandler();
			break;
		case SYS_MSG_USB_EP4_TX_INT:
			 UsbEp4TxIntHandler();
			break;
		case SYS_MSG_USB_EP1_RX_INT:
			UsbEp1RxIntHandler();
			break;
		case SYS_MSG_USB_EP1_TX_INT:	//ANDY 20110617
			UsbEp1TxIntHandler();
			break;
		case SYS_MSG_USB_EP2_RX_INT:
			UsbEp1RxIntHandler();
			break;
		case SYS_MSG_USB_EP4_RX_INT:
			 UsbEp4RxIntHandler();
			break;			
		case SYS_MSG_USB_DMA_EP1_RX_INT:
			UsbEp1RxIntHandler();
			break;
		case SYS_MSG_USB_DMA_EP2_TX_INT:
			UsbEp2TxIntHandler();
			break;
		case SYS_MSG_USB_DMA_EP1_TX_INT:
			UsbEp1TxIntHandler();
			break;
		
    	default:
			RTNA_DBG_Str(0, "unregconize USB intr\r\n");
			break;
    }
		msg->err = err;
        /*Handler processed done*/
        /*Release semaphore*/
       /* if(msg->msg_sem) {
            MMPF_OS_ReleaseSem(*msg->msg_sem);
        } else {
            free_msg(msg, 1);
        }*/

    free_msg(msg, 1);


#if (defined(ALL_FW)&&(PCAM_EN == 0))
        MMPF_SYS_ProcessCmd();

        #ifdef BUILD_CE
        if (bWaitForSysCommandDone)
        {
	        MMPF_OS_SetFlags(SYS_Flag_Hif, SYS_FLAG_SYS_CMD_DONE, MMPF_OS_FLAG_SET);
	    }
        #endif
#endif

	#endif //#if defined(ALL_FW)        
    }
}

static char *gbSysCurFBPtr = 0 ;
static int  glSysResetFB   = 1 ;
void MMPF_SYS_InitFB(void) 
{
    glSysResetFB = 1 ;    
}

void *MMPF_SYS_AllocFB(char *tag,MMP_ULONG size, MMP_USHORT align)
{
    char *ptr ;
    MMP_ULONG alignSize ;
    #if 1 //align cache
    TASK_CRITICAL(
        if (glSysResetFB) {
            MMP_ULONG fw_end;
            MMPF_SYS_GetFWEndAddr(&fw_end);
            gbSysCurFBPtr = (char *) fw_end ;  
            glSysResetFB = 0 ; }
        ptr = (char *)(((MMP_ULONG)gbSysCurFBPtr + (MMP_ULONG)align - 1) & ((MMP_ULONG) -align));
        alignSize = ( size +(MMP_ULONG) align - 1) & ((MMP_ULONG) -align ) ;
        gbSysCurFBPtr = ptr + alignSize;
    )
    #else
    TASK_CRITICAL(
        if(glSysResetFB) {
            MMP_ULONG fw_end;
            MMPF_SYS_GetFWEndAddr(&fw_end);
            gbSysCurFBPtr = (char *) fw_end ;  
            glSysResetFB = 0 ;         
        }
        ptr = (char *)gbSysCurFBPtr ;        alignSize = ( size +(MMP_ULONG) align - 1) & ((MMP_ULONG) -align ) ;
        gbSysCurFBPtr += alignSize ;
    )
    #endif

    RTNA_DBG_Str0(tag);RTNA_DBG_Str0("\r\n");
    RTNA_DBG_Str0("@Addr:");RTNA_DBG_Long0( (MMP_ULONG)ptr ) ; RTNA_DBG_Str0("\r\n");
    RTNA_DBG_Str0("@Size:");RTNA_DBG_Long0( alignSize ) ; RTNA_DBG_Str0("\r\n");

    
    return (void *)ptr ;
}

void MMPF_SYS_SetCurFBAddr(char *ptr)
{
    TASK_CRITICAL(
        gbSysCurFBPtr = ptr ;
    )
}

void *MMPF_SYS_GetCurFBAddr(void)
{
    TASK_CRITICAL(
        if(glSysResetFB) {
            MMP_ULONG fw_end;
            MMPF_SYS_GetFWEndAddr(&fw_end);
            gbSysCurFBPtr = (char *) fw_end ;  
            glSysResetFB = 0 ;         
        }
    )
    return gbSysCurFBPtr ;
}
MMP_ERR MMPF_SYS_EnableGNRClock(MMP_BOOL bEnable)
{
	AITPS_GBL   pGBL = AITC_BASE_GBL;
	if (bEnable) {
		pGBL->GBL_CLK_DIS2 &= ~GBL_CLK_GNR_DIS;
	}
	else {
		pGBL->GBL_CLK_DIS2 |= GBL_CLK_GNR_DIS;
	}
	return MMP_ERR_NONE;
}

MMP_BOOL MMPF_SYS_GetGNRClockStatus(void)
{
	AITPS_GBL   pGBL = AITC_BASE_GBL;
	if ((pGBL->GBL_CLK_DIS2 & GBL_CLK_GNR_DIS)!=0) {
		return 0;
	}
	else {
		return 1;
	}

}
#if defined(MBOOT_FW) || defined(UPDATER_FW)
void MMPF_Display_GpioISR(void) {}
void MMPF_Display_IbcISR(void) {}
#endif

/** @}*/ //end of MMPF_SYS
#endif
