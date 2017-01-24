#ifndef _MMPF_PIO_H_
#define _MMPF_PIO_H_

//#include "os_wrap.h"
#include <mach/mmp_err.h>
#include <mach/mmpf_typedef.h>
#include <mach/mmp_reg_gpio.h>


//#include <mach/includes_fw.h>


//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#if 0//(CHIP == VSN_V2)
//#define  PIO_MAX_PIN_SIZE 48
#endif
#if 1//(CHIP == VSN_V3)
//#define  PIO_MAX_PIN_SIZE 66
#endif 
#define  PIO_BITPOSITION_INFO 0x1f
#define  PIO_GET_INDEX(x) (x>>5)
#define  PIO_SEM_TIMEOUT 0x100000
#define  PIO_HIGH	0x1
#define  PIO_LOW	0x0

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef void PioCallBackFunc(MMP_ULONG);

typedef enum _MMPF_PIO_REG {
	//===========================GPIO
	MMPF_PIO_REG_GPIO0 	= 	0x0,
	MMPF_PIO_REG_GPIO1	= 	0x1,
	MMPF_PIO_REG_GPIO2	= 	0x2,
	MMPF_PIO_REG_GPIO3	= 	0x3,
	MMPF_PIO_REG_GPIO4	= 	0x4,
	MMPF_PIO_REG_GPIO5	= 	0x5,
	MMPF_PIO_REG_GPIO6	= 	0x6,
	MMPF_PIO_REG_GPIO7	= 	0x7,
	MMPF_PIO_REG_GPIO8	= 	0x8,
	MMPF_PIO_REG_GPIO9	= 	0x9,
	MMPF_PIO_REG_GPIO10 = 	0xa,
	MMPF_PIO_REG_GPIO11	= 	0xb,
	MMPF_PIO_REG_GPIO12	= 	0xc,
	MMPF_PIO_REG_GPIO13	= 	0xd,
	MMPF_PIO_REG_GPIO14	= 	0xe,
	MMPF_PIO_REG_GPIO15	= 	0xf,
	MMPF_PIO_REG_GPIO16	= 	0x10,
	MMPF_PIO_REG_GPIO17	= 	0x11,
	MMPF_PIO_REG_GPIO18	= 	0x12,
	MMPF_PIO_REG_GPIO19	= 	0x13,
	MMPF_PIO_REG_GPIO20 = 	0x14,
	MMPF_PIO_REG_GPIO21	= 	0x15,
	MMPF_PIO_REG_GPIO22	= 	0x16,
	MMPF_PIO_REG_GPIO23	= 	0x17,
	MMPF_PIO_REG_GPIO24	= 	0x18,
	MMPF_PIO_REG_GPIO25	= 	0x19,
	MMPF_PIO_REG_GPIO26	= 	0x1a,
	MMPF_PIO_REG_GPIO27	= 	0x1b,
	MMPF_PIO_REG_GPIO28	= 	0x1c,
	MMPF_PIO_REG_GPIO29	= 	0x1d,
	MMPF_PIO_REG_GPIO30 = 	0x1e,
	MMPF_PIO_REG_GPIO31	= 	0x1f,
	MMPF_PIO_REG_GPIO32	= 	0x20,
	MMPF_PIO_REG_GPIO33	= 	0x21,
	MMPF_PIO_REG_GPIO34	= 	0x22,
	MMPF_PIO_REG_GPIO35	= 	0x23,
	MMPF_PIO_REG_GPIO36	= 	0x24,
	MMPF_PIO_REG_GPIO37	= 	0x25,
	MMPF_PIO_REG_GPIO38	= 	0x26,
	MMPF_PIO_REG_GPIO39	= 	0x27,
	MMPF_PIO_REG_GPIO40 = 	0x28,
	MMPF_PIO_REG_GPIO41	= 	0x29,
	MMPF_PIO_REG_GPIO42	= 	0x2a,
	MMPF_PIO_REG_GPIO43	= 	0x2b,
	MMPF_PIO_REG_GPIO44	= 	0x2c,
	MMPF_PIO_REG_GPIO45	= 	0x2d,
	MMPF_PIO_REG_GPIO46	= 	0x2e,
	MMPF_PIO_REG_GPIO47	= 	0x2f,
	#if 1//(CHIP == VSN_V3)
	MMPF_PIO_REG_GPIO48 = 	0x30,
	MMPF_PIO_REG_GPIO49	= 	0x31,
	MMPF_PIO_REG_GPIO50	= 	0x32,
	MMPF_PIO_REG_GPIO51	= 	0x33,
	MMPF_PIO_REG_GPIO52	= 	0x34,
	MMPF_PIO_REG_GPIO53	= 	0x35,
	MMPF_PIO_REG_GPIO54	= 	0x36,
	MMPF_PIO_REG_GPIO55	= 	0x37,
	MMPF_PIO_REG_GPIO56	= 	0x38,
	MMPF_PIO_REG_GPIO57	= 	0x39,
	MMPF_PIO_REG_GPIO58 = 	0x3a,
	MMPF_PIO_REG_GPIO59	= 	0x3b,
	MMPF_PIO_REG_GPIO60	= 	0x3c,
	MMPF_PIO_REG_GPIO61	= 	0x3d,
	MMPF_PIO_REG_GPIO62	= 	0x3e,
	MMPF_PIO_REG_GPIO63	= 	0x3f,
	MMPF_PIO_REG_GPIO64	= 	0x40,
	MMPF_PIO_REG_GPIO65	= 	0x41,
	#endif
	PIO_MAX_PIN_SIZE,
	MMPF_PIO_REG_UNKNOWN = 0xFFF
} MMPF_PIO_REG;


