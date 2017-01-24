/** @file mmpf_sd.c
@brief Driver functions of SD/MMC/SDIO
@author Philip Lin
@author Sunny
@author Hans Liu
@author Penguin Torng
@version 2.0
*/

#ifdef __linux__

#include <linux/kernel.h>
#include <linux/delay.h>

#include <mach/mmpf_typedef.h>
//#include <mach/mmp_err.h>
#include <mach/mmpf_sd.h>
#include <mach/mmpf_pll.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_sd.h>

#define RTNA_DBG_Str(x,y) printk(y)
#define RTNA_DBG_Str0(x) printk(x)
#define RTNA_DBG_Str3 RTNA_DBG_Str0
#define RTNA_DBG_Str2 RTNA_DBG_Str0
#define RTNA_DBG_Str1 RTNA_DBG_Str0

#define RTNA_DBG_Long(x, y) printk("%l",y)
#define RTNA_DBG_Long0(y) RTNA_DBG_Long("%l",y)
#define RTNA_DBG_Long1(y) RTNA_DBG_Long("%l",y)
#define RTNA_DBG_Long2(y) RTNA_DBG_Long("%l",y)
#define RTNA_DBG_Long3(y) RTNA_DBG_Long("%l",y)

#define RTNA_DBG_Short1(x) RTNA_DBG_Long("%d",x)
#define RTNA_DBG_Short2(x) RTNA_DBG_Long("%d",x)

#define RTNA_DBG_Byte0(x) RTNA_DBG_Long("%d",x)
#define RTNA_DBG_Byte2(x) RTNA_DBG_Long("%d",x)

#define OS_NO_ERR 0
#define RTNA_WAIT_MS(x) msleep_interruptible(x)
#define SD_BUS_REENTRY_PROTECT (0)
MMP_UBYTE	m_gbSystemCoreID;
#define DBG_W0(x)
#define DBG_S0(x)
#define RTNA_DBG_PrintLong(x,y)
#define RTNA_DBG_PrintShort(x,y)
#define RTNA_DBG_PrintByte(x,y)



#else
#include <config.h>

#include <asm/arch/mmpf_typedef.h>
//#include <asm/arch/mmp_err.h>
#include <asm/arch/mmpf_sd.h>
#include <asm/arch/mmp_reg_gbl.h>

#include <asm/arch/mmpf_pll.h>

#define RTNA_DBG_Str(x,y) printf(y)
#define RTNA_DBG_Str0(x) printf(x)
#define RTNA_DBG_Str3 RTNA_DBG_Str0
#define RTNA_DBG_Str2 RTNA_DBG_Str0
#define RTNA_DBG_Str1 RTNA_DBG_Str0

#define RTNA_DBG_Long(x, y) printf("%l",y)
#define RTNA_DBG_Long0(y) printf("%l",y)
#define RTNA_DBG_Long1(y) printf("%l",y)
#define RTNA_DBG_Long2(y) printf("%l",y)
#define RTNA_DBG_Long3(y) printf("%l",y)

#define RTNA_DBG_Short1(x) printf("%d",x)
#define RTNA_DBG_Short2(x) printf("%d",x)

#define RTNA_DBG_Byte0(x) printf("%d",x)
#define RTNA_DBG_Byte2(x) printf("%d",x)

#define OS_NO_ERR 0
#define RTNA_WAIT_MS(x) mdelay(x)

#define SD_BUS_REENTRY_PROTECT (0)
MMP_UBYTE	m_gbSystemCoreID;
#define DBG_W0(x)
#define DBG_S0(x)
#define RTNA_DBG_PrintLong(x,y)
#define RTNA_DBG_PrintShort(x,y)
#define RTNA_DBG_PrintByte(x,y)
#endif


//MMP_ERR MMPF_SD_BusRelease(MMPF_SD_ID id){}
//MMP_ERR MMPF_SD_BusAcquire(MMPF_SD_ID id, MMP_ULONG ulTimeout){}

#define USING_SD_IF 1
typedef int MMPF_OS_SEMID ;
typedef int MMPF_OS_FLAGID ;


#define SD_BUSWIDTH (4)
#define SD1_BUSWIDTH (4)
#define SD2_BUSWIDTH (4)
#define SD_BUS_TYPE (4) //< Valid input: 1 or 4, dafault: 4
#define ENABLE_SDIO_FEATURE (0) //< 0: disable SDIO feature; 1: enable SDIO feature.
#define ENABLE_SDHC_SWITCH_HIGH_SPEED (1) //< 0: disable HSM for SDHC; 1: enable HSM for SDHC
#define SD_BUS_REENTRY_PROTECT (1) //< 0: disable SD BUS reentry protect; 1: enable SD BUS reentry protect
#define ENABLE_SD_CLKGATING (0) //< 0: disable SD clock gating; 1: enable SD clock gating
#define DEFINE_MMC_BUS_WIDTH   (8)  //< 4: support 4bit bus width; 8: support 8bit bus width; else is 1 bit
#define ENABLE_READ_RETRY (1)



/** @addtogroup MMPF_SD
@{
*/
#if (PMP_USE_MUON == 1)
extern void MMPF_MMU_FlushCache(MMP_ULONG ulRegion, MMP_ULONG ulSize);
#endif

#if (USING_SD_IF)
    #define SD_TIMEOUT_COUNT     0x100000
    MMP_ERR MMPF_SD_WaitIsrDone(MMPF_SD_ID id, MMP_ULONG waitcount);
#endif

MMP_UBYTE gbIsSD[MMPF_SD_DEV_NUM];


	MMP_USHORT mCurSDClockDiv[MMPF_SD_DEV_NUM] = {SD_CLOCK_DIV_256, SD_CLOCK_DIV_256};
	MMP_ULONG glsdCardAddr[MMPF_SD_DEV_NUM] = {0x00100000, 0x00110000};
	MMP_UBYTE mSdPadMapping[MMPF_SD_DEV_NUM] = {MMPF_SD_PAD1, MMPF_SD_PAD1};
	MMP_UBYTE mSdBusWidth[MMPF_SD_DEV_NUM] = {4, 4};//, SD2_BUSWIDTH, SD3_BUSWIDTH};
	MMP_USHORT musBlockLen[MMPF_SD_DEV_NUM] = {512, 512, };
	MMP_UBYTE  mIsHC[MMPF_SD_DEV_NUM] = {0, 0, };
	MMP_UBYTE  mIs2GMMC[MMPF_SD_DEV_NUM] = { 0, 0};
	MMP_UBYTE  mIsMMC4X[MMPF_SD_DEV_NUM] = {0, 0 };
	MMP_UBYTE  mMMC4XClockMode[MMPF_SD_DEV_NUM] = {0, 0}; //0: 26M, 1: 52M
	/// Used to identify SDHC High-Speed Mode
	/// 0 means default speed mode (0~25MHz)
	/// 1 means High-Speed Mode (up to 50MHz)
	MMP_UBYTE  mSdhcHsm[MMPF_SD_DEV_NUM] = {0, 0 };

	MMP_BOOL m_bSDAccessFail[MMPF_SD_DEV_NUM] = {MMP_FALSE, MMP_FALSE};

	MMP_UBYTE  mPowerOnFirst[MMPF_SD_DEV_NUM] = {MMP_TRUE, MMP_TRUE};
