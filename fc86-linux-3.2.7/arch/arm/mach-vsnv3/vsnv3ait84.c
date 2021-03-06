/*
 *  arch/arm/mach-vsnv3/vsnv3_ait84.c
 *  Copyright (C) 2014 Alpha Image Tech. Corp.
 * Modify from arch/arm/mach-at91/at91sam9260.c
 *
 *  Copyright (C) 2006 SAN People
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */ 

#include <linux/module.h>
#include <linux/pm.h>

#include <asm/irq.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <mach/cpu.h>
#include <mach/board.h>
//#include <mach/at91_dbgu.h>
#include <mach/vsnv3def.h>
//#include <mach/at91_pmc.h>
//#include <mach/at91_rstc.h>
//#include <mach/at91_shdwc.h>

#include "soc.h"
#include "generic.h"
#include "clock.h"

#include <mach/mmp_reg_gbl.h>
#include <mach/mmpf_wd.h>
#include "mach/mmpf_system.h"
#include <mach/mmp_reg_vif.h>
#include <mach/mmpf_vif.h>
#include <mach/reg_retina.h>
#include <mach/ROMLIKE_BIN.h>

#include <mach/mmp_reg_sf.h>

/* --------------------------------------------------------------------
 *  Clocks
 * -------------------------------------------------------------------- */

/*
 * The peripheral clocks.
 */

extern struct clk cpu_peri_clk;
#if 0
static struct clk pioA_clk = {
	.name		= "pioA_clk",
	.pmc_mask	= 1 << AITVSNV3_ID_GPIO,
	.type		= CLK_TYPE_PERIPHERAL,
};

static struct clk pioB_clk = {
	.name		= "pioB_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_PIOB,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk pioC_clk = {
	.name		= "pioC_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_PIOC,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk adc_clk = {
	.name		= "adc_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_ADC,
	.type		= CLK_TYPE_PERIPHERAL,
};
#endif
static struct clk usart0_clk = {
	.name		= "usart0_clk",
	.parent		=&cpu_peri_clk,	
	.pmc_mask	= 1 << AITVSNV3_ID_UART,
	.type		= CLK_TYPE_PERIPHERAL,
};

static struct clk usart1_clk = {
	.name		= "usart1_clk",
	.parent		=&cpu_peri_clk,	
	.pmc_mask	= 1 << AITVSNV3_ID_UART,
	.type		= CLK_TYPE_PERIPHERAL,
};
#if 0
static struct clk usart1_clk = {
	.name		= "usart1_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_US1,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk usart2_clk = {
	.name		= "usart2_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_US2,
	.type		= CLK_TYPE_PERIPHERAL,
};

#endif
extern struct clk usb_clk;
static struct clk udc_clk = {
	.name		= "udc_clk",
	.parent		=&usb_clk,		
	.pmc_mask	= 1 << AITVSNV3_ID_USB,
	.type		= CLK_TYPE_PERIPHERAL,
};

static struct clk __maybe_unused mmc_clk = {
	.name		= "mmc_clk",
	.pmc_mask	= 1<<AITVSNV3_ID_SD,
	.type		= CLK_TYPE_CLK_CTL1,
};

extern struct clk sd0_clk;
static struct clk sd0_ctl_clk = {

	.name		= "sd0_ctl_clk ",
	.parent		=&sd0_clk,
	.pmc_mask	= 1 << AITVSNV3_ID_SD,
	.type		= CLK_TYPE_PERIPHERAL,
};

extern struct clk sd1_clk;
static struct clk sd1_ctl_clk = {
	.name		= "sd1_ctl_clk ",
	.parent		=&sd1_clk,		
	.pmc_mask	= 1 << AITVSNV3_ID_SD1,
	.type		= CLK_TYPE_PERIPHERAL,
};

static struct clk __maybe_unused  twi_clk = {
	.name		= "twi_clk",
	.pmc_mask	= 1 << AITVSNV3_ID_I2CM,
	.type		= CLK_TYPE_PERIPHERAL,
};

extern struct clk i2c_clk;
static struct clk i2cm_ctl_clk = {
	.name		="i2cm_ctl_clk ",
	.parent 		=&i2c_clk,
	.pmc_mask	= 1 << AITVSNV3_ID_I2CM,
	.type		= CLK_TYPE_PERIPHERAL,
};
extern struct clk pspi_clk;
static struct clk pspi_ctl_clk = {
	.name		= "pspi_ctl_clk",
	.parent 		=&pspi_clk,		
	.pmc_mask	= 1 << AITVSNV3_ID_PSPI,
	.type		= CLK_TYPE_PERIPHERAL,
};

