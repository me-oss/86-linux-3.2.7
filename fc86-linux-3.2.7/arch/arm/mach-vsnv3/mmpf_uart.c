//==============================================================================
//
//  File        : mmpf_uart.c
//  Description : Firmware UART Control Function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

//#include "includes_fw.h"
#include <mach/mmp_register.h>
#include <mach/mmpf_uart.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_uart.h>
#include <mach/mmpf_pio.h>
//#include "lib_retina.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#if (UART_RXINT_MODE_EN == 1)
#define RX_ENTER_SIGNAL     13
#define RX_SENSITIVE        100
#define RX_QUEUE_SIZE       128
#endif

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

#if (UART_RXINT_MODE_EN == 1)
static  MMP_BYTE    m_bDebugString[MMPF_UART_MAX_COUNT][RX_QUEUE_SIZE];
static  MMP_ULONG   m_bDebugStringLen[MMPF_UART_MAX_COUNT];
MMPF_OS_SEMID       m_semUartDebug[MMPF_UART_MAX_COUNT];
#endif

#if (UART_DMA_MODE_EN == 1)
MMPF_OS_SEMID       m_semUartDmaMode;
static UartCallBackFunc *gUart_CallBackFunc[MMPF_UART_DMA_INT_MAX];
#endif

static MMP_SHORT    m_bUartDmaInUsedId = MMPF_UART_MAX_COUNT;

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : Return the AITPS_US of given id
//  Description : Helper function to get correct pTC from ID
//  Note        : Because timer hardware is split into 2 group. The ID
//                to pTC is handled here 
//  Return      : 
//------------------------------------------------------------------------------
//static __inline AITPS_US GetpUS(MMPF_UART_ID id)
AITPS_US GetpUS(const int mmpf_uart_id)
{
	//AITPS_UART  pUART = AITC_BASE_UART;
	AITPS_UART	pUART  = AITC_BASE_UART_BASE;

    switch(mmpf_uart_id) {
    case MMPF_UART_ID_0:
        return &(pUART->US0);
    case MMPF_UART_ID_1:
        return &(pUART->US1);
    #if (CHIP == MERCURY)
    case MMPF_UART_ID_2:
        return &(pUART->US2);
    case MMPF_UART_ID_3:
        return &(pUART->US3);
    #endif
    default:
        RTNA_DBG_Str(0, "Invalid uart ID!!\r\n");
        return 0;
    }
}

