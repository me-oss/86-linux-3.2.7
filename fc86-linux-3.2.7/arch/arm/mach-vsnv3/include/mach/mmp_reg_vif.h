//==============================================================================
//
//  File        : mmp_reg_vif.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMP_REG_VIF_H_
#define _MMP_REG_VIF_H_

//#include    "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if 1//(CHIP == VSN_V2) || (CHIP == VSN_V3)
#define     VIF_NUM         (2)

typedef struct _AITS_VIF_GRAB {
    AIT_REG_W   PIXL_ST;
    AIT_REG_W   PIXL_ED;
    AIT_REG_W   LINE_ST;
    AIT_REG_W   LINE_ED;
} AITS_VIF_GRAB, *AITPS_VIF_GRAB;

//------------------------------
// VIF  Structure (0x8000 6000)
//------------------------------
typedef struct _AITS_VIF {
    AIT_REG_B   VIF_INT_HOST_EN [VIF_NUM];                              // 0x00
    AIT_REG_W                           _x02;
    AIT_REG_B   VIF_INT_HOST_SR [VIF_NUM];                              // 0x04
    AIT_REG_W                           _x06;
    AIT_REG_B   VIF_INT_CPU_EN [VIF_NUM];                               // 0x08
    AIT_REG_W                           _x0A;
    AIT_REG_B   VIF_INT_CPU_SR [VIF_NUM];                               // 0x0C
        /*-DEFINE-----------------------------------------------------*/
		#define     VIF_INT_GRAB_END        0x01
        #define     VIF_INT_FRM_END         0x02
        #define     VIF_INT_FRM_ST	        0x04
        #define     VIF_INT_LINE_0          0x08
        #define     VIF_INT_BUF_FULL        0x10
        #define     VIF_INT_LINE_1          0x20
        #define     VIF_INT_LINE_2          0x40
        #define     VIF_INT_SNR_FRM_END     0x80
        #define     VIF_INT_ALL             0xFF
        /*------------------------------------------------------------*/
    AIT_REG_W                           _x0E;
    AIT_REG_W   VIF_INT_LINE_NUM_0 [VIF_NUM];                           // 0x10
    AIT_REG_W   VIF_INT_LINE_NUM_1 [VIF_NUM];                           // 0x14
    AIT_REG_W   VIF_INT_LINE_NUM_2 [VIF_NUM];                           // 0x18
    AIT_REG_W                           _x0C;
    AIT_REG_B   VIF_INT_MODE [VIF_NUM];                                 // 0x1E
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_INT_EVERY_FRM       0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_IN_EN [VIF_NUM];                                    // 0x20
    AIT_REG_B   VIF_OUT_EN [VIF_NUM];                                   // 0x22
    AIT_REG_W                           _x24;
    AIT_REG_B   VIF_FRME_SKIP_EN [VIF_NUM];                             // 0x26
    AIT_REG_B   VIF_FRME_SKIP_NO [VIF_NUM];                             // 0x28
    AIT_REG_B                           _x2A[0x36];

	AIT_REG_B   VIF_BL_FILTER_EN [VIF_NUM];                             // 0x60
	AIT_REG_B   VIF_BL_FILTER_CC1 [VIF_NUM];                            // 0x62
	AIT_REG_B   VIF_BL_FILTER_CC2 [VIF_NUM];                            // 0x64
	AIT_REG_B   VIF_BL_FILTER_CC3 [VIF_NUM];                            // 0x66
	AIT_REG_W   VIF_BL_FILTER_TH [VIF_NUM];                             // 0x68
    AIT_REG_B                           _x6C[0x4];

    AIT_REG_B   VIF_IGBT_EN [VIF_NUM];                                  // 0x70
    AIT_REG_W   VIF_IGBT_LINE_ST [VIF_NUM];                             // 0x72
    AIT_REG_W   VIF_IGBT_OFST_ST [VIF_NUM];                             // 0x76
    AIT_REG_W   VIF_IGBT_LINE_ED [VIF_NUM];                             // 0x7A
    AIT_REG_B                           _x7E[0x2];

    AIT_REG_B   VIF_SENSR_CLK_CTL [VIF_NUM];                            // 0x80
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_SENSR_CLK_EN            0x01
        #define     VIF_SENSR_POLAR_NEG         0x02
        #define     VIF_SENSR_POLAR_PST         0x00
        #define     VIF_SENSR_PHASE_DELAY_NONE  0x00
        #define     VIF_SENSR_PHASE_DELAY_0_5F  0x10
        #define     VIF_SENSR_PHASE_DELAY_1_0F  0x20
        #define     VIF_SENSR_PHASE_DELAY_1_5F  0x30
        #define     VIF_SENSR_PHASE_DELAY_MASK  0x30
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_SENSR_CLK_FREQ [VIF_NUM];                           // 0x82
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_SENSR_CLK_PLL_D1    0x00
        #define     VIF_SENSR_CLK_PLL_D2    0x01
        #define     VIF_SENSR_CLK_PLL_D3    0x02
        #define     VIF_SENSR_CLK_PLL_D4    0x03
        #define     VIF_SENSR_CLK_PLL_D5    0x04
        #define     VIF_SENSR_CLK_PLL_D6    0x05
        #define     VIF_SENSR_CLK_PLL_D7    0x06
        #define     VIF_SENSR_CLK_PLL_D8    0x07
        #define     VIF_SENSR_CLK_PLL_D9    0x08
        #define     VIF_SENSR_CLK_PLL_D10   0x09
        #define     VIF_SENSR_CLK_PLL_D11   0x0A
        #define     VIF_SENSR_CLK_PLL_D12   0x0B
        #define     VIF_SENSR_CLK_PLL_D14   0x0D
        #define     VIF_SENSR_CLK_PLL_D15   0x0E
        #define     VIF_SENSR_CLK_PLL_D16   0x0F
        #define     VIF_SENSR_CLK_PLL_D22   0x15
        #define     VIF_SENSR_CLK_PLL_D32   0x1F
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_SENSR_CTL [VIF_NUM];                                // 0x84
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_SENSR_LATCH_PST     0x00
        #define     VIF_SENSR_LATCH_NEG     0x80
        #define     VIF_FIXED_OUT_EN        0x40
        #define     VIF_14BPP_OUT_EN        0x10
        #define     VIF_LINE_ID_POLAR       0x08
        #define     VIF_PIXL_ID_POLAR       0x04
        #define     VIF_COLORID_FORMAT_MASK     0x0C
        #define     VIF_HSYNC_POLAR_NEG     0x02
        #define     VIF_VSYNC_POLAR_NEG     0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_FIXED_OUT_DATA [VIF_NUM];                           // 0x86
    AIT_REG_B   VIF_IN_PIXL_CLK_DLY [VIF_NUM];                          // 0x88
    AIT_REG_B                           _x8A[0x6];
    AITS_VIF_GRAB   VIF_GRAB[VIF_NUM];

    AIT_REG_B   VIF_OPR_UPD [VIF_NUM];                                  // 0xA0
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_OPR_UPD_FRAME       0x01
        #define     VIF_OPR_UPD_EN          0x02
        /*------------------------------------------------------------*/
    AIT_REG_B                           _xA2[0xE];
    AIT_REG_B   VIF_RAW_OUT_EN [VIF_NUM];                               // 0xB0
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_2_RAW_EN            0x01
        #define     VIF_2_ISP_EN            0x02
        #define     VIF_JPG_2_RAW_EN        0x04
        #define     VIF_1_TO_ISP            0x08
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_YUV_CTL [VIF_NUM];                                  // 0xB2
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_YUV_EN              0x01
        #define     VIF_Y2B_EN              0x02
        #define     VIF_Y2B_COLR_ID_MASK    0x0C
        #define     VIF_Y2B_COLR_ID(_pixl, _line)   (((_pixl << 2)|(_line << 3))&VIF_Y2B_COLR_ID_MASK)
        #define     VIF_YUV_FORMAT_YUV      0x00
        #define     VIF_YUV_FORMAT_YVU      0x10
        #define     VIF_YUV_FORMAT_UVY      0x20
        #define     VIF_YUV_FORMAT_VUY      0x30
		#define     VIF_YUV_FORMAT_MASK     0x30
		#define     VIF_YUV_LPF_DIS         0x40
		#define     VIF_YUV_YCBCR_SEL       0x80
        /*------------------------------------------------------------*/
    AIT_REG_B                           _xB4[0xC];
    AIT_REG_B   VIF_0_SENSR_SIF_EN;                                     // 0xC0
    AIT_REG_B   VIF_0_SENSR_SIF_DATA;                                   // 0xC1
    AIT_REG_B   VIF_1_SENSR_SIF_EN;                                     // 0xC2
    AIT_REG_B   VIF_1_SENSR_SIF_DATA;                                   // 0xC3
        /*-DEFINE-----------------------------------------------------*/
        #define VIF_SIF_SEN_SHFT            0
        #define VIF_SIF_SCL_SHFT            1
        #define VIF_SIF_SDA_SHFT            2
        #define VIF_SIF_RST_SHFT            3
        #define VIF_SIF_SEN                 (0x1 << VIF_SIF_SEN_SHFT)
        #define VIF_SIF_SCL                 (0x1 << VIF_SIF_SCL_SHFT)
        #define VIF_SIF_SDA                 (0x1 << VIF_SIF_SDA_SHFT)
        #define VIF_SIF_RST                 (0x1 << VIF_SIF_RST_SHFT)
        /*------------------------------------------------------------*/
    AIT_REG_B                           _xC4[0x4];
    AIT_REG_W   VIF_SNR_LINE_CNT [VIF_NUM];                             // 0xC8
    AIT_REG_B                           _xCC[0x4];

    AIT_REG_B   VIF_DNSPL_RATIO_CTL [VIF_NUM];                          // 0xD0
    AIT_REG_B                           _xD2[0x2E];

    AIT_REG_B   VIF_0_STS_MISSING_DAT;                                  // 0x100
    AIT_REG_B                           _x101;
    AIT_REG_W   VIF_0_ERR_PIXL_CNT;                                     // 0x102
    AIT_REG_W   VIF_0_ERR_LINE_CNT;                                     // 0x104
    AIT_REG_W   VIF_0_CRC_ERR_LINE_CNT;                                 // 0x106
    AIT_REG_B   VIF_1_STS_MISSING_DAT;                                  // 0x108
    AIT_REG_B                           _x109;
    AIT_REG_W   VIF_1_ERR_PIXL_CNT;                                     // 0x10A
    AIT_REG_W   VIF_1_ERR_LINE_CNT;                                     // 0x10C
    AIT_REG_W   VIF_1_CRC_ERR_LINE_CNT;                                 // 0x10E

} AITS_VIF, *AITPS_VIF;

