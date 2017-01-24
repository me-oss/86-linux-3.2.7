//==============================================================================
//
//  File        : mmp_register.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REGISTER_H_
#define _MMP_REGISTER_H_

#include <mach/hardware.h>

#include <mach/mmp_err.h>
#include <mach/mmpf_typedef.h>


// ********************************
//   Register Naming Abbreviation
// ********************************

// AUD          : audio
// ADDR         : address (new)
// ADJST        : adjust
// ACC          : accumulate
// AF           : auto focus
// AWB          : auto white balance
// AE           : auto exporsure
// AZOOM        : auto zoom
// BLD          : blend
// BS           : bit stream
// BUF          : buffer
// CFG          : configuration
// CLK          : clock
// CMD          : command
// CNT          : count, counter
// COLR         : color
// COMPEN       : compensation
// COMP         : compress
// CONV         : convert
// CSR          : control and status register
// CTL          : control (new)
// CUR          : current (new)
// DBL          : double
// DBLK         : deblocking
// DEC          : decode (new)
// DLINE        : delay line
// DNSAMP       : down sample
// DNLD         : download
// DEFT         : defect
// Dn           : divided by n
// DSI          : display serial interface
// DSPY         : display
// EN,DIS       : enable, disable
// END          : finish, the end
// FRM          : frame
// FMT          : format
// FOUT         : FIFO out
// GAMA         : gamma
// GBL          : global
// PANL         : panel
// PAL          : palette
// PRED         : prediction (new)
// ENC,DEC      : encode, decode
// FINISH       : finish, consider using DONE if possible(new)
// FREQ         : frequency
// SHFT         : shift
// HSTGRM       : histogram
// H,V          : horizontal, vertical
// INT          : interrupt
// IN,OUT       : input, output
// IDX          : index
// IMG          : image
// LCL          : local
// LS           : lenshading
// LOWBD        : lower bound (new)
// LPF          : low-pass filter
// LRP          : linear interpolation
// MTX          : matrix
// MDL          : module
// NO           : number id
// NUM          : count number
// NR           : noise reduction
// OPT          : option (new)
// OVLY         : overlay
// OFST         : offset
// PWR          : power
// PAR          : parity
// PARM         : paramter
// PHL          : peripheral
// PRIO         : priority
// PREVW        : preview
// POLAR        : polarity
// PST,NEG      : positive, negative
// PIXL         : pixel
// QLTY         : quality
// RES          : resolution (new)
// RC,WC        : read cycle, cycle
// RW,RD,WR     : read/write, read, write
// RX           : receive
// RST          : reset
// ROTE         : rotate
// SEL          : select (new)
// SRC          : source id
// STG          : storage
// ST,ED        : start and end point
// SENSR        : sensor
// SERL,PARL    : serial, parellel
// SR           : status register
// SCAL         : scaling
// SIN,SOUT     : scaling in, scaling out
// SPC          : space
// TBL          : table
// TH           : threshold (new)
// TX           : transfer
// U            : Cb (new)
// UPBND        : upper bound (new)
// UPD          : update
// V            : Cr (new), verticial
// VAL          : value (new)
// VID          : video
// WT           : weight
// W,H          : width , height
// WC           : Write cycle, RC
// WD           : watchdog
// WR           : write
// X,Y          : coordinate X, Y
//

// ADDR         : address
// BD           : bound
// BS           : bit stream
// BUF          : buffer
// CUR          : current
// CTL          : control
// FINISH       : finish, consider using DONE if possible
// HDR          : header
// LOWBD        : lower bound
// OPT          : option
// RES          : resolution
// SEL          : select
// TH           : threshold
// UPBD         : upper bound
// VAL          : value
// ===== naming that is not defined before =====
// BITSTREAM some of them uses BS

// ****************************************************************************
//                              Retina Control Register
// ****************************************************************************

// *********************************************************************
//                              Retina Peripheral Base Address
// *********************************************************************
#include <mach/hardware.h>


#define AIT8455_OPR_P2V(x) AIT_OPR_P2V(x)


#define AITC_BASE_PHY_USBCTL	(0x80001000)
#define AITC_BASE_PHY_USBDMA	(0x80001400)

#define AITC_BASE_PHY_SD1 		(0x80005F00)
#define AITC_BASE_PHY_SD0 		(0x80006300)
#define AITC_BASE_PHY_SIF 		(0x80006700)
#define AITC_BASE_PHY_PSPI 		(0x80006D00)
#define AITC_BASE_PHY_UARTB 	(0x80006A00)
#define AITC_BASE_PHY_UART0 	(0x80006A00)
#define AITC_BASE_PHY_UART1 	(0x80005C00)
#define AITC_BASE_PHY_UART_BASE 	(0x80005C00)