#if (UART_DMA_MODE_EN == 1)
//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_SwitchToDmaMode
//  Description :
//  Note        : This is the 1st step of UART using DMA mode
//------------------------------------------------------------------------------
/** @brief This function set the UART device from normal mode to DMA mode.

This function set the UART device from normal mode to DMA mode.
@param[in] uartId indicate which UART device, please refer the data structure of MMPF_UART_ID
@param[in] bEnable stands for enable switch to DMA mode or back from DMA mode.
@return It reports the status of the operation.
*/
MMP_ERR MMPF_Uart_SwitchToDmaMode(MMPF_UART_ID uartId, MMP_BOOL bEnable)
{
    AITPS_US    pUS = GetpUS(uartId);

    if (bEnable == MMP_TRUE) {
        #if (UART_RXINT_MODE_EN == 1)
        pUS->US_IER &= ~(US_RX_FIFO_OVER_TH);  //close uart RX normal interrupt
        #endif

        #if (UART_DMA_MODE_EN == 1)
        MMPF_OS_AcquireSem(m_semUartDmaMode, 0x0);
        m_bUartDmaInUsedId = uartId;
        #endif
    }
    else {
        if (uartId == m_bUartDmaInUsedId) {
            //Clean DMA mode settings
            pUS->US_CR &= US_DMA_CLEAN;

            #if (UART_DMA_MODE_EN == 1)
            MMPF_OS_ReleaseSem(m_semUartDmaMode);
            m_bUartDmaInUsedId = MMPF_UART_MAX_COUNT;
            #endif

            #if (UART_RXINT_MODE_EN == 1)
            pUS->US_IER = US_RX_FIFO_OVER_TH;  //Resume uart RX normal interrupt
            #endif
        }
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_SetTxDmaMode
//  Description :
//  Note        : This is the 2nd step of UART using DMA TX mode
//------------------------------------------------------------------------------
/** @brief This function set the parameters using by UART DMA TX mode.

This function set the parameters using by UART DMA Tx mode.
@param[in] uartId indicate which UART device, please refer the data structure of MMPF_UART_ID
@param[in] MMPF_UART_DMAMODE indicate which DMA mode to be used, please refer the data structure MMPF_UART_DMAMODE.
@param[in] uartTxStartAddr indicate the Tx DMA start address.
@param[in] uartTxTotalByte indicate number of bytes would be sent (start from start address).
@return It reports the status of the operation.
*/
MMP_ERR MMPF_Uart_SetTxDmaMode(MMPF_UART_ID uartId, MMPF_UART_DMAMODE uartDmaMode, MMP_ULONG uartTxStartAddr, MMP_USHORT uartTxTotalByte)
{
    AITPS_US    pUS = GetpUS(uartId);

    if( m_bUartDmaInUsedId != uartId) {
        RTNA_DBG_Str(0, "uart Device have not got the DMA resource !!\r\n");
        return MMP_ERR_NONE;
    }

    if (uartDmaMode == MMPF_UART_TXDMA) {
        pUS->US_TXDMA_START_ADDR = uartTxStartAddr;
        pUS->US_TXDMA_TOTAL_BYTE = uartTxTotalByte;
    }
    else {
        RTNA_DBG_Str(0, "MMPF_Uart_SetRxDmaMode(): Error TX DMA mode \r\n");
        return MMP_ERR_NONE;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_SetRxDmaMode
//  Description :
//  Note        : This is the 2nd step of UART using DMA RX mode
//------------------------------------------------------------------------------
/** @brief This function set the parameters using by UART DMA RX mode.

This function set the parameters using by UART DMA Tx mode.
@param[in] uartId indicate which UART device, please refer the data structure of MMPF_UART_ID
@param[in] MMPF_UART_DMAMODE indicate which DMA mode to be used, please refer the data structure MMPF_UART_DMAMODE.
@param[in] uartRxStartAddr indicate the RX DMA start address.
@param[in] uartRxEndAddr indicate the RX DMA End address.
@param[in] uartRxLowBoundAddr indicate the RX lower bound address (Using by RX DMA Ring Mode).
@return It reports the status of the operation.
*/
MMP_ERR MMPF_Uart_SetRxDmaMode(MMPF_UART_ID uartId, MMPF_UART_DMAMODE uartDmaMode, MMP_ULONG uartRxStartAddr, MMP_ULONG uartRxEndAddr, MMP_ULONG uartRxLowBoundAddr)
{
    AITPS_US    pUS = GetpUS(uartId);

    if( m_bUartDmaInUsedId != uartId) {
        RTNA_DBG_Str(0, "uart Device have not got the DMA resource !!\r\n");
        return MMP_ERR_NONE;
    }

    if ((uartDmaMode == MMPF_UART_RXDMA) || (uartDmaMode == MMPF_UART_RXDMA_RING)) {
        pUS->US_RXDMA_START_ADDR = uartRxStartAddr;
        pUS->US_RXDMA_END_ADDR   = uartRxEndAddr;
        if(uartDmaMode == MMPF_UART_RXDMA_RING) {
            pUS->US_RXDMA_LB_ADDR = uartRxLowBoundAddr;
        }
    }
    else {
        RTNA_DBG_Str(0, "MMPF_Uart_SetRxDmaMode(): Error RX DMA mode \r\n");
        return MMP_ERR_NONE;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_SetDmaInterruptMode
//  Description :
//  Note        : This is the step of UART using DMA interrupt mode settings. (This step can be done betwee step2 and step3)
//------------------------------------------------------------------------------
/** @brief This function sets the UART DMA interrupt mode.

This function sets the UART DMA interrupt mode.
@param[in] uartId indicate which UART device, please refer the data structure of MMPF_UART_ID
@param[in] MMPF_UART_DMA_INT_MODE indicate which DMA interrupt mode to be used, please refer the data structure MMPF_UART_DMA_INT_MODE.
@param[in] bEnable stands for "enable the related interrupt mode or not".
@param[in] callBackFunc is used as interrupt handler.
@param[in] uartRxThreshold is used by RX DMA mode, when dma count reaches the Threshold and the related interrupt occurs.
@return It reports the status of the operation.
*/
MMP_ERR MMPF_Uart_SetDmaInterruptMode (MMPF_UART_ID uartId, MMPF_UART_DMA_INT_MODE intMode, MMP_BOOL bEnable, UartCallBackFunc* callBackFunc, MMP_USHORT uartRxThreshold)
{
    AITPS_US    pUS = GetpUS(uartId);

    if (bEnable == MMP_TRUE) {
        switch (intMode) {
            case MMPF_UART_TXDMA_FINISH_IENABLE:
                pUS->US_IER |= US_TXDMA_FINISH_IENABLE;
                break;
            case MMPF_UART_RXDMA_THR_IEABLE:
                pUS->US_RXDMA_TOTAL_THR = uartRxThreshold;
                pUS->US_IER |= US_RXDMA_THR_IEABLE;
                break;
            case MMPF_UART_RXDMA_WRITEMEM_IENALBE:
                pUS->US_IER |= US_RXDMA_WRITEMEM_IENALBE;
                break;
            case MMPF_UART_RXDMA_DROPDATA_IENALBE:
                pUS->US_IER |= US_RXDMA_DROPDATA_IENALBE;
                break;
            default:
                RTNA_DBG_Str(0, "Error Dma Interrupt Mode ! \r\n");
                return MMP_ERR_NONE;
                break;
        }

        if (callBackFunc != NULL) {
            gUart_CallBackFunc[intMode] = callBackFunc;
        }
    }
    else {
        switch (intMode) {
            case MMPF_UART_TXDMA_FINISH_IENABLE:
                pUS->US_IER &= (~US_TXDMA_FINISH_IENABLE);
                break;
            case MMPF_UART_RXDMA_THR_IEABLE:
                pUS->US_RXDMA_TOTAL_THR = 0;
                pUS->US_IER &= (~US_RXDMA_THR_IEABLE);
                break;
            case MMPF_UART_RXDMA_WRITEMEM_IENALBE:
                pUS->US_IER &= (~US_RXDMA_WRITEMEM_IENALBE);
                break;
            case MMPF_UART_RXDMA_DROPDATA_IENALBE:
                pUS->US_IER &= (~US_RXDMA_DROPDATA_IENALBE);
                break;
            default:
                RTNA_DBG_Str(0, "Error Dma Interrupt Mode ! \r\n");
                return MMP_ERR_NONE;
                break;
        }
        gUart_CallBackFunc[intMode] = NULL;
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_EnableDmaMode
//  Description :
//  Note        : This function is the 3rd step of UART DMA settings.
//------------------------------------------------------------------------------
/** @brief This function is used to enable or disable UART DMA mode.

This function is used to enable or disable UART DMA mode.
@param[in] uartId indicate which UART device, please refer the data structure of MMPF_UART_ID
@param[in] MMPF_UART_DMAMODE indicate which DMA mode to be used, please refer the data structure MMPF_UART_DMAMODE.
@param[in] bEnable stands for "enable the related mode or not".
@return It reports the status of the operation.
*/
MMP_ERR MMPF_Uart_EnableDmaMode(MMPF_UART_ID uartId, MMPF_UART_DMAMODE uartDmaMode, MMP_BOOL bEnable)
{
    AITPS_US    pUS = GetpUS(uartId);

    if (bEnable == MMP_TRUE) {
        switch (uartDmaMode) {
            case MMPF_UART_RXDMA_RING:
                pUS->US_CR |= US_RXDMA_RING_ENABLE;
                break;
            case MMPF_UART_TXDMA:
                pUS->US_CR |= US_TXDMA_ENABLE;
                break;
            case MMPF_UART_RXDMA:
                pUS->US_CR |= US_RXDMA_ENABLE;
                break;
            default:
                RTNA_DBG_Str(0, "Error! wrong DMA mode enable !\r\n");
                break;
        }
    }
    else {
        switch (uartDmaMode) {
            case MMPF_UART_RXDMA_RING:
                pUS->US_CR &= (~US_RXDMA_RING_ENABLE);
                break;
            case MMPF_UART_TXDMA:
                pUS->US_CR &= (~US_TXDMA_ENABLE);
                break;
            case MMPF_UART_RXDMA:
                pUS->US_CR &= (~US_RXDMA_ENABLE);
                break;
            default:
                RTNA_DBG_Str(0, "Error! wrong DMA mode disable !\r\n");
                break;
        }
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_EnableDmaMode
//  Description :
//  Note        : This operation need to be done after settings done or DMA RX restart.
//------------------------------------------------------------------------------
/** @brief This function to enable UART DMA RX operations.

This function to enable UART DMA RX operations.
@param[in] uartId indicate which UART device, please refer the data structure of MMPF_UART_ID
@return It reports the status of the operation.
*/
MMP_ERR MMPF_Uart_RxDmaStart(MMPF_UART_ID uartId)
{
    AITPS_US    pUS = GetpUS(uartId);

    pUS->US_CR |= US_RXDMA_START_FLAG;

    return MMP_ERR_NONE;
}
#endif

#if 0
//------------------------------------------------------------------------------
//  Function    : MMPF_UART_ISR
//  Description :
//  Note        :
//------------------------------------------------------------------------------
void MMPF_UART_ISR(void)
{
    #if (UART_DBG_EN == 0x1)
    #if (UART_DMA_MODE_EN == 1)
    AITPS_US    pUS;
    MMP_ULONG   len, i, j = 0;
    #endif

    if (m_bUartDmaInUsedId != MMPF_UART_MAX_COUNT) {  //UART DMA mode
        #if (UART_DMA_MODE_EN == 1)
        pUS = GetpUS(j);
        if (pUS->US_ISR & US_TXDMA_FINISH_IENABLE) {
            pUS->US_ISR &= US_TXDMA_FINISH_IENABLE;  //clean interrupt status
            if (gUart_CallBackFunc[MMPF_UART_TXDMA_FINISH_IENABLE] != NULL) {
                (*gUart_CallBackFunc[MMPF_UART_TXDMA_FINISH_IENABLE]) ();
            }
        }
        else if (pUS->US_ISR & US_RXDMA_THR_IEABLE) {
            pUS->US_ISR &= US_RXDMA_THR_IEABLE; //clean interrupt status
            if (gUart_CallBackFunc[MMPF_UART_RXDMA_THR_IEABLE] != NULL) {
                (*gUart_CallBackFunc[MMPF_UART_RXDMA_THR_IEABLE]) ();
            }
        }
        else if (pUS->US_ISR & US_RXDMA_WRITEMEM_IENALBE) {
            pUS->US_ISR &= US_RXDMA_WRITEMEM_IENALBE;   //clean interrupt status
            if (gUart_CallBackFunc[MMPF_UART_RXDMA_WRITEMEM_IENALBE] != NULL) {
                (*gUart_CallBackFunc[MMPF_UART_RXDMA_WRITEMEM_IENALBE]) ();
            }
        }
        else if (pUS->US_ISR & US_RXDMA_DROPDATA_IENALBE) {
            pUS->US_ISR &= US_RXDMA_DROPDATA_IENALBE;   //clean interrupt status
            if (gUart_CallBackFunc[MMPF_UART_RXDMA_DROPDATA_IENALBE] != NULL) {
                (*gUart_CallBackFunc[MMPF_UART_RXDMA_DROPDATA_IENALBE]) ();
            }
        }
        #endif
    }
    else {
        #if (UART_RXINT_MODE_EN == 1)
        for (j = 0; j < MMPF_UART_MAX_COUNT; j++) {

            pUS = GetpUS(j);

            if (pUS->US_ISR & US_RX_FIFO_OVER_TH) {

                #if (CHIP == VSN_V3)||(CHIP == MERCURY)
                len = pUS->US_RX_FIFO_DATA_CNT;
                #endif

                for(i = 0; i < len; i++) {
                    m_bDebugString[j][(m_bDebugStringLen[j] + i)%RX_QUEUE_SIZE] = pUS->US_RXPR;
                    MMPF_Uart_Write(DEBUG_UART_NUM, &(m_bDebugString[j][m_bDebugStringLen[j]]), 1);
                }

                m_bDebugStringLen[j] += len;

                if (m_bDebugStringLen[j] >= RX_QUEUE_SIZE) {
                    RTNA_DBG_Str(0, "Error: UART RX overflow\r\n");
                }
                break;
            }
        }
        #endif
    }
    #endif //#if (UART_DBG_EN == 0x1)
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_GetDebugString
//  Description : This function will send back the string until the user enter the "Enter"
//  Note        :
//------------------------------------------------------------------------------
#if 0 //(UART_RXINT_MODE_EN == 1)
MMP_ERR MMPF_Uart_GetDebugString(MMPF_UART_ID uartId, MMP_BYTE *bDebugString,
                                MMP_ULONG *ulDebugStringLength)
{
    MMP_LONG    i, j;
    MMP_BOOL    bEnterDetect = MMP_FALSE;
    AITPS_US    pUS = GetpUS(uartId);

    MMPF_Uart_Init();

    MMPF_OS_AcquireSem(m_semUartDebug[uartId], 0);
    
    m_bDebugStringLen[uartId] = 0;
    
    pUS->US_CR |= US_RXEN;

    while(bEnterDetect == MMP_FALSE) 
    {
        for(i = m_bDebugStringLen[uartId]; i >= 0; i--) 
        {
            //Copy string except the "Enter" signal
            if( m_bDebugString[uartId][i] == RX_ENTER_SIGNAL) {
                *ulDebugStringLength = i;
                for (j = 0; j < i; j++) {
                    bDebugString[j] = m_bDebugString[uartId][j];
                }
                bDebugString[i] = '\0';
                bEnterDetect = MMP_TRUE;
                MEMSET0(&m_bDebugString[uartId]);
                break;
            }
        }
        MMPF_OS_Sleep(RX_SENSITIVE);
    }
    
    pUS->US_CR &= ~(US_RXEN);
    
    MMPF_OS_ReleaseSem(m_semUartDebug[uartId]);
    
    return MMP_ERR_NONE;
}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_ConfigPad
//  Description : Configure the UARR pads.
//  Note        :
//------------------------------------------------------------------------------
//static MMP_ERR MMPF_Uart_ConfigPad(MMPF_UART_ID uartId, MMPF_UART_PADSET padset)
static MMP_ERR MMPF_Uart_ConfigPad(MMPF_UART_ID uartId,MMPF_UART_ATTRIBUTE *attr)
{
    AITPS_GBL   pGBL = AITC_BASE_GBL;
    MMPF_UART_PADSET padset = attr->padset;

    #if (CHIP == VSN_V3)
    if(uartId==0)
    {
        // Clean PAD config first.
        pGBL->GBL_IO_CTL1 &= ~(GBL_UART_TX_PAD0 | GBL_UART_TX_PAD1);
        pGBL->GBL_IO_CTL3 &= ~(GBL_UART_RX_PAD0 | GBL_UART_RX_PAD1 |
                                GBL_UART_RX_PAD2 | GBL_UART_TX_PAD2);

    switch(padset) {
    case MMPF_UART_PADSET_0:    // use AGPIO0 as uart TX
        pGBL->GBL_IO_CTL1 |= GBL_UART_TX_PAD0;
        #if (UART_RXINT_MODE_EN == 0x1)
        pGBL->GBL_IO_CTL3 |= GBL_UART_RX_PAD0;
        #endif
        break;
    case MMPF_UART_PADSET_1:    // use PSNR_D8 as uart TX
        pGBL->GBL_IO_CTL1 |= GBL_UART_TX_PAD1;
        #if (UART_RXINT_MODE_EN == 0x1)
        pGBL->GBL_IO_CTL3 |= GBL_UART_RX_PAD1;
        #endif
        break;
    case MMPF_UART_PADSET_2:    // use BGPIO14 as uart tx
        pGBL->GBL_IO_CTL3 |= GBL_UART_TX_PAD2;
        #if (UART_RXINT_MODE_EN == 0x1)
        pGBL->GBL_IO_CTL3 |= GBL_UART_RX_PAD2;
        #endif
        break;
    default:
        RTNA_DBG_Str(0, "Un-supported uart PAD !!\r\n");
        return MMP_UART_ERR_PARAMETER;
        break;
		}
    }
    else if(uartId==1)
    {
    	pGBL->GBL_LCD_BYPASS_CTL1 &= ~(GBL_UART1_TX_PAD0|GBL_UART1_RX_PAD0|
    			                       GBL_UART1_TX_PAD1|GBL_UART1_RX_PAD1);

		switch(padset) {
		case MMPF_UART_PADSET_0:    // use AGPIO0 as uart TX
			MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO34,0); //TX
			MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO35,0); //RX
			if(attr->mode & AIT_UART_RTSCTS)
			{
				MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO32,0); //CTS
				MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO33,0); //RTS
			}
			pGBL->GBL_LCD_BYPASS_CTL1 |= GBL_UART1_TX_PAD0;
			#if (UART_RXINT_MODE_EN == 0x1)
			pGBL->GBL_LCD_BYPASS_CTL1 |= GBL_UART1_RX_PAD0;
			#endif
			break;
		case MMPF_UART_PADSET_1:    // use PSNR_D8 as uart TX
			pGBL->GBL_LCD_BYPASS_CTL1 |= GBL_UART1_TX_PAD1;
			#if (UART_RXINT_MODE_EN == 0x1)
			pGBL->GBL_LCD_BYPASS_CTL1 |= GBL_UART1_RX_PAD1;
			#endif
			break;
		default:
			//RTNA_DBG_Str(0, "Un-supported uart PAD !!\r\n");
			return MMP_UART_ERR_PARAMETER;
			break;
		}
    }
    #endif

    #if (CHIP == MERCURY)
    switch(uartId) {
    case MMPF_UART_ID_0:
        // Clean PAD config first
        pGBL->GBL_IO_CTL3 &= ~(GBL_UART0_TX_PAD_MASK);
        pGBL->GBL_IO_CTL3 |= GBL_UART0_TX_PAD(padset);
        #if (UART_RXINT_MODE_EN == 0x1)
        pGBL->GBL_LCD_BYPASS_CTL1 &= ~(GBL_UART0_RX_PAD_MASK);
        pGBL->GBL_LCD_BYPASS_CTL1 |= GBL_UART0_RX_PAD(padset);
        #endif
        break;
    case MMPF_UART_ID_1:
        pGBL->GBL_UART1_PAD_CTL &= ~(GBL_UART1_TX_PAD_MASK|GBL_UART1_RX_PAD_MASK);
        pGBL->GBL_UART1_PAD_CTL |= GBL_UART1_TX_PAD(padset);
        #if (UART_RXINT_MODE_EN == 0x1)
        pGBL->GBL_UART1_PAD_CTL |= GBL_UART1_RX_PAD(padset);
        #endif
        break;
    case MMPF_UART_ID_2:
        pGBL->GBL_UART2_PAD_CTL &= ~(GBL_UART2_TX_PAD_MASK|GBL_UART2_RX_PAD_MASK);
        pGBL->GBL_UART2_PAD_CTL |= GBL_UART2_TX_PAD(padset);
        #if (UART_RXINT_MODE_EN == 0x1)
        pGBL->GBL_UART2_PAD_CTL |= GBL_UART2_RX_PAD(padset);
        #endif
        break;
    case MMPF_UART_ID_3:
        pGBL->GBL_UART3_PAD_CTL &= ~(GBL_UART3_TX_PAD_MASK|GBL_UART3_RX_PAD_MASK);
        pGBL->GBL_UART3_PAD_CTL |= GBL_UART3_TX_PAD(padset);
        #if (UART_RXINT_MODE_EN == 0x1)
        pGBL->GBL_UART3_PAD_CTL |= GBL_UART3_RX_PAD(padset);
        #endif
        break;
    default:
        RTNA_DBG_Str(0, "Invalid uart ID!!\r\n");
        return MMP_UART_ERR_PARAMETER;
    }
    #endif

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_Init
//  Description : Initial the semaphore and call-back functions.
//------------------------------------------------------------------------------
#if 0
MMP_ERR MMPF_Uart_Init(void)
{
    #if (UART_DBG_EN == 0x1)
    #if (UART_RXINT_MODE_EN == 1)
    //To make sure the semaphore initial is called only once.
    static MMP_BOOL bUartInitFlag = MMP_FALSE;
    AITPS_AIC   pAIC = AITC_BASE_AIC;
    MMP_USHORT  i = 0;

    if (!bUartInitFlag) 
    {
        for (i = 0; i < MMPF_UART_MAX_COUNT; i++) {
            m_semUartDebug[i] = MMPF_OS_CreateSem(1);
        }

        RTNA_AIC_Open(pAIC, AIC_SRC_UART, uart_isr_a,
                        AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 3);
        RTNA_AIC_IRQ_En(pAIC, AIC_SRC_UART);

        #if (UART_DMA_MODE_EN == 1)
        m_semUartDmaMode =  MMPF_OS_CreateSem(1);
        
        for (i = 0; i < MMPF_UART_DMA_INT_MAX; i++) {
            gUart_CallBackFunc[i] = NULL;
        }
        #endif
        
        bUartInitFlag = MMP_TRUE;
        RTNA_DBG_Str0("UART enable interrupt !\r\n");
    }
    #endif
    #endif
    return  MMP_ERR_NONE;
}
#endif
//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_Open
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_Uart_Open(MMPF_UART_ID uartId, MMPF_UART_ATTRIBUTE *uartattribute)
{
    //#if (UART_DBG_EN == 0x1)
    AITPS_US    pUS = GetpUS(uartId);

    // PAD config for UART interface
    //MMPF_Uart_ConfigPad(uartId, uartattribute->padset);
    MMPF_Uart_ConfigPad(uartId, uartattribute);

    MMPF_Uart_Close(uartId);

    // Define the baud rate divisor register
    pUS->US_BRGR = (((uartattribute->ulMasterclk << 1) /
                    uartattribute->ulBaudrate) + 1) >> 1;

    //printk(KERN_ERR" pUS->US_BRGR : %x \n",pUS->US_BRGR);

    if(uartattribute->mode & AIT_UART_RTSCTS) //if hw flow control is enabled
    {
    	//pUS->US_CR = US_ASYNC_MODE | US_TXEN | US_RXEN |		 	 	 	 US_CTS_RTS_ENABLE | US_CTS_RTS_MODE1 | US_CTS_RTS_HOST_MODE | 0x30;
	pUS->US_CR = US_TXEN | US_RXEN | US_CTS_RTS_ENABLE | US_CTS_RTS_MODE1 | US_CTS_RTS_HOST_MODE | 0x30;
//AITJFY

    				 //US_CTS_RTS_ENABLE | US_CTS_RTS_MODE1 | US_CTS_RTS_HOST_MODE | US_CTS_RTS_ACTIVE_H | 0x30; //for testing
    }
    else
    pUS->US_CR = US_ASYNC_MODE | US_TXEN | US_RXEN; //enable TX,RX

    #if (UART_RXINT_MODE_EN == 1)
    pUS->US_FTHR &= ~(US_RX_FIFO_TH_MASK);
    pUS->US_FTHR |= US_RX_FIFO_TH(1);
    pUS->US_IER = US_RX_FIFO_OVER_TH;		///Enable RX_FIFO_OVER_THRES interrupt
    pUS->US_ISR &= pUS->US_IER;
    #endif

    //pUS->US_CR = US_ASYNC_MODE | US_TXEN | US_RXEN;
    //#endif

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_Write
//  Description : Debug output a string
//------------------------------------------------------------------------------
MMP_ERR MMPF_Uart_Write(MMPF_UART_ID uartId, const char *str, MMP_ULONG ulLength)
{
    #if (UART_DBG_EN == 0x1)
    AITPS_US    pUS = GetpUS(uartId);
    MMP_ULONG   i, txcnt, j = 0;

    while(ulLength) 
    {
        #if (CHIP == VSN_V3)||(CHIP == MERCURY)
        txcnt = pUS->US_TX_FIFO_DATA_CNT;
        #endif

        if(txcnt) {
            if(txcnt > ulLength) {
                txcnt = ulLength;
            }
            for(i = 0;i < txcnt;i++,j++) {
                pUS->US_TXPR = *(str+j);
            }
            ulLength -= txcnt;
        }
    }
    #endif

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_Close
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_Uart_Close(MMPF_UART_ID uartId)
{
    //#if (UART_DBG_EN == 0x1)
    AITPS_US    pUS = GetpUS(uartId);

    // Disable interrupts
    pUS->US_IER = 0;
    // Reset receiver and transmitter
    pUS->US_CR = US_RSTRX | US_RSTTX | US_RXDIS | US_TXDIS;
    //#endif

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_TryRead
//  Description : Non-blocking read, return the actual read chars
//------------------------------------------------------------------------------
MMP_ULONG MMPF_Uart_TryRead(MMPF_UART_ID uartId, MMP_UBYTE *buf, MMP_ULONG ulMaxReadLen)
{
    #if (UART_DBG_EN == 0x1)
    AITPS_US    pUS = GetpUS(uartId);
    MMP_ULONG   nr = 0;

    while ((pUS->US_RX_FIFO_DATA_CNT) && (nr < ulMaxReadLen)) {
        buf[nr++] = pUS->US_RXPR;
    }

    return nr;
    #else
    return 0;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_CheckState
//  Description :
//------------------------------------------------------------------------------
MMP_BOOL MMPF_Uart_CheckState(MMPF_UART_ID uartId, MMPF_UART_STAT State)
{
    #if (UART_DBG_EN == 0x1)
    AITPS_US    pUS = GetpUS(uartId);

    switch (State) {
    case MMPF_UART_STAT_TXEMPTY:
        return (pUS->US_ISR & US_TX_FIFO_EMPTY)? MMP_TRUE: MMP_FALSE;
    case MMPF_UART_STAT_RXEMPTY:
        return (pUS->US_ISR & US_RX_FIFO_EMPTY)? MMP_TRUE: MMP_FALSE;
    case MMPF_UART_STAT_TXUNDERTH:
        return (pUS->US_ISR & US_TX_FIFO_UNDER_TH)? MMP_TRUE: MMP_FALSE;
    case MMPF_UART_STAT_RXOVERTH:
        return (pUS->US_ISR & US_RX_FIFO_OVER_TH)? MMP_TRUE: MMP_FALSE;
    default:
        return MMP_FALSE;
    }
    #else
    return MMP_FALSE;
    #endif
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_Reset
//  Description : Reset RX, TX or both
//------------------------------------------------------------------------------
MMP_ERR MMPF_Uart_Reset(MMPF_UART_ID uartId, MMPF_UART_DIRECTION direction)
{
    #if (UART_DBG_EN == 0x1)
    AITPS_US    pUS = GetpUS(uartId);
    MMP_ULONG   flags = 0;

    if (direction & MMPF_UART_DIRECTION_RX) {
        flags |= US_RSTRX;
    }
    if (direction & MMPF_UART_DIRECTION_TX) {
        flags |= US_RSTTX;
    }
    pUS->US_CR |= flags;

    #endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Uart_Reset
//  Description : Reset RX, TX or both
//------------------------------------------------------------------------------
MMP_ERR MMPF_Uart_SetInterruptEnable(MMPF_UART_ID uartId, MMPF_UART_EVENT event, MMP_BOOL bEnable)
{
    //#if (UART_DBG_EN == 0x1)
    static const MMP_ULONG flag[MMPF_UART_EVENT_MAX] = {
        US_RX_FIFO_OVER_TH, US_TX_FIFO_UNDER_TH
    };
    AITPS_US pUS = GetpUS(uartId);

    if (bEnable) {
        //if (flag[event] & US_SR_W1C_MASK) {
            //pUS->US_ISR |= flag[event];
        //}
        //pUS->US_IER |= flag[event];

    } else {
        //pUS->US_IER &= ~(flag[event]);
        //pUS->US_ISR |= flag[event];
    }

    //#endif

    return MMP_ERR_NONE;
}


