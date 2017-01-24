//==============================================================================
//
//  File        : reg_retina.h
//  Description : Retina regsiter definition header file
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================


#ifndef _REG_RETINA_H_
#define _REG_RETINA_H_

#include <mach/mmpf_typedef.h>


//#include	"config_fw.h"
#include <mach/hardware.h>

//------------------------------
// Register Naming Abbreviation
//------------------------------

// AUD          : audio
// ADJST        : adjust
// ACC          : accumulate
// AF           : auto focus
// AWB          : auto white balance
// AE           : auto exporsure
// AZOOM        : auto zoom
// BLD          : blend
// CTL          : control
// CLK          : clock
// COMPEN       : compensation
// COMP         : compress
// CSR          : control and status register
// CONV         : convert
// CNT          : count
// CFG          : configuration
// COLR         : color
// CMD          : command
// DBL          : double
// DBLK         : deblocking
// DLINE        : delay line
// DNSAMP       : down sample
// DNLD         : download
// DEFT         : defect
// Dn           : divided by n
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
// ENC,DEC      : encode, decode
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
// LPF          : low-pass filter
// LRP          : linear interpolation
// MTX          : matrix
// MDL          : module
// NO           : number id
// NUM          : count number
// NR           : noise reduction
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
// RC,WC        : read cycle, cycle
// RW,RD,WR     : read/write, read, write
// RX           : receive
// RST          : reset
// ROTE         : rotate
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
// TOUT         : timeout
// TX           : transfer
// UPBND        : upper bound
// UPD          : update
// VID          : video
// WT           : weight
// W,H          : width , height
// WD           : watchdog
// X,Y          : coordinate X, Y
//


//==============================================================================
//
//                              Retina Control Register Definitions
//
//==============================================================================


//-----------------------------
// ISP Structure (0x8000 0400)
//-----------------------------

typedef struct _AITS_ISP {
    AIT_REG_W   ISP_COLOR_ID;                                            // 0x00
    AIT_REG_W                           _x02[97];
    AIT_REG_B   ISP_INT_HOST_EN;                                         // 0xC4
    AIT_REG_B   ISP_INT_HOST_SR;
    AIT_REG_B   ISP_INT_CPU_EN;
    AIT_REG_B   ISP_INT_CPU_SR;
        /*-DEFINE-----------------------------------------------------*/
        #define     ISP_INT_FRAME_START     0x01
        #define     ISP_INT_AE_CALC_DONE    0x02
        #define     ISP_INT_AF_CALC_DONE    0x04
        #define     ISP_INT_AWB_CALC_DONE   0x08
        #define     ISP_INT_FRAME_END	    0x10
        #define     ISP_INT_ALL             0x1F
        /*------------------------------------------------------------*/
} AITS_ISP, *AITPS_ISP;

//------------------------------
// HINT structure (0x8000 9800)
//------------------------------

typedef struct _AITS_HINT {
    AIT_REG_W   HINT_CTL;                                                // 0x00
        /*-DEFINE-----------------------------------------------------*/
		#define	HINT_INT_EN				0x01
		#define	HINT_HIGH_ASSERT		0x00
		#define	HINT_INT_ASSERT			0x02
        /*------------------------------------------------------------*/
    AIT_REG_W  			_0x2[7];
    AIT_REG_D   HINT_EN;                                              	 // 0x10	
        /*-DEFINE-----------------------------------------------------*/
		#define	HINT_VIF_INT			0x00000001
		#define	HINT_COLR_INT			0x00000002
		#define	HINT_JPG_INT			0x00000004
		#define	HINT_LCD_INT			0x00000008
		#define	HINT_GRA_INT			0x00000010
		#define	HINT_SD_INT				0x00000040
		#define	HINT_CPU_INT			0x00000080
		#define	HINT_USB_INT			0x00001000
		#define	HINT_SM_INT 			0x00004000
		#define	HINT_GPIO_INT			0x00010000
		#define	HINT_IIS_INT			0x00020000
		#define	HINT_AFE_INT			0x00040000
		#define	HINT_AFEFIFO_INT		0x00080000
		#define	HINT_AUDIOFIFO_INT		0x00100000
		#define	HINT_VIDEO_INT			0x00200000
		#define	HINT_MEMC_INT			0x00400000
		#define	HINT_VLD_INT			0x00800000
		#define	HINT_ACDCPREC_INT		0x01000000
		#define	HINT_DBLOCK_INT			0x08000000
        /*------------------------------------------------------------*/
    AIT_REG_D   HINT_SR;                                              	 // 0x14	
} AITS_HINT, *AITPS_HINT;

