#ifndef _MMPF_SPI_C
#define _MMPF_SPI_C
//==============================================================================
//
//  File        : MMPF_spi.c
//  Description : Retina V1 Programmable Serial Peripheral Interface
//                Module Control driver function
//  Author      : Sunny Sun
//  Revision    : 1.0
//
//==============================================================================
/**
*  @file mmpf_spi.c
*  @brief The PSPI Module Control functions
*  @author Sunny Sun
*  @version 1.0
*/
#include "mmp_register.h"
#include "mmp_reg_spi.h"
#include "mmpf_spi.h"
#include "mmpf_pio.h"

#pragma O0

#if (SUPPORT_SPI_FLASH)  

/** @addtogroup MMPF_Spi
 *  @{
 */
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
static  SPICallBackFunc *SPICallBack[3];
//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
void dbg_printf(unsigned long level, char *fmt, ...);

//------------------------------------------------------------------------------
//  Function    : MMPF_SPI_ISR
//------------------------------------------------------------------------------
/** @brief SPI interrupt service routine

    This ISR call SPICallBackFunc needed by caller's operation.
*/
void MMPF_SPI_ISR(void)
{
AITPS_SPI pSPI;
MMP_ULONG int_src = 0;
MMP_USHORT i;

#if (CHIP == P_V2)||(CHIP == VSN_V2)||(CHIP == VSN_V3)
MMP_USHORT loop = 1;
#endif

    for (i = 0; i < loop; i++) {
        pSPI = &AITC_BASE_SPIB->SPI[i];
        int_src = pSPI->SPI_INT_CPU_SR & pSPI->SPI_INT_CPU_EN;
        //pSPI->SPI_INT_CPU_EN &= (~int_src);
        pSPI->SPI_INT_CPU_SR = int_src;
        {
            static MMP_BOOL curTxUf = MMP_FALSE;
            static MMP_BOOL curRxOf = MMP_FALSE;
            if (((int_src & SPI_TXFIFO_UF) != 0) != curTxUf) {
                curTxUf = !curTxUf;
                dbg_printf(0, "____________________ PSPI SPI_TXFIFO_UF = %#x\r\n", ((int_src & SPI_TXFIFO_UF) != 0));
            } 
            if ((int_src & SPI_TXFIFO_UF) != 0) {
                pSPI->SPI_CFG &= (~SPI_TX_EN);
                pSPI->SPI_CTL |= SPI_TXFIFO_CLR;
                pSPI->SPI_INT_CPU_SR = SPI_TXFIFO_UF;
            }
            if (((int_src & SPI_RXFIFO_OF) != 0) != curRxOf) {
                curRxOf = !curRxOf;
                dbg_printf(0, "____________________ PSPI SPI_RXFIFO_OF = %#x\r\n", ((int_src & SPI_RXFIFO_OF) != 0));
            } 
            if ((int_src & SPI_RXFIFO_OF) != 0) {
                pSPI->SPI_CFG &= (~SPI_RX_EN);
                pSPI->SPI_CTL |= SPI_RXFIFO_CLR;
                pSPI->SPI_INT_CPU_SR = SPI_RXFIFO_OF;
            }
        } 
        if (int_src >= 0x10000) {
            //dbg_printf(0, "@@@@ PSPI int_src = %#x\r\n", int_src);
        }
#if 0
        if (int_src & SPI_FIFO_TX_DONE) {
            if (SPICallBack[i]) {
                (*SPICallBack[i])();
            }
        }
        if (int_src & SPI_RXDMA_DONE) {
            if (SPICallBack[i]) {
                (*SPICallBack[i])();
            }
            RTNA_DBG_Str(0, "RXDMA DONE\r\n");
        }
        if (int_src & SPI_TXFIFO_LE) {
            if (SPICallBack[i]) {
                (*SPICallBack[i])();
            }
            RTNA_DBG_Str(0, "TX FIFO_LE\r\n");
        }
        if (int_src & SPI_SLAVE_ERR) {
            RTNA_DBG_Str(0, "SPI_SLAVE_ERR\r\n");
        }

        if (int_src & SPI_RXFIFO_GE) {
            if (SPICallBack[i]) {
                (*SPICallBack[i])();
            }
        }
        if (int_src & SPI_TXDMA_DONE) {
            if (SPICallBack[i]) {
                (*SPICallBack[i])();
            }
            RTNA_DBG_Str(0, "TX DM done\r\n");
        }
#else
#if 0
        if (int_src & SPI_TXDMA_DONE) RTNA_DBG_Str(0, "TX DMA done\r\n");
        if (int_src & SPI_RXDMA_DONE) RTNA_DBG_Str(0, "RX DMA done\r\n");
#endif
        if (SPICallBack[i]) {
            (*SPICallBack[i])(int_src);
        }
#endif
        //pSPI->SPI_INT_CPU_EN |= (int_src);
    }
}
//------------------------------------------------------------------------------
//  Function    : MMPF_SPI_Initialize
//------------------------------------------------------------------------------
/** @brief SPI module initialize

    This function initialize some parameter or create something necessary, like
    ISR open

*/
MMP_ERR MMPF_SPI_Initialize(void)
{
AITPS_GBL   pGBL = AITC_BASE_GBL;
AITPS_AIC pAIC = AITC_BASE_AIC;


    #if (CHIP == P_V2)
    //pGBL->GBL_RST_SEL |= GBL_MODULE_SPI;
    pGBL->GBL_RST_SW_EN = GBL_MODULE_SPI;
    RTNA_WAIT_CYCLE(1000);
    pGBL->GBL_RST_SW_DIS = GBL_MODULE_SPI;
    //pGBL->GBL_RST_SEL &= ~GBL_MODULE_SPI;
    #endif
#if 0
    #if (CHIP == VSN_V2 || CHIP == VSN_V3)
    pGBL->GBL_RST_CTL2 |= GBL_REG_PSPI_RST;
    pGBL->GBL_RST_CTL2 |= GBL_PSPI_RST;
    //RTNA_WAIT_CYCLE(1000);
		MMPF_PLL_WaitCount(0x20);
    pGBL->GBL_RST_CTL2 &= ~(GBL_PSPI_RST);
    pGBL->GBL_RST_CTL2 &= ~(GBL_REG_PSPI_RST);
    #endif
#endif

    #if (CHIP == VSN_V2 || CHIP == VSN_V3)
    pGBL->GBL_CLK_DIS1 &= (~GBL_CLK_PSPI_DIS);
    #endif
    #if (CHIP == P_V2)
    RTNA_AIC_Open(pAIC, AIC_SRC_SIF_SPI_0_1_2, spi_isr_a, AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 2);
    RTNA_AIC_IRQ_En(pAIC, AIC_SRC_SIF_SPI_0_1_2);
    #endif
#if 1
    #if (CHIP == VSN_V3)
    RTNA_AIC_Open(pAIC, AIC_SRC_SPI, spi_isr_a, AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 3);
    RTNA_AIC_IRQ_En(pAIC, AIC_SRC_SPI);
    #endif
#endif

    return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPF_Spi_SetAttributes
//------------------------------------------------------------------------------
/** @brief The function sets the attributes to the specified spi channel with its spi ID

The function sets the attributes to the specified spi channel with its icon ID. These attributes include
spi master/slave mode, signal type, INT enable, and related clock. It is implemented by programming PSPI
Controller registers to set those attributes.

  @param[in] usSpiID the SPI ID
  @param[in] spiattribute the SPI attribute
  @return It reports the status of the operation.
*/
MMP_ERR XMMPF_SPI_SetAttributes(MMPF_SPI_ID usSpiID, MMPF_SPI_ATTRIBUTE *spiattribute)
{
AITPS_GBL   pGBL = AITC_BASE_GBL;
AITPS_SPI pSPI = &AITC_BASE_SPIB->SPI[usSpiID];
    dbg_printf(3,"### pSPI = %#x\r\n", pSPI);

    //pGBL->GBL_MIO_SPI_CTL |= (GBL_SPI_PADMAP(spiattribute->padCtl, usSpiID)|GBL_SPI_PADSET_EN(spiattribute->padCtl));
    pGBL->GBL_IO_CTL1 &= ~(GBL_PSPI_IO_PAD1_EN);
    pGBL->GBL_IO_CTL3 |= 0x10;

    pSPI->SPI_CTL |= (SPI_TXFIFO_CLR | SPI_RXFIFO_CLR);
    if (spiattribute->mode == MMPF_SPI_MASTER_MODE) {
        pSPI->SPI_CFG = (MASTER_RX_PAD_CLK|SPI_MASTER_MODE | spiattribute->usSignalCtl);
        //RTNA_DBG_Long1(pSPI->SPI_INT_CPU_SR);
        //RTNA_DBG_Str1(" :");
        //RTNA_DBG_Short1(pSPI->SPI_CFG);
        //RTNA_DBG_Str1(" =MASTER SPICFG\r\n");
        pSPI->SPI_WORD_LEN = spiattribute->ubWordLength-1;
        pSPI->SPI_CLK_DIV = spiattribute->ubSclkDiv;
        pSPI->SPI_DLY_CYCLE = spiattribute->ubPspiDelay;
        pSPI->SPI_WAIT_CYCLE = spiattribute->ubPspiWait;
        pSPI->SPI_TXFIFO_THD = spiattribute->ubTxFIFOThres;
        pSPI->SPI_RXFIFO_THD = spiattribute->ubRxFIFOThres;
        pSPI->SPI_INT_CPU_SR = spiattribute->ulINTEna;
        pSPI->SPI_INT_CPU_EN = spiattribute->ulINTEna;
    }
    else {
        pSPI->SPI_CFG = (MASTER_RX_PAD_CLK|spiattribute->usSignalCtl);
        //RTNA_DBG_Short1(pSPI->SPI_INT_CPU_SR);
        //RTNA_DBG_Str1(" :");
        //RTNA_DBG_Short1(pSPI->SPI_CFG);
        //RTNA_DBG_Str1(" =SLAVE SPICFG\r\n");
        pSPI->SPI_WORD_LEN = spiattribute->ubWordLength - 1;
        pSPI->SPI_TXFIFO_THD = spiattribute->ubTxFIFOThres;
        pSPI->SPI_RXFIFO_THD = spiattribute->ubRxFIFOThres;
        pSPI->SPI_INT_CPU_SR = spiattribute->ulINTEna;
        pSPI->SPI_INT_CPU_EN = spiattribute->ulINTEna;
    }
    return MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPF_SPI_Operation
//------------------------------------------------------------------------------
/** @brief The function excute fifo/dma read/write with its spi ID

The function excute fifo/dma read/write operation according to its operation setting.

  @param[in] usSpiID the SPI ID
  @param[in] spiop operation setting
  @param[in] fpSPICallBack call back function pointer
  @return It reports the status of the operation.
*/
MMP_ERR MMPF_SPI_Operation(MMPF_SPI_ID usSpiID, MMPF_SPI_OPERATION *spiop
                            , SPICallBackFunc *fpSPICallBack)
{
AITPS_SPI pSPI = &AITC_BASE_SPIB->SPI[usSpiID];
MMP_BOOL    tx_en = MMP_FALSE, rx_en = MMP_FALSE;
MMP_BOOL    tx_dma_mode = MMP_FALSE, rx_dma_mode = MMP_FALSE;
MMP_USHORT  outbyte, transferbyte, i;
MMP_UBYTE   *ptr, *ptr2;
MMP_USHORT  byte_count;

    SPICallBack[usSpiID] = fpSPICallBack;
    if (spiop->dir & MMPF_SPI_TX) {
        tx_en = MMP_TRUE;
        if (spiop->ulTxDmaAddr != 0) {
            tx_dma_mode = MMP_TRUE;
        }
    }
    if (spiop->dir & MMPF_SPI_RX) {
        rx_en = MMP_TRUE;
        if (spiop->ulRxDmaAddr != 0) {
            rx_dma_mode = MMP_TRUE;
        }
    }
    if (tx_en == MMP_TRUE) {
        if (rx_en == MMP_TRUE) {
            // tx/rx at the same time
            if (tx_dma_mode == MMP_TRUE && rx_dma_mode == MMP_TRUE) {
                // clear interrupt
                pSPI->SPI_INT_CPU_SR = SPI_TXDMA_DONE | SPI_RXDMA_DONE;
                if (! (pSPI->SPI_CFG & SPI_MASTER_MODE) ){
                    pSPI->SPI_INT_CPU_SR = SPI_SLAVE_ERR;
                }
                pSPI->SPI_TXDMA_ADDR = spiop->ulTxDmaAddr;
                pSPI->SPI_RXDMA_ADDR = spiop->ulRxDmaAddr;
                pSPI->SPI_TXDMA_SIZE = spiop->usTransferSizeTx - 1;
                pSPI->SPI_RXDMA_SIZE = spiop->usTransferSizeRx - 1;
                pSPI->SPI_CTL = (SPI_RX_DMA_START | SPI_TX_DMA_START);

                RTNA_WAIT_CYCLE(100);  // must add delay here to wait FIFO ready

                pSPI->SPI_CFG |= (SPI_TX_EN | SPI_RX_EN|TX_XCH_MODE);
                if (!(pSPI->SPI_CFG & TX_NON_XCH_MODE) ){
                    if (pSPI->SPI_CFG & SPI_MASTER_MODE){
                        pSPI->SPI_XCH_CTL = XCH_START;
                    }
                }

#if 0
                //if (!((pSPI->SPI_INT_CPU_EN & (SPI_RXDMA_DONE|SPI_TXDMA_DONE) )== (SPI_RXDMA_DONE|SPI_TXDMA_DONE) )) {
                    while( !(pSPI->SPI_INT_CPU_SR &SPI_TXDMA_DONE) )  	MMPF_WD_Kick();
                    while( !(pSPI->SPI_INT_CPU_SR &SPI_FIFO_TX_DONE))  	MMPF_WD_Kick();
                    while( !(pSPI->SPI_INT_CPU_SR &SPI_RXDMA_DONE) )  	MMPF_WD_Kick();
                //}
#endif

            }
            // fifo mode
            else {
                if (! (pSPI->SPI_CFG & SPI_MASTER_MODE)) {
                    pSPI->SPI_INT_CPU_SR = SPI_SLAVE_ERR;
                }
                pSPI->SPI_CFG |= (SPI_RX_EN | SPI_TX_EN|TX_NON_XCH_MODE);
                outbyte = spiop->usTransferSizeTx;

                ptr = spiop->ubTxFifoPtr;
                ptr2 = spiop->ubRxFifoPtr;
#if 0
                if (pSPI->SPI_WORD_LEN >= 31) {
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    outbyte -= 4;
                }

                else if (pSPI->SPI_WORD_LEN >= 16) {
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    outbyte -= 3;
                }
                else if (pSPI->SPI_WORD_LEN >= 8) {
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    outbyte -= 2;
                }
                else {
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    outbyte -= 1;
                }
#endif
                while (outbyte) {
                    transferbyte = 32 - pSPI->SPI_TXFIFO_SPC;
                    transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                    if (transferbyte) {
                        for (i = 0; i < transferbyte; i++) {
                            pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                            if (pSPI->SPI_RXFIFO_SPC) {
                                *ptr2++ = pSPI->SPI_RXFIFO_DATA.B[0];
                            }
                        }
                    }
                    else {
                        if (! (pSPI->SPI_CFG & SPI_MASTER_MODE) ){
                            if (pSPI->SPI_INT_CPU_SR & SPI_TXFIFO_UF) {
                                RTNA_DBG_Str(1,"Slave TX underflow\r\n");
                                return MMP_SPI_ERR_TX_UNDERFLOW;
                            }
                        }
                    }
                    outbyte -= transferbyte;
                }
                while(! (pSPI->SPI_INT_CPU_SR & SPI_FIFO_TX_DONE));//  	MMPF_WD_Kick();
                while(pSPI->SPI_RXFIFO_SPC) {
                    *ptr2++ = pSPI->SPI_RXFIFO_DATA.B[0];
                }
#if 0
        SPIflow_RxPrint();
        SPItest_TxDMA();
#endif
            }
        }
        else {
            // tx only
            if (tx_dma_mode == MMP_TRUE) {
                // tx only, dma mode
                // clear interrupt
                pSPI->SPI_INT_CPU_SR = SPI_TXDMA_DONE | SPI_RXDMA_DONE;
                if (! (pSPI->SPI_CFG & SPI_MASTER_MODE) ){
                    pSPI->SPI_INT_CPU_SR = SPI_SLAVE_ERR;
                }

                pSPI->SPI_TXDMA_ADDR = spiop->ulTxDmaAddr;
                pSPI->SPI_TXDMA_SIZE = spiop->usTransferSizeTx - 1;
                pSPI->SPI_CTL = SPI_TX_DMA_START;

                RTNA_WAIT_CYCLE(100);  // must add delay here to wait FIFO ready

                pSPI->SPI_CFG &= (~SPI_RX_EN);
                //pSPI->SPI_CTL |= SPI_RXFIFO_CLR; //desmond
                pSPI->SPI_CFG |= (SPI_TX_EN|TX_XCH_MODE);
                if ( !(pSPI->SPI_CFG & TX_NON_XCH_MODE)) {
                    if (pSPI->SPI_CFG & SPI_MASTER_MODE){
                        pSPI->SPI_XCH_CTL = XCH_START;
                    }

                }

#if 0
                //if (! (pSPI->SPI_INT_CPU_EN & SPI_TXDMA_DONE) ) {
                    while( !(pSPI->SPI_INT_CPU_SR & SPI_TXDMA_DONE))  	MMPF_WD_Kick();
                    while( !(pSPI->SPI_INT_CPU_SR & SPI_FIFO_TX_DONE))  	MMPF_WD_Kick();
                //}
#endif

            }
            else {
            #if 0
                // tx only, fifo mode
                if (!(pSPI->SPI_CFG & SPI_MASTER_MODE) ) {
                    pSPI->SPI_INT_CPU_SR = SPI_SLAVE_ERR;
                }

                pSPI->SPI_CFG &= (~SPI_RX_EN);
                pSPI->SPI_CFG |= (SPI_TX_EN|TX_NON_XCH_MODE);

                outbyte = spiop->usTransferSize;
                ptr = spiop->ubTxFifoPtr;
#if 0
                if (pSPI->SPI_WORD_LEN >= 31) {
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    outbyte -= 4;
                }

                else if (pSPI->SPI_WORD_LEN >= 16) {
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    outbyte -= 3;
                }
                else if (pSPI->SPI_WORD_LEN >= 8) {
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                    outbyte -= 2;
                }
                else {
                    pSPI->SPI_TXFIFO_DATA.B[0] = (MMP_UBYTE)*ptr++;
                    outbyte -= 1;
                }
#endif
                if (!(pSPI->SPI_CFG & TX_NON_XCH_MODE) ){
                    if (pSPI->SPI_CFG & SPI_MASTER_MODE){
                        pSPI->SPI_XCH_CTL = XCH_START;
                    }
                }
                while (outbyte) {
                    transferbyte = 32 - pSPI->SPI_TXFIFO_SPC;
                    transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                    if (transferbyte) {
                        for (i = 0; i < transferbyte; i++) {
                            pSPI->SPI_TXFIFO_DATA.B[0] = *ptr++;
                        }
                    }
                    else {
                        if (!(pSPI->SPI_CFG & SPI_MASTER_MODE)) {
                            if (pSPI->SPI_INT_CPU_SR & SPI_TXFIFO_UF) {
                                RTNA_DBG_Str(1, "Slave TX underflow\r\n");
                                return MMP_SPI_ERR_TX_UNDERFLOW;
                            }
                        }
                    }
                    outbyte -= transferbyte;
                }
                while(!(pSPI->SPI_INT_CPU_SR & SPI_FIFO_TX_DONE));
            	#endif
            }
        }
    }
    else {
        // rx only
        if (rx_dma_mode == MMP_TRUE) {
            // rx only ,dma mode
            // clear interrupt status

            pSPI->SPI_INT_CPU_SR = SPI_RXDMA_DONE;

            if (! (pSPI->SPI_CFG & SPI_MASTER_MODE)) {
                pSPI->SPI_INT_CPU_SR = SPI_SLAVE_ERR;
            }
#if 0
            else {
                pSPI->SPI_TXDMA_SIZE = spiop->usTransferSizeTx - 1;
            }
#endif

            pSPI->SPI_RXDMA_ADDR = spiop->ulRxDmaAddr;
            pSPI->SPI_RXDMA_SIZE = spiop->usTransferSizeRx - 1;

            pSPI->SPI_CFG &= (~SPI_TX_EN);
            //pSPI->SPI_CTL |= SPI_TXFIFO_CLR; //desmond
            pSPI->SPI_CFG |= (SPI_RX_EN|TX_XCH_MODE);

            pSPI->SPI_CTL = SPI_RX_DMA_START;

            if (!(pSPI->SPI_CFG & TX_NON_XCH_MODE) ) {
                if (pSPI->SPI_CFG & SPI_MASTER_MODE){
                        pSPI->SPI_XCH_CTL = XCH_START;
                }
            }

#if 0
            if ( !(pSPI->SPI_INT_CPU_EN & SPI_RXDMA_DONE) ) {
                while( !(pSPI->SPI_INT_CPU_SR & SPI_RXDMA_DONE) );
            }
#endif
        }
        else {
        #if 0
            // rx only fifo mode
            if (!(pSPI->SPI_CFG & SPI_MASTER_MODE) ){
                pSPI->SPI_INT_CPU_SR = SPI_SLAVE_ERR;

                pSPI->SPI_CFG &= (~SPI_TX_EN);
                pSPI->SPI_CFG |= (SPI_RX_EN|TX_NON_XCH_MODE);

                outbyte = spiop->usTransferSize;
                ptr = spiop->ubRxFifoPtr;
                while (outbyte) {
                    transferbyte = pSPI->SPI_RXFIFO_SPC;
                    transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                    for (i = 0; i < transferbyte; i++) {
                        *ptr++ = pSPI->SPI_RXFIFO_DATA.B[0];
                    }
                    outbyte -= transferbyte;
                }
            }
            else {
                // for master mode, enable tx fifo, too
#if 1
                if (pSPI->SPI_WORD_LEN >= 16) {
                    byte_count = 3;
                }
                else if ( pSPI->SPI_WORD_LEN >= 8){
                    byte_count = 2;
                }
                else {
                    byte_count = 1;
                }
#endif
                pSPI->SPI_CFG |= (SPI_TX_EN | SPI_RX_EN|TX_NON_XCH_MODE);

                outbyte = spiop->usTransferSize;
                ptr = spiop->ubRxFifoPtr;
                if (byte_count == 3) {
                    while (outbyte) {

                        transferbyte = 32 - pSPI->SPI_TXFIFO_SPC;
                        transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                        for (i = 0; i < transferbyte; i+=3) {
                            pSPI->SPI_TXFIFO_DATA.B[0] = 0xFF;
                            pSPI->SPI_TXFIFO_DATA.B[0] = 0xFF;
                            pSPI->SPI_TXFIFO_DATA.B[0] = 0xFF;
                        }

                        while(pSPI->SPI_RXFIFO_SPC != transferbyte);
                        for (i = 0; i < transferbyte; i+=3) {
                            *ptr++ = pSPI->SPI_RXFIFO_DATA.B[0];
                            *ptr++ = pSPI->SPI_RXFIFO_DATA.B[0];
                            *ptr++ = pSPI->SPI_RXFIFO_DATA.B[0];
                        }
                        outbyte -= transferbyte;
                    }
                }
                else if (byte_count == 2) {
                    while (outbyte) {

                        transferbyte = 32 - pSPI->SPI_TXFIFO_SPC;
                        transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                        for (i = 0; i < transferbyte; i+=2) {
                            pSPI->SPI_TXFIFO_DATA.B[0] = 0xFF;
                            pSPI->SPI_TXFIFO_DATA.B[0] = 0xFF;
                        }

                        while( pSPI->SPI_RXFIFO_SPC != transferbyte);
                        for (i = 0; i < transferbyte; i+=2) {
                            *ptr++ = pSPI->SPI_RXFIFO_DATA.B[0];
                            *ptr++ = pSPI->SPI_RXFIFO_DATA.B[0];
                        }
                        outbyte -= transferbyte;
                    }
                }
                else if (byte_count == 1) {
                    while (outbyte) {
                        transferbyte = 32 - pSPI->SPI_TXFIFO_SPC;
                        transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                        for (i = 0; i < transferbyte; i++) {
                            pSPI->SPI_TXFIFO_DATA.B[0] = 0xFF;
                        }
                        while( pSPI->SPI_RXFIFO_SPC != transferbyte);
                        for (i = 0; i < transferbyte; i++) {
                            *ptr++ = pSPI->SPI_RXFIFO_DATA.B[0];
                        }
                        outbyte -= transferbyte;
                    }
                }
            }
        #endif
        }
    }

    return MMP_ERR_NONE;

}

#endif

#if 0//(CHIP == PYTHON) 
#define SPI_ID  2
MMPF_SPI_OPERATION m_UsbSpiOp;

MMP_ERR MMPF_SPI_InitUSBPhyReg(MMP_ULONG bufaddr)
{
    AITPS_GBL   pGBL = AITC_BASE_GBL;
    //AITPS_AIC   pAIC = AITC_BASE_AIC;
    AITPS_SPI   pSPI = &AITC_BASE_SPIB->SPI[SPI_ID];

    pGBL->GBL_CLK_EN |= GBL_CLK_SPI;
	//pGBL->GBL_CHIP_CFG_EN |= 0x01;
	//pGBL->GBL_CHIP_CFG &= ~(0x0040);

    pGBL->GBL_RST_SEL = GBL_MODULE_SPI;
    pGBL->GBL_RST_SW_CTL |= GBL_MODULE_SPI;
    RTNA_WAIT_MS(1);
    pGBL->GBL_RST_SW_CTL &= ~(GBL_MODULE_SPI);
	pGBL->GBL_RST_SEL = 0;

    pSPI->SPI_CFG = (MASTER_RX_PAD_CLK | SPI_MASTER_MODE | SS_BURST_MODE | SCLK_IDLE_LOW | SCLK_PHASE0_OP | SS_POLAR_LOW );
    
    pSPI->SPI_WORD_LEN = 8 - 1;
    pSPI->SPI_CLK_DIV = 10;//0xFF;
    pSPI->SPI_DLY_CYCLE = 0;
    pSPI->SPI_WAIT_CYCLE = 0;
    pSPI->SPI_TXFIFO_THD = 0;
    pSPI->SPI_RXFIFO_THD = 0;
    pSPI->SPI_INT_CPU_SR = 0;
    pSPI->SPI_INT_CPU_EN = 0;

    m_UsbSpiOp.ulTxDmaAddr = bufaddr;
    m_UsbSpiOp.ulRxDmaAddr = bufaddr;// + 32;

//    RTNA_DBG_Str0("DMA start addr: ");
//    RTNA_DBG_Long0(bufaddr);
//    RTNA_DBG_Str0("\r\n");
    
    return MMP_ERR_NONE;
}


MMP_ERR MMPF_SPI_WriteUSBPhyReg(MMP_UBYTE Addr, MMP_USHORT data)
{
        AITPS_SPI pSPI = &AITC_BASE_SPIB->SPI[SPI_ID];
        MMP_UBYTE i;

//        RTNA_DBG_Str0("Write addr, data : ");
//        RTNA_DBG_Short0(Addr);
//        RTNA_DBG_Short0(data);
//        RTNA_DBG_Str0("\r\n");

        for(i = 0; i < 4; i++){
            *(MMP_UBYTE *)(m_UsbSpiOp.ulTxDmaAddr + i) = 0;
            *(MMP_UBYTE *)(m_UsbSpiOp.ulRxDmaAddr + i) = 0;
        }
      
        *(MMP_UBYTE *)(m_UsbSpiOp.ulTxDmaAddr) = 0x20;//0x24;
        *(MMP_UBYTE *)(m_UsbSpiOp.ulTxDmaAddr + 1) = (Addr & 0xFE);
        *(MMP_USHORT *)(m_UsbSpiOp.ulTxDmaAddr + 2) = data;
    
        // clear interrupt
        pSPI->SPI_INT_CPU_SR = SPI_TXDMA_DONE | SPI_RXDMA_DONE | SPI_FIFO_TX_DONE;

        pSPI->SPI_TXDMA_ADDR = m_UsbSpiOp.ulTxDmaAddr;
        pSPI->SPI_RXDMA_ADDR = m_UsbSpiOp.ulRxDmaAddr;
        pSPI->SPI_TXDMA_SIZE = 4 - 1;
        pSPI->SPI_RXDMA_SIZE = 4 - 1;
        pSPI->SPI_CFG |= (SPI_TX_EN | SPI_RX_EN| TX_XCH_MODE);
        pSPI->SPI_CTL = (SPI_RX_DMA_START | SPI_TX_DMA_START);
        pSPI->SPI_XCH_CTL = XCH_START;
        
        while( !(pSPI->SPI_INT_CPU_SR & SPI_TXDMA_DONE) );
        //RTNA_DBG_Str0("&");
        //while( !(pSPI->SPI_INT_CPU_SR &SPI_RXDMA_DONE));
        //RTNA_DBG_Str0("@");
        while( !(pSPI->SPI_INT_CPU_SR & SPI_FIFO_TX_DONE) );
        
		return MMP_ERR_NONE;
}

MMP_USHORT MMPF_SPI_ReadUSBPhyReg(MMP_UBYTE Addr)
{
        AITPS_SPI pSPI = &AITC_BASE_SPIB->SPI[SPI_ID];
        MMP_UBYTE i;

        for(i = 0; i < 4; i++){
            *(MMP_UBYTE *)(m_UsbSpiOp.ulTxDmaAddr + i) = 0;
            *(MMP_UBYTE *)(m_UsbSpiOp.ulRxDmaAddr + i) = 0;
        }
      
        *(MMP_UBYTE *)(m_UsbSpiOp.ulTxDmaAddr) = 0x21;//0x25;
        *(MMP_UBYTE *)(m_UsbSpiOp.ulTxDmaAddr + 1) = (Addr & 0xFE);
        
        // clear interrupt
        pSPI->SPI_INT_CPU_SR = SPI_TXDMA_DONE | SPI_RXDMA_DONE | SPI_FIFO_TX_DONE;

        pSPI->SPI_TXDMA_ADDR = m_UsbSpiOp.ulTxDmaAddr;
        pSPI->SPI_RXDMA_ADDR = m_UsbSpiOp.ulRxDmaAddr;
        pSPI->SPI_TXDMA_SIZE = 4 - 1;
        pSPI->SPI_RXDMA_SIZE = 4 - 1;
        pSPI->SPI_CFG |= (SPI_TX_EN | SPI_RX_EN| TX_XCH_MODE);
        pSPI->SPI_CTL = (SPI_RX_DMA_START | SPI_TX_DMA_START);
        pSPI->SPI_XCH_CTL = XCH_START;

        while( !(pSPI->SPI_INT_CPU_SR & SPI_TXDMA_DONE) );
        //RTNA_DBG_Str0("&");
        while( !(pSPI->SPI_INT_CPU_SR & SPI_FIFO_TX_DONE) );
        //RTNA_DBG_Str0("@");
        while( !(pSPI->SPI_INT_CPU_SR & SPI_RXDMA_DONE));

        //if(pSPI->SPI_INT_CPU_SR &SPI_RXDMA_DONE)
        //    RTNA_DBG_Str0("@");

//        RTNA_DBG_Str0("Read addr, data : ");
//        RTNA_DBG_Short0(Addr);
//        RTNA_DBG_Short0(*(MMP_USHORT *)(m_UsbSpiOp.ulRxDmaAddr + 2));
//        RTNA_DBG_Str0("\r\n");
        
		return (*(MMP_USHORT *)(m_UsbSpiOp.ulRxDmaAddr + 2));
}
#endif
/// @}

#endif // for #ifndef _MMPF_SPI_C
