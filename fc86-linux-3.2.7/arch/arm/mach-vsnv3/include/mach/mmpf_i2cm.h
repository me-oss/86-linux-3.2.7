//==============================================================================
//
//  File        : mmpf_i2cm.h
//  Description : INCLUDE File for the Firmware I2CM Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMPF_I2CM_H_
#define _MMPF_I2CM_H_

//#include    "includes_fw.h"
#include 	"mmpf_pio.h"
#if 0//(CHIP == P_V2)
#define I2CM_RX_FIFO_DEPTH		0x8
#define I2CM_TX_FIFO_DEPTH		0xF
#endif
#if 1//(CHIP == VSN_V2)||(CHIP == VSN_V3)
#define I2CM_RX_FIFO_DEPTH		0x20
#define I2CM_TX_FIFO_DEPTH		0x20
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
#define I2CM_SEM_TIMEOUT 0x0
#if 0//(CHIP == P_V2)
#define I2CM_HW_MAX_ID	MMPF_I2CM_ID_2
#endif
#if 1//(CHIP == VSN_V2)||(CHIP == VSN_V3)
#define I2CM_HW_MAX_ID	MMPF_I2CM_ID_1
#endif
//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
typedef enum _MMPF_I2CM_ID
{
    MMPF_I2CM_ID_0 = 0,   	//HW I2CM, seserved for Sensor HW I2C
    MMPF_I2CM_ID_1,
	#if 0//(CHIP == P_V2)
    MMPF_I2CM_ID_2,
	#endif
    MMPF_I2CM_ID_SW,		//General SW I2CM, using GPIO pins
    MMPF_I2CM_ID_SW_SENSOR,	//SW I2CM, sensor uses VIF pins
    MMPF_I2CM_MAX_COUNT
} MMPF_I2CM_ID;

typedef enum _MMPF_I2CM_SPEED_MODE
{
	MMPF_I2CM_SPEED_NONE = 0,
    MMPF_I2CM_SPEED_SW,  	//SW I2CM, the speed is control by each attribute ubDelayTime
    MMPF_I2CM_SPEED_HW_10K,
    MMPF_I2CM_SPEED_HW_20K,
    MMPF_I2CM_SPEED_HW_30K,
    MMPF_I2CM_SPEED_HW_40K,
    MMPF_I2CM_SPEED_HW_50K,
    MMPF_I2CM_SPEED_HW_60K,
    MMPF_I2CM_SPEED_HW_70K,
    MMPF_I2CM_SPEED_HW_80K,
    MMPF_I2CM_SPEED_HW_90K,
    MMPF_I2CM_SPEED_HW_100K,
    MMPF_I2CM_SPEED_HW_150K,
    MMPF_I2CM_SPEED_HW_200K,
    MMPF_I2CM_SPEED_HW_250K,
    MMPF_I2CM_SPEED_HW_400K,
    MMPF_I2CM_SPEED_HW_550K,
    MMPF_I2CM_SPEED_HW_700K,
    MMPF_I2CM_SPEED_HW_850K,
    MMPF_I2CM_SPEED_HW_1MHZ,
    MMPF_I2CM_MAX_SPEED_COUNT
} MMPF_I2CM_SPEED_MODE;