//-----------------------------
// PAD Structure (0x8000 9C00)
//-----------------------------
typedef struct _AITS_PAD {
    AIT_REG_B                           _x00[64];
    AIT_REG_B   PAD_PS_RST;
    AIT_REG_B   PAD_PHSYNC;
    AIT_REG_B   PAD_PVSYNC;
    AIT_REG_B   PAD_PSEN;
    AIT_REG_B   PAD_PSDA;
    AIT_REG_B   PAD_PSCK;
    AIT_REG_B   PAD_PPXL_CLK;
    AIT_REG_B   PAD_PDCLK;
#if 0 // ooxooxx
    AIT_REG_B                           _x48[184];

#else
    AIT_REG_B                           _x48[8];
    AIT_REG_B   PAD_PD0;
    AIT_REG_B   PAD_PD1;
    AIT_REG_B   PAD_PD4_11;
    AIT_REG_B   PAD_PSGPIO;
    AIT_REG_B   PAD_PD2;
    AIT_REG_B   PAD_PD3;
    AIT_REG_B                           _x56[170];
#endif
    AIT_REG_B   PAD_GPIO[64];
        /*-DEFINE-----------------------------------------------------*/
        #define     PAD_SCHMITT_TRIG        0x01
        #define     PAD_PULL_LOW            0x02
        #define     PAD_PULL_HIGH           0x04
        #define     PAD_SLEW_RATE_SLOW      0x07
        #define     PAD_E4_CURRENT          0x20
        #define     PAD_E8_CURRENT          0x40
        /*------------------------------------------------------------*/
} AITS_PAD, *AITPS_PAD;

//-----------------------------
// MCI structure (0x8000 B000)
//-----------------------------

typedef struct _AITS_MCI {
    AIT_REG_W   MCI_FB_CTL;                                             // 0x00
    AIT_REG_W                           _x02[7];

    AIT_REG_B   MCI_EXTM_TYPE;
        /*-DEFINE-----------------------------------------------------*/
        #define MCI_EXTM_NONE               0x00
        #define MCI_EXTM_SRAM               0x01
        #define MCI_EXTM_DRAM               0x02
        /*------------------------------------------------------------*/
    AIT_REG_B   MCI_DRAM_FMT;
    AIT_REG_B   MCI_SRAM_PAGE;
    AIT_REG_B   MCI_SRAM_SEL;
    AIT_REG_B   MCI_SRAM_RD_CYC;
    AIT_REG_B   MCI_SRAM_WR_CYC;
    AIT_REG_B   MCI_SRAM_WR_EN_CYC;
    AIT_REG_B   MCI_SRAM_WR_PRECHARGE;
    AIT_REG_B   MCI_SRAM_WR_TIMEING;
    AIT_REG_B                           _x19[7];

    AIT_REG_B   MCI_WT_MAJOR_SRC0;                                      // 0x20
    AIT_REG_B   MCI_WT_MAJOR_SRC1;
    AIT_REG_B   MCI_WT_MAJOR_SRC2;
    AIT_REG_B   MCI_WT_MAJOR_SRC3;
    AIT_REG_B   MCI_WT_MAJOR_SRC4;
    AIT_REG_B   MCI_WT_MAJOR_SRC5;
    AIT_REG_B                           _x26[10];

    AIT_REG_B   MCI_WT_MINOR_SRC0;                                      // 0x30
    AIT_REG_B   MCI_WT_MINOR_SRC1;
    AIT_REG_B   MCI_WT_MINOR_SRC2;
    AIT_REG_B   MCI_WT_MINOR_SRC3;
    AIT_REG_B   MCI_WT_MINOR_SRC4;
    AIT_REG_B   MCI_WT_MINOR_SRC5;
    AIT_REG_B   MCI_WT_MINOR_SRC6;
    AIT_REG_B   MCI_WT_MINOR_SRC7;
    AIT_REG_B   MCI_WT_MINOR_SRC8;
    AIT_REG_B   MCI_WT_MINOR_SRC9;
    AIT_REG_B   MCI_WT_MINOR_SRCA;
    AIT_REG_B                           _x3B[5];
} AITS_MCI, *AITPS_MCI;


