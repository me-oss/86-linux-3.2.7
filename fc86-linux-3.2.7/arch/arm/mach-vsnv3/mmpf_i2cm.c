//==============================================================================
//
//  File        : mmpf_i2cm.c
//  Description : MMPF_I2C functions
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

//#include "includes_fw.h"
#include <stddef.h>
#include <linux/module.h>
#include <linux/spinlock.h>

//#include <mach/lib_retina.h>
#include <mach/reg_retina.h>
#include <mach/mmp_reg_i2cm.h>
#include <mach/mmp_reg_vif.h>
//#if !defined(UPDATER_FW)
#include <mach/mmpf_pio.h>
//#endif //#if !defined(UPDATER_FW)

#include <mach/mmpf_i2cm.h>
#include <mach/mmpf_vif.h>
#include <mach/mmpf_pll.h>

#include <mach/mmp_reg_gbl.h>
#include <linux/delay.h>


static DEFINE_SPINLOCK(i2cm_lock);

/** @addtogroup MMPF_I2CM
@{
*/
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
//#define SOFTWARE_I2CM


//static MMP_USHORT gI2CM_SPEED_CONTROL[MMPF_I2CM_MAX_SPEED_COUNT] = {0, 0, 100, 250, 400, 550, 700, 850, 1000,50,10,150,125,200,120,110,130,140};
static MMP_USHORT gI2CM_SPEED_CONTROL[MMPF_I2CM_MAX_SPEED_COUNT] = {0,0,10,20,30,40,50,60,70,80,90,100,150,200, 250, 400, 550, 700, 850, 1000};

#define I2C_SEN             VIF_SIF_SEN
#define I2C_SCL             VIF_SIF_SCL
#define I2C_SDA             VIF_SIF_SDA
#define I2C_RST             VIF_SIF_RST

#define I2C_INPUT           MMP_FALSE
#define I2C_OUTPUT          MMP_TRUE

#define I2C_SET_OUT         MMP_TRUE
#define I2C_CLEAR_OUT       MMP_FALSE
#if	!defined(SENSOR_SOFTWARE_I2CM)
#define I2C_DELAY			0x20
#endif


#define I2CM_INT_MODE_EN 0
//==============================================================================
//
//                          MODULE VARIABLES
//
//==============================================================================
MMP_UBYTE   m_bRegLen[MMPF_I2CM_MAX_COUNT] = {0}, m_bDataLen[MMPF_I2CM_MAX_COUNT] = {0};
MMP_UBYTE	m_bSlaveAddr[MMPF_I2CM_SW_COUNT] = {0};
MMP_UBYTE	m_bI2CDelayTime[MMPF_I2CM_SW_COUNT] = {0};
static int gI2cmSemID[MMPF_I2CM_MAX_COUNT];  //Vin need change to spinlock
static MMP_ULONG    m_ulI2cRxReadTimeout[I2CM_HW_MAX_COUNT];

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//*----------------------------------------------------------------------------
// function : MMPF_I2cm_InitializeDriver
// input    : none
// output   : none
// descript : The semaphores initialize function
//*----------------------------------------------------------------------------
MMP_ERR MMPF_I2cm_InitializeDriver(void)
{
	MMP_UBYTE i = 0;
	static MMP_BOOL m_bInitialFlag = MMP_FALSE;
	MMP_ERR status = MMP_ERR_NONE;
	if (m_bInitialFlag == MMP_FALSE) {
		//#if (defined(ALL_FW) && (I2CM_INT_MODE_EN == 0x1))
		#if (I2CM_INT_MODE_EN == 0x1)
		status |= MMPF_I2C_PWM_Initialize();
		#endif
		
		for(i = 0; i < I2CM_HW_MAX_COUNT; i++) {
			#if (I2CM_INT_MODE_EN == 0x1)
//			gI2cmIntSemID[i] = MMPF_OS_CreateSem(0);//Vin spinlock
			#endif
			//m_I2cmCallBackFunc[i] = NULL;
			m_ulI2cRxReadTimeout[i] = 0;
		}
		
		for(i = 0; i < MMPF_I2CM_MAX_COUNT; i++) {
			//gI2cmSemID[i] = MMPF_OS_CreateSem(1);
			m_bRegLen[i] = 0;
			m_bDataLen[i] = 0;
			
		}
		
		/*for(i = 0; i < I2CM_SW_MAX_COUNT; i++) {
			m_bSlaveAddr[i] = 0;
			m_bI2CDelayTime[i] = 0;
		}
		m_I2cmHwClkSemID = MMPF_OS_CreateSem(1);
		m_ulClkCounter = 0;
		*/
		m_bInitialFlag = MMP_TRUE;
	}
	return status;
}

//*----------------------------------------------------------------------------
//* Function Name       : MMPF_I2c_Open
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------


//*----------------------------------------------------------------------------
//* Function Name       : MMPF_I2c_Close
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------


//*----------------------------------------------------------------------------
//* Function Name       : SetI2CDelay
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void MMPF_I2c_Delay(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute)
{
    int   j;
  
    for (j = 0; j < m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW] ;j++);
}

