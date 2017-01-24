//==============================================================================
//
//  File        : mmp_register_uart.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_UART_H_
#define _MMP_REG_UART_H_

#include    "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/
//-------------------------------
// US structure (0x80006400)
//-------------------------------
#pragma pack(push,1)
#if 0
typedef struct _AITS_US {
    AIT_REG_D   US_CR;              // Control Register                 // 0x00
		/*-DEFINE-----------------------------------*/
		#define US_RSTRX            		0x0001				// Reset Receiver
		#define US_RSTTX		            0x0002				// Reset Transmitter
		#define US_RXEN                     0x0004				// Receiver Enable
		#define US_RXDIS                    0x0000				// Receiver Disable
		#define US_TXEN                     0x0008				// Transmitter Enable
		#define US_TXDIS                    0x0000				// Transmitter Disable
		#define US_PAR_DIS                  0x0100				// Parity Check/Generate Disable
		#define US_PAR_EN                   0x0000				// Parity Check/Generate Enable
		#define US_PAR_EVEN                 0x0000				// Even Parity
		#define US_PAR_ODD                  0x0200				// Odd Parity
		#define US_PAR_0                    0x0400				// Parity Force to "0"
		#define US_PAR_1                    0x0600				// Parity Force to "1"
		#define US_RXERR_WRITE_EN           0x0800				// Parity Error Write to RX Fifo
		#if 0//(CHIP == P_V2)
		#define US_RXDMA_START_FLAG			0x010000
		#define US_RXDMA_RING_ENABLE		0x020000
		#define US_TXDMA_ENABLE				0x040000
		#define US_RXDMA_ENABLE				0x080000
		#define US_DMA_CLEAN				0x00FFFF
		#endif
		#define US_RXERR_WRITE_DIS          0x0000				// Parity Error Not Write to RX Fifo
        #define US_ASYNC_MODE               (US_PAR_DIS + US_RXERR_WRITE_DIS)
		/*------------------------------------------*/
    AIT_REG_D   US_BRGR;            // Baud Rate Generator Register     // 0x04
    AIT_REG_B   US_TXPR;            // Tx Data Port                     // 0x08
    AIT_REG_B                           _x09[3];
    AIT_REG_B   US_RXPR;            // Rx Data Port                     // 0x0C
    AIT_REG_B   US_RXSR;            // Rx Data Port Status
    AIT_REG_B                           _x0E[2];

    AIT_REG_D   US_IER;             // Interrupt Enable Register        // 0x10
        /*-DEFINE-----------------------------------*/
		#define US_RX_PARITY_ERR            0x0001            // Receiver Parity Check Error
		#define US_RX_FRAME_ERR             0x0002            // Receiver Frame Error
		#define US_RX_FIFO_FULL             0x0004            // Receiver FIFO Full
		#define US_RX_FIFO_EMPTY            0x0008            // Receiver FIFO Empty
		#define US_RX_FIFO_OVER_THRES       0x0010            // Receiver FIFO Over(equal) Threshold
		#define US_RX_FIFO_OVERFLOW         0x0020            // Receiver FIFO Overflow
		#define US_TX_FIFO_FULL             0x0100            // Transmitter FIFO Full
		#define US_TX_FIFO_EMPTY            0x0200            // Transmitter FIFO Empty
		#define US_TX_FIFO_UNDER_THRES      0x0400            // Transmitter FIFO Under(equal) Threshold
		#if 0//(CHIP == P_V2)
		#define US_TXDMA_FINISH_IENABLE		0x010000
		#define US_RXDMA_THR_IEABLE			0x020000
		#define US_RXDMA_WRITEMEM_IENALBE	0x040000
		#define US_RXDMA_DROPDATA_IENALBE	0x080000
		#endif

		#define US_RX_ERROR                 (US_RX_PAR_ERR | US_RX_FRAME_ERR)
        /*------------------------------------------*/
    AIT_REG_D   US_ISR;             // Interrupt Status Register        // 0x14
    AIT_REG_D                           _x18[2];
    AIT_REG_D   US_FTHR;            // FIFO Threshold Register          // 0x20
        /*-DEFINE-----------------------------------*/
       
		#define US_TX_FIFO_THRES_MASK       0x00FF            // Transmitter FIFO Threshold Mask
		#define US_RX_FIFO_THRES_MASK       0x3F00            // Receiver FIFO Threshold Mask
		#define US_TX_FIFO_THRES(_a)    	(_a)              // Transmitter FIFO Threshold Shift
		#define US_RX_FIFO_THRES(_a)		(_a<<8)			  // Receiver FIFO Threshold Shift
		
        /*------------------------------------------*/
    #if 0//(CHIP == VSN_V2)
    AIT_REG_D   US_FSR;             // FIFO Status Register             // 0x24
    	/*-DEFINE-----------------------------------*/	
		#if (CHIP == P_V2)
		#define US_RX_FIFO_UNRD_MASK        0x003F            // Transmitter FIFO Un-Write Mask
		#define US_TX_FIFO_UNWR_MASK        0xFF00            // Receiver FIFO Un-Read Mask
		#endif
		#if (CHIP == VSN_V2)
		#define US_TX_FIFO_UNWR_MASK        0x000F            // Receiver FIFO Un-Read Mask
		#define US_RX_FIFO_UNRD_MASK        0x00F0            // Transmitter FIFO Un-Write Mask
		#endif
        /*------------------------------------------*/
    #endif
    #if 1//(CHIP == VSN_V3)
    AIT_REG_B	US_RX_FIFO_DATA_CNT;									// 0x24
    AIT_REG_B	US_TX_FIFO_DATA_CNT;									// 0x25
    AIT_REG_B                           _x26[2];
    #endif
    AIT_REG_D                           _x28[2];

    AIT_REG_D                           _x30[4];                        // 0x30
    
    #if 0 //(CHIP == P_V2)   //Ben_201203
    AIT_REG_D	US_TXDMA_START_ADDR;									// 0x40
    AIT_REG_W	US_TXDMA_TOTAL_BYTE;									// 0x44
    AIT_REG_W   						_x46;
    AIT_REG_W	US_TXDMA_UNRD_COUNT;									// 0x48
    AIT_REG_W	US_RXDMA_UNWR_COUNT;									// 0x4a
    AIT_REG_D   						_x4c;
    AIT_REG_D	US_RXDMA_START_ADDR;									// 0x50
    AIT_REG_D	US_RXDMA_END_ADDR;										// 0x54
    AIT_REG_D	US_RXDMA_LB_ADDR;										// 0x58
    AIT_REG_W	US_RXDMA_TOTAL_THR;										// 0x5C
    AIT_REG_W   						_x5E;
    AIT_REG_D   US_RXDBR;           // Rx Debug Register                // 0x60
    AIT_REG_D   US_TXDBR;           // Tx Debug Register                // 0x64
    AIT_REG_D   US_DBLBR;           // Debug Loop Back Register         // 0x68
    AIT_REG_D                           _x6C;
    AIT_REG_D	US_RXDMA_ADDR;											// 0x70
    AIT_REG_B	US_RXDMA_STATUS;										// 0x74
    AIT_REG_B							_x75[11];
    AIT_REG_D	US_TXDMA_ADDR;											// 0x80
    AIT_REG_D                           _x84[31];						// 0x84 - 0xFF
    #endif
} AITS_US, *AITPS_US;
#endif
/** @addtogroup MMPH_reg
@{
*/
//-------------------------------
// US structure (0x80006400)
//-------------------------------
typedef struct _AITS_US {
    AIT_REG_D   US_CR;              // Control Register                 // 0x00
		/*-DEFINE-----------------------------------*/
        #define US_RSTRX            0x00000001  // Reset Rx
        #define US_RSTTX            0x00000002  // Reset Tx
        #define US_RXEN             0x00000004  // Rx Enable
        #define US_RXDIS            0x00000000  // Rx Disable
        #define US_TXEN             0x00000008  // Tx Enable
        #define US_TXDIS            0x00000000  // Tx Disable
        #define US_PAR_DIS          0x00000100  // Parity Check/Generate Disable
        #define US_PAR_EN           0x00000000  // Parity Check/Generate Enable
        #define US_PAR_EVEN         0x00000000  // Even Parity
        #define US_PAR_ODD          0x00000200  // Odd Parity
        #define US_PAR_0            0x00000400  // Parity Force to "0"
        #define US_PAR_1            0x00000600  // Parity Force to "1"
        #define US_RXERR_WRITE_EN   0x00000800  // Parity Error Write to Rx Fifo
        #define US_RXERR_WRITE_DIS  0x00000000  // Parity Error Not Write to Rx Fifo
        #define US_RX_TIMEOUT_EN    0x00001000  // Enable uart Rx timeout
        #define US_RXDMA_ADDR_UPD   0x01000000  // Load Rx DMA start address
        #define US_RXDMA_RING_EN    0x02000000  // Enable Rx DMA ring mode
        #define US_TXDMA_EN         0x04000000  // Enable Tx DMA
        #define US_RXDMA_EN         0x08000000  // Enable Rx DMA
        #define US_RST_TXDMA        0x10000000  // Reset Tx DMA
        #define US_RST_RXDMA        0x20000000  // Reset Rx DMA
        #define US_ASYNC_MODE               (US_PAR_DIS + US_RXERR_WRITE_DIS)
		#define US_CTS_RTS_HOST_MODE 0x00080000
		#define US_CTS_RTS_ACTIVE_H	0x00040000	// CTS/RTS Active High
		#define US_CTS_RTS_MODE0	0x00000000	// CTS/RTS mode 0 select
		#define US_CTS_RTS_MODE1	0x00020000	//
		#define US_CTS_RTS_ENABLE	0x00010000
		/*------------------------------------------*/
    AIT_REG_D   US_BRGR;            // Baud Rate Generator Register     // 0x04
    AIT_REG_B   US_TXPR;            // Tx Data Port                     // 0x08
    AIT_REG_B                           _x09[3];
    AIT_REG_B   US_RXPR;            // Rx Data Port                     // 0x0C
    AIT_REG_B   US_RXSR;            // Rx Data Port Status
    AIT_REG_B                           _x0E[2];

    AIT_REG_D   US_IER;             // Interrupt Enable Register        // 0x10
        /*-DEFINE-----------------------------------*/
        #define US_RX_PARITY_ERR    0x00000001  // Rx Parity Check Error
        #define US_RX_FRAME_ERR     0x00000002  // Rx Frame Error
        #define US_RX_FIFO_FULL     0x00000004  // Rx FIFO Full
        #define US_RX_FIFO_EMPTY    0x00000008  // Rx FIFO Empty
        #define US_RX_FIFO_OVER_TH  0x00000010  // Rx FIFO >= Threshold
        #define US_RX_FIFO_OVERFLOW 0x00000020  // Rx FIFO Overflow
        #if (CHIP == MERCURY)
        #define US_RX_TIMEOUT 		0x00000040  // Rx TimeOut Detect
        #endif
        #define US_TX_FIFO_FULL     0x00000100  // Tx FIFO Full
        #define US_TX_FIFO_EMPTY    0x00000200  // Tx FIFO Empty
        #define US_TX_FIFO_UNDER_TH 0x00000400  // Tx FIFO <= Threshold
        #define US_TXDMA_DONE_EN    0x00010000  // Tx DMA finish
        #define US_RXDMA_TH_EN      0x00020000  // Rx DMA count >= Threshold
        #if (CHIP == MERCURY)
		#define US_RXDMA_WRMEM_EN	0x00040000
		#define US_RXDMA_DROPDATA_EN 0x00080000
        #endif
        #define US_RX_ERROR         (US_RX_PARITY_ERR | US_RX_FRAME_ERR)
        /*------------------------------------------*/
    AIT_REG_D   US_ISR;             // Interrupt Status Register        // 0x14
    AIT_REG_D                           _x18[2];
    AIT_REG_D   US_FTHR;            // FIFO Threshold Register          // 0x20
        /*-DEFINE-----------------------------------*/
        #if (CHIP == MERCURY)
        #define US0_TX_FIFO_DEPTH       0x80    // Tx FIFO 0 Depth 128 Byte
        #endif
        #define US_TX_FIFO_DEPTH        0x20    // Tx FIFO Depth
        #define US_TX_FIFO_TH_MASK      0x00FF  // Tx FIFO Threshold Mask
        #define US_RX_FIFO_TH_MASK      0x3F00  // Rx FIFO Threshold Mask
        #define US_TX_FIFO_TH(_a)       (_a)    // Tx FIFO Threshold Shift
        #define US_RX_FIFO_TH(_a)       (_a<<8) // Rx FIFO Threshold Shift
        /*------------------------------------------*/
    #if (CHIP == VSN_V3)||(CHIP == MERCURY)
    AIT_REG_B	US_RX_FIFO_DATA_CNT;									// 0x24
    AIT_REG_B	US_TX_FIFO_DATA_CNT;									// 0x25
    AIT_REG_B                           _x26[2];
    #endif
    AIT_REG_D           _x28[6];


    AIT_REG_D	US_TXDMA_START_ADDR;									// 0x40
    AIT_REG_W	US_TXDMA_TOTAL_BYTE;									// 0x44
    AIT_REG_W   						_x46;
    AIT_REG_W	US_TXDMA_UNRD_COUNT;									// 0x48
    AIT_REG_W	US_RXDMA_UNWR_COUNT;									// 0x4a
    AIT_REG_D           _x4C;
    AIT_REG_D	US_RXDMA_START_ADDR;									// 0x50
    AIT_REG_D	US_RXDMA_END_ADDR;										// 0x54
    AIT_REG_D	US_RXDMA_LB_ADDR;										// 0x58
    AIT_REG_W	US_RXDMA_TOTAL_THR;										// 0x5C
    AIT_REG_W   						_x5E;
    AIT_REG_D   US_RXDBR;           // Rx Debug Register                // 0x60
    AIT_REG_D   US_TXDBR;           // Tx Debug Register                // 0x64
    AIT_REG_D   US_DBLBR;           // Debug Loop Back Register         // 0x68
    AIT_REG_D                           _x6C;
    AIT_REG_D	US_RXDMA_ADDR;											// 0x70
    AIT_REG_B	US_RXDMA_STATUS;										// 0x74
    AIT_REG_B							_x75[11];
    AIT_REG_D	US_TXDMA_ADDR;											// 0x80
    AIT_REG_D                           _x84[31];						// 0x84 - 0xFF

} AITS_US, *AITPS_US;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct _AITS_UART {
    #if (CHIP == VSN_V3)
    AITS_US     US1;                                        // 0x5C00~0x5CFF
    AIT_REG_D                           _x5D00[0x340];      // 0x5D00~0x69FF
    AITS_US     US0;                                        // 0x6A00~0x6AFF
    #endif
    #if 0//(CHIP == MERCURY)
    AITS_US     US3;                                        // 0x5300~0x53FF
    AITS_US     US2;                                        // 0x5400~0x54FF
    AIT_REG_D                           _x5500[0x1C0];      // 0x5500~0x5BFF
    AITS_US     US1;                                        // 0x5C00~0x5CFF
    AIT_REG_D                           _x5D00[0x340];      // 0x5D00~0x69FF
    AITS_US     US0;                                        // 0x6A00~0x6AFF
    #endif
} AITS_UART, *AITPS_UART;
#pragma pack(pop)

////////////////////////////////////
// Register definition
//

#if !defined(BUILD_FW)
// INT OPR
#define UART0_CR            (UARTB_BASE +(MMP_ULONG)(&(((AITPS_UARTB )0)->US[0].US_CR     )))
#define UART0_IER           (UARTB_BASE +(MMP_ULONG)(&(((AITPS_UARTB )0)->US[0].US_IER    )))
#define UART0_ISR           (UARTB_BASE +(MMP_ULONG)(&(((AITPS_UARTB )0)->US[0].US_ISR    )))
#define UART0_BRGR          (UARTB_BASE +(MMP_ULONG)(&(((AITPS_UARTB )0)->US[0].US_BRGR   )))
#define UART0_TXPR          (UARTB_BASE +(MMP_ULONG)(&(((AITPS_UARTB )0)->US[0].US_TXPR   )))
#define UART0_RXPR          (UARTB_BASE +(MMP_ULONG)(&(((AITPS_UARTB )0)->US[0].US_RXPR   )))
#endif

/// @}
AITPS_US GetpUS(const int mmpf_uart_id);

#endif	// _REG_UART_H_