extern struct clk bootspi_clk;
static struct clk sif_ctl_clk = {
	.name		= "sif_ctl_clk",
	.parent 		=&bootspi_clk,		
	.pmc_mask	= 1 << AITVSNV3_ID_SIF,
	.type		= CLK_TYPE_PERIPHERAL,
};

extern struct clk audio_clk;
extern struct clk audio2_clk;
static struct clk ssc_clk = {
	.name		= "ssc_clk",
	.parent 		=&audio_clk,				
	.pmc_mask	= 1<<AITVSNV3_ID_I2S_FIFO,
	.type		= CLK_TYPE_PERIPHERAL,
};



static struct clk afe_clk = {
	.name		= "afe_clk",
	.parent 		=&audio2_clk,		
	.pmc_mask	= 1<<AITVSNV3_ID_AFE_FIFO,
	.type		= CLK_TYPE_PERIPHERAL,
};



static struct clk tc0_clk = {
	.name		= "tc0_clk",
	.parent 		=&cpu_peri_clk,		
	.pmc_mask	= 1 << AITVSNV3_ID_TC0,
	.type		= CLK_TYPE_PERIPHERAL,
};

static struct clk tc1_clk = {
	.name		= "tc1_clk",
	.parent 		=&cpu_peri_clk,		
	.pmc_mask	= 1 << AITVSNV3_ID_TC1,
	.type		= CLK_TYPE_PERIPHERAL,
};

static struct clk tc2_clk = {
	.name		= "tc2_clk",
	.parent 		=&cpu_peri_clk,		
	.pmc_mask	= 1 << AITVSNV3_ID_TC2,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk wdt_clk = {
	.name		= "wdt_clk",
	.parent 		=&cpu_peri_clk,
	.pmc_mask	= 1 << AITVSNV3_ID_WDT,
	.type		= CLK_TYPE_PERIPHERAL,
};
#if 0
static struct clk ohci_clk = {
	.name		= "ohci_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_UHP,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk macb_clk = {
	.name		= "macb_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_EMAC,
	.type		= CLK_TYPE_PERIPHERAL,
};

static struct clk isi_clk = {
	.name		= "isi_clk",
	.pmc_mask	= 1 << AT91SAM9260_ID_VIF,//AT91SAM9260_ID_ISI,
	.type		= CLK_TYPE_PERIPHERAL,
};

#endif

static struct clk *periph_clocks[] __initdata = {
//	&pioA_clk,
#if 0		
	&pioB_clk,
	&pioC_clk,
#endif	
//	&adc_clk,
	&usart0_clk,
	&usart1_clk,
	
#if 0	
	&usart1_clk,