//------------------------------
// MIPI  Structure (0x8000 6110)
//------------------------------
typedef struct _AITS_MIPI {
    AIT_REG_W   MIPI_CLK_CFG [VIF_NUM];                                 // 0x10
    	/*-DEFINE-----------------------------------------------------*/
		#define MIPI_CSI2_EN           	0x01
		#define MIPI_DLY_EN         	0x02
		#define MIPI_CLK_EDGE_POS       0x04
		#define MIPI_CLK_SRC_SEL_1      0x00
		#define MIPI_CLK_SRC_SEL_2      0x08
		#define MIPI_LANE_SWAP_EN       0x10

		#define MIPI_CLK_DLY_MASK       0x0F00
		#define MIPI_CLK_DLY(_a)        ((_a << 8) & CSI2_CLK_DLY_MASK)
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x14[0xA];
    AIT_REG_B   MIPI_CLK_ERR_SR[VIF_NUM];                               // 0x1E
    	/*-DEFINE-----------------------------------------------------*/
    	// ref to MIPI_DATA0_ERR_SR
        /*------------------------------------------------------------*/

    AIT_REG_B   MIPI_DATA0_CFG [VIF_NUM];                               // 0x20
    	/*-DEFINE-----------------------------------------------------*/
		#define MIPI_DAT_LANE_EN        0x01
		#define MIPI_DAT_DLY_EN         0x02
		#define MIPI_DAT_SRC_PHY_0      0x00
		#define MIPI_DAT_SRC_PHY_1      0x04
		#define MIPI_DAT_SRC_PHY_2      0x08
		#define MIPI_DAT_SRC_PHY_3      0x0C
		#define MIPI_DAT_SRC_SEL_MASK   0x0C
		// data lane dp/dn swap ref to MIPI_CLK_CFG
        /*------------------------------------------------------------*/
    AIT_REG_W                           _x22;
    AIT_REG_W   MIPI_DATA0_DLY [VIF_NUM];                               // 0x24
    AIT_REG_B                           _x28[0x6];
    AIT_REG_B   MIPI_DATA0_ERR_SR [VIF_NUM];                            // 0x2E
    	/*-DEFINE-----------------------------------------------------*/
		#define MIPI_SOT_SEQ_ERR        0x01
		#define MIPI_SOT_SYNC_ERR       0x02
		#define MIPI_CTL_ERR         	0x04
        /*------------------------------------------------------------*/

    AIT_REG_B   MIPI_DATA1_CFG [VIF_NUM];                               // 0x30
    AIT_REG_W                           _x32;
    AIT_REG_W   MIPI_DATA1_DLY [VIF_NUM];                               // 0x34
    AIT_REG_B                           _x38[0x6];
    AIT_REG_B   MIPI_DATA1_ERR_SR [VIF_NUM];                            // 0x3E

    AIT_REG_B   MIPI_DATA2_CFG [VIF_NUM];                               // 0x40
    AIT_REG_W                           _x42;
    AIT_REG_W   MIPI_DATA2_DLY [VIF_NUM];                               // 0x44
    AIT_REG_B                           _x48[0x6];
    AIT_REG_B   MIPI_DATA2_ERR_SR [VIF_NUM];                            // 0x4E

    AIT_REG_B   MIPI_DATA3_CFG [VIF_NUM];                               // 0x50
    AIT_REG_W                           _x52;
    AIT_REG_W   MIPI_DATA3_DLY [VIF_NUM];                               // 0x54
    AIT_REG_B                           _x58[0x6];
    AIT_REG_B   MIPI_DATA3_ERR_SR [VIF_NUM];                            // 0x5E
} AITS_MIPI, *AITPS_MIPI;
#endif

