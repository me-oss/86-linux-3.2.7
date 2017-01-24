//==============================================================================
//
//  File        : mmp_register_gpio.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Rogers Chen
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_GPIO_H_
#define _MMP_REG_GPIO_H_

#include    "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

// ********************************
//   GPIO structure (0x8000 6900)
// ********************************
typedef struct _AITS_GPIO {
    AIT_REG_D   GPIO_EN[4];                 // 0x00
    AIT_REG_D   GPIO_DATA[4];               // 0x10
    AIT_REG_D   GPIO_INT_H2L_EN[4];         // 0x20
    AIT_REG_D   GPIO_INT_L2H_EN[4];         // 0x30
    AIT_REG_D   GPIO_INT_H_EN[4];        	// 0x40
    AIT_REG_D   GPIO_INT_L_EN[4];         	// 0x50
    AIT_REG_D   GPIO_INT_CPU_EN[4];         // 0x60
    AIT_REG_D   GPIO_INT_H2L_SR[4];        // 0x70
	AIT_REG_D   GPIO_INT_L2H_SR[4];        // 0x80
	AIT_REG_D   GPIO_INT_H_SR[4];        	// 0x90
	AIT_REG_D   GPIO_INT_L_SR[4];        	// 0xA0
	AIT_REG_D   GPIO_INT_HOST_EN[4];        // 0xB0
    AIT_REG_D   GPIO_INT_HOST_H2L_SR[4];   // 0xC0
	AIT_REG_D   GPIO_INT_HOST_L2H_SR[4];   // 0xD0
	AIT_REG_D   GPIO_INT_HOST_H_SR[4];     // 0xE0
	AIT_REG_D   GPIO_INT_HOSTL_SR[4];     	// 0xF0
} AITS_GPIO, *AITPS_GPIO;


// ********************************
//   PWM structure (0x8000 5C00)
// ********************************
//The reason that PWM module is placed with GPIO register is in Grace-serials chips, PWM works as one of GPIO's module

typedef struct _AITS_PWM {
    AIT_REG_B   PWM_INT_HOST_EN;            // 0x00
    AIT_REG_B                   _0x01[15];
    AIT_REG_B   PWM_INT_HOST_SR;            // 0x10
    AIT_REG_B                   _0x11[15];
    AIT_REG_B   PWM_INT_CPU_EN;             // 0x20
    AIT_REG_B                  _0x21[15];
    AIT_REG_B   PWM_INT_CPU_SR;             // 0x30
    AIT_REG_B                   _0x31[15];

    AIT_REG_W   PWM_PULSE_A_T0;             // 0x40
    AIT_REG_W                   _0x42;
    AIT_REG_W   PWM_PULSE_A_T1;             // 0x44
    AIT_REG_W                   _0x46;
    AIT_REG_W   PWM_PULSE_A_T2;             // 0x48
    AIT_REG_W                   _0x4A;
    AIT_REG_W   PWM_PULSE_A_T3;             // 0x4C
    AIT_REG_W                   _0x4E;
    AIT_REG_W   PWM_PULSE_A_CLK_DIV;        // 0x50
    AIT_REG_W                   _0x52[7];

    AIT_REG_W   PWM_PULSE_B_T0;             // 0x60
    AIT_REG_W                   _0x62;
    AIT_REG_W   PWM_PULSE_B_T1;             // 0x64
    AIT_REG_W                   _0x66;
    AIT_REG_W   PWM_PULSE_B_T2;             // 0x68
    AIT_REG_W                   _0x6A;
    AIT_REG_W   PWM_PULSE_B_T3;             // 0x6C
    AIT_REG_W                   _0x6E;
    AIT_REG_W   PWM_PULSE_B_CLK_DIV;        // 0x70
    AIT_REG_W                   _0x72[7];
    AIT_REG_D                   _0x80[16];

    AIT_REG_B   PWM_CTL;                    // 0xC0
        /*-DEFINE-----------------------------------------------------*/
        #define PWM_PULSE_A_FIRST  0x00
        #define PWM_PULSE_B_FIRST  0x10
        #define PWM_ONE_ROUND	   0x00
        #define PWM_AUTO_CYC       0x08
        #define PWM_PULSE_B_POS    0x00
        #define PWM_PULSE_B_NEG    0x04
        #define PWM_PULSE_A_POS    0x00
        #define PWM_PULSE_A_NEG    0x02
        #define PWM_EN             0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                   _0xC1;
    AIT_REG_B   PWM_PULSE_A_NUM;      // 0xC2
    AIT_REG_B                   _0xC3;
    AIT_REG_B   PWM_PULSE_B_NUM;      // 0xC4
    AIT_REG_B                   _0xC5;
    AIT_REG_W                   _0xC6[5];
    AIT_REG_D                   _0xD0[12];
} AITS_PWM, *AITPS_PWM;


/*typedef struct _AITS_UARTB {
    AITS_PWM    PWM[2];
} AITS_PWMB, *AITPS_PWMB;*/

#if 0//fined(BUILD_FW)

#define PWM_INT_HOST_EN             (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_INT_HOST_EN       )))
#define PWM_INT_HOST_SR             (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_INT_HOST_SR       )))
#define PWM_INT_CPU_EN              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_INT_CPU_EN        )))
#define PWM_INT_CPU_SR              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_INT_CPU_SR        )))
#define PWM_PULSE_A_T0              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_A_T0        )))
#define PWM_PULSE_A_T1              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_A_T1        )))
#define PWM_PULSE_A_T2              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_A_T2        )))
#define PWM_PULSE_A_T3              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_A_T3        )))
#define PWM_PULSE_A_CLK_DIV         (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_A_CLK_DIV   )))
#define PWM_PULSE_B_T0              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_B_T0        )))
#define PWM_PULSE_B_T1              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_B_T1        )))
#define PWM_PULSE_B_T2              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_B_T2        )))
#define PWM_PULSE_B_T3              (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_B_T3        )))
#define PWM_PULSE_B_CLK_DIV         (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_B_CLK_DIV   )))
#define PWM_CTL                     (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_CTL               )))
#define PWM_PULSE_A_NUM             (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_A_NUM       )))
#define PWM_PULSE_B_NUM             (PWM_BASE +(MMP_ULONG)(&(((AITPS_PWM )0)->PWM_PULSE_B_NUM       )))

#endif
/// @}
#endif // _MMPH_REG_GPIO_H_