#if 0//(CHIP == P_V2)

	#if (EN_CARD_DETECT == 1)
	MMP_UBYTE   gbSDCardDetectionPinNum[MMPF_SD_DEV_NUM] = {CARD_DET_PIN_NUM, CARD1_DET_PIN_NUM);//, CARD2_DET_PIN_NUM, CARD3_DET_PIN_NUM};
	MMP_UBYTE   gbSDCardDetectionPolarity[MMPF_SD_DEV_NUM] = {CARD_DET_POLARITY, CARD1_DET_POLARITY);//, CARD2_DET_POLARITY, CARD3_DET_POLARITY};
	#endif /* SD_CARD_DETECT */

	#if (EN_CARD_WRITEPROTECT == 1)
	MMP_UBYTE   gbSDCardWProtectPinNum[MMPF_SD_DEV_NUM] = {CARD_WP_PIN_NUM, CARD1_WP_PIN_NUM};//, CARD2_WP_PIN_NUM, CARD3_WP_PIN_NUM};
	MMP_UBYTE   gbSDCardWProtectPolarity[MMPF_SD_DEV_NUM] = {CARD_WP_POLARITY, CARD1_WP_POLARITY};//, CARD2_WP_POLARITY, CARD3_WP_POLARITY};
	#endif /* SD_WRITE_PROTECT */

	#if (EN_CARD_PWRCTL == 1)
	MMP_UBYTE   gbSDCardPwrCtlPinNum[MMPF_SD_DEV_NUM] = {CARD_PWR_PIN_NUM, CARD1_PWR_PIN_NUM};//, CARD2_PWR_PIN_NUM, CARD3_PWR_PIN_NUM};
	MMP_UBYTE   gbSDCardPwrCtlPolarity[MMPF_SD_DEV_NUM] = {CARD_PWR_POLARITY, CARD1_PWR_POLARITY;//, CARD2_PWR_POLARITY, CARD3_PWR_POLARITY};
	#endif /* SD_PWR_CTL */

	MMP_UBYTE  mPowerOnFirst[MMPF_SD_DEV_NUM] = {MMP_TRUE, MMP_TRUE, MMP_TRUE, MMP_TRUE};


#endif

MMP_UBYTE m_ubCardStatus[MMPF_SD_DEV_NUM] = {0, 0};
MMP_ULONG glsdCardSize[MMPF_SD_DEV_NUM];
MMP_ULONG m_sdReadTimeout[MMPF_SD_DEV_NUM];
MMP_ULONG m_sdWriteTimeout[MMPF_SD_DEV_NUM];
MMP_ULONG glMMCBootSize[MMPF_SD_DEV_NUM];
MMP_ULONG glMMCCurrentPartition[MMPF_SD_DEV_NUM];
unsigned char gStorageSerialNumber[16];

MMP_UBYTE mGblSdInited = MMP_FALSE;


MMPF_OS_SEMID mSdIntTriggerSemID[MMPF_SD_DEV_NUM];


/// This vairable is ONLY temp buffer for SDHC switch function (ACMD51 + CMD6)
//MMP_ULONG m_ulSDDmaAddr[512];
//#endif /* (ENABLE_SDHC_SWITCH_HIGH_SPEED==1) */

/** @addtogroup MMPF_SDIO
@{
*/
#if 1//(ENABLE_SDIO_FEATURE == 1)
///SDIO command argument (in CMD)
#define RW_FLAG(x)      ((MMP_ULONG)x<<31)
#define FUN_NUM(x)      ((MMP_ULONG)x<<28)
#define RAW_FLAG(x)     ((MMP_ULONG)x<<27)
#define REG_ADD(x)      ((MMP_ULONG)x<<9)
#define WTITE_DATA(x)   ((MMP_ULONG)x)
#define BLK_MODE(x)     ((MMP_ULONG)x<<27)
#define OP_CODE(x)      ((MMP_ULONG)x<<26)

///SDIO return status (in DATA)
#define COM_CRC_ERROR       0x8000
#define ILLEGAL_COMMAND     0x4000
#define UNKNOW_ERROR        0x0800
#define INV_FUN_NUM         0x0200
#define OUT_OF_RANGE        0x0100

MMPF_OS_SEMID mSDIOBusySemID[MMPF_SD_DEV_NUM];
extern MMP_USHORT MMPF_SDIO_BusRelease(MMPF_SD_ID);
extern MMP_ERR MMPF_SDIO_BusAcquire(MMPF_SD_ID);

/// Add for SDIO NONBLOCKING CPU mode (pending for Block Transfer Done interrupt)
#define SDIO_CPU_NONBLOCKING (1)

extern MMPF_OS_FLAGID SYS_Flag_Hif;

#endif /* (ENABLE_SDIO_FEATURE==1) */
/** @} */ // MMPF_SDIO

extern MMPF_OS_FLAGID SYS_Flag_Hif;

#if (SD_BUS_REENTRY_PROTECT==1)
#if 0//(CHIP == D_V1)||(CHIP == PYTHON)
MMPF_OS_SEMID mSDBusySemID[MMPF_SD_DEV_NUM] = {0xFE, 0xFE, 0xFE};
#endif
#if 1//(CHIP == P_V2)
MMPF_OS_SEMID mSDBusySemID[MMPF_SD_DEV_NUM] = {0xFE, 0xFE, 0xFE, 0xFE};
MMPF_OS_SEMID mSDAccessSemID[MMPF_SD_DEV_NUM] = {0xFE, 0xFE, 0xFE, 0xFE};
#endif


#endif /* (SD_BUS_REENTRY_PROTECT==1) */

#if (ENABLE_SD_CLKGATING == 1)
extern void MMPF_SD_EnableModuleClock(MMPF_SD_ID id, MMP_BOOL bEnable);
#endif

MMP_ULONG m_ulSDG0SrcClk;

static  MMP_UBYTE   m_ubReadRetryCnt = 2;
#if 0
MMP_ERR  MMPF_SD_SoftwareResetDevice(MMPF_SD_ID id)
{
    AITPS_GBL    pGBL = AITC_BASE_GBL;

printk("MMPF_SD_SoftwareResetDevice: %d\n",id);


        switch(id)
        {
            case 0: 

		pGBL->GBL_RST_CTL2 |= GBL_SD0_SW_RST_EN;
		pGBL->GBL_RST_CTL2 |=GBL_SD0_RST;	

		MMPF_SD_WaitCount(4000);//msleep(1000);
		pGBL->GBL_RST_CTL2 &=~GBL_SD0_RST;				

                break;
                
            case 1: 

		pGBL->GBL_RST_CTL3 |= GBL_SD1_SW_RST_EN;				
		pGBL->GBL_RST_CTL3 |=GBL_SD1_RST;	
		MMPF_SD_WaitCount(4000);//msleep(100);
		pGBL->GBL_RST_CTL3 &=~GBL_SD1_RST;
				
                break;

	}

	return MMP_ERR_NONE;
}

/** @brief Control working clock for SD module

This function control working clock for SD module
@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@param[in] usClkDiv prefer working clock
@return NONE
*/
#if 0
void MMPF_SD_SwitchClock(MMPF_SD_ID id, MMP_USHORT usClkDiv)
{
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//    AITPS_SD pSD = AITC_BASE_SD + id;
	MMP_ULONG time_out = 0;
	static MMP_USHORT curDiv = 0;

pr_debug("MMPF_SD_SwitchClock:Div = %d\r\n",usClkDiv);
	if(curDiv==usClkDiv)
		return;

    // Wait for SD clk can be switched
	while(!(pSD->SD_CPU_FIFO_SR & CLK_SWITCH_INDICATE)) {
		time_out++;
		if (time_out >= SD_TIMEOUT_COUNT) {
#ifdef __linux__			
			printk("clk switch fail: SR = 0x%x\r\n",pSD->SD_CPU_FIFO_SR);
#else			
			RTNA_DBG_Byte2(pSD->SD_CPU_FIFO_SR);
			RTNA_DBG_Str2("(");
			RTNA_DBG_Long2((unsigned long) &(pSD->SD_CPU_FIFO_SR));
			RTNA_DBG_Str2("):clk switch fail\r\n");
#endif
			break;
		}
	}

    	pSD->SD_CTL_0 &= ~CLK_IN_SYS_DIV_3;
    	
    // Assign new SD clock
    if(usClkDiv == SD_CLOCK_DIV_1) {

        pSD->SD_CTL_0 |= CLK_IN_NORM_SYS;
    }
    else if (usClkDiv == SD_CLOCK_DIV_3) {
        pSD->SD_CTL_0 |= CLK_IN_SYS_DIV_3;
    }
    else {
        pSD->SD_CLK_CTL = usClkDiv;
    }

    mCurSDClockDiv[id] = usClkDiv;

	curDiv = usClkDiv;
    return;
} /* MMPF_SD_SwitchClock */
#endif
/** @brief Control working clock for SD module by G0 clock

This function control working clock for SD module according to G0 clock
@param[in] ulG0Clock the clock frequency of group 0
@return NONE
*/
#if 1//(ENABLE_SD_CLKGATING == 1)
/** @brief Control SD module clock

This function control SD module clock
@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@param[in] bEnable 0 to turn-off, 1 to turn-on SD module clock
@return It reports the status of the operation. Now it always return success.
*/
void MMPF_SD_EnableModuleClock(MMPF_SD_ID id, MMP_BOOL bEnable)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;
printk("MMPF_SD_EnableModuleClock: %d %s\n",id,bEnable?"Enable":"Disable");

	if (bEnable) {
//		if(!enable)
			{
//		pSD->SD_CTL_1 &= ~AUTO_CLK_EN;		
		pSD->SD_CTL_1 |= CLK_EN;
//		enable = 1;
			}
	}
	else {
//		pSD->SD_CTL_1 &= ~AUTO_CLK_EN;				
		pSD->SD_CTL_1 &= ~CLK_EN;
	}
} /* MMPF_SD_EnableModuleClock */
#endif
#endif
/** @brief Config PAD setting for SD host controller

This function config PAD setting for SD host controller and MUST be called before file system initialize.
@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@param[in] padid ID of SD PAD. For DIAMOND, valid input is 0, 1 and 2.
@return It reports the status of the operation. Now it always return success.
*/
MMP_ERR MMPF_SD_ConfigPadMapping1(MMPF_SD_ID devid, MMPF_SD_PAD padid, MMP_BOOL b8bitBus)
{

	AITPS_GBL pGBL = AITC_BASE_GBL;

	AITPS_SD pSD = devid?AITC_BASE_SD1:AITC_BASE_SD0;
	MMP_USHORT padCfg;


	if(devid==MMPF_SD_0)
	{
		if(padid==MMPF_SD_PAD0)
		{
			//todo
			pGBL->GBL_IO_CTL3 |=GBL_SD0_IO_PAD0_EN;	
		}			
		else	
		{
			pGBL->GBL_IO_CFG_PCGPIO[0] = (1<<5)|(1<<2);	//PCGPIO10 ==> SD1_CLK , 9.6mA , pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[1] = (1<<5)|(1<<2);	//PCGPIO11 ==> SD1_CMD, 9.6mA ,pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[2] = (1<<5)|(1<<2);	//PCGPIO12 ==> SD1_DATA0, 8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[3] = (1<<5)|(1<<2);	//PCGPIO13 ==> SD1_DATA1,  8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[4] = (1<<5)|(1<<2);	//PCGPIO14 ==> SD1_DATA2,  8.4mA, pull-up 		
			pGBL->GBL_IO_CFG_PCGPIO[5] = (1<<5)|(1<<2);	//PCGPIO15 ==> SD1_DATA3,  8.4mA, pull-up

			pGBL->GBL_IO_CTL3 |= GBL_SD0_IO_PAD1_EN;
		}
	
	}
	else if(devid==MMPF_SD_1)
	{
		if(padid==MMPF_SD_PAD0)
		{
			//todo
			pGBL->GBL_IO_CTL0   |=GBL_SD1_IO_PAD0_EN;		
		}
		else	
		{
			pGBL->GBL_IO_CFG_PCGPIO6[4] = (1<<5)|(1<<2);	//PCGPIO10 ==> SD1_CLK , 9.6mA , pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[5] = (1<<5)|(1<<2);	//PCGPIO11 ==> SD1_CMD, 9.6mA ,pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[6] = (1<<5)|(1<<2);	//PCGPIO12 ==> SD1_DATA0, 8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[7] = (1<<5)|(1<<2);	//PCGPIO13 ==> SD1_DATA1,  8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[8] = (1<<5)|(1<<2);	//PCGPIO14 ==> SD1_DATA2,  8.4mA, pull-up 		
			pGBL->GBL_IO_CFG_PCGPIO6[9] = (1<<5)|(1<<2);	//PCGPIO15 ==> SD1_DATA3,  8.4mA, pull-up
		
			pGBL->GBL_IO_CTL4   |=GBL_SD1_IO_PAD1_EN;	
			pGBL->GBL_IO_CTL14 |=0x80;
		}
		
	}

		
//		padCfg = GBL_SD_PADMAP((GBL_SD_DEV0|GBL_SD_4WIRE_EN), padid);

    #if 0
    RTNA_DBG_Byte3(devid);
    RTNA_DBG_Str3(":");
    RTNA_DBG_Byte3(padid);
    RTNA_DBG_Str3(":");
    RTNA_DBG_Short3(padCfg);
    RTNA_DBG_Str3(":DevId:PadId:PadCfg.\r\n");
    #endif //0

    /* (CHIP == DIAMOND) */
    // SD_CTL0 using PAD0 as 4-bit mode: 0:0:0x0002
    // SD_CTL1 using PAD0 as 4-bit mode: 1:0:0x0006
    // SD_CTL2 using PAD0 as 4-bit mode: 2:0:0x000A
    // SD_CTL0 using PAD1 as 4-bit mode: 0:1:0x0020
    // SD_CTL1 using PAD1 as 4-bit mode: 1:1:0x0060
    // SD_CTL2 using PAD1 as 4-bit mode: 2:1:0x00A0
    // SD_CTL0 using PAD2 as 4-bit mode: 0:2:0x0200
    // SD_CTL1 using PAD2 as 4-bit mode: 1:2:0x0600
    // SD_CTL2 using PAD2 as 4-bit mode: 2:2:0x0A00
//    #if (CHIP == D_V1)||(CHIP == PYTHON)
//	pGBL->GBL_MIO_SD_CTL |= padCfg;
//    #endif

    return MMP_ERR_NONE;
}


 
/** @brief SD set timeout mode

@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@param[in] SD high speed mode or not. 
@retval MMP_ERR_NONE Success
*/
static MMP_ERR  MMPF_SD_SetTimeout(MMPF_SD_ID id, MMPF_SD_SPEED_MODE mode)
{
printk("MMPF_SD_SetTimeout(%d):  %s\n",id,mode?"High Speed":"Normal Speed");

    if(mode == MMPF_SD_HIGHSPEED_MODE){          //48M
        m_sdReadTimeout[id] = SD_HIGHSPEED_READ_TIMEOUT;
        m_sdWriteTimeout[id] = SD_HIGHSPEED_WRITE_TIMEOUT;
    }
    else{                   //24M
        m_sdReadTimeout[id] = SD_NORMAL_READ_TIMEOUT;
        m_sdWriteTimeout[id] = SD_NORMAL_WRITE_TIMEOUT;
    }

    return  MMP_ERR_NONE;
}