//*----------------------------------------------------------------------------
// function : MMPF_I2c_Start
// input    : none
// output   : none
// descript : I2C Start Condition
//            Control Pin : P11 -> SCL (O), P8 -> SDA (I/O)
//*----------------------------------------------------------------------------
static MMP_ERR MMPF_I2c_Start(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute)
{
	
	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_OUTPUT);
	    MMPF_VIF_SetPIOOutput((I2C_SCL|I2C_SDA), I2C_SET_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SDA, I2C_CLEAR_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
		
		//MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
    	//MMPF_I2c_Delay();
    }
    else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
    	#if !defined(UPDATER_FW)
    	MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_CLK_PIN, MMP_TRUE);
		MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
	
		MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
		MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_HIGH);
		
		MMPF_I2c_Delay(uI2cmAttribute);
	
		MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_LOW);
		MMPF_I2c_Delay(uI2cmAttribute);

		MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
    	MMPF_I2c_Delay(uI2cmAttribute);
    	#endif
    }
    else {
//    	RTNA_DBG_Str(0, "Error soft I2CM ID\r\n");
    }

	

	return	MMP_ERR_NONE;
}
//*----------------------------------------------------------------------------
// function : MMPF_I2C_Stop
// input    : none
// output   : none
// descript : I2C Stop Condition
//            Control Pin : P11 -> SCL (O), P8 -> SDA (I/O)
//*----------------------------------------------------------------------------
static MMP_ERR MMPF_I2c_Stop(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute)
{


 	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_OUTPUT);
	    MMPF_VIF_SetPIOOutput(I2C_SDA, I2C_CLEAR_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_SET_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SDA, I2C_SET_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	}
	else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
		#if !defined(UPDATER_FW)
    	MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
		MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_LOW);
		MMPF_I2c_Delay(uI2cmAttribute);
		MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_HIGH);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    #endif
    }
    else {
    	//RTNA_DBG_Str(0, "Error soft I2CM ID\r\n");
    }

	
	return	MMP_ERR_NONE;
}
//*----------------------------------------------------------------------------
// function : MMPF_I2c_WriteData(MMP_UBYTE data)
// input    : one byte data to write
// output   : none
// descript : send device address write command
//            Control Pin : P11 -> SCL (O), P8 -> SDA (I/O)
//*----------------------------------------------------------------------------
static MMP_ERR MMPF_I2c_WriteData(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute, MMP_UBYTE ubData)
{
    MMP_ULONG   i;
    MMP_UBYTE	cShift;
	
	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
	
	    //MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
		//MMPF_I2c_Delay(uI2cmID);
		

	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_OUTPUT);

	    cShift = 0x80;
	    for(i = 0; i < 8; i++) {
	        if(ubData & cShift) {
	            MMPF_VIF_SetPIOOutput(I2C_SDA, I2C_SET_OUT);
	            MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	        }
	        else {
	            MMPF_VIF_SetPIOOutput(I2C_SDA | I2C_SCL, I2C_CLEAR_OUT);
	        }

	        MMPF_I2c_Delay(uI2cmAttribute);
	        MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_SET_OUT);
	        MMPF_I2c_Delay(uI2cmAttribute);
	        MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	        MMPF_I2c_Delay(uI2cmAttribute);
	        cShift >>= 1;
	    }
	}
	else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
		#if !defined(UPDATER_FW)
    	MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
		cShift = 0x80;
    	for(i = 0; i < 8; i++) {
	        if(ubData & cShift) {
        	
	         	MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_HIGH);
	         	//MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
	        }
	        else {
	           	MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_LOW);
	        }	
			MMPF_I2c_Delay(uI2cmAttribute);
	        MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
	        MMPF_I2c_Delay(uI2cmAttribute);
	        MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
			 if (i == 7)
			 {
				MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_FALSE);
			 }
	        MMPF_I2c_Delay(uI2cmAttribute);
	        cShift >>= 1;
    	}
    	#endif //#if !defined(UPDATER_FW)
    }
    else {
    	//RTNA_DBG_Str(0, "Error soft I2CM ID\r\n");
    }

	
	return	MMP_ERR_NONE;
}

//*----------------------------------------------------------------------------
// function : MMPF_I2c_ReadData
// input    : one MMPF_I2CM_ID
// output   : one byte data to read
// descript : get one byte data from device
//            Control Pin : P11 -> SCL (O), P8 -> SDA (I/O)
//*----------------------------------------------------------------------------
static MMP_UBYTE  MMPF_I2c_ReadData(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute)
{
    int i;
    MMP_UBYTE  ReceiveData = 0;
    MMP_UBYTE  bit_val =0 ;


	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_INPUT);

	    ReceiveData = 0;
	    for(i = 0; i < 8; i++) {
	        MMPF_I2c_Delay(uI2cmAttribute);
	        MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_SET_OUT);
		    bit_val = (MMP_UBYTE)MMPF_VIF_GetPIOOutput(I2C_SDA);
	        ReceiveData |= bit_val;
	        if(i < 7)
	            ReceiveData <<= 1;
	        MMPF_I2c_Delay(uI2cmAttribute);
		    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	    }
	    return (ReceiveData);
	}
	else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
		#if !defined(UPDATER_FW)
    	 //MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
    	MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_FALSE);
	 
    	ReceiveData = 0;
    	for(i = 0; i < 8; i++) {
			MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
	        MMPF_PIO_GetData(uI2cmAttribute->SW_DAT_PIN, &bit_val);
	        MMPF_I2c_Delay(uI2cmAttribute);

	        ReceiveData |= bit_val;
	        if(i < 7)
	            ReceiveData <<= 1;
	        MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
			 if (i == 7)
			 {
				MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
				MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_LOW);
			 }
	        MMPF_I2c_Delay(uI2cmAttribute);
	     }
	     #endif //#if !defined(UPDATER_FW)
    }
    else {
    	//RTNA_DBG_Str(0, "Error soft I2CM ID\r\n");
    }
	


    return (ReceiveData);
}

//*----------------------------------------------------------------------------
// function : MMPF_I2c_GetACK
// input    : one MMPF_I2CM_ID
// output   : none
// descript : I2C Acknowledge Polling
//            Control Pin : P11 -> SCL (O), P8 -> SDA (I/O)
//*----------------------------------------------------------------------------
MMP_ERR MMPF_I2c_GetACK(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute)
{
    MMP_UBYTE  bit_val;
    MMP_ERR ret = MMP_ERR_NONE;
    MMP_ULONG count = 0x4FFF;

	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_INPUT);
	    MMPF_I2c_Delay(uI2cmAttribute);  //add by Leon
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_SET_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);

	    do{
	        bit_val = (MMP_UBYTE)MMPF_VIF_GetPIOOutput(I2C_SDA);
            count --;
            if(count == 0){
                ret = MMP_I2CM_ERR_SLAVE_NO_ACK;
                break;
            }
	    }while(bit_val);

	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_OUTPUT);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	}
	else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
		#if !defined(UPDATER_FW)
		//MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
		
	    MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_FALSE);
	    
	    //MMPF_I2c_Delay(uI2cmAttribute);
	    
	    MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
	    
	    MMPF_I2c_Delay(uI2cmAttribute);
	    do{
	        MMPF_PIO_GetData(uI2cmAttribute->SW_DAT_PIN, &bit_val);
            count --;
            if(count == 0){
                ret = MMP_I2CM_ERR_SLAVE_NO_ACK;
                break;
            }
	    }while(bit_val);
	   
	    MMPF_I2c_Delay(uI2cmAttribute);
	    //MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
	    MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    #endif //#if !defined(UPDATER_FW)
	    
	}
    else {
    	//RTNA_DBG_Str(0, "Error soft I2CM ID\r\n");
    }

	return	ret;
}
//*----------------------------------------------------------------------------
// function : MMPF_I2c_SendNACK
// input    : one MMPF_I2CM_ID
// output   : none
// descript : I2C NotAcknowledge for read
//            Control Pin : P11 -> SCL (O), P8 -> SDA (I/O)
//*----------------------------------------------------------------------------
MMP_ERR MMPF_I2c_SendNACK(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute)
{
	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
	    // set SDA to output
	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_OUTPUT);
	    MMPF_VIF_SetPIOOutput(I2C_SDA, I2C_SET_OUT);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_SET_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	    MMPF_VIF_SetPIOOutput(I2C_SDA, I2C_CLEAR_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_SET_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	}
	else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
		#if !defined(UPDATER_FW)
	 	MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
	 	MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_HIGH);
	    //MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_LOW);
	    //MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    #endif //#if !defined(UPDATER_FW)
	}
    else {
    	//RTNA_DBG_Str(0, "Error soft I2CM ID\r\n");
    }
	return	MMP_ERR_NONE;
}
//*----------------------------------------------------------------------------
// function : SendI2CACK
// input    : one MMPF_I2CM_ID
// output   : none
// descript : I2C Acknowledge for read
//            Control Pin : P11 -> SCL (O), P8 -> SDA (I/O)
//*----------------------------------------------------------------------------
MMP_ERR MMPF_I2c_SendACK(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute)
{
	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
	    // set SDA to output
	    MMPF_VIF_SetPIODir(I2C_SDA, I2C_OUTPUT);
	    MMPF_VIF_SetPIOOutput((I2C_SCL | I2C_SDA), I2C_CLEAR_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_SET_OUT);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_VIF_SetPIOOutput(I2C_SCL, I2C_CLEAR_OUT);
	 }
    else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
    	#if !defined(UPDATER_FW)
		MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
		MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_LOW);
	    //MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_LOW);
	    MMPF_I2c_Delay(uI2cmAttribute);
	    #endif
	}
    else {
    	//RTNA_DBG_Str(0, "Error soft I2CM ID\r\n");
    }

	return	MMP_ERR_NONE;
}