// Peripheral Register of Retina
#ifndef BUILD_HOST


//==============================================================================
//
//                              Retina Peripheral Control Register Definitions
//
//==============================================================================

//------------------------------------------------------
// TC Structure (0xFFFE 0000, 0xFFFE 0040, 0xFFFE 0080)
//------------------------------------------------------

typedef struct _AITS_TC {
    AIT_REG_D   TC_CCR;             // Channel Control Register         // 0x00
    AIT_REG_D   TC_CMR;             // Channel Mode Register
    AIT_REG_D                           _x08[2];

    AIT_REG_D   TC_CVR;             // Counter Value                    // 0x10
    AIT_REG_D                           _x14[2];
    AIT_REG_D   TC_RC;              // Register Compare C
                                    //
    AIT_REG_D   TC_SR;              // Status Register                  // 0x20
    AIT_REG_D   TC_IER;             // Interrupt Enable Register
    AIT_REG_D   TC_IDR;             // Interrupt Disable Register
    AIT_REG_D   TC_IMR;             // Interrupt Mask Register

    AIT_REG_D                           _x30[4];                        // 0x30
} AITS_TC, *AITPS_TC;

#define TC_REG_OFFSET(reg) offsetof(AITS_TC,reg)

#define ait_tc_readl(base,reg) \
	readl((void __iomem *)(base) + TC_REG_OFFSET(reg))
#define ait_tc_writel(base,reg,value) \
	writel(value, (void __iomem *)(base) + TC_REG_OFFSET(reg))

#define AIT_TC_SET_CCR(base,value) \
	ait_tc_writel(base,TC_CCR,value)

#define AIT_TC_SET_CMR(base,value) \
	ait_tc_writel(base,TC_CMR,value)
#define AIT_TC_GET_CMR(base)\
	ait_tc_readl(base,TC_CMR)

#define AIT_TC_GET_CVR(base)\
	ait_tc_readl(base,TC_CVR)

#define AIT_TC_SET_RC(base,value) \
	ait_tc_writel(base,TC_RC,value)
#define AIT_TC_GET_RC(base) \
	ait_tc_readl(base,TC_RC)

#define AIT_TC_SET_SR(base,value) \
	ait_tc_writel(base,TC_SR,value)
#define AIT_TC_GET_SR(base) \
	ait_tc_readl(base,TC_SR)

#define AIT_TC_SET_IER(base,value) \
	ait_tc_writel(base,TC_IER,value)

#define AIT_TC_SET_IDR(base,value) \
	ait_tc_writel(base,TC_IDR,value)

#define AIT_TC_GET_IMR(base) \
	ait_tc_readl(base,TC_IMR)


typedef struct _AITS_TCB {
    AITS_TC     TC[3];				//TC 0,1,2
    AIT_REG_D   TC_BCR;             // Block Control Register
    AIT_REG_D                           _xC4;
    AIT_REG_D   TC_DBR;             // Debug Mode Register
    AIT_REG_D                           _xCC;
    AIT_REG_D							_xD0[4];
    AIT_REG_D							_xE0[4];
    AIT_REG_D							_xF0[4];
	AITS_TC     TC2[3]; 			//TC2[0,1,2] =  Timer[3,4,5]
} AITS_TCB, *AITPS_TCB;

// -------- TC_CCR : Timer Counter Control Register Bits Definition --------
#define TC_CLKEN                    ((unsigned short)0x1 << 0)
#define TC_CLKDIS                   ((unsigned short)0x1 << 1)
#define TC_SWTRG                    ((unsigned short)0x1 << 2)

