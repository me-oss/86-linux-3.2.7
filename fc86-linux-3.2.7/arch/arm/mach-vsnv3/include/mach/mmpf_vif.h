//==============================================================================
//
//  File        : mmpf_vif.h
//  Description : INCLUDE File for the Firmware VIF Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMPF_VIF_H_
#define _MMPF_VIF_H_

#include    "includes_fw.h"


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

typedef enum {
    MMPF_VIF_PARALLEL,
    MMPF_VIF_MIPI
}MMPF_VIF_IN_TYPE;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef enum _MMPF_VIF_MDL_ID {
    MMPF_VIF_MDL_0 = 0,
    MMPF_VIF_MDL_1,
    MMPF_VIF_MDL_MAX
} MMPF_VIF_MDL_ID;

typedef struct _MMPF_VIF_GRAB_INFO {
    MMP_USHORT  usStartX;
    MMP_USHORT  usStartY;
    MMP_USHORT  usGrabWidth;
    MMP_USHORT  usGrabHeight;
} MMPF_VIF_GRAB_INFO;

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR	MMPF_VIF_SetPIODir(MMP_UBYTE mask, MMP_BOOL bOutput);
MMP_ERR	MMPF_VIF_SetPIOOutput(MMP_UBYTE mask, MMP_BOOL bSetHigh);
MMP_BOOL	MMPF_VIF_GetPIOOutput(MMP_UBYTE mask);
MMP_ERR MMPF_VIF_EnableInputInterface(MMP_BOOL enable);
MMP_ERR	MMPF_VIF_RegisterInputInterface(MMPF_VIF_IN_TYPE type);
MMP_ERR MMPF_VIF_EnableOutput(MMP_BOOL enable);
MMP_ERR MMPF_VIF_IsInterfaceEnable(MMP_BOOL *bEnable);
MMP_ERR MMPF_VIF_SetGrabRange(MMP_UBYTE ubVid, MMPF_VIF_GRAB_INFO *pGrab);
MMP_ERR MMPF_VIF_GetVIFGrabResolution(MMP_ULONG *ulWidth, MMP_ULONG *ulHeight);

//==============================================================================
//
//                              MACRO FUNCTIONS
//
//==============================================================================


#endif // _MMPD_VIF_H_
