//==============================================================================
//
//  File        : mmpf_vif.c
//  Description : MMPF_VIF functions
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#include <linux/module.h>
#include <mach/mmpf_typedef.h>
#include <mach/mmp_err.h>
#include <mach/reg_retina.h>
#include <mach/mmp_reg_vif.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmpf_vif.h>
#define RTNA_DBG_Str0(x) printk(x)
/** @addtogroup MMPF_VIF
@{
*/
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
MMPF_VIF_IN_TYPE m_VIFInterface[MMPF_VIF_MDL_MAX] = {MMPF_VIF_PARALLEL, MMPF_VIF_PARALLEL};
//==============================================================================
//
//                          MODULE VARIABLES
//
//==============================================================================
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
//* Function Name       : MMPF_VIF_SetPIOOutput
//* Input Parameters    : <mask>   = bit mask identifying the PIOs
//*                     : <config> = mask identifying the PIOs configuration
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
MMP_ERR	MMPF_VIF_SetPIODir(MMP_UBYTE mask, MMP_BOOL bOutput)
{
    AITPS_VIF   pVIF    = AITC_BASE_VIF;
    MMP_UBYTE   vid = 0;


    if (bOutput == MMP_TRUE) {
        if (vid == 0) {
            pVIF->VIF_0_SENSR_SIF_EN |= mask;
        }
        else {
            pVIF->VIF_1_SENSR_SIF_EN |= mask;
        }
    }
    else {
        if (vid == 0) {
            pVIF->VIF_0_SENSR_SIF_EN &= ~mask;
        }
        else {
            pVIF->VIF_1_SENSR_SIF_EN &= ~mask;
        }
    }


	return MMP_ERR_NONE;
}
//*----------------------------------------------------------------------------
//* Function Name       : MMPF_VIF_SetPIOOutput
//* Input Parameters    : <mask>   = bit mask identifying the PIOs
//*                     : <config> = mask identifying the PIOs configuration
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
MMP_ERR	MMPF_VIF_SetPIOOutput(MMP_UBYTE mask, MMP_BOOL bSetHigh)
{
    AITPS_VIF   pVIF    = AITC_BASE_VIF;
    MMP_UBYTE   vid = 0;

    if (bSetHigh == MMP_TRUE) {
        if (vid == 0) {
            pVIF->VIF_0_SENSR_SIF_DATA |= mask;
        }
        else {
            pVIF->VIF_1_SENSR_SIF_DATA |= mask;
        }
    }
    else {
        if (vid == 0) {
            pVIF->VIF_0_SENSR_SIF_DATA &= ~mask;
        }
        else {
            pVIF->VIF_1_SENSR_SIF_DATA &= ~mask;
        }
    }

	return MMP_ERR_NONE;
}
//*----------------------------------------------------------------------------
//* Function Name       : MMPF_VIF_GetPIOOutput
//* Input Parameters    : <mask>   = bit mask identifying the PIOs
//*                     : <config> = mask identifying the PIOs configuration
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
MMP_BOOL	MMPF_VIF_GetPIOOutput(MMP_UBYTE mask)
{
    AITPS_VIF   pVIF    = AITC_BASE_VIF;
    MMP_UBYTE   vid = 0;

	if (vid == 0) {
	    return (pVIF->VIF_0_SENSR_SIF_DATA & mask)? MMP_TRUE: MMP_FALSE;
	}
	else {
	    return (pVIF->VIF_1_SENSR_SIF_DATA & mask)? MMP_TRUE: MMP_FALSE;
	}
}


MMP_ERR	MMPF_VIF_RegisterInputInterface(MMPF_VIF_IN_TYPE type)
{
    MMP_UBYTE   vid = 0;

    if(type == MMPF_VIF_MIPI){
//        RTNA_DBG_Str0("Set MIPI mode\r\n");
    }
    else{
//        RTNA_DBG_Str0("Set CCIR mode\r\n");
    }

    m_VIFInterface[vid] = type;

    return MMP_ERR_NONE;
}