	&mmc_clk,
#endif	
	&udc_clk,
	
//	&mmc_clk,
	&sd0_ctl_clk,
	&sd1_ctl_clk,
	&i2cm_ctl_clk,
//	&twi_clk,
	&pspi_ctl_clk,
	&sif_ctl_clk,
//	&spi1_clk,
//	&ssc_clk,
	&tc0_clk,
	&tc1_clk,
	&tc2_clk,
	&wdt_clk,
#if 0	
	&ohci_clk,
	&macb_clk,
	&isi_clk,
#endif		
	&afe_clk
	// irq0 .. irq2
};

static struct clk_lookup periph_clocks_lookups[] = {
	CLKDEV_CON_DEV_ID("sd0_ctl_clk", DEVICE_NAME_SD0, &sd0_ctl_clk),	
	CLKDEV_CON_DEV_ID("sd1_ctl_clk", DEVICE_NAME_SD1, &sd1_ctl_clk),			

//	CLKDEV_CON_DEV_ID("spi_clk", "ait_spi.1", &spi1_clk),
	CLKDEV_CON_DEV_ID("sif_ctl_clk", "vsnv3_sif.0", &sif_ctl_clk),
	CLKDEV_CON_DEV_ID("pspi_ctl_clk", "vsnv3_pspi.1", &pspi_ctl_clk),
//	CLKDEV_CON_DEV_ID("vsnv3_timer_ctl_clk", "atmel_tcb.0", &tc0_clk),
	CLKDEV_CON_DEV_ID("tc1_clk", "vsnv3_tc1", &tc1_clk),
	CLKDEV_CON_DEV_ID("tc2_clk", "vsnv3_tc2", &tc2_clk),
	CLKDEV_CON_DEV_ID("wdt_ctl_clk", "vsnv3_wdt", &wdt_clk),
	CLKDEV_CON_DEV_ID("pclk", "vsnv3aud.0", &ssc_clk),
	CLKDEV_CON_DEV_ID("pclk", "vsnv3aud.1", &afe_clk),
	CLKDEV_CON_DEV_ID("udc_clk", "vsnv3udc", &udc_clk),	
	CLKDEV_CON_DEV_ID("pclk", "ait-ssc-dai.0", &ssc_clk),	
	CLKDEV_CON_DEV_ID("pclk", "ait-ssc-dai.1", &ssc_clk),
	
	
	/* more usart lookup table for DT entries */
//	CLKDEV_CON_DEV_ID("usart", "fffff200.serial", &mck),
//	CLKDEV_CON_DEV_ID("usart", "fffb0000.serial", &usart0_clk),
	CLKDEV_CON_DEV_ID("afe_clk", "vsnv3-afe-codec", &afe_clk),	
};

static struct clk_lookup usart_clocks_lookups[] = {
	CLKDEV_CON_DEV_ID("usart", "atmel_usart.0", &usart0_clk),
	CLKDEV_CON_DEV_ID("usart", "atmel_usart.1", &usart1_clk),

};
#if 0
/*
 * The two programmable clocks.
 * You must configure pin multiplexing to bring these signals out.
 */
static struct clk pck0 = {
	.name		= "pck0",
	.pmc_mask	= AT91_PMC_PCK0,
	.type		= CLK_TYPE_PROGRAMMABLE,
	.id		= 0,
};
static struct clk pck1 = {
	.name		= "pck1",
	.pmc_mask	= AT91_PMC_PCK1,
	.type		= CLK_TYPE_PROGRAMMABLE,
	.id		= 1,
};
#endif
static void __init at91sam9260_register_clocks(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(periph_clocks); i++)
		clk_register(periph_clocks[i]);

	clkdev_add_table(periph_clocks_lookups,
			 ARRAY_SIZE(periph_clocks_lookups));
	clkdev_add_table(usart_clocks_lookups,
			 ARRAY_SIZE(usart_clocks_lookups));

//	clk_register(&pck0);
//	clk_register(&pck1);
}

static struct clk_lookup console_clock_lookup;

void __init at91sam9260_set_console_clock(int id)
{
	if (id >= ARRAY_SIZE(usart_clocks_lookups))
		return;

	console_clock_lookup.con_id = "usart";
	console_clock_lookup.clk = usart_clocks_lookups[id].clk;
	clkdev_add(&console_clock_lookup);
}

/* --------------------------------------------------------------------
 *  GPIO
 * -------------------------------------------------------------------- */

static struct vsnv3_gpio_bank __maybe_unused ait_gpio[] = {
#if 0
	{
		.id		= AT91SAM9260_ID_PIOA,
		.offset		= AT91_PIOA,
		.clock		= &pioA_clk,
	}

, {
		.id		= AT91SAM9260_ID_PIOB,
		.offset		= AT91_PIOB,
		.clock		= &pioB_clk,
	}, {
		.id		= AT91SAM9260_ID_PIOC,
		.offset		= AT91_PIOC,
		.clock		= &pioC_clk,
	}
#endif		
};

static void __maybe_unused ait_poweroff(void)
{

}


/* --------------------------------------------------------------------
 *  AT91SAM9260 processor initialization
 * -------------------------------------------------------------------- */