#if 0 // for IDE function name list by section
void __I2CM_GENERAL_INTERFACE__(){}
#endif
//*----------------------------------------------------------------------------
// function : MMPF_I2cm_Initialize
// input    : (enable/disable)
// output   : none
// descript : Enable I2C Device
//*----------------------------------------------------------------------------
//MMP_UBYTE ubSlaveAddr, MMP_UBYTE ubRegLen, MMP_UBYTE ubDataLen, MMP_USHORT usSckDiv)
MMP_ERR  MMPF_I2cm_Initialize(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute)
{
	AITPS_GBL   pGBL = AITC_BASE_GBL;
	AITPS_PAD   pPAD = AITC_BASE_PAD;
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
	MMP_USHORT  usSckDiv = 0;
	MMP_ULONG   ulSysGroupClk = 0;

	MMPF_PLL_GetGroupFreq(0, &ulSysGroupClk);
	//pGBL->GBL_CLK_EN |= GBL_CLK_I2CM ;
//printk("ulSysGroupClk  = %x\r\n");	
	ulSysGroupClk = 0x40740;
	if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR) {
		//Attention: Please note the SLAVE_ADDR is different among software and HW I2CM
	    pPAD->PAD_PSDA = PAD_E4_CURRENT | PAD_PULL_HIGH;
		
		m_bSlaveAddr[1] = (uI2cmAttribute->ubSlaveAddr) << 1;
		
		MMPF_VIF_SetPIODir(VIF_SIF_SDA, MMP_TRUE);
		MMPF_VIF_SetPIOOutput(VIF_SIF_SDA, MMP_TRUE);

		MMPF_VIF_SetPIODir(VIF_SIF_SCL, MMP_TRUE);
		MMPF_VIF_SetPIOOutput(VIF_SIF_SCL, MMP_TRUE);
		m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW] = uI2cmAttribute->ubDelayTime;
	}
	else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW){
		#if !defined(UPDATER_FW)
		if((uI2cmAttribute->SW_CLK_PIN == NULL) || (uI2cmAttribute->SW_DAT_PIN == NULL)) {
			//RTNA_DBG_Str(0, "SW I2C pin assign error !!\r\n");
			printk(KERN_ERR "I2C Device(%s) with address 0x%02x: SW I2C pin assign error ! r\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
			return MMP_ERR_NONE;
		}
		//Attention: Please note the SLAVE_ADDR is different among software and HW I2CM
        	m_bSlaveAddr[0] = uI2cmAttribute->ubSlaveAddr;
		MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_CLK_PIN, MMP_TRUE);
		MMPF_PIO_EnableOutputMode(uI2cmAttribute->SW_DAT_PIN, MMP_TRUE);
	
		MMPF_PIO_SetData(uI2cmAttribute->SW_CLK_PIN, PIO_HIGH);  
		MMPF_PIO_SetData(uI2cmAttribute->SW_DAT_PIN, PIO_HIGH);
		m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW] = uI2cmAttribute->ubDelayTime;
		#endif
		
	}
	else if (uI2cmAttribute->uI2cmID <= I2CM_HW_MAX_ID) { 

		#if 0//(CHIP == P_V2)
		if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_0) { 
		    pPAD->PAD_PSCK = PAD_PULL_HIGH;
		    pPAD->PAD_PSDA = PAD_PULL_HIGH;
		    pPAD->PAD_PSEN = PAD_PULL_HIGH;
		}
		#endif
		
#if 1//(CHIP == VSN_V2)||(CHIP == VSN_V3)
		if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_0) {    //HW I2C
            pGBL->GBL_IO_CTL1 |= (GBL_I2C_0_PAD_EN);
			if(uI2cmAttribute->ubPadNum == 0x0) {
				pGBL->GBL_IO_CTL1 &= (~GBL_I2C_0_PAD_SEL_SNR);
			}
			else {
				pGBL->GBL_IO_CTL1 |= GBL_I2C_0_PAD_SEL_SNR;
			}
		}
		else if (uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_1) {    //HW I2C
			pGBL->GBL_IO_CTL1 |= GBL_I2C_1_PAD_EN;
			if (uI2cmAttribute->ubPadNum == 0x0) {
				pGBL->GBL_IO_CTL0 &= ~(0x40);
			}
			else {
				pGBL->GBL_IO_CTL0 |= 0x40;
			}
			 //pGBL->GBL_IO_CTL1 |= GBL_I2C_1_PAD_EN;
		}