#if 0//(CHIP == P_V2)
//------------------------------
// VIF  Structure (0x8000 0000)
//------------------------------
typedef struct _AITS_VIF {
    AIT_REG_B   VIF_INT_HOST_EN;                                         // 0x00
    AIT_REG_B   VIF_INT_HOST_SR;
    AIT_REG_B   VIF_INT_CPU_EN;
    AIT_REG_B   VIF_INT_CPU_SR;
        /*-DEFINE-----------------------------------------------------*/
		#define     VIF_INT_GRAB_END        0x01
        #define     VIF_INT_FRM_END         0x02
        #define     VIF_INT_FRM_ST	        0x04
        #define     VIF_INT_LINE_NO         0x08
        #define     VIF_INT_ALL             0x0F
        //--------------------------------------------------------------
    AIT_REG_W   VIF_INT_LINE;
    AIT_REG_B   VIF_INT_MODE;
    AIT_REG_B                           _x07[9];

    AIT_REG_B   VIF_IN_EN;                                               // 0x10
    AIT_REG_B   VIF_OUT_EN;
    AIT_REG_B   VIF_DEFT_EN;
    AIT_REG_B   VIF_RAW_OUT_EN;
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_2_RAW_EN	        0x01
        #define     VIF_2_ISP_EN	        0x02
        #define     VIF_HIGH_SPEED_MODE     0x08
        #define     VIF_H_DOWNSAMPLE_V1        0x10
        #define     VIF_V_DOWNSAMPLE_V1        0x20
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_OPR_UPD;
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_OPR_UPD_FRAME       0x01
        #define     VIF_OPR_UPD_EN          0x02
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_FRME_SKIP_EN;
    AIT_REG_B   VIF_FRME_SKIP_NO;
    AIT_REG_B   VIF_IGBT_EN;
    AIT_REG_B   VIF_AUTO_DEFT_EN;
    AIT_REG_B   VIF_10BPP_IN_EN;                                         // 2 cycles, 2+8 bits on 8-bit bus
    AIT_REG_B                           _x1A[6];

    AIT_REG_B   VIF_SENSR_CLK_CTL;                                       // 0x20
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_SENSR_CLK_EN            0x01
        #define     VIF_SENSR_POLAR_NEG         0x02
        #define     VIF_SENSR_POLAR_PST         0x00
        #define     VIF_SENSR_PHASE_DELAY_NONE  0x00
        #define     VIF_SENSR_PHASE_DELAY_0_5F  0x10
        #define     VIF_SENSR_PHASE_DELAY_1_0F  0x20
        #define     VIF_SENSR_PHASE_DELAY_1_5F  0x30
        #define     VIF_SENSR_PHASE_DELAY_MASK  0x30
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_SENSR_CLK_FREQ;
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_SENSR_CLK_PLL_D1    0x00
        #define     VIF_SENSR_CLK_PLL_D2    0x01
        #define     VIF_SENSR_CLK_PLL_D3    0x02
        #define     VIF_SENSR_CLK_PLL_D4    0x03
        #define     VIF_SENSR_CLK_PLL_D5    0x04
        #define     VIF_SENSR_CLK_PLL_D6    0x05
        #define     VIF_SENSR_CLK_PLL_D7    0x06
        #define     VIF_SENSR_CLK_PLL_D8    0x07
        #define     VIF_SENSR_CLK_PLL_D9    0x08
        #define     VIF_SENSR_CLK_PLL_D10   0x09
        #define     VIF_SENSR_CLK_PLL_D11   0x0A
        #define     VIF_SENSR_CLK_PLL_D12   0x0B
        #define     VIF_SENSR_CLK_PLL_D14   0x0D
        #define     VIF_SENSR_CLK_PLL_D15   0x0E
        #define     VIF_SENSR_CLK_PLL_D16   0x0F
        #define     VIF_SENSR_CLK_PLL_D32   0x1F
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_SENSR_CTL;
        /*-DEFINE-----------------------------------------------------*/
        #define     VIF_SENSR_LATCH_PST     0x00
        #define     VIF_SENSR_LATCH_NEG     0x80
        #define     VIF_FIXED_OUT_EN        0x40
        #define     VIF_PCLK_DLY            0x20
        #define     VIF_12BPP_OUT_EN        0x10
        #define     VIF_LINE_ID_POLAR       0x08
        #define     VIF_PIXL_ID_POLAR       0x04
        #define     VIF_COLORID_FORMAT_MASK     0x0C
        #define     VIF_HSYNC_POLAR         0x02
        #define     VIF_VSYNC_POLAR         0x01
        /*------------------------------------------------------------*/
    AIT_REG_B   VIF_FIXED_OUT_DATA;
    AIT_REG_B   VIF_YUV_CTL;
        /*-DEFINE-----------------------------------------------------*/
//        #define     VIF_HREF_POLAR              0x80
        #define     VIF_YUV_FORMAT_YUV      0x00
        #define     VIF_YUV_FORMAT_YVU      0x10
        #define     VIF_YUV_FORMAT_UVY      0x20
        #define     VIF_YUV_FORMAT_VUY      0x30
		#define     VIF_YUV_FORMAT_MASK     0x30
        #define     VIF_YUV_EN              0x01
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x25[11];
    AIT_REG_W   VIF_GRAB_PIXL_ST;                                        // 0x30
    AIT_REG_W   VIF_GRAB_PIXL_ED;
    AIT_REG_W   VIF_GRAB_LINE_ST;
    AIT_REG_W   VIF_GRAB_LINE_ED;
    AIT_REG_W   VIF_IGBT_LINE_ST;
    AIT_REG_W   VIF_IGBT_OFST_ST;
    AIT_REG_W   VIF_IGBT_LINE_ED;
    AIT_REG_B                           _x3E[2];

    AIT_REG_B   VIF_BL_COMPEN_EN;                                        // 0x40
    AIT_REG_B   VIF_BL_COMPEN_UPBND_L;
    AIT_REG_B   VIF_BL_COMPEN_OFST;
    AIT_REG_B   VIF_BL_COMPEN_UPBND_H;
    AIT_REG_B   VIF_BL_COMPEN_H_ST;                                      // in pixel
    AIT_REG_B   VIF_BL_COMPEN_V_ST;                                      // in line
    AIT_REG_B                           _x46[2];
    AIT_REG_B   VIF_00_ADJST;
    AIT_REG_B   VIF_01_ADJST;
    AIT_REG_B   VIF_10_ADJST;
    AIT_REG_B   VIF_11_ADJST;
    AIT_REG_B                           _x4C[4];
    AIT_REG_B   VIF_SENSR_SIF_EN;                                        // 0x50
    AIT_REG_B   VIF_SENSR_SIF_DATA;
        /*-DEFINE-----------------------------------------------------*/
        #define VIF_SIF_SEN_SHFT            0
        #define VIF_SIF_SCL_SHFT            1
        #define VIF_SIF_SDA_SHFT            2
        #define VIF_SIF_RST_SHFT            3
        #define VIF_SIF_SEN                 (0x1 << VIF_SIF_SEN_SHFT)
        #define VIF_SIF_SCL                 (0x1 << VIF_SIF_SCL_SHFT)
        #define VIF_SIF_SDA                 (0x1 << VIF_SIF_SDA_SHFT)
        #define VIF_SIF_RST                 (0x1 << VIF_SIF_RST_SHFT)
        /*------------------------------------------------------------*/
    AIT_REG_B                           _x52[14];

    AIT_REG_W   VIF_00_BP_ACC;                                           // 0x60
    AIT_REG_W   VIF_01_BP_ACC;
    AIT_REG_W   VIF_11_BP_ACC;
    AIT_REG_W   VIF_10_BP_ACC;
    AIT_REG_W   VIF_SENSR_LINE_SR;
    AIT_REG_W   VIF_AUTO_DEFT_SR;
    AIT_REG_B                           _x6C[4];
	AIT_REG_B   VIF_BL_FILTER_EN;										// 0x70
	AIT_REG_B   VIF_BL_FILTER_CC1;										// 0x71
	AIT_REG_B   VIF_BL_FILTER_CC2;										// 0x72
	AIT_REG_B   VIF_BL_FILTER_CC3;										// 0x73
	AIT_REG_B   VIF_BL_FILTER_TH;										// 0x74
	AIT_REG_B   VIF_BL_FILTER;											// 0x75
} AITS_VIF, *AITPS_VIF;

