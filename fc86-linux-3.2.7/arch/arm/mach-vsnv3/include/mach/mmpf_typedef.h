//==============================================================================
//
//  File        : mmpf_typedef.h
//  Description : Type define file for A-I-T MMPF source code
//  Author      : Philip Lin
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_TYPEDEF_H_
#define _MMPF_TYPEDEF_H_

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef unsigned char   MMP_BOOL;
typedef char            MMP_BYTE;
typedef short           MMP_SHORT;
typedef int             MMP_LONG;
typedef unsigned char   MMP_UBYTE;
typedef unsigned short  MMP_USHORT;
typedef unsigned int    MMP_ULONG;
typedef unsigned long   MMP_U_LONG;
typedef unsigned long long  MMP_ULONG64;
typedef long long       MMP_LONG64;
typedef volatile unsigned char  AIT_REG_B;
typedef volatile unsigned short AIT_REG_W;
typedef volatile unsigned int   AIT_REG_D;


typedef void*           MMP_HANDLE;

#define MMP_TRUE        (1)
#define MMP_FALSE       (0)
#endif /* _MMPF_TYPEDEF_H_ */
