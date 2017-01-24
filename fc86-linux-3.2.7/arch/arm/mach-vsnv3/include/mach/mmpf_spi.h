//==============================================================================
//
//  File        : mmpf_spi.h
//  Description : INCLUDE File for the Host PSPI Driver.
//  Author      : Sunny Sun
//  Revision    : 1.0
//
//==============================================================================


/**
 *  @file mmpf_spi.h
 *  @brief The header File for the Host PSPI Driver
 *  @author Sunny Sun
 *  @version 1.0
 */

#ifndef _MMPF_SPI_H_
#define _MMPF_SPI_H_

#if 0
#include "mmp_reg_spi.h"
#include "config_fw.h"
#include "mmpf_typedef.h"
#include "lib_retina.h"
#include "reg_retina.h"
//#include "mmp_err.h"
#endif
/** @addtogroup MMPF_Spi
 *  @{
 */


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
#define SPI_OFFSET 0x100

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================


typedef enum _MMPF_SPI_ID
{
    MMPF_SPI_ID_0 = 0,
    MMPF_SPI_ID_1
} MMPF_SPI_ID;

typedef enum _MMPF_SPI_DIR
{
    MMPF_SPI_TX = 0x1,
    MMPF_SPI_RX = 0x2
} MMPF_SPI_DIR;

typedef enum _MMPF_SPI_MODE
{
    MMPF_SPI_MASTER_MODE = 0,
    MMPF_SPI_SLAVE_MODE
} MMPF_SPI_MODE;

typedef enum _MMPF_SPI_PAD
{
    MMPF_SPI_PAD_0 = 0,
    MMPF_SPI_PAD_1
} MMPF_SPI_PAD;

typedef struct _MMPF_SPI_ATTRIBUTE
{
    MMPF_SPI_MODE       mode;           /// Master or Slave
    MMP_USHORT          usSignalCtl;    /// control signal setting
    MMP_UBYTE			ubWordLength;   /// word length in one time transfer
    MMP_UBYTE			ubSclkDiv;      /// spi clock div
    MMP_USHORT			ubPspiDelay;    /// delay count in consecutive transfer when CS is inactive
    MMP_USHORT			ubPspiWait;     /// wait count in consecutive transfer when CS is active
    MMP_UBYTE           ubTxFIFOThres;  /// Tx fifo threshold
    MMP_UBYTE           ubRxFIFOThres;  /// Rx fifo threshold
    MMP_ULONG           ulINTEna;       /// Int Enable setting
    MMPF_SPI_PAD        padCtl;         /// choose which pad to be used by SPI mmodule
} MMPF_SPI_ATTRIBUTE;

typedef struct _MMPF_SPI_OPERATION
{
    MMPF_SPI_DIR        dir;            /// transfer direction
    MMP_ULONG	        ulTxDmaAddr;    /// Tx DMA address
    MMP_UBYTE	        *ubTxFifoPtr;   /// Tx fifo address
    MMP_ULONG	        ulRxDmaAddr;    /// Rx DMA address
    MMP_UBYTE	        *ubRxFifoPtr;   /// Rx fifo address
    MMP_USHORT	        usTransferSizeTx; /// data transfer size
    MMP_USHORT	        usTransferSizeRx; /// data transfer size

} MMPF_SPI_OPERATION;

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
//==============================================================================
//
//                              MACRO FUNCTIONS
//
//==============================================================================
#if 0
typedef void SPICallBackFunc(void);
#else
typedef void SPICallBackFunc(MMP_ULONG intSrc);
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR MMPF_SPI_SetAttributes(MMPF_SPI_ID spiID, MMPF_SPI_ATTRIBUTE *spiattribute);
MMP_ERR MMPF_SPI_Operation(MMPF_SPI_ID spiID, MMPF_SPI_OPERATION *spiop, SPICallBackFunc* SPICallBack);
void MMPF_SPI_ISR(void);
MMP_ERR MMPF_SPI_Initialize(void);
MMP_ERR MMPF_SPI_TPGet(MMPF_SPI_ID usSpiID, MMPF_SPI_OPERATION *spiop);
MMP_ERR MMPF_SPI_PollingTPDMADone(MMPF_SPI_ID usSpiID);
MMP_ERR MMPF_SPI_SendTPCmd(MMPF_SPI_ID usSpiID,MMPF_SPI_OPERATION *spiop);

MMP_ERR MMPF_SPI_InitUSBPhyReg(MMP_ULONG bufaddr);
MMP_ERR MMPF_SPI_WriteUSBPhyReg(MMP_UBYTE Addr, MMP_USHORT data);
MMP_USHORT MMPF_SPI_ReadUSBPhyReg(MMP_UBYTE Addr);

/// @}

#endif // _MMPF_SPI_H_