#if 0
static void __init at91sam9xe_map_io(void)
{
	unsigned long sram_size;

	switch (vsnv3_soc_initdata.cidr & AT91_CIDR_SRAMSIZ) {
		case AT91_CIDR_SRAMSIZ_32K:
			sram_size = 2 * SZ_16K;
			break;
		case AT91_CIDR_SRAMSIZ_16K:
		default:
			sram_size = SZ_16K;//sram_size = SZ_128K;
	}

//	at91_init_sram(0, AT91SAM9XE_SRAM_BASE, sram_size);
}
#endif
static void __init at91sam9260_map_io(void)
{
#if 0
	if (cpu_is_at91sam9xe()) {
		at91sam9xe_map_io();
	} else if (cpu_is_at91sam9g20()) {
//		at91_init_sram(0, AT91SAM9G20_SRAM0_BASE, AT91SAM9G20_SRAM0_SIZE);
//		at91_init_sram(1, AT91SAM9G20_SRAM1_BASE, AT91SAM9G20_SRAM1_SIZE);
	} else {

		at91_init_sram(0, AT91SAM9260_SRAM0_BASE, AT91SAM9260_SRAM0_SIZE);
		at91_init_sram(1, AT91SAM9260_SRAM1_BASE, AT91SAM9260_SRAM1_SIZE);
#endif	
}

extern void vsnv3_wdt_force_timeout(void);

#define WRITE_IN_PROGRESS       0x01
#define WRITE_ENABLE_LATCH      0x02


#define WRITE_STATUS            0x01
#define PAGE_PROGRAM            0x02
#define READ_DATA               0x03
#define WRITE_DISABLE           0x04
#define EXIT_OTP_MODE           0x04
#define READ_STATUS             0x05
#define WRITE_ENABLE            0x06
#define FAST_READ_DATA          0x0B
#define SECTOR_ERASE            0x20
#define ENTER_OTP_MODE          0x3A
#define READ_UNIQUE_ID		0x4B
#define BLOCK_ERASE             0x52
#define CHIP_ERASE              0x60
#define EBSY                    0x70
#define DBSY                    0x80
#define READ_DEVICE_ID          0x9F
#define RELEASE_DEEP_POWER_DOWN 0xAB
#define ADDR_AUTO_INC_WRITE     0xAD
#define DEEP_POWER_DOWN         0xB9

#define   WRITE_EAR      0xC5

MMP_ERR MMPF_SF_CheckReady(void)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
    
    while((pSIF->SIF_INT_CPU_SR & SIF_CMD_DONE) == 0) ;
	
    return  MMP_ERR_NONE;
}

MMP_ERR MMPF_SF_EnableWrite(void)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
	
    pSIF->SIF_INT_CPU_SR = SIF_CLR_CMD_STATUS;
   
    pSIF->SIF_CMD = WRITE_ENABLE;
   
    pSIF->SIF_CTL = SIF_START;
    
    MMPF_SF_CheckReady();
    
    return  MMP_ERR_NONE;
}


MMP_ERR MMPF_SF_ReadStatus(MMP_UBYTE *ubStatus)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
	
	pSIF->SIF_INT_CPU_SR = SIF_CLR_CMD_STATUS;
   
   	pSIF->SIF_CMD = READ_STATUS;
    
    pSIF->SIF_CTL = SIF_START | SIF_R | SIF_DATA_EN;

    MMPF_SF_CheckReady();
    
    *ubStatus = pSIF->SIF_DATA_RD;
    
    return  MMP_ERR_NONE;
}


MMP_ERR MMPF_SF_WriteEAR(MMP_UBYTE ubData)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
	MMP_UBYTE ubStatus;
	
	MMPF_SF_EnableWrite();
	
	pSIF->SIF_INT_CPU_SR = SIF_CLR_CMD_STATUS;
    
    pSIF->SIF_CMD = WRITE_EAR;
    
    pSIF->SIF_DATA_WR = ubData;
    
    pSIF->SIF_CTL = SIF_START | SIF_W | SIF_DATA_EN;
    
    MMPF_SF_CheckReady();
   
    do {
        MMPF_SF_ReadStatus(&ubStatus);
    } while ((ubStatus & WRITE_IN_PROGRESS) || (ubStatus & WRITE_ENABLE_LATCH));
   
    
    return  MMP_ERR_NONE;
}