#endif

    	//Attention: Please note the SLAVE_ADDR is different among software and HW I2CM
		if (uI2cmAttribute->ubRegLen == 16) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR = uI2cmAttribute->ubSlaveAddr;
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL = 	I2CM_REG_16_MODE
	    	    											| I2CM_SCK_OH_MODE	// output high by internal signal
		    	    										| I2CM_SDA_OD_MODE  // by pull-up current, or resistor
		    	    										| I2CM_STOP_2_NOACK;
		} else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR = uI2cmAttribute->ubSlaveAddr;
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL = 	I2CM_REG_8_MODE
															| I2CM_SCK_OH_MODE // output high by internal signal
	       													| I2CM_SDA_OD_MODE // by pull-up current, or resistor
															| I2CM_STOP_2_NOACK;
		}


		#if 1//(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
		if (uI2cmAttribute->bInputFilterEn == MMP_TRUE) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CMDSET_WAIT_CNT |= I2CM_INPUT_FILTERN_EN;
		}
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CMDSET_WAIT_CNT &= (~I2CM_INPUT_FILTERN_EN);
		}
		
		if (uI2cmAttribute->bDelayWaitEn == MMP_TRUE) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CMDSET_WAIT_CNT |= I2CM_DELAY_WAIT_EN;
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_WAIT_DELAY_CYC = uI2cmAttribute->ubDelayCycle;
		}
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CMDSET_WAIT_CNT &= (~I2CM_DELAY_WAIT_EN);
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_WAIT_DELAY_CYC = 0;
		}
		
		if (uI2cmAttribute->b10BitModeEn == MMP_TRUE) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CMDSET_WAIT_CNT |= I2CM_10BITMODE_EN;
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR1 = uI2cmAttribute->ubSlaveAddr1;
		}
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CMDSET_WAIT_CNT &= (~I2CM_10BITMODE_EN);
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR1 = 0;
		}

		if (uI2cmAttribute->bClkStretchEn == MMP_TRUE) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CLK_STRETCH_EN |= I2CM_STRETCH_ENABLE;
		}
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CLK_STRETCH_EN &= (~I2CM_STRETCH_ENABLE);
		}
		#endif //(CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
		if((uI2cmAttribute->uI2cmSpeed) != MMPF_I2CM_SPEED_NONE) {
			usSckDiv = (ulSysGroupClk/2)/gI2CM_SPEED_CONTROL[uI2cmAttribute->uI2cmSpeed]; // / 2 is right
			//usSckDiv = glGroupFreq[0]/gI2CM_SPEED_CONTROL[uI2cmAttribute->uI2cmSpeed];
		}
		#if 0 //DEBUG
		RTNA_DBG_PrintLong(0, (MMP_ULONG)(usSckDiv));
		#endif
		
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SCK_DIVIDER =  usSckDiv;        //(I2CModule clock 96/2MHz -> 400KHz)


		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_DATA_HOLD_CNT = usSckDiv>>3;//usSckDiv>>3;//(usSckDiv>>3);  //MUST not > (usSckDiv/2). Sugguest as (usSckDiv/4)~(usSckDiv/8)
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CMDSET_WAIT_CNT = ((usSckDiv>>3) - 1); // Wait N+1 i2cm_clk for next I2C cmd set

        #if 1//IP == VSN_V2)||(CHIP == VSN_V3)
        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SCL_DUTY_CNT = (usSckDiv>>1);
        #endif

	}
	else {
		//RTNA_DBG_Str(0, "Error I2CM ID ! \r\n");
			printk(KERN_ERR "I2C Device(%s) with address 0x%02x: Error I2CM ID ! r\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
	}
	m_bRegLen[uI2cmAttribute->uI2cmID] = uI2cmAttribute->ubRegLen;
	m_bDataLen[uI2cmAttribute->uI2cmID] = uI2cmAttribute->ubDataLen;
	
	return MMP_ERR_NONE;


}


//*----------------------------------------------------------------------------
// function : MMPF_I2cm_WriteReg
// input    : one MMPF_I2CM_ATTRIBUTE, register address and the data
// output   : (data)
// descript : I2C Signal Register Word Reading
//*----------------------------------------------------------------------------
MMP_ERR	MMPF_I2cm_WriteReg(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_USHORT usData)
{
	 
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
	MMP_ULONG	ulI2cmTimeOut = 0xfff;
	MMP_ERR ret=0;
	unsigned long	flags;

	//spin_lock_irqsave(&i2cm_lock, flags);

	
	MMPF_I2cm_Initialize(uI2cmAttribute);
#if 0
    //RTNA_DBG_Str3("I2C.addr:");
    printk(KERN_ERR"Write\n");
    printk(KERN_ERR"I2C.addr:\n");
    //RTNA_DBG_Byte3(uI2cmAttribute->ubSlaveAddr);
    printk(KERN_ERR"%02x\n",uI2cmAttribute->ubSlaveAddr);
    //RTNA_DBG_Str3("\r\n");
    printk("\r\n");
    
    //RTNA_DBG_Str3("Reg:");
    printk(KERN_ERR"Reg:\n");
    //RTNA_DBG_Byte3(usReg);
    printk(KERN_ERR"%04x\n",usReg);
    //RTNA_DBG_Str3("\r\n");


    //RTNA_DBG_Str3("Data:");
    printk(KERN_ERR"Data:\n");
    //RTNA_DBG_Byte3(usData);
    printk(KERN_ERR"%04x\n",usData);
    //RTNA_DBG_Str3("\r\n");
    printk(KERN_ERR"\r\n");
    
#endif	
	//printk("i2c: reg[0x%x] = 0x%x\r\n",usReg,usData);	
	if ((uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW)||(uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR)) {		    
		if((uI2cmAttribute->SW_CLK_PIN == NULL) || (uI2cmAttribute->SW_DAT_PIN == NULL)) {
			//RTNA_DBG_Str(0, "SW I2C pin assign error !!\r\n");
			ret = MMP_ERR_NONE;
			goto exit;
		}
		
	    MMPF_I2c_Start(uI2cmAttribute);
	    MMPF_I2c_WriteData(uI2cmAttribute, m_bSlaveAddr[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	    MMPF_I2c_GetACK(uI2cmAttribute);
//Vin: Todo : Maybe we don't use SW i2c		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
		if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
			MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)(usReg >> 8));
			MMPF_I2c_GetACK(uI2cmAttribute);
		}
		MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)usReg);
	   	MMPF_I2c_GetACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);

		if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
			MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)(usData >> 8));
			MMPF_I2c_GetACK(uI2cmAttribute);
		}	    
	   	MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)usData);
		MMPF_I2c_GetACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);

		MMPF_I2c_Stop(uI2cmAttribute);
	}
	else {
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR = I2CM_SLAVE_NO_ACK | I2CM_TX_DONE;

		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR &= ~(I2CM_READ_MODE | I2CM_WRITE_MODE);

//		if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
		if(uI2cmAttribute->ubRegLen == 16){
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usReg >> 8);
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
		}
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
		}
		if(uI2cmAttribute->ubDataLen == 16){//if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 2;
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usData >> 8);
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData;
		}
		else if (uI2cmAttribute->ubDataLen == 0 ){ 
                        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 0; 
                        //pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData; 
                }
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 1;
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData;
		}