MMP_ERR MMPF_VIF_IsInterfaceEnable(MMP_BOOL *bEnable)
{
    AITPS_VIF   pVIF = AITC_BASE_VIF;
    AITPS_MIPI  pMIPI = AITC_BASE_MIPI;
    MMP_UBYTE   vid = 0;

    *bEnable = MMP_FALSE;

    if (m_VIFInterface[vid] == MMPF_VIF_MIPI) {
        if (pMIPI->MIPI_CLK_CFG[vid] & MIPI_CSI2_EN) {
            *bEnable = MMP_TRUE;
        }
    }
    else {
        if (pVIF->VIF_IN_EN[vid]) {
            *bEnable = MMP_TRUE;
        }
    }

    return MMP_ERR_NONE;
}

MMP_ERR MMPF_VIF_EnableInputInterface(MMP_BOOL enable)
{
    AITPS_VIF  pVIF = AITC_BASE_VIF;
    AITPS_MIPI  pMIPI = AITC_BASE_MIPI;
    MMP_UBYTE   vid = 0;
    
    if (enable == MMP_TRUE) {
        if(m_VIFInterface[vid] == MMPF_VIF_MIPI){
            pVIF->VIF_OUT_EN[vid] = 1;
            pMIPI->MIPI_CLK_CFG[vid] |= MIPI_CSI2_EN;
        }
        else
        {        
            pVIF->VIF_OUT_EN[vid] = 0;
            pVIF->VIF_IN_EN[vid] = 1;
            pVIF->VIF_OUT_EN[vid] = 1;
    	}
//        RTNA_DBG_Str3("MMPF_VIF_EnableInputInterface : TRUE\r\n");
    }
    else {
        if (m_VIFInterface[vid] == MMPF_VIF_MIPI) {
 			pVIF->VIF_OUT_EN[vid] = 0;
            pMIPI->MIPI_CLK_CFG[vid] &= ~MIPI_CSI2_EN;
        }
        else {
            pVIF->VIF_IN_EN[vid] = 0;
        }    
        //RTNA_DBG_Str3("MMPF_VIF_EnableInputInterface : False\r\n");
    }

	return MMP_ERR_NONE;
}

MMP_ERR MMPF_VIF_EnableOutput(MMP_BOOL enable)
{
    AITPS_VIF  pVIF = AITC_BASE_VIF;
    MMP_UBYTE   vid = 0;

    if(enable == MMP_TRUE){
        pVIF->VIF_OUT_EN[vid] = 1;
    }
    else{
        pVIF->VIF_OUT_EN[vid] = 0;
    }

	return MMP_ERR_NONE;
}

MMP_ERR MMPF_VIF_SetGrabRange(MMP_UBYTE ubVid, MMPF_VIF_GRAB_INFO *pGrab)
{
    AITPS_VIF  pVIF = AITC_BASE_VIF;

    pVIF->VIF_GRAB[ubVid].PIXL_ST = pGrab->usStartX;
    pVIF->VIF_GRAB[ubVid].PIXL_ED = pGrab->usStartX + pGrab->usGrabWidth - 1;
    pVIF->VIF_GRAB[ubVid].LINE_ST = pGrab->usStartY;
    pVIF->VIF_GRAB[ubVid].LINE_ED = pGrab->usStartY + pGrab->usGrabHeight - 1;

    return MMP_ERR_NONE;
}

MMP_ERR MMPF_VIF_GetVIFGrabResolution(MMP_ULONG *ulWidth, MMP_ULONG *ulHeight)
{
    AITPS_VIF  pVIF = AITC_BASE_VIF;
    MMP_UBYTE   vid = 0;

    *ulWidth = (pVIF->VIF_GRAB[vid].PIXL_ED - pVIF->VIF_GRAB[vid].PIXL_ST + 1);
    *ulHeight = (pVIF->VIF_GRAB[vid].LINE_ED - pVIF->VIF_GRAB[vid].LINE_ST + 1);

	return MMP_ERR_NONE;
}
/** @}*/ //end of MMPF_I2CM