void vsnv3_reset(char mode)
{
        AITPS_GBL   pGBL = AITC_BASE_GBL;
        AITPS_AIC    pAIC = AITC_BASE_AIC;
//	AITPS_GPIO pGPIO = AITC_BASE_GPIO;
	AITPS_WD 	pWD = AITC_BASE_WD;
	void (*FW_Entry)(void) = NULL;
	MMP_ULONG   counter = 0;
	MMP_ULONG i, size;
	MMP_UBYTE *ptr;
 //AITPS_GBL pGBL = AITC_BASE_GBL;
        //Make sure that VI Clock is ON
//JFY
        pGBL->GBL_CLK_DIS0 &= (MMP_UBYTE)(~(GBL_CLK_VI_DIS));
        MMPF_VIF_SetPIODir(VIF_SIF_RST, 0x01);
	MMPF_VIF_SetPIOOutput(VIF_SIF_RST, 0x00);

        //printk(KERN_ERR "Reg6903 = 0x%x\r\n", pGBL->GBL_CLK_DIS0);
        //printk(KERN_ERR "Reg6604 = 0x%x\r\n", pGPIO->GPIO_EN[0x01]);  // CGPIO7(GPIO49) is used for power hold key
        //printk(KERN_ERR "Reg6614 = 0x%x\r\n", pGPIO->GPIO_DATA[0x01]);  // CGPIO7(GPIO49) is used for power hold key

        //RTNA_AIC_IRQ_DisAll(pAIC);
        pAIC->AIC_IDCR = 0xFFFFFFFF;                // Disable timer interrupt at AIC level
	//RTNA_AIC_IRQ_ClearAll(pAIC);
        pAIC->AIC_ICCR = 0xFFFFFFFF;                // Clear ALL interrups if any pending

  	//MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_VI, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_ISP, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_JPG, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_SCAL, MMP_TRUE);

	MMPF_SF_WriteEAR(0x00);


//	if(mode=='h')
//		MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_GPIO, MMP_TRUE);
	
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_AUD, MMP_TRUE);

	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_RAW, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_DMA, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_I2C, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_USB, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_H264, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_IBC, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_GRA, MMP_TRUE);

	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_SD, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_PWM, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_PSPI, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_USB_PHY, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_CPU_SRAM, MMP_TRUE);

	pGBL->GBL_CLK_DIS0 = (MMP_UBYTE)(~(GBL_CLK_MCI_DIS | GBL_CLK_VI_DIS| GBL_CLK_JPG_DIS | GBL_CLK_CPU_DIS | GBL_CLK_GPIO_DIS));
	pGBL->GBL_CLK_DIS1 = (MMP_USHORT)(~(GBL_CLK_DRAM_DIS | GBL_CLK_PWM_DIS | GBL_CLK_I2C_DIS | GBL_CLK_DMA_DIS | GBL_CLK_USB_DIS | GBL_CLK_CPU_PHL_DIS));
	pGBL->GBL_CLK_DIS2 = (MMP_UBYTE)(~(GBL_CLK_CIDC_DIS | GBL_CLK_GNR_DIS | GBL_CLK_COLOR_DIS)) ;
	pGBL->GBL_CLK_DIS1 &= ~GBL_CLK_BS_SPI_DIS;

        //pGPIO->GPIO_DATA[0x01] &= ~(0x00010000);  // reset WIFI reset pin
        //printk(KERN_ERR "Reg6903 = 0x%x\r\n", pGBL->GBL_CLK_DIS0);
        //printk(KERN_ERR "Reg6604 = 0x%x\r\n", pGPIO->GPIO_EN[0x01]);  // CGPIO7(GPIO49) is used for power hold key
        //printk(KERN_ERR "Reg6614 = 0x%x\r\n", pGPIO->GPIO_DATA[0x01]);  // CGPIO7(GPIO49) is used for power hold key
       
	//mdelay(10);

#if 0  // use SW reboot method

       ptr = (MMP_UBYTE *)0x00;//AIT_ITCM_VIRT_BASE;
       for (i = 0; i < 0x1000; i++)
           *ptr++ = m_ulRomLikeBin[i];

       ptr = (MMP_UBYTE *)AIT_SRAM_VIRT_BASE;
       size = sizeof(m_ulRomLikeBin)/sizeof(m_ulRomLikeBin[0]);
       for (i = 0x1000; i < (size-0x1000); i++)
           *ptr++ = m_ulRomLikeBin[i];

       FW_Entry = (void (*)(void))0x00;//(AIT_ITCM_VIRT_BASE);
       FW_Entry();	//enter the firmware entry
       while(1);
	
