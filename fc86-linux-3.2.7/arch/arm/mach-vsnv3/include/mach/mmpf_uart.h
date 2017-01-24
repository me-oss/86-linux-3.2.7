//==============================================================================
//
//  File        : mmpf_uart.h
//  Description : INCLUDE File for the Firmware UART Control Driver
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_UART_H_
#define _MMPF_UART_H_

#include <mach/os_wrap.h>

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#if (OS_TYPE == OS_LINUX)
//#define UART_RXINT_MODE_EN  (0)
#define UART_RXINT_MODE_EN  (1)
#endif
#if (OS_TYPE == OS_UCOSII)
#define UART_RXINT_MODE_EN  (1)
#endif
#define UART_DMA_MODE_EN    (0)


//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_UART_ID
{
    MMPF_UART_ID_0 = 0,
    MMPF_UART_ID_1,
    #if (CHIP == MERCURY)
    MMPF_UART_ID_2,
    MMPF_UART_ID_3,
    #endif
    MMPF_UART_MAX_COUNT
} MMPF_UART_ID;

typedef enum _MMPF_UART_PADSET
{
    MMPF_UART_PADSET_0 = 0,
    MMPF_UART_PADSET_1,
    MMPF_UART_PADSET_2,
    MMPF_UART_PADSET_3,
    #if (CHIP == MERCURY)
    MMPF_UART_PADSET_4,
    #endif
    MMPF_UART_PADSED_MAX
} MMPF_UART_PADSET;

typedef enum _MMPF_UART_STAT {
    MMPF_UART_STAT_TXEMPTY = 0,
    MMPF_UART_STAT_RXEMPTY,
    MMPF_UART_STAT_TXUNDERTH,
    MMPF_UART_STAT_RXOVERTH
} MMPF_UART_STAT;

typedef enum _MMPF_UART_DIRECTION {
    MMPF_UART_DIRECTION_RX = 0x01,
    MMPF_UART_DIRECTION_TX = 0x02
} MMPF_UART_DIRECTION;

typedef enum _MMPF_UART_EVENT {
    MMPF_UART_EVENT_RXFIFO_OVERTH = 0,
    MMPF_UART_EVENT_TXFIFO_UNDERTH,
    MMPF_UART_EVENT_MAX
} MMPF_UART_EVENT;

#if (UART_DMA_MODE_EN == 1)
typedef enum _MMPF_UART_DMAMODE
{
    MMPF_UART_RXDMA_RING = 0,
    MMPF_UART_TXDMA,
    MMPF_UART_RXDMA,
    MMPF_UART_DMA_MAX
} MMPF_UART_DMAMODE;

typedef enum _MMPF_UART_DMA_INT_MODE
{
    MMPF_UART_TXDMA_FINISH_IENABLE = 0,
    MMPF_UART_RXDMA_THR_IEABLE,
    MMPF_UART_RXDMA_WRITEMEM_IENALBE,
    MMPF_UART_RXDMA_DROPDATA_IENALBE,
    MMPF_UART_DMA_INT_MAX
} MMPF_UART_DMA_INT_MODE;
#endif

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================


#if (UART_DMA_MODE_EN == 1)
typedef void UartCallBackFunc(void);
#endif

#define AIT_UART_RTSCTS 0x01
typedef struct _MMPF_UART_ATTRIBUTE
{
    MMPF_UART_PADSET    padset;
    MMP_ULONG           ulMasterclk;
    MMP_ULONG           ulBaudrate;
    MMP_ULONG 			mode;
} MMPF_UART_ATTRIBUTE;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR MMPF_Uart_Open(MMPF_UART_ID uartId, MMPF_UART_ATTRIBUTE *uartattribute);
MMP_ERR MMPF_Uart_Write(MMPF_UART_ID uartId, const char *str, MMP_ULONG ulLength);
MMP_ERR MMPF_Uart_Close(MMPF_UART_ID uartId);
MMP_ERR MMPF_Uart_GetDebugString(MMPF_UART_ID uartId, MMP_BYTE *bDebugString,
                        MMP_ULONG *ulDebugStringLength);
MMP_ERR MMPF_Uart_Init(void);
MMP_ULONG MMPF_Uart_TryRead(MMPF_UART_ID uartId, MMP_UBYTE *buf, MMP_ULONG ulMaxReadLen);
MMP_BOOL MMPF_Uart_CheckState(MMPF_UART_ID uartId, MMPF_UART_STAT State);
MMP_ERR MMPF_Uart_Reset(MMPF_UART_ID uartID, MMPF_UART_DIRECTION direction);
MMP_ERR MMPF_Uart_SetInterruptEnable(MMPF_UART_ID uartId, MMPF_UART_EVENT event, MMP_BOOL bEnable);

#if (UART_DMA_MODE_EN == 1)
MMP_ERR MMPF_Uart_SwitchToDmaMode(MMPF_UART_ID uartId, MMP_BOOL bEnable);
MMP_ERR MMPF_Uart_SetTxDmaMode(MMPF_UART_ID uartId,
                        MMPF_UART_DMAMODE uartDmaMode,
                        MMP_ULONG uartTxStartAddr,
                        MMP_USHORT uartTxTotalByte);
MMP_ERR MMPF_Uart_SetRxDmaMode(MMPF_UART_ID uartId,
                        MMPF_UART_DMAMODE uartDmaMode,
                        MMP_ULONG uartRxStartAddr,
                        MMP_ULONG uartRxEndAddr,
                        MMP_ULONG uartRxLowBoundAddr);
MMP_ERR MMPF_Uart_SetDmaInterruptMode (MMPF_UART_ID uartId,
                        MMPF_UART_DMA_INT_MODE intMode,
                        MMP_BOOL bEnable, UartCallBackFunc* callBackFunc,
                        MMP_USHORT uartRxThreshold);
MMP_ERR MMPF_Uart_EnableDmaMode(MMPF_UART_ID uartId,
                        MMPF_UART_DMAMODE uartDmaMode, MMP_BOOL bEnable);
MMP_ERR MMPF_Uart_RxDmaStart(MMPF_UART_ID uartId);
#endif

#endif // _INCLUDES_H_