/*^M
                else if (uI2cmAttribute->ubDataLen == 0 ){^M
                        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 0;^M
                        //pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData;^M
                }*/



		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SET_CNT = 1;


		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR |= I2CM_WRITE_MODE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_MASTER_EN;
		while (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL & I2CM_MASTER_EN){
			udelay(10);
			ulI2cmTimeOut--;
			if(ulI2cmTimeOut==0){
				//RTNA_DBG_Str(0, "I2C time out w\r\n");
				printk(KERN_ERR "I2C Device(%s) with address 0x%02x Write Register time out r\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
				ret = MMP_I2CM_ERR_SLAVE_NO_ACK;				
				goto exit;
			}
		}

		if (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR & I2CM_SLAVE_NO_ACK) {
			printk(KERN_ERR "I2C Device(%s) with address 0x%02x Write Register No ACK\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
			//RTNA_DBG_Str(0, "I2C Error 1\r\n");
			//Clear FIFO. Otherwise, keeping for next transmission.
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RST_FIFO_SW = I2CM_FIFO_RST;
			/*      if (semStatus == OS_NO_ERR) {

			}*/
			ret = MMP_I2CM_ERR_SLAVE_NO_ACK;
			goto exit;
		}


		}
exit:	
	//spin_unlock_irqrestore(&i2cm_lock, flags);

	//if (semStatus == OS_NO_ERR) {

	//}
    return ret;
}
//*----------------------------------------------------------------------------
// function : MMPF_I2cm_ReadReg
// input    : one MMPF_I2CM_ATTRIBUTE , register address the related data pointer
// output   : (data)
// descript : I2C Signal Register Word Reading
//*----------------------------------------------------------------------------
MMP_ERR	MMPF_I2cm_ReadReg(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg,MMP_ULONG *usData )//MMP_USHORT *usData)
{
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
	MMP_UBYTE	semStatus = 0xFF;
	MMP_ULONG	ulI2cmTimeOut = 0xfff;
	MMP_USHORT  usCount = 0;
	unsigned long	flags;
	MMP_ERR		ret = MMP_ERR_NONE;

	//spin_lock_irqsave(&i2cm_lock, flags);

//Vin	semStatus = MMPF_OS_AcquireSem(gI2cmSemID[uI2cmAttribute->uI2cmID], I2CM_SEM_TIMEOUT);
	
//	if(semStatus == OS_ERR_PEND_ISR) {
//		MMPF_OS_AcceptSem(gI2cmSemID[uI2cmAttribute->uI2cmID], &usCount);
//		 if(usCount == 0) {
		 	//RTNA_DBG_Str(0, "\r\n\r\n!!!!!!!   I2CM@OS_ISR Fail R\r\n");
//		 	return MMP_I2CM_ERR_NO_RESOURCE;
//		 }
//	}
	
	MMPF_I2cm_Initialize(uI2cmAttribute);
	
	if ((uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW)||(uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR)) {
	
		if((uI2cmAttribute->SW_CLK_PIN == NULL) || (uI2cmAttribute->SW_DAT_PIN == NULL)) {
			//RTNA_DBG_Str(0, "SW I2C pin assign error !!\r\n");
//			if (semStatus == OS_NO_ERR) {
//				MMPF_OS_ReleaseSem(gI2cmSemID[uI2cmAttribute->uI2cmID]);   
//			}
			ret = MMP_ERR_NONE;
			goto exit;
		}
		
		MMPF_I2c_Start(uI2cmAttribute);
		MMPF_I2c_WriteData(uI2cmAttribute, m_bSlaveAddr[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
		MMPF_I2c_GetACK(uI2cmAttribute);
		//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);

		if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
			MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)(usReg >> 8));
			MMPF_I2c_GetACK(uI2cmAttribute);
		}
		MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)usReg);
		MMPF_I2c_GetACK(uI2cmAttribute);
		//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
		MMPF_I2c_Stop(uI2cmAttribute);

		MMPF_I2c_Start(uI2cmAttribute);
		MMPF_I2c_WriteData(uI2cmAttribute, m_bSlaveAddr[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW] + 1);
		MMPF_I2c_GetACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);

		*usData = 0;
		if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
			*usData = ((MMP_USHORT)MMPF_I2c_ReadData(uI2cmAttribute)) << 8;
			MMPF_I2c_SendACK(uI2cmAttribute);
		}
		*usData |= MMPF_I2c_ReadData(uI2cmAttribute);
		MMPF_I2c_SendNACK(uI2cmAttribute);
		//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
		MMPF_I2c_Stop(uI2cmAttribute);

	}
	else {	
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RST_FIFO_SW = I2CM_FIFO_RST;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR = I2CM_SLAVE_NO_ACK | I2CM_TX_DONE;

		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR &= ~(I2CM_READ_MODE | I2CM_WRITE_MODE);
#if 1
		if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usReg >> 8);
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
		}
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
		}
#endif

		

		if (m_bDataLen[uI2cmAttribute->uI2cmID] == 32) {
//printk(KERN_ERR"=>m_bDataLen=>length 32 \n");
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 4;
		}else if (m_bDataLen[uI2cmAttribute->uI2cmID] == 24) {
//printk(KERN_ERR"=>m_bDataLen=>length 24 \n");
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 3;
		}else if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
//printk(KERN_ERR"=>m_bDataLen=>length 16 \n");
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 2;
		}
		else {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 1;
		}

		if(uI2cmAttribute->ubDelayTime>0){
			//printk(KERN_ERR" udelay time %d microseconds\n",uI2cmAttribute->ubDelayTime);
			udelay(uI2cmAttribute->ubDelayTime);
		}


		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SET_CNT = 1;

		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR |= I2CM_READ_MODE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_MASTER_EN;
		while (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL & I2CM_MASTER_EN){
			ulI2cmTimeOut--;
			udelay(10);
			if(ulI2cmTimeOut==0){
			//RTNA_DBG_Str(0, "I2C time out r\r\n");
				printk(KERN_ERR "I2C Device(%s):I2C time out r\r\n",uI2cmAttribute->name);
				ret = MMP_I2CM_ERR_READ_TIMEOUT;
				goto exit;
			}
		}
		if (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR & I2CM_SLAVE_NO_ACK) {
			printk(KERN_ERR "I2C Device(%s) read with Slave Address 0x%02x No Ack\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
			
	        //Clear FIFO. Otherwise, keeping for next transmission.
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RST_FIFO_SW = I2CM_FIFO_RST;
//	        if (semStatus == OS_NO_ERR) {
//				MMPF_OS_ReleaseSem(gI2cmSemID[uI2cmAttribute->uI2cmID]);   
//			}
			ret = MMP_I2CM_ERR_SLAVE_NO_ACK;
			goto exit;
		}

		if (m_bDataLen[uI2cmAttribute->uI2cmID] == 32) {
//printk(KERN_ERR"=>length 32 \n");
			*usData = ((MMP_ULONG)pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]) << 24;
			*usData |= ((MMP_ULONG)pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]) << 16;
			*usData |= ((MMP_ULONG)pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]) << 8;
			*usData |= (MMP_ULONG)(pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]);
		}else if (m_bDataLen[uI2cmAttribute->uI2cmID] == 24) {
//printk(KERN_ERR"=>length 24 \n");
			*usData = ((MMP_ULONG)pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]) << 16;
			*usData |= ((MMP_ULONG)pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]) << 8;
			*usData |= (MMP_ULONG)(pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]);
		}else if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
//printk(KERN_ERR"=>length 16 \n");
			*usData = ((MMP_USHORT)pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]) << 8;
			*usData |= (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]);
		}
		else {
			*usData = pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0];
		}

	}
//printk(KERN_ERR"i2c: rreg[0x%x]=0x%X \r\n",usReg,*usData);
exit:
	//spin_unlock_irqrestore(&i2cm_lock, flags);
	
	//printk("i2c: rreg[0x%x]=0x%X \r\n",usReg,*usData);
