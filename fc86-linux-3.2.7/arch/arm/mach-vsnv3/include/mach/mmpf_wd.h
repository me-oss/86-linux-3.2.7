#ifndef _MMPF_WD_H_
#define _MMPF_WD_H_
//#include "os_wrap.h"


typedef struct _AITS_WD {//0xFFFF 8000~800F
    AIT_REG_D   WD_MODE_CTL0;		//0x00
        #define WD_CTL_ACCESS_KEY        0x2340
        #define WD_INT_EN      0x04 
        #define WD_RT_EN          0x02 
        #define WD_EN                0x01      
    AIT_REG_D   WD_MODE_CTL1;		//0x04
        #define WD_CLK_CTL_ACCESS_KEY    0x3700       
    AIT_REG_D   WD_RE_ST;            		//0x08
        #define WD_RESTART    0xC071
    AIT_REG_D   WD_SR;        			//0x0C
        #define WD_RESET_SR        0x02
        #define WD_OVERFLOW_SR     0x01
} AITS_WD, *AITPS_WD;

typedef enum _WD_CLK_DIVIDER
{
	WD_CLK_MCK_D8    = 0x00, 
	WD_CLK_MCK_D32,
	WD_CLK_MCK_D128,
	WD_CLK_MCK_D1024
}WD_CLK_DIVIDER;

#define WD_REG_OFFSET(reg) offsetof(AITS_WD,reg)//(((AITPS_AFE)0)->reg-(AITPS_AFE)0)

#define ait_wd_readl(base,reg) \
	__raw_readl(base + WD_REG_OFFSET(reg))
#define ait_wd_writel(base,reg,value) \
	__raw_writel((value), base + WD_REG_OFFSET(reg))

typedef void WdCallBackFunc(void);
MMP_ERR MMPF_WD_SetTimeOut(MMP_UBYTE ubCounter, MMP_USHORT clockDiv);
MMP_ERR MMPF_WD_Kick(void);
MMP_ULONG MMPF_WD_GetFreq(void);
MMP_USHORT MMPF_WD_GetTimeOut(void);



#endif