#define AITC_BASE_PHY_I2CM1	(0x80007A00)
#define AITC_BASE_PHY_I2CM2	(0x80007B00)
#define AITC_BASE_PHY_AUD		(0x80007800)
#define AITC_BASE_PHY_I2S_FIFO		(0x80007800)
#define AITC_BASE_PHY_I2S_CTRL		(0x80007828)
#define AITC_BASE_PHY_I2S_MUX		(0x80007870)
#define AITC_BASE_PHY_I2S_TIMER	(0x80007880)
#define AITC_BASE_PHY_AFE		(0x80007F00)
#define AITC_BASE_PHY_AFE_FIFO	(0x80007F00)
#define AITC_BASE_PHY_AFE_GBL		(0x80007F50)
#define AITC_BASE_PHY_AFE_ADC		(0x80007F60)

#define AITC_BASE_PHY_TCB		(0xFFFE0000)         // TCs      Base Address (Timer control block)
#define AITC_BASE_PHY_TC0		(0xFFFE0000)         // TC0      Base Address
#define AITC_BASE_PHY_TC1		(0xFFFE0040)         // TC1      Base Address
#define AITC_BASE_PHY_TC2		(0xFFFE0080)         // TC2      Base Address
#define AITC_BASE_PHY_WD		(0xFFFF8000)         // WD       Base Address

#define AITC_BASE_OPR      (((AIT_REG_B*)   0x80000000)
#define AITC_BASE_H264DEC_CTL			((AITPS_H264DEC_CTL) 			AIT8455_OPR_P2V(0x80000000))
#define AITC_BASE_H264DEC_REF			((AITPS_H264DEC_REF) 			AIT8455_OPR_P2V(0x80000010))
#define AITC_BASE_H264DEC_REC			((AITPS_H264DEC_REC)			AIT8455_OPR_P2V(0x80000060))
#define AITC_BASE_H264DEC_DBLK		((AITPS_H264DEC_DBLK)			AIT8455_OPR_P2V(0x80000070))
#define AITC_BASE_H264DEC_VLD			((AITPS_H264DEC_VLD)			AIT8455_OPR_P2V(0x80000080))
#define AITC_BASE_H264DEC_PARSE_EP3	((AITPS_H264DEC_PARSE_EP3)		AIT8455_OPR_P2V(0x800000F0))
#define AITC_BASE_H264DEC_MB_DATA0	((AITPS_H264DEC_MB_DATA)		AIT8455_OPR_P2V(0x80000100))
#define AITC_BASE_H264DEC_MB_DATA1	((AITPS_H264DEC_MB_DATA)		AIT8455_OPR_P2V(0x80000180))
#define AITC_BASE_H264ENC				((AITPS_H264ENC)            			AIT8455_OPR_P2V(0x80000400))

#define AITC_BASE_PWM				((AITPS_PWM)    		AIT8455_OPR_P2V(0x80000800))
#define AITC_BASE_USBCTL			((AITPS_USB_CTL)		AIT8455_OPR_P2V(AITC_BASE_PHY_USBCTL))
#define AITC_BASE_USBDMA			((AITPS_USB_DMA)	AIT8455_OPR_P2V(AITC_BASE_PHY_USBDMA))
#define AITC_BASE_LS				((AIT_REG_B*)		AIT8455_OPR_P2V(0x80001800))

#if (CHIP == MERCURY)
#define AITC_BASE_TNR				((AITPS_TNR)	0x80003000)		// TNR		Base Address
#define AITC_BASE_RAWPROC2          ((AITPS_RAWPROC2)0x80003910)    // RAWPROC2 Base Address
#define AITC_BASE_IBC       ((AITPS_IBC)    AIT8455_OPR_P2V(0x80005600))             // IBC      Base Address
#endif
#define AITC_BASE_GBL		((AITPS_GBL )   	AIT8455_OPR_P2V(0x80005D00))         // GBL      Base Address
#define AITC_BASE_SD1		((AITPS_SD)		AIT8455_OPR_P2V(AITC_BASE_PHY_SD1))//AIT8455_OPR_P2V(AITC_BASE_PHY_SD1))         // SD       Base Address
#define AITC_BASE_VIF		((AITPS_VIF )   	AIT8455_OPR_P2V(0x80006000))         // VIF      Base Address
#define AITC_BASE_MIPI		((AITPS_MIPI)   	AIT8455_OPR_P2V(0x80006110))         // MIPI     Base Address
#if (CHIP == MERCURY)
#define AITC_BASE_VIF_SNR2  ((AITPS_VIF_SNR2)   AIT8455_OPR_P2V(0x800061C0))         // VIF_SNR2 Base Address
#endif
#define AITC_BASE_JPG		((AITPS_JPG )   	AIT8455_OPR_P2V(0x80006200))         // JPG      Base Address
#define AITC_BASE_SD0		((AITPS_SD)     	AIT8455_OPR_P2V(AITC_BASE_PHY_SD0))         // SD       Base Address
#define AITC_BASE_SCAL		((AITPS_SCAL)	AIT8455_OPR_P2V(0x80006400))         // SCAL     Base Address
#define AITC_BASE_IBC		((AITPS_IBC )		AIT8455_OPR_P2V(0x80006500))         // IBC      Base Address
#define AITC_BASE_GPIO		((AITPS_GPIO)   	AIT8455_OPR_P2V(0x80006600))         // GPIO     Base Address
#define AITC_BASE_SIF		((AITPS_SIF)		AIT8455_OPR_P2V(AITC_BASE_PHY_SIF))