// -------- TC_CMR : Timer Counter Channel Mode Register Bits Definition --------
#define TC_CPCTRG                   ((unsigned short)0x1 << 14)

#define TC_CLK_MCK_D2               0x0000
#define TC_CLK_MCK_D8               0x0001
#define TC_CLK_MCK_D32              0x0002
#define TC_CLK_MCK_D128             0x0003
#define TC_CLK_MCK_D1024            0x0004
#define TC_CLK_MCK_D4               0x0005
#define TC_CLK_MCK_D16              0x0006
#define TC_CLK_MCK                  0x0007

// -------- TC_SR : Timer Counter Status Register Bits Definition --------
#define TC_COVFS                    ((unsigned int)0x1 << 0)            // Counter Overflow Status
#define TC_CPCS                     ((unsigned int)0x1 << 4)            // RC Compare Status
#define TC_CLKSTA                   ((unsigned int)0x1 << 16)           // Clock enable status

// -------- TC_BCR : Timer Counter Block Control Register Bits Definition --------
#define TC_SYNC                     ((unsigned short)0x1 << 0)          // Synchronisation Trigger

// -------- TC_DBR : ICE Debug Mode --------
#define TC_DBG_EN                   ((unsigned short)0x1 << 0)          // Debug Mode Enable


//-----------------------------
// AIC Structure (0xFFFF F000)
//-----------------------------

typedef struct _AITS_AIC {
    AIT_REG_D   AIC_SMR[32];        // Source Mode egister              // 0x000 - 0x07C
    AIT_REG_D   AIC_SVR[32];        // Source Vector egister            // 0x080 - 0x0FC
    AIT_REG_D   AIC_IVR;            // IRQ Vector Register              // 0x100
    AIT_REG_D   AIC_FVR;            // FIQ Vector Register              // 0x104
    AIT_REG_D   AIC_ISR;            // Interrupt Status Register        // 0x108
    AIT_REG_D   AIC_IPR;            // Interrupt Pending Register       // 0x10C
    AIT_REG_D   AIC_IMR;            // Interrupt Mask Register          // 0x110
    AIT_REG_D   AIC_CISR;           // Core Interrupt Status Register   // 0x114
    AIT_REG_D                           _x118[2];
    AIT_REG_D   AIC_IECR;           // Interrupt Enable Command Register// 0x120
    AIT_REG_D   AIC_IDCR;           // Interrupt Disable Command egister// 0x124
    AIT_REG_D   AIC_ICCR;           // Interrupt Clear Command Register // 0x128
    AIT_REG_D   AIC_ISCR;           // Interrupt Set Command Register   // 0x12C
    AIT_REG_D   AIC_EOICR;          // End of Interrupt Command Register// 0x130
    AIT_REG_D                           _x134;
    AIT_REG_D   AIC_DBR;            // Debug Mode                       // 0x138
} AITS_AIC, *AITPS_AIC;


// -------- AIC_SMR[]: Interrupt Source Mode Registers --------
#define AIC_INT_MASK                ((unsigned short)0x0080)            // Interrupt direct to IRQ or FIQ
#define AIC_PRIOR_MASK              ((unsigned short)0x0007)            // Priority
#define AIC_SRCTYPE_MASK            ((unsigned short)0x0060)            // Source Type Definition
// Interrupt Priority
#define AIC_PRIOR_LOWEST            ((unsigned int)0x0000)              // Lowest priority level
#define AIC_PRIOR_HIGHEST           ((unsigned int)0x0007)              // Highest priority level
// Interrupt Direction
#define AIC_INT_TO_FIQ              ((unsigned short)0x0080)            // Interrupt is route to FIQ
#define AIC_INT_TO_IRQ              ((unsigned short)0x0000)            // Interrupt is route to IRQ
// Interrupts Sensibility
#define AIC_SRCTYPE_LOW_LEVEL_SENSITIVE     ((unsigned short)0x0000)    // Low Level
#define AIC_SRCTYPE_NEGATIVE_EDGE_TRIGGERED ((unsigned short)0x0020)    // Negative Edge
#define AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE    ((unsigned short)0x0040)    // High Level
#define AIC_SRCTYPE_POSITIVE_EDGE_TRIGGERED ((unsigned short)0x0060)    // Positive Edge