typedef struct _MMPF_I2CM_ATTRIBUTE
{
   MMPF_I2CM_ID uI2cmID;			//MMPF_I2CM_ID_0 ~ MMPF_I2CM_ID_2 stand for HW I2CM
   MMP_UBYTE ubSlaveAddr;
   MMP_UBYTE ubRegLen;				//Indicate register as the 8 bit mode or 16 bit mode.
   MMP_UBYTE ubDataLen;				//Indicate data as the 8 bit mode or 16 bit mode.
   MMP_UBYTE ubDelayTime;			//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW or MMPF_I2CM_ID_SW_SENSOR)
   									//To Adjust the speed of software I2CM
   MMP_BOOL  bDelayWaitEn;
   MMP_BOOL  bInputFilterEn;		//HW feature, to filter input noise
   MMP_BOOL  b10BitModeEn;			//HW I2CM supports 10 bit slave address, the bit8 and bit9 are in ubSlaveAddr1
   MMP_BOOL	 bClkStretchEn; 		//HW support stretch clock
   MMP_UBYTE ubSlaveAddr1;
   MMP_UBYTE ubDelayCycle;  		//When bDelayWaitEn enable, set the delay cycle after each 8 bit transmission
   MMP_UBYTE ubPadNum;      		//HW pad map, the relate pad definition, please refer global register spec.
   									// ========= For Vision V2=========
									// If I2CM_ID = MMPF_I2CM_ID_0, ubPadNum = 0 stands for I2CM0_SCK = PHI2C_SCL
									//                              ubPadNum = 1 stands for I2CM0_SCK = PSNR_HSYNC
									// IF I2CM_ID = MMPF_I2CM_ID_1, only one pad(I2CM1_SCK = PSNR_SCK) is used
   									// ========= For Vision V2=========
   MMPF_I2CM_SPEED_MODE uI2cmSpeed; //HW I2CM speec control
   MMPF_PIO_REG SW_CLK_PIN;  		//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW only), indicate the clock pin
   MMPF_PIO_REG SW_DAT_PIN;	 		//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW only), indicate the data pin
   char* name;
   MMP_USHORT deviceID;
} MMPF_I2CM_ATTRIBUTE;

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

//Sensor HW I2C
#if !defined(SENSOR_SOFTWARE_I2CM)
#if 0//(CHIP == VSN_V2)
#define SENSOR_I2CM_ID MMPF_I2CM_ID_1
#endif
#if 1//(CHIP == P_V2)||(CHIP == VSN_V3)
//#define SENSOR_I2CM_ID MMPF_I2CM_ID_0
#endif
#else
//#define SENSOR_I2CM_ID MMPF_I2CM_ID_SW_SENSOR
#endif

#define MMPF_I2CM_SW_COUNT   0x2
#if 0//(CHIP == P_V2)
#define I2CM_HW_MAX_COUNT		0x3		//Number of HW I2CM controller
#endif
#if 1//(CHIP == VSN_V2)||(CHIP == VSN_V3)
#define I2CM_HW_MAX_COUNT		0x2		//Number of HW I2CM controller
#endif
//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_I2cm_Initialize(MMPF_I2CM_ATTRIBUTE *i2cAttribute);
MMP_ERR	MMPF_I2cm_WriteReg(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_USHORT usData);
MMP_ERR	MMPF_I2cm_ReadReg(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_ULONG *usData);//Jefferry
MMP_ERR	MMPF_I2cm_WriteRegSet(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT *usReg, MMP_USHORT *usData, MMP_UBYTE usSetCnt);
MMP_ERR	MMPF_I2cm_ReadRegSet(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT *usReg, MMP_USHORT *usData, MMP_UBYTE usSetCnt);
MMP_UBYTE MMPF_I2cm_StartSemProtect(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute);
MMP_UBYTE MMPF_I2cm_EndSemProtect(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute);
//MMP_ERR MMPF_I2cm_SemInit(void);
MMP_ERR MMPF_I2cm_WriteBurstData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_UBYTE *usData, MMP_UBYTE usDataCnt); //yidongq
MMP_ERR MMPF_I2cm_ReadBurstData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute, MMP_USHORT usReg, MMP_UBYTE *usData, MMP_UBYTE usDataCnt); //yidongq
MMP_ERR MMPF_I2cm_InitializeDriver(void);
MMP_ERR	MMPF_I2cm_CCI_ReadData(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute,MMP_UBYTE ubReadDataCount ,MMP_UBYTE *usData);
MMP_ERR	MMPF_I2cm_DisableInterrupt(MMPF_I2CM_ATTRIBUTE* uI2cmAttribute);

//==============================================================================
//
//                              MACRO FUNCTIONS
//
//==============================================================================
extern MMP_ERR  MMPF_I2cm_WriteXfr(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute,unsigned char *buf, int length);
extern MMP_ERR  MMPF_I2cm_ReadXfr(MMPF_I2CM_ATTRIBUTE *uI2cmAttribute,unsigned char *buf, int length);
#endif // _MMPD_SCALER_H_