#define AITC_BASE_UARTB	((AITPS_UARTB)	AIT8455_OPR_P2V(AITC_BASE_PHY_UARTB))//AIT8455_OPR_P2V(AITC_BASE_PHY_UARTB))         // US       Base Address
#define AITC_BASE_UART_BASE	((AITPS_US)	AIT8455_OPR_P2V(AITC_BASE_PHY_UART_BASE))//AIT8455_OPR_P2V(AITC_BASE_PHY_UARTB))
#define AITC_BASE_UART0		((AITPS_US)	AIT8455_OPR_P2V(AITC_BASE_PHY_UART0)) // US0 Base Address
#define AITC_BASE_UART1		((AITPS_US)	AIT8455_OPR_P2V(AITC_BASE_PHY_UART1)) // US0 Base Address
#define AITC_BASE_RAWPROC	((AITPS_RAWPROC)AIT8455_OPR_P2V(0x80006B00))         // RAWPROC  Base Address
#define AITC_BASE_ICOB		((AITPS_ICOB)	AIT8455_OPR_P2V(0x80006C00))         // ICOs     Base Address
#define AITC_BASE_PSPI		((AITPS_SPI) 	AIT8455_OPR_P2V(AITC_BASE_PHY_PSPI))         // SPIs     Base Address
#define AITC_BASE_DRAM		((AITPS_DRAM)	AIT8455_OPR_P2V(0x80006E00))
//#define AITC_BASE_DSI          ((AITPS_DSI)    0x80006F00)         // DSI      Base Address
#define AITC_BASE_ISP		((AITPS_ISP )		AIT8455_OPR_P2V(0x80007000))        // ISP      Base Address
#define AITC_BASE_ISP1		((AITPS_ISP1 )	AIT8455_OPR_P2V(0x80007100))        // ISP      Base Address
#define AITC_BASE_ISP2		((AITPS_ISP2 )	AIT8455_OPR_P2V(0x80007200))         // ISP      Base Address
#if (CHIP == MERCURY)
#define AITC_BASE_BAYERSCAL ((AITPS_BAYER_SCAL)	AIT8455_OPR_P2V(0x80007100))         // BAYER SCAL Base Address
#define AITC_BASE_RAWPROC1  ((AITPS_RAWPROC1)	AIT8455_OPR_P2V(0x80007E10))         // RAWPROC1 Base Address
#endif
#define AITC_BASE_DFT		((AIT_REG_B*)	AIT8455_OPR_P2V(0x80007400))
#define AITC_BASE_DMA		((AITPS_DMA)		AIT8455_OPR_P2V(0x80007600))         // DMA      Base Address
#define AITC_BASE_MCI		((AITPS_MCI)		AIT8455_OPR_P2V(0x80007700))
#define AITC_BASE_AUD		((AITPS_AUD)		AIT8455_OPR_P2V(AITC_BASE_PHY_AUD))         // AUDIO    BASE Address
#define 	AITC_BASE_I2S_FIFO	((AITPS_AUD_I2SFIFO)		AIT8455_OPR_P2V(AITC_BASE_PHY_I2S_FIFO))         // AUDIO    BASE Address
#define 	AITC_BASE_I2S_CTRL 	((AITPS_AUD_I2SCTRL)		AIT8455_OPR_P2V(AITC_BASE_PHY_I2S_CTRL))         // AUDIO    BASE Address
#define 	AITC_BASE_I2S_MUX		((AITPS_AUD_I2S_MUX)	AIT8455_OPR_P2V(AITC_BASE_PHY_I2S_MUX))         // AUDIO    BASE Address
#define 	AITC_BASE_I2S_TIMER	((AITPS_AUD_I2STIMER)	AIT8455_OPR_P2V(AITC_BASE_PHY_I2S_TIMER))         // AUDIO    BASE Address

#define AITC_BASE_GRA		((AITPS_GRA )	AIT8455_OPR_P2V(0x80007900))         // GRA      Base Address
#define AITC_BASE_PAD		((AITPS_PAD)		AIT8455_OPR_P2V(0x80009C00))         // PAD      Base Address
#define AITC_BASE_I2CM		((AITPS_I2CMS)	AIT8455_OPR_P2V(AITC_BASE_PHY_I2CM1))//- AIT8455_OPR_PHYS_BASE+AIT8455_OPR_VIRT_BASE))
#define AITC_BASE_TBL_Q	((AIT_REG_B*)	AIT8455_OPR_P2V(0x80007C00))

#define AITC_BASE_AFE		((AITPS_AFE)	AIT8455_OPR_P2V(AITC_BASE_PHY_AFE))			// AUDIO AFE BASE Address

/// @}

#endif // _MMPH_HIF_H_
///@end_ait_only