//	if (semStatus == OS_NO_ERR) {
//		MMPF_OS_ReleaseSem(gI2cmSemID[uI2cmAttribute->uI2cmID]);   
//	}
	return ret;
}
#if 0
MMP_ERR	MMPF_I2cm_CCI_ReadData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute,MMP_UBYTE ubReadDataCount ,MMP_UBYTE *usData)
{
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
	MMP_UBYTE	semStatus = 0xFF;
	MMP_ULONG	ulI2cmTimeOut = 0xfff, i = 0x0;
	//MMP_USHORT  usCount = 0;
	
	/*semStatus = MMPF_OS_AcquireSem(gI2cmSemID[uI2cmAttribute->uI2cmID], I2CM_SEM_TIMEOUT);
	
	if(semStatus == OS_ERR_PEND_ISR) {

		 if(usCount == 0) {
		 	RTNA_DBG_Str(0, "\r\n\r\n!!!!!!!   I2CM@OS_ISR Fail R\r\n");
		 	return MMP_I2CM_ERR_NO_RESOURCE;
		 }
	}
	*/
//	MMPF_I2cm_Initialize(uI2cmAttribute);
	
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RST_FIFO_SW = I2CM_FIFO_RST;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR = I2CM_SLAVE_NO_ACK | I2CM_TX_DONE;

		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR &= ~(I2CM_READ_MODE | I2CM_WRITE_MODE);

		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_RFCL_MODE_EN; //Turn-on CCI RFCL mode
		if(m_bDataLen[uI2cmAttribute->uI2cmID] == 0x8) {
			if (ubReadDataCount < 0xFF) {
			    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = ubReadDataCount;
		    }
		    else {
			    //RTNA_DBG_Str(0, "\r\nByte Count error 1 !!!!\r\n");
			    return 0;
		    }
	    }
	    else {
	    	if (ubReadDataCount < 0x7F) {
			    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = ubReadDataCount;
		    }
		    else {
			    //RTNA_DBG_Str(0, "\r\nByte Count error 2 !!!!\r\n");
			    return 0;
		    }
	    }

	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SET_CNT = 1;

		
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR |= I2CM_READ_MODE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_MASTER_EN;
		while (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL & I2CM_MASTER_EN){
			ulI2cmTimeOut--;
					udelay(10);
			if(ulI2cmTimeOut==0){
				printk(KERN_ERR "I2C Device(%s) with address 0x%02x IIC Read Data time out r\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
				return MMP_I2CM_ERR_READ_TIMEOUT;
			}
		}

		if (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR & I2CM_SLAVE_NO_ACK) {
			//RTNA_DBG_Str(0, "I2C Error 2\r\n");
			//Clear FIFO. Otherwise, keeping for next transmission.
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RST_FIFO_SW = I2CM_FIFO_RST;
/*
			if (semStatus == OS_NO_ERR) {

			}
*/			
			return MMP_I2CM_ERR_SLAVE_NO_ACK;
		}
		for(i = 0x0; i < ubReadDataCount; i++) {
			if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
				*usData = ((MMP_USHORT)pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]) << 8;
				*usData |= (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]);
			}
			else {
				*usData = pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0];
			}
			usData++;
		}
		
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL &= (~I2CM_RFCL_MODE_EN); //Turn-off CCI RFCL mode
	
	
//	if (semStatus == OS_NO_ERR) {

//	}
    return MMP_ERR_NONE;
}
#endif
//*----------------------------------------------------------------------------
// function : MMPF_I2cm_WriteRegSet
// input    : (address)
// output   : (data)
// descript : I2C Signal Register Word Reading
//*----------------------------------------------------------------------------
MMP_ERR	MMPF_I2cm_WriteRegSet(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT *usReg, MMP_USHORT *usData, MMP_UBYTE usSetCnt)
{
	MMP_USHORT  i;
	MMP_ERR		ret;
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
//	MMP_UBYTE	semStatus = 0xFF;
	MMP_ULONG ulI2cmTimeOut= 0xfff;
	
	if ((uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW)||(uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR)) {
		for (i = 0; i < usSetCnt; i++) {
			ret = MMPF_I2cm_WriteReg(uI2cmAttribute, usReg[i], usData[i]);
			if (ret != MMP_ERR_NONE)
				return	ret;
		}
	}
	else {

		MMPF_I2cm_Initialize(uI2cmAttribute);
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR = I2CM_SLAVE_NO_ACK | I2CM_TX_DONE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR &= ~(I2CM_READ_MODE | I2CM_WRITE_MODE);

	    for (i = 0; i < usSetCnt; i++) {
	    	if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
	    	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usReg[i] >> 8);
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg[i];
	    	}
		    else {
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg[i];
	    	}
		    if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 2;
	    	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usData[i] >> 8);
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData[i];
	        }
	        else {
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 1;
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData[i];
	        }
	    }
	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SET_CNT = usSetCnt;

		
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR |= I2CM_WRITE_MODE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_MASTER_EN;
		while (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL & I2CM_MASTER_EN){
			ulI2cmTimeOut--;
					udelay(10);
			if(ulI2cmTimeOut==0){
				printk(KERN_ERR "I2C Device(%s) with address 0x%02x Write Register time out r\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
				return MMP_I2CM_ERR_READ_TIMEOUT;

			}
		}
			
			

		if (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR & I2CM_SLAVE_NO_ACK) {
			//RTNA_DBG_Str(3, "I2C Error 3\r\n");
			//if (semStatus == OS_NO_ERR) {

			//}
			return MMP_I2CM_ERR_SLAVE_NO_ACK;
		}
		//if (semStatus == OS_NO_ERR) {

		//}
    }
    return MMP_ERR_NONE;
}
//*----------------------------------------------------------------------------
// function : MMPF_I2cm_ReadRegSet
// input    : (address)
// output   : (data)
// descript : I2C Signal Register Word Reading
//*----------------------------------------------------------------------------
MMP_ERR	MMPF_I2cm_ReadRegSet(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT *usReg, MMP_USHORT *usData, MMP_UBYTE usSetCnt)
{
	MMP_USHORT  i;
	MMP_ERR		ret;
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
	MMP_ULONG ulI2cmTimeOut=0xfff;
//	MMP_UBYTE	semStatus = 0xFF;
	
	if ((uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW)||(uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR)) {
		for (i = 0; i < usSetCnt; i++) {
			ret = MMPF_I2cm_ReadReg(uI2cmAttribute, usReg[i], &(usData[i]));
			if (ret != MMP_ERR_NONE)
				return	ret;
		}
	}
	else {

		MMPF_I2cm_Initialize(uI2cmAttribute);
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RST_FIFO_SW = I2CM_FIFO_RST;

		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR = I2CM_SLAVE_NO_ACK | I2CM_TX_DONE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR &= ~(I2CM_READ_MODE | I2CM_WRITE_MODE);

	    for (i = 0; i < usSetCnt; i++) {
	    	if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usReg[i] >> 8);
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg[i];
	    	}
		    else {
	    	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg[i];
		    }
	    	if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 2;
	        }
	        else {
		        pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 1;
	        }
	    }

	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SET_CNT = usSetCnt;

		
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR |= I2CM_READ_MODE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_MASTER_EN;
		while (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL & I2CM_MASTER_EN){
			ulI2cmTimeOut--;
					udelay(10);
			if(ulI2cmTimeOut==0){
				//RTNA_DBG_Str(0, "I2C time out rs\r\n");
			}
		}

		if (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR & I2CM_SLAVE_NO_ACK) {
			//RTNA_DBG_Str(3, "I2C Error 4\r\n");
			//if (semStatus == OS_NO_ERR) {

			//}
			return 1;
		}

	    for (i = 0; i < usSetCnt; i++) {
	    	if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
		        usData[i] = (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0] << 8);
		        usData[i] += (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]);
	    	}
		    else {
		        usData[i] = pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0];
	    	}
	    }
	    
	    //if (semStatus == OS_NO_ERR) {

		//}
	}
    return MMP_ERR_NONE;
}