//------------------------------
// MIPI  Structure (0x8000 0100)
//------------------------------
typedef struct _AITS_MIPI {
	AIT_REG_B                           _x00[16];
    AIT_REG_B   MIPI_CLK_CFG;                                         	// 0x10
    	/*-DEFINE-----------------------------------------------------*/
		#define CSI2_SRC_SEL_1	      	0x00
		#define CSI2_SRC_SEL_2	      	0x08
		#define CSI2_EDGE_SEL	      	0x04
        /*------------------------------------------------------------*/
    AIT_REG_B   MIPI_CLK_DEL;                                         	// 0x11
    AIT_REG_B                           _x12[13];
    AIT_REG_B   MIPI_CLK_ERR_SR;                                        // 0x1F
    AIT_REG_B   MIPI_DATA1_CFG;                                        	// 0x20
    AIT_REG_B   MIPI_DATA1_DEL;                                         // 0x21
    AIT_REG_B   MIPI_DATA1_SOT;                                         // 0x22
    AIT_REG_B                           _x23[12];
    AIT_REG_B   MIPI_DATA1_ERR_SR;                                      // 0x2F
    AIT_REG_B   MIPI_DATA2_CFG;                                        	// 0x30
    	/*-DEFINE-----------------------------------------------------*/
		#define MIPI_CSI2_EN           	0x01
		#define MIPI_DLY_EN         	0x02
		#define MIPI_SRC_SEL_1	      	0x00
		#define MIPI_SRC_SEL_2	      	0x04
		#define MIPI_SRC_SEL_3	      	0x08
		#define MIPI_SRC_SEL_4	      	0x0C
		#define MIPI_SWAP_EN      		0x10
        /*------------------------------------------------------------*/
    AIT_REG_B   MIPI_DATA2_DEL;                                         // 0x31
    AIT_REG_B   MIPI_DATA2_SOT;                                         // 0x32
    AIT_REG_B                           _x33[12];
    AIT_REG_B   MIPI_DATA2_ERR_SR;                                      // 0x3F
    	/*-DEFINE-----------------------------------------------------*/
		#define MIPI_SOP_SEQ_ERR        0x01
		#define MIPI_SOP_SYNC_ERR       0x02
		#define MIPI_CTL_ERR         	0x04
        /*------------------------------------------------------------*/
    AIT_REG_B   MIPI_DATA3_CFG;                                        	// 0x40
    AIT_REG_B   MIPI_DATA3_DEL;                                         // 0x41
    AIT_REG_B   MIPI_DATA3_SOT;                                         // 0x42
    AIT_REG_B                           _x43[12];
    AIT_REG_B   MIPI_DATA3_ERR_SR;                                      // 0x4F
    AIT_REG_B   MIPI_DATA4_CFG;                                        	// 0x50
    AIT_REG_B   MIPI_DATA4_DEL;                                         // 0x51
    AIT_REG_B   MIPI_DATA4_SOT;                                         // 0x52
    AIT_REG_B                           _x53[12];
    AIT_REG_B   MIPI_DATA4_ERR_SR;                                      // 0x5F
    
} AITS_MIPI, *AITPS_MIPI;
#endif