typedef enum _MMPF_PIO_TRIGMODE {
		MMPF_PIO_TRIGMODE_EDGE_H2L		=	0x0,
		MMPF_PIO_TRIGMODE_EDGE_L2H		=	0x1,
		MMPF_PIO_TRIGMODE_LEVEL_H		=	0x2,
		MMPF_PIO_TRIGMODE_LEVEL_L		=	0x3,
		MMPF_PIO_TRIGMODE_UNKNOWN		=	0xFF
} MMPF_PIO_TRIGMODE;


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR MMPF_PIO_Initialize(void);
MMP_ERR MMPF_PIO_EnableOutputMode(MMPF_PIO_REG piopin, MMP_BOOL bEnable);
MMP_ERR MMPF_PIO_GetOutputMode(MMPF_PIO_REG piopin, MMP_UBYTE* returnValue);
MMP_ERR MMPF_PIO_SetData(MMPF_PIO_REG piopin, MMP_UBYTE outputValue);
MMP_ERR MMPF_PIO_GetData(MMPF_PIO_REG piopin, MMP_UBYTE *returnValue);
MMP_ERR MMPF_PIO_EnableTrigMode(MMPF_PIO_REG piopin, MMPF_PIO_TRIGMODE trigmode, MMP_BOOL bEnable);
//MMP_ERR MMPF_PIO_GetInterruptStatus(MMPF_PIO_REG piopin, MMP_UBYTE *returnStatus);
//MMP_ERR MMPF_PIO_CleanInterruptStatus(MMPF_PIO_REG piopin);
MMP_ERR MMPF_PIO_EnableInterrupt(MMPF_PIO_REG piopin, MMP_BOOL bEnable, MMP_ULONG boundingTime, PioCallBackFunc *CallBackFunc);
MMP_ERR MMPF_PIO_EnableGpioMode(MMPF_PIO_REG piopin, MMP_BOOL bEnable);
//void MMPF_PIO_ISR(void);
//void MMPF_PIO_ISR(unsigned int irq,struct irq_desc *desc);



#endif
