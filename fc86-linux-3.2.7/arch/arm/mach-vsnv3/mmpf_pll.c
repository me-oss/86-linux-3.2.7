//==============================================================================
//
//  File        : mmpf_pll.c
//  Description : Firmware PLL Control Function
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================
//#include "includes_fw.h"
#include <mach/mmpf_typedef.h>
#include <mach/mmpf_pll.h>
#include <mach/mmp_reg_gbl.h>
//#include "config_fw.h"
//#include "lib_retina.h"
#include <mach/reg_retina.h>
//========================FUNCTION DEFINITION===================================

#if 0//(CHIP == VSN_V2)
#define MAX_DPLL_SRC	2
#endif

#if 1//(CHIP == VSN_V3)
#define MAX_DPLL_SRC	3
#endif

/*ATTENTION: 	PLL_500_1500_1000MHZ is just used in DPLL0
			 	PLL_300_700_500MHZ is in DPLL1
			 	PLL_200_500_350MHZ is in DPLL1*/

/*ATTENTION2: 	Analog team suggest that PLL1's M valuse as "1" or "2" is the best*/
MMP_UBYTE MMPF_PLL_TABLE_VSN_V2[MMPF_PLL_FREQ_TOTAL_NO][6]=
//Note: "N" for PLL0 should fill the value "N - 1", for PLL1 should fill "N-2", "M" is only used in PLL1
// M,	N,					K(P),					DPLL_F,					AudioDiv, 	PostDiv 
{{0,	100,				PLL0_CLK_DIV(2),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_600MHz
 {0,	 83,				PLL0_CLK_DIV(2),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_498MHz
 {0,	100,				PLL0_CLK_DIV(3),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_400MHz
 {0,	111,				PLL0_CLK_DIV(4),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_333MHz
 {0,	83,					PLL0_CLK_DIV(3),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_332MHz
 {0,	100,				PLL0_CLK_DIV(4),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_300MHz
 {0,	89,					PLL0_CLK_DIV(4),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_266MHz
 {1,	PLL1_CLK_N(44),		PLL1_CLK_DIV_1,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_264MHz_PLL1
 {0,	80,					PLL0_CLK_DIV(4),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(1)},   //FREQ_240MHz_PLL0
 {1,	PLL1_CLK_N(40),		PLL1_CLK_DIV_1,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_240MHz_PLL1
 //Post-divider input clock must less than 470MHz, and to take it into consideration, our PostDiv does not 
 //take the "Fixed divid 2" of Global_clk and dram clock(for the clock larger than 240 MHz). So, if the 
 //Global or Dram clock share the same CPU pll source, please double check this.
 
 {0,	108,				PLL0_CLK_DIV(3),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(2)},   //MMPF_PLL_FREQ_216MHz_PLL0
  {1,	PLL1_CLK_N(36),		PLL1_CLK_DIV_1,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //MMPF_PLL_FREQ_216MHz_PLL1
 {0,	100,				PLL0_CLK_DIV(3),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_200MHz_PLL0
 {3,	PLL1_CLK_N(100),	PLL1_CLK_DIV_1,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_200MHz_PLL1
  {0,	96,					PLL0_CLK_DIV(6),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_192MHz_PLL0
 {2,	PLL1_CLK_N(64),		PLL1_CLK_DIV_1,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_192MHz_PLL1
 {0,	112,				PLL0_CLK_DIV(4),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_168MHz
 {0,	83,					PLL0_CLK_DIV(3),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_166MHz_PLL0
 {3,	PLL1_CLK_N(166),	PLL1_CLK_DIV_2,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_166MHz_PLL1
 {0,	81,					PLL0_CLK_DIV(3),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_162MHz
 {0,	78,					PLL0_CLK_DIV(3),		PLL_500_1500_1000MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_156MHz
 {1,	PLL1_CLK_N(48),		PLL1_CLK_DIV_2,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_144MHz
 {3,	PLL1_CLK_N(133),	PLL1_CLK_DIV_2,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_133MHz
 {1,	PLL1_CLK_N(44),		PLL1_CLK_DIV_2,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_132MHz
 {1,	PLL1_CLK_N(40),		PLL1_CLK_DIV_2,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_120MHz
 {1,	PLL1_CLK_N(32),		PLL1_CLK_DIV_2,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_96MHz
 {1,	PLL1_CLK_N(40),		PLL1_CLK_DIV_4,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_60MHz
 {1,	PLL1_CLK_N(36),		PLL1_CLK_DIV_4,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_54MHz
 {1,	PLL1_CLK_N(32),		PLL1_CLK_DIV_4,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_48MHz
 {1,	PLL1_CLK_N(52),		PLL1_CLK_DIV_8,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_39MHz
 {1,	PLL1_CLK_N(32),		PLL1_CLK_DIV_8,			PLL_300_700_500MHZ, 	0, 			GROUP_CLK_DIV(2)},   //FREQ_24MHz
};
//=======================FUNCTION GLOBAL VARIABLE===============================
MMP_ULONG        		glGroupFreq[MAX_GROUP_NUM];
static MMPF_GROUP_SRC   m_group_src[MAX_GROUP_NUM];
static MMPF_PLL_MODE    m_pll_mode = MMPF_PLL_MODE_NUMBER;
static MMPF_PLL_FREQ    m_pll_freq[MMPF_PLL_ID_MAX];
static MMPF_PLL_ID      m_cpu_src = MMPF_PLL_ID_0;
static MMP_ULONG		m_pll_out_clk[MMPF_PLL_ID_MAX] = {0};

MMP_ULONG               glCPUFreq = 0;
MMP_ULONG               glMemFreq = 0;


#pragma O0
void MMPF_PLL_WaitCount(MMP_ULONG count)
{
    while (count--);
}
#pragma

//------------------------------------------------------------------------------
//  Function    : MMPF_PLL_Setting
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPF_PLL_Setting(MMPF_PLL_MODE PLLMode, MMP_BOOL KeepG0)
{
	#if 0//(CHIP == VSN_V2)
    return MMPF_PLL_SetVSNV2PLL(PLLMode, KeepG0);
    #endif
    #if 1//(CHIP == VSN_V3)
    return MMPF_PLL_SetVSNV3PLL(PLLMode, KeepG0);
    #endif
}
//------------------------------------------------------------------------------
//  Function    : MMPF_PLL_GetCPUFreq
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function get the CPU frequence.
@param[out] ulCPUFreq 1000*(CPU frequency) (ex: 133MHz -> 133000) 
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PLL_GetCPUFreq(MMP_ULONG *ulCPUFreq)
{
	*ulCPUFreq = glCPUFreq;
	return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_PLL_GetGroupFreq
//  Description : 
//------------------------------------------------------------------------------
/** @brief The function get the Group frequence.
@param[in] ubGroupNum select to get group number (ex: 0 -> group 0) 
@param[out] ulGroupFreq 1000*(GX frequency) (ex: 133MHz -> 133000) 
@return It reports the status of the operation.
*/
MMP_ERR MMPF_PLL_GetGroupFreq(MMP_UBYTE ubGroupNum, MMP_ULONG *ulGroupFreq)
{
//	*ulGroupFreq = glGroupFreq[ubGroupNum];

	if(ubGroupNum==1)
		*ulGroupFreq = 0x2bf20;
	else if(ubGroupNum==2)
		*ulGroupFreq = 0x2ee0;		
	else
		*ulGroupFreq = 0x40740;
	
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_PLL_PowerUp
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPF_PLL_PowerUp(MMPF_PLL_ID m_pll_id, MMP_BOOL bPowerUpEn)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;
	MMP_USHORT      usWaitCycle = 50;
	
	#if 0//(CHIP == VSN_V2)
	if(bPowerUpEn) {
		pGBL->GBL_DPLL_PWR &= (~(DPLL_PWR_DOWN_EN << m_pll_id));
		MMPF_PLL_WaitCount(usWaitCycle);
	}
	else {
		pGBL->GBL_DPLL_PWR |= (DPLL_PWR_DOWN_EN << m_pll_id);
		MMPF_PLL_WaitCount(usWaitCycle);
	}
	#endif
	
	#if 1//(CHIP == VSN_V3)
	if(bPowerUpEn) {
		if((m_pll_id == MMPF_PLL_ID_0)||(m_pll_id == MMPF_PLL_ID_1)) {
			pGBL->GBL_DPLL01_PWR &= (~(DPLL_PWR_DOWN_EN << m_pll_id));
		}
		else { //DPLL2
			pGBL->GBL_DPLL2_PWR &= (~(DPLL_PWR_DOWN_EN));
		}
		MMPF_PLL_WaitCount(usWaitCycle);
	}
	else {
		if((m_pll_id == MMPF_PLL_ID_0)||(m_pll_id == MMPF_PLL_ID_1)) {
			pGBL->GBL_DPLL01_PWR |= (DPLL_PWR_DOWN_EN << m_pll_id);
		}
		else { //DPLL2
			pGBL->GBL_DPLL2_PWR |= (DPLL_PWR_DOWN_EN);
		}
		MMPF_PLL_WaitCount(usWaitCycle);
	}
	#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_PLL_SetVSNV2PLL
//  Description : 
//------------------------------------------------------------------------------
#if 0//(CHIP == VSN_V2)
MMP_ERR MMPF_PLL_SetVSNV2PLL(MMPF_PLL_MODE target_pll_mode, MMP_BOOL KeepG0) 
{
	MMP_BYTE i;
	MMP_USHORT	usWaitCycle = 50;
	MMPF_PLL_SRC	m_pll_src[2], m_pll_src_temp;
	AITPS_GBL pGBL = AITC_BASE_GBL;
	
	if (m_pll_mode == target_pll_mode) {
        RTNA_DBG_Str(0, "MMPF_PLL_Setting: Used original PLL setting.\r\n");
        return MMP_ERR_NONE;
    }

	/*================Note=================================
	Frequncy more than 156MHz should be assigned to DPLL0
	Frequncy less than 144MHz should be assigned to DPLL1
	=====================================================*/
	switch (target_pll_mode) {
	case MMPF_PLL_24CPU_24G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_24MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 24000;
        glGroupFreq[0] = 24000;
        glGroupFreq[1] = 24000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 24000;
        glGroupFreq[4] = 24000;
		break;
	case MMPF_PLL_48CPU_48G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_48MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 48000;
        glGroupFreq[0] = 48000;
        glGroupFreq[1] = 48000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 48000;
        glGroupFreq[4] = 48000;
		break;
	case MMPF_PLL_96CPU_96G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_96MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 96000;
        glGroupFreq[0] = 96000;
        glGroupFreq[1] = 96000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 96000;
        glGroupFreq[4] = 96000;
		break;
	case MMPF_PLL_120CPU_120G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_120MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 120000;
        glGroupFreq[0] = 120000;
        glGroupFreq[1] = 120000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 120000;
        glGroupFreq[4] = 120000;
		break;
	case MMPF_PLL_132CPU_132G0134_X:
		RTNA_DBG_Str(0, "set PLL 132 \r\n");
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_132MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 132000;
        glGroupFreq[0] = 132000;
        glGroupFreq[1] = 132000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 132000;
        glGroupFreq[4] = 132000;
		break;
	case MMPF_PLL_133CPU_133G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_133MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 133000;
        glGroupFreq[0] = 133000;
        glGroupFreq[1] = 133000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 133000;
        glGroupFreq[4] = 133000;
		break;
	case MMPF_PLL_144CPU_144G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_144MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 144000;
        glGroupFreq[0] = 144000;
        glGroupFreq[1] = 144000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 144000;
        glGroupFreq[4] = 144000;
		break;
	case MMPF_PLL_166CPU_166G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_166MHz_PLL0;
		m_pll_freq[1] = MMPF_PLL_FREQ_EXT_CLK;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_0;
        m_group_src[1] = MMPF_GUP_SRC_PLL_0;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_0;
        m_group_src[4] = MMPF_GUP_SRC_PLL_0;
        
        glCPUFreq = 166000;
        glGroupFreq[0] = 166000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 166000;
        glGroupFreq[4] = 166000;
		break;
	case MMPF_PLL_266CPU_144G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_266MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_144MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 266000;
        glGroupFreq[0] = 144000;
        glGroupFreq[1] = 144000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 144000;
        glGroupFreq[4] = 144000;
		break;
	case MMPF_PLL_300CPU_144G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_300MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_144MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 300000;
        glGroupFreq[0] = 144000;
        glGroupFreq[1] = 144000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 144000;
        glGroupFreq[4] = 144000;
    case MMPF_PLL_332CPU_192G034_166G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_332MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_0;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 332000;
        glGroupFreq[0] = 192000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 192000;
        glGroupFreq[4] = 192000;
		break;
	case MMPF_PLL_332CPU_200G034_166G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_332MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_200MHz_PLL1;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_0;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 332000;
        glGroupFreq[0] = 200000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 200000;
        glGroupFreq[4] = 200000;
		break;
	case MMPF_PLL_400CPU_144G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_400MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_144MHz;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 400000;
        glGroupFreq[0] = 144000;
        glGroupFreq[1] = 144000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 144000;
        glGroupFreq[4] = 144000;
		break;
	case MMPF_PLL_168CPU_133G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_168MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_133MHz;
		
		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 168000;
        glGroupFreq[0] = 133000;
        glGroupFreq[1] = 133000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 133000;
        glGroupFreq[4] = 133000;
		break;
	case MMPF_PLL_192CPU_133G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_192MHz_PLL0;
		m_pll_freq[1] = MMPF_PLL_FREQ_133MHz;
		
		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 192000;
        glGroupFreq[0] = 133000;
        glGroupFreq[1] = 133000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 133000;
        glGroupFreq[4] = 133000;
		break;
	case MMPF_PLL_400CPU_166G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_400MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_166MHz_PLL1;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 400000;
        glGroupFreq[0] = 166000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 166000;
        glGroupFreq[4] = 166000;
		break;
	case MMPF_PLL_498CPU_166G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_498MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_166MHz_PLL1;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 498000;
        glGroupFreq[0] = 166000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 166000;
        glGroupFreq[4] = 166000;
		break;
	case MMPF_PLL_600CPU_166G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_166MHz_PLL1;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 166000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 166000;
        glGroupFreq[4] = 166000;
		break;
	default:
		return MMP_ERR_NONE;
		break;
	}
	
	glMemFreq =  glGroupFreq[1];
	
	#if defined(ALL_FW)
	RTNA_DBG_Open(((glGroupFreq[0]/1000) >> 1), 115200);
	return MMP_ERR_NONE;
	#endif
	
	//Step 1: Switch to external clock (PMCLK), Bypass DPLL0 & DPLL1
	pGBL->GBL_SYS_CLK_CTL |= USB_CLK_SEL_EXT0;
	pGBL->GBL_CLK_PATH_CTL = (BYPASS_DPLL0 | BYPASS_DPLL1 | USB_CLK_SEL_EXT1);
	
	MMPF_PLL_WaitCount(usWaitCycle);
	
	//Step 2: Turn off some mudules' clock for de-glitch.
	//pGBL->GBL_CLK_DIS0 = (GBL_CLK_SCAL_DIS | GBL_CLK_JPG_DIS);  	//To enable all CLK
	
	//Step 3: Power-down all DPLL
	for(i = 0x0; i < (MMPF_PLL_ID_MAX - 1); i++) {
		MMPF_PLL_PowerUp((MMPF_PLL_ID)(MMPF_PLL_ID_0 + i), MMP_FALSE);
	}
	
	//Step 4: Adjust each PLL targe frequency and power-up
	if(m_pll_freq[0] != MMPF_PLL_FREQ_EXT_CLK) {
		pGBL->GBL_DPLL0_N = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[0]][1];
		pGBL->GBL_DPLL0_K = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[0]][2]; 
		//Update DPLL0 configuration
		pGBL->GBL_DPLL_CFG3 |= DPLL0_UPDATE_EN;
		m_pll_out_clk[0] = ((EXT_PMCLK_CKL)*pGBL->GBL_DPLL0_N)/(pGBL->GBL_DPLL0_K + 1);
		//RTNA_DBG_PrintLong(0, m_pll_out_clk[0]);
		MMPF_PLL_PowerUp((MMPF_PLL_ID)(MMPF_PLL_ID_0), MMP_TRUE); 	
	}
	
	if(m_pll_freq[1] != MMPF_PLL_FREQ_EXT_CLK) {
		MMP_UBYTE ubCfgDivid = 0x8;
		pGBL->GBL_DPLL1_M = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][0];
		pGBL->GBL_DPLL1_N = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][1];
		#if 0//(CHIP == VSN_V2)
		pGBL->GBL_DPLL1_CFG = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][2];
		ubCfgDivid = (ubCfgDivid >> pGBL->GBL_DPLL1_CFG);
		
		if(PLL_300_700_500MHZ == MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][3]) {
			MMP_ULONG ulPllOutClk = (((EXT_PMCLK_CKL/1000)*(pGBL->GBL_DPLL1_N+2))/pGBL->GBL_DPLL1_M);
			pGBL->GBL_DPLL1_CFG |= PLL_300_700_500MHZ;
			
			//Adjust Analog gain settings
			if(ulPllOutClk < 350) {
				pGBL->GBL_DPLL1_CFG |= PLL1_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
				pGBL->GBL_DPLL1_CFG &= (~PLL1_V2I_HIGH_GAIN_EN); //bit 7 set 0
			}
			else if(ulPllOutClk < 400) {
				pGBL->GBL_DPLL1_CFG |= PLL1_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
				pGBL->GBL_DPLL1_CFG |= PLL1_V2I_HIGH_GAIN_EN; //bit 7 set 1
			}
			else if(ulPllOutClk < 500) {
				pGBL->GBL_DPLL1_CFG &= (~PLL1_V2I_GAIN_ADJUST_DIS); //bit 16 set 0
				pGBL->GBL_DPLL1_CFG &= (~PLL1_V2I_HIGH_GAIN_EN); //bit 7 set 0
			}
			else {
				pGBL->GBL_DPLL1_CFG &= (~PLL1_V2I_GAIN_ADJUST_DIS); //bit 16 set 0
				pGBL->GBL_DPLL1_CFG |= PLL1_V2I_HIGH_GAIN_EN; //bit 7 set 1
			}
			
		}
		else {
			pGBL->GBL_DPLL1_CFG |= PLL1_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
			pGBL->GBL_DPLL1_CFG &= (~PLL1_V2I_HIGH_GAIN_EN); //bit 7 set 0
			RTNA_DBG_Str(0, "Warning: Bad PLL settings !\r\n");
		}
		#endif
		pGBL->GBL_DPLL_CFG3 |= DPLL1_UPDATE_EN;
		
		/*RTNA_DBG_PrintLong(0, pGBL->GBL_DPLL1_M);
		RTNA_DBG_PrintLong(0, (pGBL->GBL_DPLL1_N+2));
		RTNA_DBG_PrintLong(0, ubCfgDivid);*/
		m_pll_out_clk[1] = (((EXT_PMCLK_CKL)/pGBL->GBL_DPLL1_M)*(pGBL->GBL_DPLL1_N+2))/ubCfgDivid;
		//RTNA_DBG_PrintLong(0, m_pll_out_clk[1]); 
		MMPF_PLL_PowerUp((MMPF_PLL_ID)(MMPF_PLL_ID_1), MMP_TRUE);  	
	}
	
	
	MMPF_PLL_WaitCount(20000); //wait H/W switch time 5ms (WaitCount(12000) = 5ms) (WaitCount(20000) =9.4ms) 
	
	//Step 5: Setup each group's mux and diviver
	//Step 5-1: Group 0 (GBL_CLK)
	//pGBL->GBL_CLK_DIV = (pGBL->GBL_CLK_DIV & PLL_CLK_MUX); //Clean previous diver settings
	m_pll_src_temp = (m_group_src[0] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[0] != EXT_PMCLK_CKL) {	
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			pGBL->GBL_CLK_DIV = (m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN; //MUX PLL
			pGBL->GBL_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[0]) != 0x0) {
				RTNA_DBG_Str(0, "!!!!    Error PLL settings @group0\r\n");
				return 1;
			}
			ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[0]);
			
			if((ubPostDiv & 0x1) != 0x0) {
				RTNA_DBG_Str(0, "!!!!    Error PLL settings @group0\r\n");
				return 1;
			}
			if(ubPostDiv != 0x2) {
				ubPostDiv = (ubPostDiv >> 1); //GBL_CLK have postDivider (divede 2)
				//Note: We can not turn off the PLL_POST_DIV_EN bit in Group 0
				
				pGBL->GBL_CLK_DIV |= (ubPostDiv - 1);
			}
			//pGBL->GBL_CLK_DIV |= PLL_POST_DIV_EN;
		}
	}
	else {
		pGBL->GBL_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	
	//Step 5-2: Group 1 (DRAM_CLK)
	
	if(glGroupFreq[1] != glGroupFreq[0]) {//DRAM ASYNC mode.
		if(glGroupFreq[1] != EXT_PMCLK_CKL) { 
			pGBL->GBL_DRAM_CLK_DIV = (pGBL->GBL_DRAM_CLK_DIV & PLL_CLK_MUX); //Clean previous diver settings;
			m_pll_src_temp = (m_group_src[1] - MMPF_GUP_SRC_PLL_0);
			if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
				MMP_UBYTE ubPostDiv = 0x0;
				pGBL->GBL_DRAM_CLK_DIV = (m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN; //MUX PLL
				pGBL->GBL_DRAM_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
				if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[1]) != 0x0) {
					RTNA_DBG_Str(0, "!!!!    Error PLL settings @group1\r\n");
					return 1;
				}
				
				ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[1]);
				
				if((ubPostDiv & 0x1) != 0x0) {
					RTNA_DBG_Str(0, "!!!!    Error PLL settings @group1\r\n");
					return 1;
				}
				if(ubPostDiv != 0x2) {
					ubPostDiv = (ubPostDiv >> 1); //DRAM_CLK have postDivider (divede 2)
					pGBL->GBL_DRAM_CLK_DIV |= (ubPostDiv - 1);
				}
				
				//pGBL->GBL_DRAM_CLK_DIV |= PLL_POST_DIV_EN;
			}
		}
		else {
			pGBL->GBL_DRAM_CLK_DIV = PLL_CLK_DIV_MCLK;
		}
		pGBL->GBL_SYS_CLK_CTL |= DRAM_ASYNC_EN;	
	}
	else { //DRAM SYNC mode
		pGBL->GBL_SYS_CLK_CTL &= (~DRAM_ASYNC_EN);
	}
	//pGBL->GBL_DRAM_CLK_DIV |= PLL_POST_DIV_EN;
	//pGBL->GBL_SYS_CLK_CTL |= (DRAM_ASYNC_EN);
	
	//Step 5-3: Group 2 (USBPHY_CLK)
	pGBL->GBL_USBPHY_CLK_DIV = (pGBL->GBL_USBPHY_CLK_DIV & PLL_CLK_MUX); //Clean previous diver settings
	m_pll_src_temp = (m_group_src[2] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[2] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			pGBL->GBL_USBPHY_CLK_DIV = (m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN; //MUX PLL
			pGBL->GBL_USBPHY_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[2]) != 0x0) {
				RTNA_DBG_Str(0, "!!!!    Error PLL settings @group2\r\n");
				return 1;
			}
				
			ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[2]);
			pGBL->GBL_USBPHY_CLK_DIV |= (ubPostDiv - 1);
			
			//pGBL->GBL_USBPHY_CLK_DIV |= PLL_POST_DIV_EN;
		}
	}
	else {
		pGBL->GBL_USBPHY_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	//Step 5-4: Group 3 (RX_BIST_CLK)
	if(glGroupFreq[3] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			pGBL->GBL_MIPI_RX_CLK_SEL = (m_group_src[3] - MMPF_GUP_SRC_PLL_0); //MUX PLL
		}
	}
	else {
		pGBL->GBL_MIPI_RX_CLK_SEL = (PLL_CLK_DIV_MCLK >> PLL_SEL_PLL_OFFSET);
	}
	
	//Step 5-5: Group 4 (SENSOR_CLK)
	pGBL->GBL_VI_CLK_DIV = (pGBL->GBL_VI_CLK_DIV & PLL_CLK_MUX);
	m_pll_src_temp = (m_group_src[4] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[4] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			pGBL->GBL_VI_CLK_DIV = (m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN; //MUX PLL
			pGBL->GBL_VI_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[4]) != 0x0) {
				RTNA_DBG_Str(0, "!!!!    Error PLL settings @group4\r\n");
				return 1;
			}
				
			ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[4]);
			pGBL->GBL_VI_CLK_DIV |= (ubPostDiv - 1);
		}
		//pGBL->GBL_VI_CLK_DIV |= PLL_POST_DIV_EN;
	}
	else {
		pGBL->GBL_VI_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	//Step 5-6: CPU async
	//pGBL->GBL_CPU_CLK_DIV = (pGBL->GBL_CPU_CLK_DIV & PLL_CLK_MUX);
	m_pll_src_temp = (m_cpu_src - MMPF_GUP_SRC_PLL_0);
	if(glCPUFreq != glGroupFreq[0]) {
		if(glCPUFreq != EXT_PMCLK_CKL) {
			if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
				MMP_UBYTE ubPostDiv = 0x0;
				pGBL->GBL_CPU_CLK_DIV = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN); //MUX PLL
				pGBL->GBL_CPU_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
				if((m_pll_out_clk[m_pll_src_temp]%glCPUFreq) != 0x0) {
					RTNA_DBG_Str(0, "!!!!    Error PLL settings @CPU\r\n");
					return 1;
				}
				ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glCPUFreq);
				//Note: We can not turn off the PLL_POST_DIV_EN bit in CPU clk path
				pGBL->GBL_CPU_CLK_DIV |= (ubPostDiv - 1);
			}
			//pGBL->GBL_CPU_CLK_DIV |= PLL_POST_DIV_EN;
		}	
		else {
			pGBL->GBL_CPU_CLK_DIV = PLL_CLK_DIV_MCLK;
		}
		pGBL->GBL_CPU_CFG |= GBL_CPU_ASYNC_EN;
	}
	
	MMPF_PLL_WaitCount(usWaitCycle);
	
	//Step 6: Switch to defined PLL
	pGBL->GBL_SYS_CLK_CTL &= (~USB_CLK_SEL_EXT0);
	pGBL->GBL_CLK_PATH_CTL &= (~(BYPASS_DPLL0 | BYPASS_DPLL1 | USB_CLK_SEL_EXT1));
	
	m_pll_mode = target_pll_mode;
	RTNA_DBG_Open(((glGroupFreq[0]/1000) >> 1), 115200);
    MMPF_PLL_WaitCount(5000);
	RTNA_DBG_PrintLong(0, m_pll_out_clk[0]);
	RTNA_DBG_PrintLong(0, m_pll_out_clk[1]);
    RTNA_DBG_PrintLong(0, glCPUFreq);
	return MMP_ERR_NONE;
}
#endif //#if (CHIP == VSN_V2)

//------------------------------------------------------------------------------
//  Function    : MMPF_PLL_SetVSNV3PLL
//  Description : 
//------------------------------------------------------------------------------
#if 1//(CHIP == VSN_V3)
MMP_ERR MMPF_PLL_SetVSNV3PLL(MMPF_PLL_MODE target_pll_mode, MMP_BOOL KeepG0) 
{
	MMP_BYTE i;
	MMP_USHORT	usWaitCycle = 50;
	MMPF_PLL_SRC	m_pll_src[3], m_pll_src_temp;
	AITPS_GBL pGBL = AITC_BASE_GBL;
	
	if (m_pll_mode == target_pll_mode) {
//        RTNA_DBG_Str(0, "MMPF_PLL_Setting: Used original PLL setting.\r\n");
        return MMP_ERR_NONE;
    }

	/*================Note=================================
	Frequncy more than 156MHz should be assigned to DPLL0
	Frequncy less than 144MHz should be assigned to DPLL1
	=====================================================*/
	switch (target_pll_mode) {
	case MMPF_PLL_96CPU_96G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_96MHz;
		m_pll_freq[2] = MMPF_PLL_FREQ_EXT_CLK;
		
		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        m_group_src[5] = MMPF_GUP_SRC_PLL_1;
        m_group_src[6] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 96000;
        glGroupFreq[0] = 96000;
        glGroupFreq[1] = 96000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 96000;
        glGroupFreq[4] = 96000;
        glGroupFreq[5] = 96000;
        glGroupFreq[6] = 96000;
		break;
	case MMPF_PLL_144CPU_144G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_EXT_CLK;
		m_pll_freq[1] = MMPF_PLL_FREQ_144MHz;
		m_pll_freq[2] = MMPF_PLL_FREQ_EXT_CLK;
		
		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_1;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        m_group_src[5] = MMPF_GUP_SRC_PLL_1;
        m_group_src[6] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 144000;
        glGroupFreq[0] = 144000;
        glGroupFreq[1] = 144000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 144000;
        glGroupFreq[4] = 144000;
        glGroupFreq[5] = 144000;
        glGroupFreq[6] = 144000;
		break;
    case MMPF_PLL_332CPU_192G034_166G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_332MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_EXT_CLK;

		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_0;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = MMPF_GUP_SRC_PLL_1;
        m_group_src[4] = MMPF_GUP_SRC_PLL_1;
        m_group_src[5] = MMPF_GUP_SRC_PLL_1;
        m_group_src[6] = MMPF_GUP_SRC_PLL_1;
        
        glCPUFreq = 332000;
        glGroupFreq[0] = 192000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = 192000;
        glGroupFreq[4] = 192000;
        glGroupFreq[5] = 192000;
        glGroupFreq[6] = 192000;
		break;
    case MMPF_PLL_400CPU_166G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_400MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_216MHz_PLL1;
        m_pll_freq[2] = MMPF_PLL_FREQ_192MHz_PLL1;
        
		m_pll_src[0]   = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1]   = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2]   = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_2;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 400000;
        glGroupFreq[0] = 216000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_400CPU_192G034_200G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_400MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_EXT_CLK;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_0;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 400000;
        glGroupFreq[0] = 192000;
        glGroupFreq[1] = 200000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_400CPU_240G034_192G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_400MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_240MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_192MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_2;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 400000;
        glGroupFreq[0] = 240000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_498CPU_216G034_192G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_498MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_216MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_192MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_2;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 498000;
        glGroupFreq[0] = 216000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_498CPU_264G034_192G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_498MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_264MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_192MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_2;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 498000;
        glGroupFreq[0] = 264000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_498CPU_192G034_200G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_498MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_200MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_2;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 498000;
        glGroupFreq[0] = 192000;
        glGroupFreq[1] = 200000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_600CPU_192G034_166G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_166MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_2;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 192000;
        glGroupFreq[1] = 166000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_600CPU_192G0134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_EXT_CLK;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 192000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	case MMPF_PLL_600CPU_216G0_192G134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_216MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_2;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[1];
        m_group_src[4] = m_group_src[1];
        m_group_src[5] = m_group_src[1];
        m_group_src[6] = m_group_src[1];
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 216000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[1];
        glGroupFreq[4] = glGroupFreq[1];
        glGroupFreq[5] = glGroupFreq[1];
        glGroupFreq[6] = glGroupFreq[1];
		break;
	case MMPF_PLL_600CPU_240G0_192G134_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_240MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_2;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[1];
        m_group_src[4] = m_group_src[1];
        m_group_src[5] = m_group_src[1];
        m_group_src[6] = m_group_src[1];
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 240000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[1];
        glGroupFreq[4] = glGroupFreq[1];
        glGroupFreq[5] = glGroupFreq[1];
        glGroupFreq[6] = glGroupFreq[1];
		break;
	case MMPF_PLL_600CPU_264G0_192G134_X:
        #if 1
        m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_264MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_2;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[1];
        m_group_src[4] = m_group_src[1];
        m_group_src[5] = m_group_src[1];
        m_group_src[6] = m_group_src[1];
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 264000;
        glGroupFreq[1] = 192000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[1];
        glGroupFreq[4] = glGroupFreq[1];
		#if FxxxxN == 1
		glGroupFreq[4] = glGroupFreq[0];
		#endif
        glGroupFreq[5] = glGroupFreq[1];
        glGroupFreq[6] = glGroupFreq[1];
        #else
		m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_200MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_264MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_2;
        m_group_src[1] = MMPF_GUP_SRC_PLL_1;
        
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 264000;
        glGroupFreq[1] = 200000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
		#if FxxxxN == 1
		glGroupFreq[4] = glGroupFreq[0];
		#endif
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
        #endif
		break;
	case MMPF_PLL_600CPU_192G034_200G1_X:
		m_pll_freq[0] = MMPF_PLL_FREQ_600MHz;
		m_pll_freq[1] = MMPF_PLL_FREQ_192MHz_PLL1;
		m_pll_freq[2] = MMPF_PLL_FREQ_200MHz_PLL1;
		
		m_pll_src[0] = MMPF_PLL_SRC_PMCLK;	
        m_pll_src[1] = MMPF_PLL_SRC_PMCLK;
        m_pll_src[2] = MMPF_PLL_SRC_PMCLK;
        
        m_cpu_src = MMPF_GUP_SRC_PLL_0;
        m_group_src[0] = MMPF_GUP_SRC_PLL_1;
        m_group_src[1] = MMPF_GUP_SRC_PLL_2;
        m_group_src[2] = MMPF_GUP_SRC_PMCLK;
        m_group_src[3] = m_group_src[0];
        m_group_src[4] = m_group_src[0];
        m_group_src[5] = m_group_src[0];
        m_group_src[6] = m_group_src[0];
        
        glCPUFreq = 600000;
        glGroupFreq[0] = 192000;
        glGroupFreq[1] = 200000;
        glGroupFreq[2] = EXT_PMCLK_CKL;
        glGroupFreq[3] = glGroupFreq[0];
        glGroupFreq[4] = glGroupFreq[0];
        glGroupFreq[5] = glGroupFreq[0];
        glGroupFreq[6] = glGroupFreq[0];
		break;
	default:
		return MMP_ERR_NONE;
		break;
	}
	
	glMemFreq =  glGroupFreq[1];
	
	#if defined(ALL_FW)
	RTNA_DBG_Open(((glGroupFreq[0]/1000) >> 1), 115200);
	return MMP_ERR_NONE;
	#endif
	
	
	
	
	//Step 1: Switch to external clock (PMCLK), Bypass DPLL0 & DPLL1
	pGBL->GBL_SYS_CLK_CTL |= USB_CLK_SEL_EXT0;
	pGBL->GBL_CLK_PATH_CTL = (BYPASS_DPLL0 | BYPASS_DPLL1 | BYPASS_DPLL2 | USB_CLK_SEL_EXT1);
	
	MMPF_PLL_WaitCount(usWaitCycle);
	
	//Step 2: Turn off some mudules' clock for de-glitch.
	//pGBL->GBL_CLK_DIS0 = (GBL_CLK_SCAL_DIS | GBL_CLK_JPG_DIS);  	//To enable all CLK
	
	//Step 3: Power-down all DPLL
	for(i = 0x0; i < (MMPF_PLL_ID_MAX - 1); i++) {
		MMPF_PLL_PowerUp((MMPF_PLL_ID)(MMPF_PLL_ID_0 + i), MMP_FALSE);
	}
	
	//Step 4: Adjust each PLL targe frequency and power-up
	if(m_pll_freq[0] != MMPF_PLL_FREQ_EXT_CLK) {
		pGBL->GBL_DPLL0_N = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[0]][1];
		pGBL->GBL_DPLL0_K = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[0]][2]; 
		//Update DPLL0 configuration
		pGBL->GBL_DPLL_CFG3 |= DPLL0_UPDATE_EN;
		m_pll_out_clk[0] = ((EXT_PMCLK_CKL)*pGBL->GBL_DPLL0_N)/(pGBL->GBL_DPLL0_K + 1);
		//RTNA_DBG_PrintLong(0, m_pll_out_clk[0]);
		MMPF_PLL_PowerUp((MMPF_PLL_ID)(MMPF_PLL_ID_0), MMP_TRUE); 	
	}
	
	if(m_pll_freq[1] != MMPF_PLL_FREQ_EXT_CLK) {
		MMP_UBYTE ubCfgDivid = 0x8;
		pGBL->GBL_DPLL1_M = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][0];
		pGBL->GBL_DPLL1_N = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][1];
		pGBL->GBL_DPLL1_CFG01 = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][2];
		ubCfgDivid = (ubCfgDivid >> pGBL->GBL_DPLL1_CFG01);
		
		if(PLL_300_700_500MHZ == MMPF_PLL_TABLE_VSN_V2[m_pll_freq[1]][3]) {
			MMP_ULONG ulPllOutClk = (((EXT_PMCLK_CKL/1000)*(pGBL->GBL_DPLL1_N+2))/pGBL->GBL_DPLL1_M);
			pGBL->GBL_DPLL1_CFG01 |= PLL_300_700_500MHZ;
			
			//Adjust Analog gain settings
			if(ulPllOutClk < 350) {
				pGBL->GBL_DPLL1_CFG2 |= PLL_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
				pGBL->GBL_DPLL1_CFG01 &= (~PLL_V2I_HIGH_GAIN_EN); //bit 7 set 0
			}
			else if(ulPllOutClk < 400) {
				pGBL->GBL_DPLL1_CFG2 |= PLL_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
				pGBL->GBL_DPLL1_CFG01 |= PLL_V2I_HIGH_GAIN_EN; //bit 7 set 1
			}
			else if(ulPllOutClk < 500) {
				pGBL->GBL_DPLL1_CFG2 &= (~PLL_V2I_GAIN_ADJUST_DIS); //bit 16 set 0
				pGBL->GBL_DPLL1_CFG01 &= (~PLL_V2I_HIGH_GAIN_EN); //bit 7 set 0
			}
			else {
				pGBL->GBL_DPLL1_CFG2 &= (~PLL_V2I_GAIN_ADJUST_DIS); //bit 16 set 0
				pGBL->GBL_DPLL1_CFG01 |= PLL_V2I_HIGH_GAIN_EN; //bit 7 set 1
			}
			
		}
		else {
			pGBL->GBL_DPLL1_CFG2 |= PLL_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
			pGBL->GBL_DPLL1_CFG01 &= (~PLL_V2I_HIGH_GAIN_EN); //bit 7 set 0
			//RTNA_DBG_Str(0, "Warning: Bad PLL settings !\r\n");
		}
		pGBL->GBL_DPLL_CFG3 |= DPLL1_UPDATE_EN;
		
		/*RTNA_DBG_PrintLong(0, pGBL->GBL_DPLL1_M);
		RTNA_DBG_PrintLong(0, (pGBL->GBL_DPLL1_N+2));
		RTNA_DBG_PrintLong(0, ubCfgDivid);*/
		m_pll_out_clk[1] = (((EXT_PMCLK_CKL)/pGBL->GBL_DPLL1_M)*(pGBL->GBL_DPLL1_N+2))/ubCfgDivid;
		//RTNA_DBG_PrintLong(0, m_pll_out_clk[1]); 
		MMPF_PLL_PowerUp((MMPF_PLL_ID)(MMPF_PLL_ID_1), MMP_TRUE);  	
	}
	
	if(m_pll_freq[2] != MMPF_PLL_FREQ_EXT_CLK) {
		MMP_UBYTE ubCfgDivid = 0x8;
		pGBL->GBL_DPLL2_M = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[2]][0];
		pGBL->GBL_DPLL2_N = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[2]][1];
		pGBL->GBL_DPLL2_CFG2 = MMPF_PLL_TABLE_VSN_V2[m_pll_freq[2]][2];
		ubCfgDivid = (ubCfgDivid >> pGBL->GBL_DPLL2_CFG2);
		
		if(PLL_300_700_500MHZ == MMPF_PLL_TABLE_VSN_V2[m_pll_freq[2]][3]) {
			MMP_ULONG ulPllOutClk = (((EXT_PMCLK_CKL/1000)*(pGBL->GBL_DPLL2_N+2))/pGBL->GBL_DPLL2_M);
			pGBL->GBL_DPLL2_CFG2 |= PLL_300_700_500MHZ;
			
			//Adjust Analog gain settings
			if(ulPllOutClk < 350) {
				pGBL->GBL_DPLL2_CFG4 |= PLL_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
				pGBL->GBL_DPLL2_CFG2 &= (~PLL_V2I_HIGH_GAIN_EN); //bit 7 set 0
			}
			else if(ulPllOutClk < 400) {
				pGBL->GBL_DPLL2_CFG4 |= PLL_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
				pGBL->GBL_DPLL2_CFG2 |= PLL_V2I_HIGH_GAIN_EN; //bit 7 set 1
			}
			else if(ulPllOutClk < 500) {
				pGBL->GBL_DPLL2_CFG4 &= (~PLL_V2I_GAIN_ADJUST_DIS); //bit 16 set 0
				pGBL->GBL_DPLL2_CFG2 &= (~PLL_V2I_HIGH_GAIN_EN); //bit 7 set 0
			}
			else {
				pGBL->GBL_DPLL2_CFG4 &= (~PLL_V2I_GAIN_ADJUST_DIS); //bit 16 set 0
				pGBL->GBL_DPLL2_CFG2 |= PLL_V2I_HIGH_GAIN_EN; //bit 7 set 1
			}
			
		}
		else {
			pGBL->GBL_DPLL2_CFG4 |= PLL_V2I_GAIN_ADJUST_DIS; //bit 16 set 1
			pGBL->GBL_DPLL2_CFG2 &= (~PLL_V2I_HIGH_GAIN_EN); //bit 7 set 0
			//RTNA_DBG_Str(0, "Warning: Bad PLL settings !\r\n");
		}
		pGBL->GBL_DPLL_CFG3 |= DPLL2_UPDATE_EN;
		
		/*RTNA_DBG_PrintLong(0, pGBL->GBL_DPLL2_M);
		RTNA_DBG_PrintLong(0, (pGBL->GBL_DPLL2_N+2));
		RTNA_DBG_PrintLong(0, ubCfgDivid);*/
		m_pll_out_clk[2] = (((EXT_PMCLK_CKL)/pGBL->GBL_DPLL2_M)*(pGBL->GBL_DPLL2_N+2))/ubCfgDivid;
		//RTNA_DBG_PrintLong(0, m_pll_out_clk[2]); 
		MMPF_PLL_PowerUp((MMPF_PLL_ID)(MMPF_PLL_ID_2), MMP_TRUE);  	
	}
	
	
	MMPF_PLL_WaitCount(20000); //wait H/W switch time 5ms (WaitCount(12000) = 5ms) (WaitCount(20000) =9.4ms) 
	
	//Step 5: Setup each group's mux and diviver
	//Step 5-1: Group 0 (GBL_CLK)
	//pGBL->GBL_CLK_DIV = (pGBL->GBL_CLK_DIV & PLL_CLK_MUX); //Clean previous diver settings
	m_pll_src_temp = (m_group_src[0] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[0] != EXT_PMCLK_CKL) {	
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			
			if(pGBL->GBL_CLK_SEL & GBL_CLK_SEL_MUX0) { //Use 0x6905 bit 6~7 as source selection
				pGBL->GBL_CLK_DIV = (m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN; //MUX PLL
				pGBL->GBL_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			}
			else {
				pGBL->GBL_CLK_SEL = (m_pll_src_temp << PLL_SEL_PLL_OFFSET1);
				pGBL->GBL_CLK_DIV |= PLL_POST_DIV_EN;
				pGBL->GBL_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			}
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[0]) != 0x0) {
				//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group0\r\n");
				return 1;
			}
			ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[0]);
			
			if((ubPostDiv & 0x1) != 0x0) {
				//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group0\r\n");
				return 1;
			}
			if(ubPostDiv != 0x2) {
				ubPostDiv = (ubPostDiv >> 1); //GBL_CLK have postDivider (divede 2)
				//Note: We can not turn off the PLL_POST_DIV_EN bit in Group 0
				
				pGBL->GBL_CLK_DIV |= (ubPostDiv - 1);
			}
			//pGBL->GBL_CLK_DIV |= PLL_POST_DIV_EN;
		}
	}
	else {
		pGBL->GBL_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	
	//Step 5-2: Group 1 (DRAM_CLK)
	m_pll_src_temp = (m_group_src[1] - MMPF_GUP_SRC_PLL_0);
	if((glGroupFreq[1] != glGroupFreq[0])) {//DRAM ASYNC mode.
		if(glGroupFreq[1] != EXT_PMCLK_CKL) { 
			pGBL->GBL_DRAM_CLK_DIV = (pGBL->GBL_DRAM_CLK_DIV & PLL_CLK_MUX); //Clean previous diver settings;
			m_pll_src_temp = (m_group_src[1] - MMPF_GUP_SRC_PLL_0);
			if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
				MMP_UBYTE ubPostDiv = 0x0;
				pGBL->GBL_DRAM_CLK_DIV = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN) ; //MUX PLL
				pGBL->GBL_DRAM_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
				if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[1]) != 0x0) {
					//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group1\r\n");
					return 1;
				}
				
				ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[1]);
				
				if((ubPostDiv & 0x1) != 0x0) {
					//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group1\r\n");
					return 1;
				}
				if(ubPostDiv != 0x2) {
					ubPostDiv = (ubPostDiv >> 1); //DRAM_CLK have postDivider (divede 2)
					pGBL->GBL_DRAM_CLK_DIV |= (ubPostDiv - 1);
				}
				
				pGBL->GBL_DRAM_CLK_DIV |= PLL_POST_DIV_EN;
			}
		}
		else {
			pGBL->GBL_DRAM_CLK_DIV = PLL_CLK_DIV_MCLK;
		}
		pGBL->GBL_SYS_CLK_CTL |= DRAM_ASYNC_EN;	
	}
	else { //DRAM SYNC mode
		pGBL->GBL_SYS_CLK_CTL &= (~DRAM_ASYNC_EN);
	}
	//pGBL->GBL_DRAM_CLK_DIV |= PLL_POST_DIV_EN;
	//pGBL->GBL_SYS_CLK_CTL |= (DRAM_ASYNC_EN);
	
	//Step 5-3: Group 2 (USBPHY_CLK)
	pGBL->GBL_USBPHY_CLK_DIV = (pGBL->GBL_USBPHY_CLK_DIV & PLL_CLK_MUX); //Clean previous diver settings
	m_pll_src_temp = (m_group_src[2] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[2] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			pGBL->GBL_USBPHY_CLK_DIV = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET)|PLL_POST_DIV_EN) ; //MUX PLL
			pGBL->GBL_USBPHY_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[2]) != 0x0) {
				//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group2\r\n");
				return 1;
			}
				
			ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[2]);
			pGBL->GBL_USBPHY_CLK_DIV = (ubPostDiv - 1);
			
			pGBL->GBL_USBPHY_CLK_DIV |= PLL_POST_DIV_EN;
		}
	}
	else {
		pGBL->GBL_USBPHY_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	//Step 5-4: Group 3 (RX_BIST_CLK)
	if(glGroupFreq[3] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			pGBL->GBL_MIPI_RX_CLK_SEL = (m_group_src[3] - MMPF_GUP_SRC_PLL_0); //MUX PLL
		}
	}
	else {
		pGBL->GBL_MIPI_RX_CLK_SEL = (PLL_CLK_DIV_MCLK >> PLL_SEL_PLL_OFFSET);
	}
	
	//Step 5-5: Group 4 (SENSOR_CLK)
	pGBL->GBL_VI_CLK_DIV = (pGBL->GBL_VI_CLK_DIV & PLL_CLK_MUX);
	m_pll_src_temp = (m_group_src[4] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[4] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			pGBL->GBL_VI_CLK_DIV = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN); //MUX PLL
			pGBL->GBL_VI_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[4]) != 0x0) {
				//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group4\r\n");
				return 1;
			}
				
			ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[4]);
			pGBL->GBL_VI_CLK_DIV |= (ubPostDiv - 1);
		}
		pGBL->GBL_VI_CLK_DIV |= PLL_POST_DIV_EN;
	}
	else {
		pGBL->GBL_VI_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	//Step 5-6: Group 5 (AUDIO_CLK)
	pGBL->GBL_AUDIO_CLK_DIV = (pGBL->GBL_AUDIO_CLK_DIV & PLL_CLK_MUX);
	m_pll_src_temp = (m_group_src[5] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[5] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			pGBL->GBL_AUDIO_CLK_DIV = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN) ; //MUX PLL
			pGBL->GBL_AUDIO_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[5]) != 0x0) {
				//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group5\r\n");
				return 1;
			}
				
			ubPostDiv = 4;//(m_pll_out_clk[m_pll_src_temp]/glGroupFreq[5]);
			pGBL->GBL_AUDIO_CLK_DIV |= (ubPostDiv - 1);
		}
		pGBL->GBL_AUDIO_CLK_DIV |= PLL_POST_DIV_EN;
	}
	else {
		pGBL->GBL_AUDIO_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	
	//Step 5-7: Group 6 (COLOR_CLK(ISP))
	pGBL->GBL_COLOR_CLK_DIV = (pGBL->GBL_COLOR_CLK_DIV & PLL_CLK_MUX);
	m_pll_src_temp = (m_group_src[6] - MMPF_GUP_SRC_PLL_0);
	if(glGroupFreq[6] != EXT_PMCLK_CKL) {
		if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
			MMP_UBYTE ubPostDiv = 0x0;
			pGBL->GBL_COLOR_CLK_DIV = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN) ; //MUX PLL
			pGBL->GBL_COLOR_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
			if((m_pll_out_clk[m_pll_src_temp]%glGroupFreq[6]) != 0x0) {
				//RTNA_DBG_Str(0, "!!!!    Error PLL settings @group6\r\n");
				return 1;
			}
				
			ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glGroupFreq[6]);
			pGBL->GBL_COLOR_CLK_DIV |= (ubPostDiv - 1);
		}
		pGBL->GBL_COLOR_CLK_DIV |= PLL_POST_DIV_EN;
	}
	else {
		pGBL->GBL_COLOR_CLK_DIV = PLL_CLK_DIV_MCLK;
	}
	
	//Step 5-8: CPU async
	//pGBL->GBL_CPU_CLK_DIV = (pGBL->GBL_CPU_CLK_DIV & PLL_CLK_MUX);
	m_pll_src_temp = (m_cpu_src - MMPF_GUP_SRC_PLL_0);
	if(glCPUFreq != glGroupFreq[0]) {
		if(glCPUFreq != EXT_PMCLK_CKL) {
			if(m_pll_src_temp < MAX_DPLL_SRC) { //Use one of available PLL
				MMP_UBYTE ubPostDiv = 0x0;
				
				if(pGBL->GBL_CPU_CLK_SEL & CPU_CLK_SEL_MUX0) {
					pGBL->GBL_CPU_CLK_DIV = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET) | PLL_POST_DIV_EN); //MUX PLL
					pGBL->GBL_CPU_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
				}
				else {
					pGBL->GBL_CPU_CLK_SEL = ((m_pll_src_temp << PLL_SEL_PLL_OFFSET1) | PLL_POST_DIV_EN);
					pGBL->GBL_CPU_CLK_DIV |= PLL_POST_DIV_EN;
					pGBL->GBL_CPU_CLK_DIV &= (~PLL_DIVIDE_MASK); //Clean divider factor
				}
				if((m_pll_out_clk[m_pll_src_temp]%glCPUFreq) != 0x0) {
					//RTNA_DBG_Str(0, "!!!!    Error PLL settings @CPU\r\n");
					return 1;
				}
				ubPostDiv = (m_pll_out_clk[m_pll_src_temp]/glCPUFreq);
				//Note: We can not turn off the PLL_POST_DIV_EN bit in CPU clk path
				pGBL->GBL_CPU_CLK_DIV |= (ubPostDiv - 1);
			}
			//pGBL->GBL_CPU_CLK_DIV |= PLL_POST_DIV_EN;
		}	
		else {
			pGBL->GBL_CPU_CLK_DIV = PLL_CLK_DIV_MCLK;
		}
		pGBL->GBL_CPU_CFG |= GBL_CPU_ASYNC_EN;
	}
	
	MMPF_PLL_WaitCount(usWaitCycle);
	
	//Step 6: Switch to defined PLL
	pGBL->GBL_SYS_CLK_CTL &= (~USB_CLK_SEL_EXT0);
	pGBL->GBL_CLK_PATH_CTL &= (~(BYPASS_DPLL0 | BYPASS_DPLL1 | BYPASS_DPLL2 | USB_CLK_SEL_EXT1));
	
	m_pll_mode = target_pll_mode;
//	RTNA_DBG_Open(((glGroupFreq[0]/1000) >> 1), 115200);
    MMPF_PLL_WaitCount(5000);
//	RTNA_DBG_PrintLong(0, m_pll_out_clk[0]);
//	RTNA_DBG_PrintLong(0, m_pll_out_clk[1]);
//	RTNA_DBG_PrintLong(0, m_pll_out_clk[2]);
//    RTNA_DBG_PrintLong(0, glCPUFreq);
	return MMP_ERR_NONE;
}
#endif //#if (CHIP == VSN_V3)