#if 0//!defined(BUILD_FW)
// VIF OPR
#define VIF_INT_HOST_EN             (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_INT_HOST_EN       )))
#define VIF_INT_HOST_SR             (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_INT_HOST_SR       )))
#define VIF_INT_LINE                (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_INT_LINE          )))
#define VIF_IN_EN                   (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_IN_EN             )))
#define VIF_OUT_EN                  (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_OUT_EN            )))
#define VIF_RAW_OUT_EN              (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_RAW_OUT_EN        )))
#define VIF_OPR_UPD                 (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_OPR_UPD           )))
#define VIF_FRME_SKIP_EN            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_FRME_SKIP_EN      )))
#define VIF_FRME_SKIP_NO            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_FRME_SKIP_NO      )))
#define VIF_IGBT_EN                 (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_IGBT_EN           )))
#define VIF_GRAB_PIXL_ST            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_GRAB_PIXL_ST      )))
#define VIF_GRAB_PIXL_ED            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_GRAB_PIXL_ED      )))
#define VIF_GRAB_LINE_ST            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_GRAB_LINE_ST      )))
#define VIF_GRAB_LINE_ED            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_GRAB_LINE_ED      )))
#define VIF_SENSR_CLK_FREQ          (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_SENSR_CLK_FREQ    )))
#define VIF_SENSR_CTL               (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_SENSR_CTL         )))
#define VIF_FIXED_OUT_DATA          (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_FIXED_OUT_DATA    )))
#define VIF_IGBT_LINE_ST            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_IGBT_LINE_ST      )))
#define VIF_IGBT_OFST_ST            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_IGBT_OFST_ST      )))
#define VIF_IGBT_LINE_ED            (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_IGBT_LINE_ED      )))
#define VIF_DEFT_EN				    (VIF_BASE +(MMP_ULONG)(&(((AITPS_VIF )0)->VIF_DEFT_EN      )))
#endif

/// @}
#endif // _MMPH_REG_VIF_H_