//*----------------------------------------------------------------------------
// function : MMPF_I2cm_WriteBurstData
// input    : (address)
// output   : (data)
// descript : I2C burst write ,write reg(usReg), reg(usReg + 1)....reg(usReg + usDataCnt - 1)
//*----------------------------------------------------------------------------
/**
 @brief Write I2C data burstly.

 write data by I2C interface in burst mode. The data is taken from ulDataAddr memory location.
 Parameters:
 @param[in] i2cm_attribute : I2CM attribute, please refer the structure MMPF_I2CM_ATTRIBUTE
 @param[in] usReg : The register sub address.
 @param[in] usData : Number of words to be sent.
 @param[in] ulDataAddr : The address which stores the data.
 @return the status of the operation
*/
#define BurstLength 28
//yidongq MMP_ERR MMPF_I2cm_WriteBurstData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_USHORT *usData, MMP_UBYTE usDataCnt)
MMP_ERR MMPF_I2cm_WriteBurstData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_UBYTE *usData, MMP_UBYTE usDataCnt)

{
	MMP_USHORT  i;
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
//	MMP_UBYTE	ubSemStatus = 0xFF;
//	AITPS_GBL   pGBL = AITC_BASE_GBL;
	MMP_ERR		status = MMP_ERR_NONE;
	
//yidongq	status |= MMPF_I2cm_Initialize(uI2cmAttribute);
	
	if ((uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW)||(uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR)) {
			
		if((uI2cmAttribute->SW_CLK_PIN == NULL) || (uI2cmAttribute->SW_DAT_PIN == NULL)) {
			//RTNA_DBG_Str(3, "SW I2C pin assign error !!\r\n");
			return MMP_I2CM_ERR_PARAMETER;
		}
		
	    status |= MMPF_I2c_Start(uI2cmAttribute);
	    status |= MMPF_I2c_WriteData(uI2cmAttribute, m_bSlaveAddr[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	    status |= MMPF_I2c_GetACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
		
		if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
		    status |= MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)(usReg >> 8));
	    	status |= MMPF_I2c_GetACK(uI2cmAttribute);
		}
	    status |= MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)usReg);
	   	status |= MMPF_I2c_GetACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);

		for( i = 0; i < usDataCnt; i++) {
			if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
			    status |= MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)(usData[i] >> 8));
		    	status |= MMPF_I2c_GetACK(uI2cmAttribute);
			}	    
		   	status |= MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)usData[i]);
		    status |= MMPF_I2c_GetACK(uI2cmAttribute);
//		    RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	    }
	    status |= MMPF_I2c_Stop(uI2cmAttribute);
	}
	else {

		
		if ((usDataCnt*(m_bDataLen[uI2cmAttribute->uI2cmID]>>3) + 3) > I2CM_TX_FIFO_DEPTH) { // 3 for the case with 16bit register address and one byte for byte count
//			//RTNA_DBG_Str(3, "   Attention! !! OVER I2C TX FIFO SIZE\r\n");
			printk(KERN_WARNING "I2C Device(%s) with address 0x%02x Over TX FIFO Size : %d\r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr,usDataCnt);

			while(usDataCnt > BurstLength) {
				status |= MMPF_I2cm_WriteBurstData(uI2cmAttribute, usReg, usData, BurstLength);
				usReg = usReg + BurstLength;
				usData = usData + BurstLength;
				usDataCnt = usDataCnt - BurstLength;
			}
			status |= MMPF_I2cm_WriteBurstData(uI2cmAttribute, usReg, usData, usDataCnt);
			return status;
		}	
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR = I2CM_SLAVE_NO_ACK | I2CM_TX_DONE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR &= ~(I2CM_READ_MODE | I2CM_WRITE_MODE);

	    
	    if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
	    	pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usReg >> 8);
		    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
	    }
		else {
		    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
	   	}
	    		
	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (m_bDataLen[uI2cmAttribute->uI2cmID] >> 3)*usDataCnt;
	    	
	    for (i = 0; i < usDataCnt; i++) {
	    	if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
				pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usData[i] >> 8);
				pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData[i];
		    }
		    else {
			    //pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = 1;
			    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usData[i];
		    }
	   	}
	    
	    pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SET_CNT = 1;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR |= I2CM_WRITE_MODE;
		
		#if (I2CM_INT_MODE_EN == 0x1)
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_EN |= I2CM_TX_FINISH_EN;
		#endif
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_MASTER_EN;
		
		
		#if (I2CM_INT_MODE_EN != 0x1)
		while (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL & I2CM_MASTER_EN);

		if (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR & I2CM_SLAVE_NO_ACK) {
			printk(KERN_ERR "I2C Device(%s) with address 0x%02x Slave no ack \r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);
			return MMP_I2CM_ERR_SLAVE_NO_ACK;
		}
		#endif
		

    }
    

    return status;
}

//*----------------------------------------------------------------------------
// function : MMPF_I2cm_ReadBurstData
// input    : (address)
// output   : (data)
// descript : I2C burst read ,read reg(usReg), reg(usReg + 1)....reg(usReg + usDataCnt - 1)
// Note		: The depth of I2CM RX fifo is only 8 bytes.
//*----------------------------------------------------------------------------
/**
 @brief Read I2C data burstly.

 Receive data from I2C interface in burst mode. The data will store at ulDataAddr memory location.
 Parameters:
 @param[in] i2cm_attribute : I2CM attribute, please refer the structure MMPF_I2CM_ATTRIBUTE
 @param[in] usSubAdd The register sub address.
 @param[in] usData Number of words to be read..
 @param[in] ulDataAddr : The address which stores the data.
 @return the status of the operation.
*/