#else

	//Change the Boot Strapping as ROM boot
	pGBL->GBL_CHIP_CTL |= MOD_BOOT_STRAP_EN;
	pGBL->GBL_CHIP_CFG = ROM_BOOT_MODE;
	pGBL->GBL_CHIP_CTL &= ~MOD_BOOT_STRAP_EN;
	
	//Turn-off watch dog
 	if((pWD->WD_MODE_CTL0 & WD_EN)!= 0x0) {
		//RTNA_DBG_Str(0, "\r\nTurn-off WD !!\r\n");
 	    pWD->WD_MODE_CTL0 = 0x2340;
 	}
	
	//VSN_V3, CPU access ROM code have HW bug, so we use reset ROM controller to archieve this purpose. 
	//Note: The busy waiting is necessary !!!  ROM controller need some time to re-load ROM code.
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_CPU_PERIF, MMP_FALSE);

	#pragma O0
	for(counter = 0x3FFFF; counter > 0 ; counter --) {
	}
	#pragma


	//Finally, use watch-dog do timeout-reset, this purpose is to reset PLL as normal speed for serial-flash acessing issue.

	pWD->WD_MODE_CTL1 = WD_CLK_CTL_ACCESS_KEY | (31 << 2) |WD_CLK_MCK_D128;

	pGBL->GBL_CHIP_CTL |= GBL_WD_RST_ALL_MODULE;

	pWD->WD_RE_ST = WD_RESTART; //Before enable, we shoudl set re-start first.
	
	pWD->WD_MODE_CTL0 = WD_CTL_ACCESS_KEY|WD_RT_EN|WD_EN;

      while(1);  

#endif
}
EXPORT_SYMBOL(vsnv3_reset);


static void __init vsnv3ait84_initialize(void)
{
	AITPS_GBL   pGBL = AITC_BASE_GBL;

printk("%s\n",__func__);
		vsnv3_arch_reset = vsnv3_reset;//at91sam9_alt_reset;
//	pm_power_off = ait_poweroff;
pGBL->GBL_CLK_DIS0 |=  GBL_CLK_SCAL_DIS|GBL_CLK_JPG_DIS|GBL_CLK_VI_DIS|GBL_CLK_ISP_DIS;
   pGBL->GBL_CLK_DIS1|=GBL_CLK_USB_DIS|GBL_CLK_PWM_DIS|GBL_CLK_PSPI_DIS|GBL_CLK_RAW_DIS|GBL_CLK_DMA_DIS|GBL_CLK_H264_DIS|
  GBL_CLK_ICON_DIS|GBL_CLK_GRA_DIS|GBL_CLK_IBC_DIS |GBL_CLK_MIPI_DIS;
	pGBL->GBL_CLK_DIS2|=GBL_CLK_GNR_DIS|GBL_CLK_SM_DIS;
	/* Register GPIO subsystem */
	vsnv3_gpio_init(NULL, 1);

}

/* --------------------------------------------------------------------
 *  Interrupt initialization
 * -------------------------------------------------------------------- */

/*
 * The default interrupt priority levels (0 = lowest, 7 = highest).
 */
static unsigned int vsnv3ait84_default_irq_priority[NR_AIC_IRQS] __initdata = {

//IRQ 0
	3,	/* VIF *//*Advanced Interrupt Controller */
	3,	/* ISP *//*System Peripherals */
	3,	/* JPEG Encoder */ /*Parallel IO Controller A */
	3,	/* Scaler */ /*Parallel IO Controller B */
	3,	/* Graphic */ /*Parallel IO Controller C */
	3,	/* IBC */ /*Analog-to-Digital Converter */
	3,	/* AFE */
	0,	/* NA */
	3,	/* UART B */	
	3,	/* Timer Counter 0 *//* Multimedia Card Interface */
//IRQ 10
	3,	/* Timer Counter 1 */ /*USB Device Port */
	3,	/* Timer Counter 2 *//*Two-Wire Interface */
	3,	/* USB */
	3,	/* SDRAM */
	3,	/* USB Ext DMA *//*Serial Synchronous Controller */
	1,	/* Watchdog */
	2,	/* GPIO */
	3,	/* I2S */
	3,	/* SD */
	3,	/* AFE FIFO */
	
//IRQ 20				
	0,	/* NA */
	3,	/* H264 Encoder */ /* Ethernet */
	3,	/* AUDIO FIFO*//*Image Sensor Interface */
	3,	/* AUDIO SAMPLE */
	3,	/* I2CM */
	3,	/* SIF */
	3,	/* SPI */
	3,	/* PWM*/
	3,	/* DMA */
	3,	/* Global */
	
//IRQ 30				
	3,	/* SM */
	0,	/* NA */
};

struct ait_init_soc __initdata vsnv3ait84_soc = {
	.map_io = at91sam9260_map_io,
	.default_irq_priority = vsnv3ait84_default_irq_priority,
	.register_clocks = at91sam9260_register_clocks,
	.init = vsnv3ait84_initialize,
};