/** @brief SD switch timeout mode

@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@param[in] Read or write timeout. 
@retval MMP_ERR_NONE Success
*/
MMP_ERR  MMPF_SD_SwitchTimeout(MMPF_SD_ID id, MMPF_SD_TIMEOUT mode)
{
        AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;

    switch(mode){
#if 0
        case MMPF_SD_READ_TIMEOUT:
            pSD->SD_DATA_TOUT[0] = m_sdReadTimeout[id]&0xFF;
            pSD->SD_DATA_TOUT[1] = (m_sdReadTimeout[id]&0xFF00)>>8;
            pSD->SD_DATA_TOUT[2] = (m_sdReadTimeout[id]&0xFF0000)>>16;			
            break;
            
        case MMPF_SD_WRITE_TIMEOUT:

            pSD->SD_DATA_TOUT[0] = m_sdWriteTimeout[id]&0xFF;
            pSD->SD_DATA_TOUT[1] = (m_sdWriteTimeout[id]&0xFF00)>>8;
            pSD->SD_DATA_TOUT[2] = (m_sdWriteTimeout[id]&0xFF0000)>>16;			
            break;

        case MMPF_SD_DEFAULT_TIMEOUT:    
#endif			
        default:

            pSD->SD_DATA_TOUT[0] = SD_DEFAULT_TIMEOUT&0xFF;
            pSD->SD_DATA_TOUT[1] = (SD_DEFAULT_TIMEOUT&0xFF00)>>8;
            pSD->SD_DATA_TOUT[2] = (SD_DEFAULT_TIMEOUT&0x0F0000)>>16;
//            pSD->SD_DATA_TOUT = SD_DEFAULT_TIMEOUT;
            break;
    }

    return  MMP_ERR_NONE;
        
}

/** @brief Initial SD module and interafce

This function initial SD module and interafce
@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@retval MMP_ERR_NONE Now it always return success.
*/
MMP_ERR  MMPF_SD_InitialInterface_remove(MMPF_SD_ID id)
{
//	AITPS_GBL pGBL = AITC_BASE_GBL;
	
//	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;
	//MMP_ULONG divValue, targetClk;
//	int i;

//#if (EN_CARD_DETECT == 1)||(EN_CARD_WRITEPROTECT == 1)||(EN_CARD_PWRCTL == 1)
//	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
//#endif
	printk("MMPF_SD_InitialInterface: %d\n",id);

//	MMPF_SD_ConfigPadMapping1(id, mSdPadMapping[id], mSdBusWidth[id]==8);
	if (mPowerOnFirst[id]){
//		MMPF_SD_SoftwareResetDevice(id);
	}

	if (mGblSdInited == MMP_FALSE) {

	//        RTNA_AIC_Open(pAIC, AIC_SRC_SD, sd_isr_a, AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 3);
	//        RTNA_AIC_IRQ_En(pAIC, AIC_SRC_SD);

		mGblSdInited = MMP_TRUE;
	}

	if (mPowerOnFirst[id]){

	

#if 0
        #if (EN_CARD_PWRCTL == 1) //GPIO output
        if (gbSDCardPwrCtlPinNum[id] != 0xFF) {
            pGPIO->GPIO_EN[(gbSDCardPwrCtlPinNum[id]/32)] |= (1 << (gbSDCardPwrCtlPinNum[id]%32));
        }
        MMPF_SD_EnableCardPWR(id, MMP_TRUE);
        #endif

        #if (EN_CARD_DETECT == 1) //GPIO input
        if (gbSDCardDetectionPinNum[id] != 0xFF) {
            pGPIO->GPIO_EN[(gbSDCardDetectionPinNum[id]/32)] &= ~(1 << (gbSDCardDetectionPinNum[id]%32));
        }
        #endif

        #if (EN_CARD_WRITEPROTECT == 1) //GPIO input
        if (gbSDCardWProtectPinNum[id] != 0xFF) {
            pGPIO->GPIO_EN[(gbSDCardWProtectPinNum[id]/32)] &= ~(1 << (gbSDCardWProtectPinNum[id]%32));
        }
        #endif

        /// Default state: no trigger
        #if (SD_CPU_NONBLOCKING==1)
//Vin        mSdIntTriggerSemID[id] = MMPF_OS_CreateSem(0);
        #endif

        #if (SD_BUS_REENTRY_PROTECT == 1)
        /// Default state: SD BUS available
//Vin        mSDBusySemID[id] = MMPF_OS_CreateSem(1);
        #endif
        
//Vin        mSDAccessSemID[id] = MMPF_OS_CreateSem(1);

#endif
		mPowerOnFirst[id] = MMP_FALSE;

	}
#if 0
	//wilson@110509: setting clk divide value before enable SD module clock
	targetClk = 100*1000;
	MMPF_PLL_GetGroupFreq(0, &m_ulSDG0SrcClk);
	divValue = (m_ulSDG0SrcClk - (m_ulSDG0SrcClk % targetClk))/targetClk;
	if (m_ulSDG0SrcClk % targetClk != 0)
		divValue++;

	

	m_ulSDG0SrcClk /= divValue;


    // Enable SD module clock before access SD module's register


	if(id)
	else
#endif

//	pSD->SD_CMD_RESP_TOUT_MAX = 0xFF;

//	MMPF_SD_SetTimeout(id, MMPF_SD_NORMAL_MODE);
//	MMPF_SD_SwitchTimeout(id, MMPF_SD_DEFAULT_TIMEOUT);

	return  MMP_ERR_NONE;
}