//MMP_ERR MMPF_I2cm_ReadBurstData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_USHORT *usData, MMP_UBYTE usDataCnt)
MMP_ERR MMPF_I2cm_ReadBurstData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_UBYTE *usData, MMP_UBYTE usDataCnt)
{
	MMP_USHORT  i;
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
	MMP_ERR		status = MMP_ERR_NONE;
	MMP_ULONG	ulI2cmTimeOut = 0;

	
	if ((uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW)||(uI2cmAttribute->uI2cmID == MMPF_I2CM_ID_SW_SENSOR)) {
		if((uI2cmAttribute->SW_CLK_PIN == NULL) || (uI2cmAttribute->SW_DAT_PIN == NULL)) {
			//RTNA_DBG_Str(3, "SW I2C pin assign error !!\r\n");
#if 0
			if (uI2cmAttribute->bOsProtectEn == MMP_TRUE) {
				if (ubSemStatus == OS_NO_ERR) {
					MMPF_OS_ReleaseSem(gI2cmSemID[uI2cmAttribute->uI2cmID]);   
			}
		}
#endif
			return MMP_I2CM_ERR_PARAMETER;
	}
		
	status |= MMPF_I2c_Start(uI2cmAttribute);
	status |= MMPF_I2c_WriteData(uI2cmAttribute, m_bSlaveAddr[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	status |= MMPF_I2c_GetACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	    
	if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
		status |= MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)(usReg >> 8));
		status |= MMPF_I2c_GetACK(uI2cmAttribute);
	}
	
	status |= MMPF_I2c_WriteData(uI2cmAttribute, (MMP_UBYTE)usReg);
	status |= MMPF_I2c_GetACK(uI2cmAttribute);
	//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	status |= MMPF_I2c_Stop(uI2cmAttribute);

	status |= MMPF_I2c_Start(uI2cmAttribute);
	status |= MMPF_I2c_WriteData(uI2cmAttribute, m_bSlaveAddr[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW] + 1);
	status |= MMPF_I2c_GetACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);

	for(i = 0; i < usDataCnt; i++) {
		usData[i] = 0;
		if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
			usData[i] = (MMPF_I2c_ReadData(uI2cmAttribute)) << 8;  //yidongq short -> byte
			status |= MMPF_I2c_SendACK(uI2cmAttribute);
		}
		usData[i] = MMPF_I2c_ReadData(uI2cmAttribute);
		if((usDataCnt > 1) && (i != (usDataCnt -1))) {
			status |= MMPF_I2c_SendACK(uI2cmAttribute);
		}
	//		    RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	}

	status |= MMPF_I2c_SendNACK(uI2cmAttribute);
//		RTNA_WAIT_US(m_bI2CDelayTime[uI2cmAttribute->uI2cmID - MMPF_I2CM_ID_SW]);
	status |= MMPF_I2c_Stop(uI2cmAttribute);
	}
	else {
	
		if ((usDataCnt*(m_bDataLen[uI2cmAttribute->uI2cmID]>>3)) > I2CM_RX_FIFO_DEPTH) {
		//yidongq	/RTNA_DBG_Str(3, "Attention! !! OVER I2C RX FIFO SIZE\r\n");

			
			while(usDataCnt > BurstLength) {
				status |= MMPF_I2cm_ReadBurstData(uI2cmAttribute, usReg, usData, BurstLength);
				usReg = usReg + BurstLength;
				usData = usData + BurstLength;
				usDataCnt = usDataCnt - BurstLength;
			}
			status |= MMPF_I2cm_ReadBurstData(uI2cmAttribute, usReg, usData, usDataCnt);
			return status;
		}
		
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RST_FIFO_SW = I2CM_FIFO_RST;

		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR = I2CM_SLAVE_NO_ACK | I2CM_TX_DONE;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR &= ~(I2CM_READ_MODE | I2CM_WRITE_MODE);

		if (m_bRegLen[uI2cmAttribute->uI2cmID] == 16) {
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)(usReg >> 8);
			pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
		}
		else {
	    		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (MMP_UBYTE)usReg;
		}
				
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_TXFIFO_DATA.B[0] = (m_bDataLen[uI2cmAttribute->uI2cmID]>>3)*usDataCnt;
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SET_CNT = 1;

		
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_SLAV_ADDR |= I2CM_READ_MODE;
#if (I2CM_INT_MODE_EN == 0x1)
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_EN |= I2CM_TX_FINISH_EN;
#endif
		pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL |= I2CM_MASTER_EN;
		
#if (I2CM_INT_MODE_EN != 0x1)
		if(m_ulI2cRxReadTimeout[uI2cmAttribute->uI2cmID] != 0) {
			ulI2cmTimeOut = m_ulI2cRxReadTimeout[uI2cmAttribute->uI2cmID];
		}
		
		while (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_CTL & I2CM_MASTER_EN) {
			if(m_ulI2cRxReadTimeout[uI2cmAttribute->uI2cmID] != 0) {
						udelay(10);
				ulI2cmTimeOut --;
				if(ulI2cmTimeOut == 0) {
					return MMP_I2CM_ERR_READ_TIMEOUT;
				}
				else {
					//Note, to use RxTimeout function, please make sure the read operations work in "task mode" instead of "ISR mode"
	//				MMPF_OS_Sleep(1);
				}
			}
		}

		if (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_INT_CPU_SR & I2CM_SLAVE_NO_ACK) {

			printk(KERN_ERR "I2C Device(%s) with address 0x%02x Slave no ack \r\n",uI2cmAttribute->name,uI2cmAttribute->ubSlaveAddr);

			return MMP_I2CM_ERR_SLAVE_NO_ACK;
		}
#endif
		
		for (i = 0; i < usDataCnt; i++) {
			if (m_bDataLen[uI2cmAttribute->uI2cmID] == 16) {
				usData[i] = (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0] << 8);
				usData[i] += (pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0]);
			}
			else {
				usData[i] = pI2CM->I2CMS[uI2cmAttribute->uI2cmID].I2CM_RXFIFO_DATA.B[0];
	    		}
		}	      
	}
	
	return status;
}


#if 0
MMP_UBYTE MMPF_I2cm_StartSemProtect(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute) {
//	return MMPF_OS_AcquireSem(gI2cmSemID[uI2cmAttribute->uI2cmID], I2CM_SEM_TIMEOUT);
}

MMP_UBYTE MMPF_I2cm_EndSemProtect(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute) {
///	return MMPF_OS_ReleaseSem(gI2cmSemID[uI2cmAttribute->uI2cmID]); 
}
#endif
/** @}*/ //end of MMPF_SYS


MMP_ERR MMPF_I2cm_DisableInterrupt(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute)
{
	AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
	pI2CM ->I2CMS[uI2cmAttribute->uI2cmID] .I2CM_INT_HOST_EN= 0x0000;
	return MMP_ERR_NONE;
}