// -------- AIC_SMR[], AIC_SVR[]: Interrupt Source --------
#if 1//(CHIP == VSN_V2)
#define AIC_SRC_VIF                 0
#define AIC_SRC_ISP                 1
#define AIC_SRC_JPG                 2
#define AIC_SRC_SCAL                3
#define AIC_SRC_GRA                 4
#define AIC_SRC_IBC                 5
#if 1//(CHIP == VSN_V3)
	#define AIC_SRC_AFE				6
#endif
//7
#define AIC_SRC_UART                8
#define AIC_SRC_TC0                 9
#define AIC_SRC_TC1                 10
#define AIC_SRC_TC2                 11
#define AIC_SRC_USB                 12
#define AIC_SRC_SDRAM               13
#define AIC_SRC_USBDMA              14
#define AIC_SRC_WD                  15
#define AIC_SRC_GPIO                16
#define AIC_SRC_I2S                 17
#define AIC_SRC_SD                  18
#if 1//(CHIP == VSN_V3)
	#define AIC_SRC_AFE_FIFO		19
#endif
//20
#define AIC_SRC_H264ENC             21
#define AIC_SRC_AUD_FIFO            22
#define AIC_SRC_AUD_SAMPLE          23
#define AIC_SRC_I2CM                24
#define AIC_SRC_SIF                 25
#define AIC_SRC_SPI                 26
#define AIC_SRC_PWM                 27
#define AIC_SRC_DMA                 28
#define AIC_SRC_GBL                 29
#endif


// -------- AIC_ISR: Interrupt Status Register --------
#define AIC_IRQID_MASK              ((unsigned short)0x1F)              // Current source interrupt

// -------- AIC_CISR: Interrupt Core Status Register --------
#define AIC_NFIQ                    ((unsigned short)0x01)              // Core FIQ Status
#define AIC_NIRQ                    ((unsigned short)0x02)              // Core IRQ Status

// -------- AIC_DBR : ICE Debug Mode --------
#define AIC_DBG_EN                  ((unsigned short)0x1 << 0)          // Debug Mode Enable


#endif // not HOST




//==============================================================================
//
//                              Retina Register Base Address
//
//==============================================================================

#define AITC_BASE_TCB               ((AITPS_TCB )   AT91_IO_P2V(0xFFFE0000))         // TCs      Base Address (Timer control block)
#define AITC_BASE_TC0               ((AITPS_TC  )   AT91_IO_P2V(0xFFFE0000))         // TC0      Base Address
#define AITC_BASE_TC1               ((AITPS_TC  )   AT91_IO_P2V(0xFFFE0040))         // TC1      Base Address
#define AITC_BASE_TC2               ((AITPS_TC  )   AT91_IO_P2V(0xFFFE0080))         // TC2      Base Address
#define AITC_BASE_WD                ((AITPS_WD  )   AT91_IO_P2V(0xFFFF8000))         // WD       Base Address
#define AITC_BASE_AIC               ((AITPS_AIC )   AT91_IO_P2V(0xFFFFF000))         // AIC      Base Address

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

//#define RTNA_CLK_M                  (192)
//#define RTNA_CPU_CLK_M              (192)
//#define RTNA_PHL_CLK_M              (RTNA_CPU_CLK_M/2)

//==============================================================================
//
//                              MACROS
//
//==============================================================================

#define ALIGN_02(_a)                ((_a + 0x01) & ~0x01)
#define ALIGN_04(_a)                ((_a + 0x03) & ~0x03)
#define ALIGN_08(_a)                ((_a + 0x07) & ~0x07)
#define ALIGN_16(_a)                ((_a + 0x0F) & ~0x0F)
#define ALIGN_32(_a)                ((_a + 0x1F) & ~0x1F)
#define ALIGN_64(_a)                ((_a + 0x3F) & ~0x3F)

#define FLOOR_08(_a)                (_a & ~0x07)
#define FLOOR_16(_a)                (_a & ~0x0F)
#define FLOOR_32(_a)                (_a & ~0x1F)


#endif // _REG_RETINA_H_