/** @brief Send command sequence for SD reset

@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@retval MMP_ERR_NONE Success
@retval MMP_SD_ERR_RESET Initial command fail, maybe no card.
@retval MMP_SD_ERR_COMMAND_FAILED CMD0 ok, but follow command fail, maybe not SD card.
*/
MMP_ERR  MMPF_SD_Reset(MMPF_SD_ID id)
{

//	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;
//	AITPS_GBL pGBL = AITC_BASE_GBL;
	printk("MMPF_SD_Reset: %d\n",id);



	m_bSDAccessFail[id] = MMP_FALSE;
	mIsMMC4X[id] = 0;
	glMMCCurrentPartition[id] = 0;

#if (ENABLE_SDHC_SWITCH_HIGH_SPEED == 1)
	mSdhcHsm[id] = 0;
#endif

#if (ENABLE_SD_CLKGATING == 1)
	x
	MMPF_SD_EnableModuleClock(id, MMP_TRUE);
#endif
	MMPF_SD_SetTimeout(id, MMPF_SD_NORMAL_MODE);
	MMPF_SD_SwitchTimeout(id, MMPF_SD_DEFAULT_TIMEOUT);
	//wilson@110509: move g0 src clk divide setting to init interface function
	//pGBL->GBL_CLK_0_LCD_SD = ((pGBL->GBL_CLK_0_LCD_SD)&0x0F) | (GBL_CLK_SD_DIV(1));


#if 1 // Before 1.05.02.Philip2: No using slow clock
//	pGBL->GBL_CLK_DIS1 &= ~(GBL_CLK_SD1_DIS);

#else // After 1.05.02.Philip2: Using slow clock
	pSD->SD_CTL_0 = (SLOW_CLK_EN | DMA_EN | CLK_IN_SYS_DIV | BUS_WIDTH_4);
	pSD->SD_SLOW_CLK_DIV = SD_CLOCK_DIV_2048;


#endif

//    pSD->SD_CTL_1 = CLK_EN|WAIT_LAST_BUSY_EN/*Vin | AUTO_CLK_EN*/ | RD_TOUT_EN | WR_TOUT_EN | R1B_TOUT_EN;

//    pSD->SDIO_ATA_CTL = SDIO_EN;//jimmyhung for SDIO CARD

    return  MMP_ERR_NONE;
}
#if 0
#ifndef __KERNEL__
/** @brief Send SEND_CSD (CMD 9) to get card info after card reset

@param[in] devid ID of SD controller. For DIAMOND, valid input is 0, 1 and 2.
@retval MMP_ERR_NONE Success
@retval MMP_SD_ERR_RESET Command fail, maybe no card.
*/
MMP_ERR  MMPF_SD_GetCardInfo(MMPF_SD_ID id, MMPF_SD_CARDTYPE btype)
{
	MMP_USHORT  c_size;
	MMP_UBYTE   csizemult;
	MMP_UBYTE   block_size;
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;

	if (!MMPF_SD_SendCommand(id, SEND_CSD, glsdCardAddr[id])) {
		int i;
		for(i=0;i<8;++i)
			printk("Resp[%d] = 0x%x\r\n",i,pSD->SD_RESP.W[i]);

		
		if ((mIsHC[id])&&(btype == MMPF_SD_SDTYPE)) {        // The maximum size of SD2.0 is 32 GB, so the (pSD->SD_RESP.W[4] & 0x3F) should be zero.
			c_size = (pSD->SD_RESP.W[3] | ((pSD->SD_RESP.W[4] & 0x3F)<<16) );
			glsdCardSize[id] = (c_size +1)*1024;        //the unit is sector
		}
		else {
			block_size = pSD->SD_RESP.W[5] & 0x000f;
			c_size = (pSD->SD_RESP.W[4] & 0x03ff) << 2;
			c_size += (pSD->SD_RESP.W[3] & 0xC000) >> 14;
			csizemult = (pSD->SD_RESP.W[3] & 0x0003) << 1;
			csizemult += (pSD->SD_RESP.B[5] & 0x80) >> 7;
			glsdCardSize[id] = (((MMP_ULONG)(c_size + 1)) * (1 << (csizemult + 2))) << (block_size - 9);
		}	

		if((btype == MMPF_SD_MMCTYPE)&&((pSD->SD_RESP.B[15]&0x3C) == 0x10)){
			mIsMMC4X[id] = 1;
			if(c_size == 0xFFF){
				mIs2GMMC[id] = 1;
			}
			else
				mIs2GMMC[id] = 0;
		}  
		else{
			mIsMMC4X[id] = 0;
		}


#if 0
	RTNA_DBG_Long2( (glsdCardSize[id])*512 );
	RTNA_DBG_Str2(": Card Size.\r\n");
#endif

		return  MMP_ERR_NONE;
	}
	else {
		return  MMP_SD_ERR_RESET;
	}
}
//------------------------------------------------------------------------------
//  Function    : MMPF_GetSDSize
//------------------------------------------------------------------------------
MMP_ERR  MMPF_SD_GetSize(MMPF_SD_ID id, MMPF_MMC_BOOTPARTITION part, MMP_ULONG *pSize)
{
    switch(part){
        case MMPF_MMC_BOOT1_AREA:
        case MMPF_MMC_BOOT2_AREA:
            *pSize = glMMCBootSize[id];
            break;
        case MMPF_MMC_USER_AREA:
            *pSize = glsdCardSize[id];
            break;
    }
    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SD_SendCommand
//------------------------------------------------------------------------------
MMP_ERR  MMPF_SD_SendCommand(MMPF_SD_ID id, MMP_UBYTE command, MMP_ULONG argument)
{
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//AITPS_SD pSD = AITC_BASE_SD + id;
int i;
	MMP_USHORT tmp;
	

		
	switch(command) {
		case GO_IDLE_STATE: /* no resp */
			pSD->SD_CMD_REG_0 = command | NO_RESP;
			break;
		case ALL_SEND_CID: /* FALL THROUGH */
		case SEND_CSD: /* r2 */
			pSD->SD_CMD_REG_0 = command | R2_RESP;
			break;
		case STOP_TRANSMISSION: /* FALL THROUGH */
		case SELECT_CARD: /* r1b*/
		case VENDER_COMMAND_62:
		case ERASE:    
			pSD->SD_CMD_REG_0 = command | R1B_RESP;
			break;
		case SWITCH_FUNC:
			if(mIsMMC4X){
				pSD->SD_CMD_REG_0 = command | R1B_RESP;
				break;
			}   
		default: /* others */
			pSD->SD_CMD_REG_0 = command | OTHER_RESP;
			break;
	}




	pSD->SD_CMD_ARG = argument; /* SD command's argument */
	pr_debug("MMPF_SD_SendCommand(%d): cmd = 0x%x     0x%x\n",id,command,argument);

    /* Clear interrupt status */
	pSD->SD_CPU_INT_SR = (CMD_RESP_CRC_ERR|CMD_RESP_TOUT|BUSY_TOUT|CMD_SEND_DONE|SDIO_INT|DATA_CRC_ERR|DATA_TOUT|DATA_ST_BIT_ERR|DATA_SEND_DONE);


	
#if 0//Vin
    #if (EN_CARD_DETECT == 1)
    if (!MMPF_SD_CheckCardIn(id)) {
        MMPF_HIF_SetCmdStatus(SD_CARD_NOT_EXIST);
        return  MMP_SD_ERR_CARD_REMOVED;
    }
	#endif

	#if (SD_CPU_NONBLOCKING == 1)
    if (command == STOP_TRANSMISSION) {
        pSD->SD_CPU_INT_EN = (CMD_SEND_DONE);
    }
	#endif
#endif
    /* SD_TRANSFER_START */
	//#if (ENABLE_SDHC_SWITCH_HIGH_SPEED == 1)
	
    if (command == SEND_SCR) {

        // Refer to Section 5.6, SCR register size is 64 bit (8 Byte)
        pSD->SD_BLK_LEN = 8;
        RTNA_DBG_Str3("Issue SEND_SCR\r\n");
        pSD->SD_BLK_NUM = 1;
        pSD->SD_DMA_ST_ADDR = m_ulSDDmaAddr;
        pSD->SD_CMD_REG_1 |= (ADTC_READ|SEND_CMD);
	
    }
    else if ((command == SWITCH_FUNC) && (mSdhcHsm[id]==1)) {

        // Refer to Section 4.3.10, SWITCH response size is 512 bit (64 Byte)
        pSD->SD_BLK_LEN = 64;
        pSD->SD_BLK_NUM = 1;
        pSD->SD_DMA_ST_ADDR = m_ulSDDmaAddr;

        pSD->SD_CMD_REG_1 |= (ADTC_READ|SEND_CMD);

		#if (SD_CPU_NONBLOCKING == 1)
        pSD->SD_CPU_INT_EN |= (DATA_SEND_DONE);
		#endif
    }
    else if ((command == SEND_EXT_CSD) && (mIsMMC4X[id]==1)) {
        RTNA_DBG_Str3("SEND_EXT_CSD cmd \r\n");

        // 512 byte extend CSD
        pSD->SD_BLK_LEN = 512;
        //RTNA_DBG_Str3("Issue SWITCH_FUN\r\n");
        pSD->SD_BLK_NUM = 1;
        pSD->SD_DMA_ST_ADDR = m_ulSDDmaAddr;

        pSD->SD_CMD_REG_1 |= (ADTC_READ|SEND_CMD);

		#if (SD_CPU_NONBLOCKING == 1)
        pSD->SD_CPU_INT_EN |= (DATA_SEND_DONE);
		#endif
    }else     if (command == SEND_STATUS) {
		// Refer to Section 5.6, SCR register size is 64 bit (8 Byte)
		pSD->SD_BLK_LEN = 64;
		RTNA_DBG_Str3("Issue SEND_STATUS\r\n");
		pSD->SD_BLK_NUM = 1;
		pSD->SD_DMA_ST_ADDR = m_ulSDDmaAddr;
		pSD->SD_CMD_REG_1 |= (ADTC_READ|SEND_CMD);
	}
    else
	//#endif /* (ENABLE_SDHC_SWITCH_HIGH_SPEED==1) */
    {
	pSD->SD_CMD_REG_1 |= SEND_CMD; // Send CMD here
    }


    /* **************** */
    /* Check CMD status */
    /* **************** */
RTNA_DBG_Str3("Check CMD status\r\n");
	i =0;
	
#if 1
    if (command == GO_IDLE_STATE) {


        while (!(pSD->SD_CPU_INT_SR & CMD_SEND_DONE))
			if(i++>10000000)
			{
				RTNA_DBG_Str3("GO_IDLE_STATE: time out\r\n");			
				break;
		    	}
    }



    else {
	while(1) {
            tmp = pSD->SD_CPU_INT_SR;
            //RTNA_DBG_Short3(tmp);RTNA_DBG_Str(3, ":pSD->SD_CPU_INT_SR.\r\n");//PhilipTrace
            if ((tmp & CMD_SEND_DONE) && (!(tmp & CMD_RESP_TOUT))) {
RTNA_DBG_Str3("Cmd done no timeout\r\n");
                break;
            }

            if(tmp & CMD_RESP_TOUT) {
RTNA_DBG_Str3("Cmd timeout\r\n");
                #if 0
                MMP_LONG pTmpPtr = (MMP_LONG) pSD;
                RTNA_DBG_Byte3(id);
                RTNA_DBG_Str3(" : ");
                RTNA_DBG_Long3(command);
                RTNA_DBG_Str3(" : id, AITPS_SD for CMD_RESP_TOUT.\r\n");
                #endif

#if (ENABLE_SDIO_FEATURE == 1)
                if (command == IO_SEND_OP_COND) {
                    break;
                }
#endif

                // Designer recommand that wait for a while
			MMPF_SD_WaitCount(500);

                if (command != SEND_IF_COND) {
                    #if 1
                    RTNA_DBG_Str2("SD cmd");
                    printk("%d",command);//MMPF_DBG_Int(command, -2);
                    RTNA_DBG_Str2(" fails. Status = ");
                    RTNA_DBG_Byte2(pSD->SD_CPU_INT_SR);
                    RTNA_DBG_Str2(".\r\n");
                    #endif
                }

                return MMP_SD_ERR_COMMAND_FAILED;
            }
        }
    }



    /* Check CMD response status */
    if ((command == STOP_TRANSMISSION)||(command == SELECT_CARD)||(command == VENDER_COMMAND_62)) { /* ac, R1b */
        //while(!(pSD->SD_DATA_BUS_SR & DAT0_SR));
        if( pSD->SD_CPU_INT_SR & CMD_RESP_CRC_ERR){
            RTNA_DBG_Str2("crc7 error\r\n");
        }
    }

    else if ((command == IO_RW_DIRECT) || (command == IO_RW_EXTENDED)) { /* adtc, R4 */
        if (pSD->SD_RESP.D[3] & (COM_CRC_ERROR | ILLEGAL_COMMAND
                                | UNKNOW_ERROR | INV_FUN_NUM
                                | OUT_OF_RANGE) ) {
            RTNA_DBG_Str2("SDIO Command ");
//Vin            MMPF_DBG_Int(command, -2);
            RTNA_DBG_Str2(" fails. Status = ");
            RTNA_DBG_Short2(pSD->SD_CPU_INT_SR);
            RTNA_DBG_Str2("\r\n");
        }
    }

    else if ((command != ALL_SEND_CID) && 
		(command != SET_RELATIVE_ADDR) &&
		(command != SEND_OP_COND) &&
		(command != IO_SEND_OP_COND) &&
		(command != SD_APP_OP_COND)
            ) {

		tmp = pSD->SD_CPU_INT_SR;

		if (tmp & CMD_RESP_CRC_ERR) {
			RTNA_DBG_Str2("SD cmd");
			//Vin            MMPF_DBG_Int(command, -2);
			RTNA_DBG_Str2(":crc7 error.\r\n");
			return MMP_SD_ERR_DATA;
		}
	}

    //#if (ENABLE_SDHC_SWITCH_HIGH_SPEED==1)
    if ( (command == SEND_STATUS)||(command==SEND_SCR) || ((command==SWITCH_FUNC) && (mSdhcHsm[id]==1)) || ((command == SEND_EXT_CSD) && (mIsMMC4X[id]==1))) {

#if (SD_CPU_NONBLOCKING == 1)
x
		MMPF_SD_WaitIsrDone(id, 0);
#else

		while((!(pSD->SD_CPU_INT_SR & DATA_SEND_DONE))& i++<1000000);
#endif

    }

	if (command == SEND_SCR) {
		printk("SCR = 0x%04x\r\n",m_ulSDDmaAddr[0]);
		printk("SCR FIFO= 0x%04x\r\n",pSD->SD_FIFO_PORT);
		printk("SCR FIFO= 0x%04x\r\n",pSD->SD_FIFO_PORT);
		
	}	

#ifdef __linux__
			printk("Status = 0x%x i = %d\r\n",pSD->SD_CPU_INT_SR,i);

#else
                    RTNA_DBG_Str2("SD cmd");
                    RTNA_DBG_Str2(" Status = ");
                    RTNA_DBG_Byte2(pSD->SD_CPU_INT_SR);
                    RTNA_DBG_Str2(".\r\n");
#endif

			pSD->SD_CPU_INT_SR = pSD->SD_CPU_INT_SR;
					
	//#endif
#endif
    return  MMP_ERR_NONE;
} /* MMPF_SD_SendCommand */

//------------------------------------------------------------------------------
//  Function    : MMPF_SD_CheckCardStatus
//------------------------------------------------------------------------------
MMP_ERR  MMPF_SD_CheckCardStatus(MMPF_SD_ID id)
{
	MMP_ERR	err;
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//AITPS_SD pSD = AITC_BASE_SD + id;

printk("MMPF_SD_CheckCardStatus\r\n");
#if 0
	#if (SD_BUS_REENTRY_PROTECT == 1)
    if (MMPF_SD_BusAcquire(id, 0)) {
		return MMP_SD_ERR_BUSY;
    }
	#endif

	#if (ENABLE_SD_CLKGATING == 1)
    MMPF_SD_EnableModuleClock(id, MMP_TRUE);
	#endif
#endif
    err = MMPF_SD_SendCommand(id, SEND_STATUS, glsdCardAddr[id]);
#if 0
	#if (ENABLE_SD_CLKGATING==1)
   	MMPF_SD_EnableModuleClock(id, MMP_FALSE);
	#endif

	#if (SD_BUS_REENTRY_PROTECT == 1)
    if (MMPF_SD_BusRelease(id)) {
		return MMP_SD_ERR_BUSY;
    }
	#endif
#endif 

printk("Resp[0] = 0x%x\r\n",pSD->SD_RESP.D[0]);
printk("Resp[1] = 0x%x\r\n",pSD->SD_RESP.D[1]);
printk("Resp[2] = 0x%x\r\n",pSD->SD_RESP.D[2]);
printk("Resp[3] = 0x%x\r\n",pSD->SD_RESP.D[3]);

	if (err)
		return MMP_SD_ERR_COMMAND_FAILED;
	else     
		return MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPF_SD_EraseSector
//  flow: start(CMD32) -> end(CMD33) -> erase(CMD38)
//------------------------------------------------------------------------------
MMP_ERR MMPF_SD_EraseSector(MMPF_SD_ID id, MMP_ULONG startsect, MMP_ULONG sectcount)
{

	if (MMPF_SD_SendCommand(id, ERASE_WR_BLK_START, (MMP_ULONG)startsect)){
		#if (ENABLE_SD_CLKGATING == 1)
	    MMPF_SD_EnableModuleClock(id, MMP_FALSE);
		#endif

		#if (SD_BUS_REENTRY_PROTECT == 1)
	    if (MMPF_SD_BusRelease(id)) {
	        return MMP_SD_ERR_BUSY;
	    }
		#endif
	    return MMP_SD_ERR_COMMAND_FAILED;
	}
	if (MMPF_SD_SendCommand(id, ERASE_WR_BLK_END, (MMP_ULONG)(startsect+sectcount))){
		#if (ENABLE_SD_CLKGATING == 1)
	    MMPF_SD_EnableModuleClock(id, MMP_FALSE);
		#endif

		#if (SD_BUS_REENTRY_PROTECT == 1)
	    if (MMPF_SD_BusRelease(id)) {
	        return MMP_SD_ERR_BUSY;
	    }
		#endif
	    return MMP_SD_ERR_COMMAND_FAILED;
	}
	if (MMPF_SD_SendCommand(id, ERASE, startsect)){
		#if (ENABLE_SD_CLKGATING == 1)
	    MMPF_SD_EnableModuleClock(id, MMP_FALSE);
		#endif

		#if (SD_BUS_REENTRY_PROTECT == 1)
	    if (MMPF_SD_BusRelease(id)) {
	        return MMP_SD_ERR_BUSY;
	    }
		#endif
	    return MMP_SD_ERR_COMMAND_FAILED;
	}
	return MMP_ERR_NONE;
}

#endif /*__KERNEL__*/


/** @addtogroup MMPF_SDIO
@{
*/
#if (ENABLE_SDIO_FEATURE==1)
x
//------------------------------------------------------------------------------
//  Function    : MMPF_SDIO_Reset
//------------------------------------------------------------------------------
MMP_ERR  MMPF_SDIO_Reset(MMPF_SD_ID id)
{
    AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//	    AITPS_SD pSD = AITC_BASE_SD + id;
    MMP_USHORT  i;
    MMP_ULONG ulSDIOCardAddr;

    pSD->SD_CTL_0 = (/*0x40|*/DMA_EN | CLK_IN_SYS_DIV | BUS_WIDTH_1/*|SLOW_CLK_EN*//*|MMC_MODE*/);
    mCurSDClockDiv[id] = SD_CLOCK_DIV_256;
    MMPF_SD_SetTimeout(id, MMPF_SD_NORMAL_MODE);
    MMPF_SD_SwitchTimeout(id, MMPF_SD_DEFAULT_TIMEOUT);

    /// Switch to SDIO mode to get SDIO INT
    /// Clear SDIO status and other SD status at the first
    pSD->SD_CPU_INT_SR = (SDIO_INT|DATA_CRC_ERR|DATA_ST_BIT_ERR|DATA_SEND_DONE);
    pSD->SD_CPU_INT_EN = (DATA_SEND_DONE|SDIO_INT);

    pSD->SDIO_ATA_CTL = SDIO_EN;

//Vin    mSDIOBusySemID[id] = MMPF_OS_CreateSem(1);
    #if 0
    RTNA_DBG_Str3("id:");
    RTNA_DBG_Byte3(id);
    RTNA_DBG_Str3(", mSDIOBusySemID[id]=");
    RTNA_DBG_Long3((MMP_LONG)mSDIOBusySemID[id]);
    RTNA_DBG_Str3(".\r\n");
    #endif


    pSD->SD_CTL_1 = WAIT_LAST_BUSY_EN| AUTO_CLK_EN | RD_TOUT_EN | WR_TOUT_EN | R1B_TOUT_EN;

    MMPF_SD_WaitCount(10000);
    i = 0;
    do {
        MMPF_SD_WaitCount(40000);
        if (!MMPF_SD_SendCommand(id, IO_SEND_OP_COND, 0x000000)) {
            i++;
        }
        else {
            RTNA_DBG_Str0("SDIO_RESET_ERROR\r\n");
            return  MMP_SD_ERR_RESET;
        }
    } while( !(pSD->SD_RESP.D[3] & 0x80000000) && (i < 10));
    if (i != 10) {
        RTNA_DBG_Str1("Get 0 OCR success. ");
        RTNA_DBG_Short1(i);
        RTNA_DBG_Str1(" times\r\n");
    }
    else {
        // After OCR request response, then set OCR
        i = 0;
        do {
            MMPF_SD_WaitCount(4000);
            if (!MMPF_SD_SendCommand(id, IO_SEND_OP_COND, 0x180000)) {
                i++;
            }
            else {
                RTNA_DBG_Str0("SDIO_RESET_ERROR\r\n");
                return  MMP_SD_ERR_RESET;
            }
        }
        while (!(pSD->SD_RESP.D[3] & 0x80000000) && (i < 10));
        if (i != 10) {
            RTNA_DBG_Str1("Get 1 OCR success. ");
            RTNA_DBG_Short1(i);
            RTNA_DBG_Str1(" times\r\n");
        }
        else {
            RTNA_DBG_Str0("Get OCR fail.");
            return MMP_SD_ERR_RESET;
        }
    }

    // CMD  3
    if(!MMPF_SD_SendCommand(id, SEND_RELATIVE_ADDR, 0)) {
        ulSDIOCardAddr = (MMP_ULONG)pSD->SD_RESP.D[3];
        #if 0
        RTNA_DBG_Long2(ulSDIOCardAddr);
        RTNA_DBG_Str2("=ulSDIOCardAddr\r\n");
        #endif
    }
    else {
        return MMP_SD_ERR_RESET;
    }
    musBlockLen[id] = 0x200;
    pSD->SD_BLK_LEN = musBlockLen[id];
    // Select SD card
    if (MMPF_SD_SendCommand(id, SELECT_CARD, ulSDIOCardAddr))
    {
        return MMP_SD_ERR_RESET;
    }

    mCurSDClockDiv[id] = SD_CLOCK_DIV_2; //For Diamond, SD defalut clock src is PLL_G0/3

    gbIsSD[id] = 1;
    //RTNA_DBG_Str(0,"RESET SDIO SUCCESS\r\n");

    RTNA_DBG_Str(0,"RESET SDIO SUCCESS2\r\n");
    //pSD->SD_CTL_1 = WAIT_LAST_BUSY_EN| AUTO_CLK_EN | RD_TOUT_EN | WR_TOUT_EN | R1B_TOUT_EN;

    return  MMP_ERR_NONE;
} /* MMPF_SDIO_Reset */


//------------------------------------------------------------------------------
//  Function    : MMPF_SDIO_WriteReg
//------------------------------------------------------------------------------
MMP_ERR MMPF_SDIO_WriteReg(MMPF_SD_ID id, MMP_UBYTE  fun_num, MMP_ULONG reg_addr, MMP_UBYTE src)
{
    MMP_USHORT ret;

    MMPF_SDIO_BusAcquire(id);
#if 1
    ret = MMPF_SD_SendCommand(id, IO_RW_DIRECT, (RW_FLAG(1)| FUN_NUM(fun_num) | RAW_FLAG(1)
                                            | REG_ADD(reg_addr) | src)) ;
#else
    param = (RW_FLAG(1)| FUN_NUM(fun_num) | RAW_FLAG(1) | REG_ADD(reg_addr) | src);
    RTNA_DBG_Long(3, (param << 8) );
    RTNA_DBG_Str(3, " ");
    RTNA_DBG_Long(3, (param) );
    RTNA_DBG_Str(3, ":param for SD controller while CMD52 write.\r\n");
    ret = MMPF_SD_SendCommand(id, IO_RW_DIRECT, param);
#endif

    MMPF_SDIO_BusRelease(id);

    if (ret) {
        RTNA_DBG_Str3("write reg fail\r\n");
        return MMP_SD_ERR_COMMAND_FAILED;
    }
    else {
        return MMP_ERR_NONE;
    }
} /* MMPF_SDIO_WriteReg */

//------------------------------------------------------------------------------
//  Function    : MMPF_SDIO_ReadReg
//------------------------------------------------------------------------------
MMP_ERR MMPF_SDIO_ReadReg(MMPF_SD_ID id, MMP_UBYTE fun_num, MMP_ULONG reg_addr, MMP_UBYTE *p_dst)
{
        AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//    AITPS_SD pSD = AITC_BASE_SD + id;
    MMP_USHORT ret;

    MMPF_SDIO_BusAcquire(id);
#if 1
    ret = MMPF_SD_SendCommand(id, IO_RW_DIRECT, (RW_FLAG(0)| FUN_NUM(fun_num) | RAW_FLAG(0)
                                            | REG_ADD(reg_addr) | 0));
#else
    param = (RW_FLAG(0)| FUN_NUM(fun_num) | RAW_FLAG(0) | REG_ADD(reg_addr) | 0);
    RTNA_DBG_Long(3, (param << 8) );
    RTNA_DBG_Str(3, " ");
    RTNA_DBG_Long(3, (param) );
    RTNA_DBG_Str(3, ":param for SD controller while CMD52 read.\r\n");
    ret = MMPF_SD_SendCommand(id, IO_RW_DIRECT, param);
#endif

    *p_dst = pSD->SD_RESP.D[3] & 0xFF;

    MMPF_SDIO_BusRelease(id);

    if (ret) {
        RTNA_DBG_Str3("read reg fail\r\n");
        return MMP_SD_ERR_COMMAND_FAILED;
    }
    else {
        #if 0
        RTNA_DBG_Str3("MMPF_SDIO_ReadReg:");
        RTNA_DBG_Long3( pSD->SD_RESP.D[3] );
        RTNA_DBG_Str3("\r\n");
        #endif
        return MMP_ERR_NONE;
    }
} /* MMPF_SDIO_ReadReg */
//------------------------------------------------------------------------------
//  Function    : MMPF_SDIO_WriteMultiReg
//------------------------------------------------------------------------------
MMP_ERR MMPF_SDIO_WriteMultiReg(
            MMPF_SD_ID id,
            MMP_UBYTE           fun_num,
            MMP_UBYTE           blk_mode,
            MMP_UBYTE           op_code,
            MMP_ULONG           reg_addr,
            MMP_ULONG           count,
            MMP_ULONG           blk_size,
            MMP_UBYTE           *p_src)
{
        AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//    AITPS_SD pSD = AITC_BASE_SD + id;
    MMP_USHORT ret;
    MMP_ULONG i;

    MMPF_SDIO_BusAcquire(id);

    #if 1 //++Using CPU to copy data into DAM src addr
    if (blk_mode) {
        for (i = 0; i < count*blk_size; i++) {
            *(MMP_UBYTE *) (m_ulSDDmaAddr + i) = p_src[i];
        }
    }
    else {
        for (i = 0; i < count; i++) {
            *(MMP_UBYTE *) (m_ulSDDmaAddr + i) = p_src[i];
        }
    }
    #endif //--Using CPU to copy data into DAM src addr

    pSD->SD_DMA_ST_ADDR = m_ulSDDmaAddr;
    #if 0
    RTNA_DBG_Long(1,pSD->SD_DMA_START_ADDR_L);
    RTNA_DBG_Str(1,"==pSD->SD_DMA_START_ADDR_L\r\n");
    #endif

    if (blk_mode) {
        pSD->SD_CMD_REG_1 = ADTC_WRITE;
        pSD->SD_BLK_NUM = count;
        pSD->SD_BLK_LEN = blk_size;
    }
    else {
        pSD->SD_CMD_REG_1 = ADTC_WRITE;
        pSD->SD_BLK_NUM = 1;
        pSD->SD_BLK_LEN = count;
    }

    pSD->SD_CPU_INT_EN |= DATA_SEND_DONE;
    ret = MMPF_SD_SendCommand(id, IO_RW_EXTENDED, (RW_FLAG(1)| FUN_NUM(fun_num)
                            | BLK_MODE(blk_mode) | OP_CODE(op_code)| REG_ADD(reg_addr) | count)) ;

    if(ret) {
        MMPF_SDIO_BusRelease(id);
        RTNA_DBG_Str0("write cmd fail.\r\n");
        return MMP_SD_ERR_COMMAND_FAILED;
    }
#if (SDIO_CPU_NONBLOCKING==1)
    MMPF_SD_WaitIsrDone(id, 0x10000);
#endif

    MMPF_SDIO_BusRelease(id);

    return MMP_ERR_NONE;
} /* MMPF_SDIO_WriteMultiReg */

//------------------------------------------------------------------------------
//  Function    : MMPF_SDIO_ReadMultiReg
//------------------------------------------------------------------------------
MMP_ERR   MMPF_SDIO_ReadMultiReg(
            MMPF_SD_ID id,
            MMP_UBYTE           fun_num,
            MMP_UBYTE           blk_mode,
            MMP_UBYTE           op_code,
            MMP_ULONG           reg_addr,
            MMP_ULONG           count,
            MMP_ULONG           blk_size,
            MMP_UBYTE          *p_dst)
{
        AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//    AITPS_SD pSD = AITC_BASE_SD + id;
    MMP_USHORT ret;
    MMP_ULONG i;
    #if (SDIO_CPU_NONBLOCKING==0)
    MMP_ULONG time_out;
    #endif /* (SDIO_CPU_NONBLOCKING==0) */

    MMPF_SDIO_BusAcquire(id);

    pSD->SD_DMA_ST_ADDR = (MMP_ULONG)m_ulSDDmaAddr;

    if (blk_mode) {
        pSD->SD_CMD_REG_1 = ADTC_READ;
        pSD->SD_BLK_NUM = count;
        pSD->SD_BLK_LEN = blk_size;
    }
    else {
        pSD->SD_CMD_REG_1 = ADTC_READ;
        pSD->SD_BLK_NUM = 1;
        pSD->SD_BLK_LEN = count;
    }

#if (SDIO_CPU_NONBLOCKING==1) //++Philip@070531: Add for ASYNC SDIO READ
    pSD->SD_CPU_INT_EN |= DATA_SEND_DONE; // Enable interrupt when "DATA SEND DONE"
#endif /* (SDIO_CPU_NONBLOCKING==1) */

    ret = MMPF_SD_SendCommand(id, IO_RW_EXTENDED, (RW_FLAG(0)| FUN_NUM(fun_num)
                            | BLK_MODE(blk_mode) | OP_CODE(op_code) | REG_ADD(reg_addr) | count));

    if(ret) {
        MMPF_SDIO_BusRelease(id);
        RTNA_DBG_Str3("read cmd fail\r\n");
        return MMP_SD_ERR_COMMAND_FAILED;
    }

#if (SDIO_CPU_NONBLOCKING==1) //++Philip@070531: Add for ASYNC SDIO READ
    MMPF_SD_WaitIsrDone(id, 0x10000);
#elif (SDIO_CPU_NONBLOCKING==0)
    if (time_out == SD_TIMEOUT_COUNT) {

        RTNA_DBG_Str0("==read time-out");
        #if 0
        RTNA_DBG_Str0("blk, num:");
        RTNA_DBG_Long3(blk_size);
        RTNA_DBG_Str0(":");
        RTNA_DBG_Long3(count);
        RTNA_DBG_Str0("\r\n");
        #endif //0
        MMPF_SDIO_BusRelease(id);
        return MMP_SD_ERR_DATA;
    }
#endif /* (SDIO_CPU_NONBLOCKING==0) */

    if (blk_mode) {
//        RTNA_DBG_Str3("read blk multi data:\r\n");
        for (i = 0; i < 0x8000; i++) ; /* dummy wait */

        for (i = 0; i < (blk_size*count); i++) {
            p_dst[i] = *(MMP_UBYTE *) (m_ulSDDmaAddr + i);
            #if 0
            RTNA_DBG_Byte3(p_dst[i]);
            RTNA_DBG_Str3(" ");
            #endif
        }
    }
    else {
//        RTNA_DBG_Str3("read byte multi data:\r\n");
        for (i = 0; i < (count); i++) {
            p_dst[i] = *(MMP_UBYTE *) (m_ulSDDmaAddr + i);
            #if 0
            RTNA_DBG_Byte3(p_dst[i]);
            RTNA_DBG_Str3(" ");
            #endif
        }
        #if 0 //PhilipDebug
        RTNA_DBG_Long(3, (int)&p_dst[0]);
        RTNA_DBG_Str(3, ": &p_dst[0]\r\n");
        RTNA_DBG_Long(3, m_ulSDDmaAddr);
        RTNA_DBG_Str(3, ": m_ulSDDmaAddr\r\n");
        for (i = 0; i < 4; i++) {

            RTNA_DBG_Byte3(p_dst[i]);
            RTNA_DBG_Str3(" ");
        }
        RTNA_DBG_Str(3, ": the first 4 bytes of p_dst[]\r\n");
        #endif
    }
    MMPF_SDIO_BusRelease(id);
    return MMP_ERR_NONE;
} /* MMPF_SDIO_ReadMultiReg */

//------------------------------------------------------------------------------
//  Function    : MMPF_SDIO_ReadMultiRegDirect
//------------------------------------------------------------------------------
MMP_ERR   MMPF_SDIO_ReadMultiRegDirect(
            MMPF_SD_ID id,
            MMP_UBYTE           fun_num,
            MMP_UBYTE           blk_mode,
            MMP_UBYTE           op_code,
            MMP_ULONG           reg_addr,
            MMP_ULONG           count,
            MMP_ULONG           blk_size,
            MMP_UBYTE          *p_dst)
{
        AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//    AITPS_SD pSD = AITC_BASE_SD + id;
    MMP_USHORT ret;
    //MMP_ULONG i;
    #if (SDIO_CPU_NONBLOCKING==0)
    MMP_ULONG time_out;
    #endif /* (SDIO_CPU_NONBLOCKING==0) */

    MMPF_SDIO_BusAcquire(id);

    pSD->SD_DMA_ST_ADDR = (MMP_ULONG)p_dst;

    if (blk_mode) {
        pSD->SD_CMD_REG_1 = ADTC_READ;
        pSD->SD_BLK_NUM = count;
        pSD->SD_BLK_LEN = blk_size;
    }
    else {
        pSD->SD_CMD_REG_1 = ADTC_READ;
        pSD->SD_BLK_NUM = 1;
        pSD->SD_BLK_LEN = count;
    }

#if (SDIO_CPU_NONBLOCKING==1) //++Philip@070531: Add for ASYNC SDIO READ
    pSD->SD_CPU_INT_EN |= DATA_SEND_DONE; // Enable interrupt when "DATA SEND DONE"
#endif /* (SDIO_CPU_NONBLOCKING==1) */

    ret = MMPF_SD_SendCommand(id, IO_RW_EXTENDED, (RW_FLAG(0)| FUN_NUM(fun_num)
                            | BLK_MODE(blk_mode) | OP_CODE(op_code) | REG_ADD(reg_addr) | count));

    if(ret) {
        MMPF_SDIO_BusRelease(id);

        RTNA_DBG_Str3("read cmd fail\r\n");
        return MMP_SD_ERR_COMMAND_FAILED;
    }

#if (SDIO_CPU_NONBLOCKING==1) //++Philip@070531: Add for ASYNC SDIO READ
    MMPF_SD_WaitIsrDone(id, 0x10000);
#elif (SDIO_CPU_NONBLOCKING==0)
    if (time_out == SD_TIMEOUT_COUNT)
    {

        RTNA_DBG_Str0("=read time-out");
        #if 0
        RTNA_DBG_Str0("blk, num:");
        RTNA_DBG_Long3(blk_size);
        RTNA_DBG_Str0(":");
        RTNA_DBG_Long3(count);
        RTNA_DBG_Str0("\r\n");
        #endif //0
        MMPF_SDIO_BusRelease(id);
        return MMP_SD_ERR_DATA;
    }
#endif /* (SDIO_CPU_NONBLOCKING==0) */

    MMPF_SDIO_BusRelease(id);
    return MMP_ERR_NONE;
} /* MMPF_SDIO_ReadMultiRegDirect */

//------------------------------------------------------------------------------
//  FUNCTION    : MMPF_SDIO_EnaISR
//------------------------------------------------------------------------------
void MMPF_SDIO_EnaISR(MMPF_SD_ID id, MMP_USHORT enable)
{
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//    AITPS_SD pSD = AITC_BASE_SD + id;

	if (enable) {
		pSD->SD_CPU_INT_EN |= SDIO_INT;
	}
	else {
		pSD->SD_CPU_INT_EN &= (~SDIO_INT);
	}
}

#endif /* (ENABLE_SDIO_FEATURE==1) */
/** @} */ // MMPF_SDIO
#if 0
//------------------------------------------------------------------------------
// FUNCTION : MMPF_SD_CheckCardIn
//------------------------------------------------------------------------------
MMP_USHORT  MMPF_SD_CheckCardIn(MMPF_SD_ID id)
{
    #if (EN_CARD_DETECT == 1)
    AITPS_GPIO pGPIO = AITC_BASE_GPIO;

    if (gbSDCardDetectionPinNum[id]==0xFF) {
        return  SD_IN;
    }
    if (gbSDCardDetectionPolarity[id]) {
        if(pGPIO->GPIO_DATA[(gbSDCardDetectionPinNum[id]/32)] & (1 << (gbSDCardDetectionPinNum[id]%32)) ) {
            return  SD_IN;
        }
        else {
            MMPF_HIF_SetCpu2HostInt(MMPF_HIF_INT_SDCARD_REMOVE);
            return  SD_OUT;
        }
    }
    else {
        if(pGPIO->GPIO_DATA[(gbSDCardDetectionPinNum[id]/32)] & (1 << (gbSDCardDetectionPinNum[id]%32))) {
            MMPF_HIF_SetCpu2HostInt(MMPF_HIF_INT_SDCARD_REMOVE);
            return  SD_OUT;
        }
        else {
            return  SD_IN;
        }
    }
	#else
    return SD_IN;
	#endif
}

//------------------------------------------------------------------------------
// FUNCTION: MMPF_SD_CheckCardWP
//------------------------------------------------------------------------------
MMP_USHORT MMPF_SD_CheckCardWP(MMPF_SD_ID id)
{
#if (EN_CARD_WRITEPROTECT)
    AITPS_GPIO pGPIO = AITC_BASE_GPIO;

    if (gbSDCardWProtectPinNum[id]==0xFF) {
        return  0;
    }

    if (gbSDCardWProtectPolarity[id]) {
        if(pGPIO->GPIO_DATA[(gbSDCardWProtectPinNum[id]/32)] & (1 << (gbSDCardWProtectPinNum[id]%32)))
            return  1;
        else
            return  0;
    }
    else {
        if(pGPIO->GPIO_DATA[(gbSDCardWProtectPinNum[id]/32)] & (1 << (gbSDCardWProtectPinNum[id]%32))){
            return  0;
        }
        else
            return  1;
    }
#else
    return 0;
#endif
}

//------------------------------------------------------------------------------
// FUNCTION: MMPF_SD_EnableCardPWR
//------------------------------------------------------------------------------
void MMPF_SD_EnableCardPWR(MMPF_SD_ID id, MMP_BOOL enable)
{
#if (EN_CARD_PWRCTL)
    AITPS_GPIO pGPIO = AITC_BASE_GPIO;

    if (gbSDCardPwrCtlPinNum[id]==0xFF) {
        return;
    }

    if (gbSDCardPwrCtlPolarity[id]) {
        if(enable)
            pGPIO->GPIO_DATA[(gbSDCardPwrCtlPinNum[id]/32)] |= (1 << (gbSDCardPwrCtlPinNum[id]%32));
        else
            pGPIO->GPIO_DATA[(gbSDCardPwrCtlPinNum[id]/32)] &= (~(1 << (gbSDCardPwrCtlPinNum[id]%32)));
    }
    else {
        if(enable)
            pGPIO->GPIO_DATA[(gbSDCardPwrCtlPinNum[id]/32)] &= (~(1 << (gbSDCardPwrCtlPinNum[id]%32)));
        else
            pGPIO->GPIO_DATA[(gbSDCardPwrCtlPinNum[id]/32)] |= (1 << (gbSDCardPwrCtlPinNum[id]%32));
    }
#endif
}

#endif
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_SD_WaitCount
//------------------------------------------------------------------------------
void    MMPF_SD_WaitCount(MMP_ULONG count)
{
	MMP_ULONG   i;
	count*=100;
	for (i = 0; i < count; i++);
}
#if 0
#if 0
MMP_ERR MMPF_SD_SetBusWidth(MMPF_SD_ID id, MMP_BYTE buswidth)
{
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;

	 pSD->SD_CTL_0 &= ~(BUS_WIDTH_8|BUS_WIDTH_4|BUS_WIDTH_1);
	if(buswidth== 4)
		pSD->SD_CTL_0|= BUS_WIDTH_4;
	else if(buswidth== 1)
		pSD->SD_CTL_0|= BUS_WIDTH_1;
}

MMP_ERR  MMPF_SD_PrepareCommand(MMPF_SD_ID id, MMP_UBYTE command, MMP_ULONG argument)
{
	AITPS_SD pSD = id?AITC_BASE_SD1:AITC_BASE_SD0;//AITPS_SD pSD = AITC_BASE_SD + id;
	MMP_USHORT tmp;

	switch(command) {
		case GO_IDLE_STATE: /* no resp */
			pSD->SD_CMD_REG_0 = command | NO_RESP;
			break;
		case ALL_SEND_CID: /* FALL THROUGH */
		case SEND_CSD: /* r2 */
			pSD->SD_CMD_REG_0 = command | R2_RESP;
			break;
		case STOP_TRANSMISSION: /* FALL THROUGH */
		case SELECT_CARD: /* r1b*/
		case VENDER_COMMAND_62:
		case ERASE:    
			pSD->SD_CMD_REG_0 = command | R1B_RESP;
			break;
		case SWITCH_FUNC:
			if(mIsMMC4X){
				pSD->SD_CMD_REG_0 = command | R1B_RESP;
				break;
			}   
		default: /* others */
			pSD->SD_CMD_REG_0 = command | OTHER_RESP;
			break;
	}



	pSD->SD_CMD_ARG = argument; /* SD command's argument */
	pr_debug("MMPF_SD_SendCommand(%d): cmd = 0x%x     0x%x\n",id,command,argument);
	
    /* Clear interrupt status */
	pSD->SD_CPU_INT_SR = (CMD_RESP_CRC_ERR|CMD_RESP_TOUT|BUSY_TOUT|CMD_SEND_DONE|SDIO_INT|DATA_CRC_ERR|DATA_TOUT|DATA_ST_BIT_ERR|DATA_SEND_DONE);
//	pSD->SD_CPU_INT_EN = CMD_SEND_DONE|CMD_RESP_TOUT|BUSY_TOUT|SDIO_INT;	//jimmyhung
	pSD->SD_CPU_INT_EN |= CMD_SEND_DONE | CMD_RESP_TOUT | BUSY_TOUT;

	if((command != ALL_SEND_CID) && 
		(command != SET_RELATIVE_ADDR) &&
		(command != SEND_OP_COND) &&
		(command != IO_SEND_OP_COND) &&
		(command != SD_APP_OP_COND)
	)
	{
			pSD->SD_CPU_INT_EN |= CMD_RESP_CRC_ERR;
	}

	if ((command == SET_BLOCKLEN))
	{
		pSD->SD_BLK_NUM = argument;
	}

	return 0;
}
#endif
/** @} */ // MMPF_SD

//#endif  /* (USING_SD_IF) */
#endif
