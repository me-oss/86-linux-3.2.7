/*
 * arch/arm/mach-vsnv3/vsnv3ait84_devices.c
 *
 *  Copyright (C) 2014 Alpha Image Tech. Corp.
 *  Modify from arch/arm/mach-at91/at91sam9260_devices.c
 *
 *  Copyright (C) 2006 Atmel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <linux/dma-mapping.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/i2c-gpio.h>

#include <linux/delay.h>
#include <linux/semaphore.h>

#include <mach/board.h>
#include <mach/cpu.h>
#include "generic.h"

#include <mach/mmp_register.h>
#include <mach/mmp_reg_vif.h>
#include <mach/mmp_reg_audio.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_sd.h>
#include <mach/mmp_reg_spi.h>
#include <mach/mmp_reg_i2cm.h>
#include <mach/mmp_reg_uart.h>
#include <mach/mmpf_i2cm.h>
#include <mach/mmpf_vif.h>
#include <mach/mmpf_i2s_ctl.h>
#include <mach/mmpf_pll.h>
#include <mach/mmpf_sd.h>
#include <mach/mmp_register.h>


/* --------------------------------------------------------------------
 *  USB Device (Gadget)
 * -------------------------------------------------------------------- */
#define CONFIG_USB_AIT
#if defined(CONFIG_USB_AIT)
static struct ait_udc_data udc_data;

static struct resource udc_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_USBCTL,
		.end	= AITC_BASE_PHY_USBCTL + SZ_1K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITC_BASE_PHY_USBDMA,
		.end	= AITC_BASE_PHY_USBDMA + SZ_256 - 1,
		.flags	= IORESOURCE_MEM,
	},	
	[2] = {
		.start	= AITVSNV3_ID_USB,
		.end	= AITVSNV3_ID_USB,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device vsnv3_udc_device = {
	.name		= "vsnv3_udc",
	.id		= -1,
	.dev		= {
				.platform_data		= &udc_data,
	},
	.resource	= udc_resources,
	.num_resources	= ARRAY_SIZE(udc_resources),
};

void __init vsnv3_add_device_udc(struct ait_udc_data *data)
{
	if (!data)
		return;
#if 0
	if (data->vbus_pin) {
		at91_set_gpio_input(data->vbus_pin, 0);
		at91_set_deglitch(data->vbus_pin, 1);
	}

	/* Pullup pin is handled internally by USB device peripheral */
#endif
	udc_data = *data;
	platform_device_register(&vsnv3_udc_device);
}
#else
void __init vsnv3_add_device_udc(struct ait_udc_data *data) {}
#endif


/* --------------------------------------------------------------------
 *  MMC / SD
 * -------------------------------------------------------------------- */

#if 1//defined(CONFIG_MMC_AIT)// || defined(CONFIG_MMC_AT91_MODULE)
static u64 mmc_dmamask = DMA_BIT_MASK(32);
static struct ait_sd_data *mmc_sd1_data;

static struct resource mmc_resources_sd1[] = {
	[0] = {
		.name = "SD1 Reg",	
		.start	= AITC_BASE_PHY_SD1,
		.end	= AITC_BASE_PHY_SD1 + SZ_256-1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_SD,
		.end			= AITVSNV3_ID_SD,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device vsnv3_sd1_device = {
	.name		= DEVICE_NAME_SD1,
	.id		= -1,
	.dev		= {
				.dma_mask		= &mmc_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				//.platform_data		=(void*) mmc_sd1_data,
	},
	.resource	= mmc_resources_sd1,
	.num_resources	= ARRAY_SIZE(mmc_resources_sd1),
};

static struct ait_sd_data *mmc_sd0_data;

static struct resource mmc_resources_sd0[] = {
	[0] = {
		.name = "SD0 Reg",	
		.start	= AITC_BASE_PHY_SD0,
		.end	= AITC_BASE_PHY_SD0 + SZ_256-1,
		.flags	= IORESOURCE_MEM,
	},	
	[1] = {
		.start	= AITVSNV3_ID_SD,
		.end	= AITVSNV3_ID_SD,
		.flags	= IORESOURCE_IRQ,
	},
};
static struct platform_device vsnv3_sd0_device = {
	.name		= DEVICE_NAME_SD0,
	.id		= -1,
	.dev		= {
				.dma_mask		= &mmc_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				//.platform_data		= (void*)mmc_sd0_data,
	},
	.resource	= mmc_resources_sd0,
	.num_resources	= ARRAY_SIZE(mmc_resources_sd0),
};

void __init vsnv3_add_device_mmc(short mmc_id, struct ait_sd_data *data)
{

	AITPS_GBL pGBL = AITC_BASE_GBL;


	if (!data)
		return;


	if(mmc_id==MMPF_SD_0)
	{
		//AITPS_SD pSD = AITC_BASE_SD0;
	
		if(data->pad_id==MMPF_SD_PAD0)
		{
			//todo
			pGBL->GBL_IO_CTL3 |=GBL_SD0_IO_PAD0_EN;	
		}			
		else	
		{
			pGBL->GBL_IO_CFG_PCGPIO[0] = 0<<5|1<<2;	//PCGPIO10 ==> SD1_CLK , 9.6mA , pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[1] = 0<<5|1<<2;	//PCGPIO11 ==> SD1_CMD, 9.6mA ,pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[2] = 0<<5|1<<2;	//PCGPIO12 ==> SD1_DATA0, 8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[3] = 0<<5|1<<2;	//PCGPIO13 ==> SD1_DATA1,  8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO[4] = 0<<5|1<<2;	//PCGPIO14 ==> SD1_DATA2,  8.4mA, pull-up 		
			pGBL->GBL_IO_CFG_PCGPIO[5] = 0<<5|1<<2;	//PCGPIO15 ==> SD1_DATA3,  8.4mA, pull-up

			pGBL->GBL_IO_CTL3 |= GBL_SD0_IO_PAD1_EN;
		}
	
	}
	else if(mmc_id==MMPF_SD_1)
	{
		if(data->pad_id==MMPF_SD_PAD0)
		{
			//todo
			pGBL->GBL_IO_CTL0   |=GBL_SD1_IO_PAD0_EN;		
		}
		else	
		{
			pGBL->GBL_IO_CFG_PCGPIO6[4] = 0<<5|1<<2;	//PCGPIO10 ==> SD1_CLK , 9.6mA , pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[5] = 0<<5|1<<2;	//PCGPIO11 ==> SD1_CMD, 9.6mA ,pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[6] = 0<<5|1<<2;	//PCGPIO12 ==> SD1_DATA0, 8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[7] = 0<<5|1<<2;	//PCGPIO13 ==> SD1_DATA1,  8.4mA, pull-up 
			pGBL->GBL_IO_CFG_PCGPIO6[8] = 0<<5|1<<2;	//PCGPIO14 ==> SD1_DATA2,  8.4mA, pull-up 		
			pGBL->GBL_IO_CFG_PCGPIO6[9] = 0<<5|1<<2;	//PCGPIO15 ==> SD1_DATA3,  8.4mA, pull-up
		
			pGBL->GBL_IO_CTL4   |=GBL_SD1_IO_PAD1_EN;	
			pGBL->GBL_IO_CTL14 |=0x80;
		}
		
	}


	if(mmc_id)
	{
//		mmc_sd1_data = data;
		vsnv3_sd1_device.dev.platform_data = data;
		platform_device_register(&vsnv3_sd1_device);
	}
	else
	{
//		mmc_sd0_data = data;
		vsnv3_sd0_device.dev.platform_data = data;
		platform_device_register(&vsnv3_sd0_device);
	}
	
}


#else
void __init vsnv3_add_device_mmc(short mmc_id, struct ait_sd_data *data) {}

#endif
#if 0

/* --------------------------------------------------------------------
 *  TWI (i2c)
 * -------------------------------------------------------------------- */

/*
 * Prefer the GPIO code since the TWI controller isn't robust
 * (gets overruns and underruns under load) and can only issue
 * repeated STARTs in one scenario (the driver doesn't yet handle them).
 */

//#if defined(CONFIG_I2C_GPIO) || defined(CONFIG_I2C_GPIO_MODULE)

static struct i2c_gpio_platform_data pdata = {
	.sda_pin		= AT91_PIN_PA23,
	.sda_is_open_drain	= 1,
	.scl_pin		= AT91_PIN_PA24,
	.scl_is_open_drain	= 1,
	.udelay			= 2,		/* ~100 kHz */
};

static struct platform_device at91sam9260_twi_device_gpio = {
	.name			= "i2c-gpio",
	.id			= -1,
	.dev.platform_data	= &pdata,
};

#endif
//#elif defined(CONFIG_I2C_AT91) || defined(CONFIG_I2C_AT91_MODULE)

static struct resource twi_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_I2CM1,//AITC_BASE_PHY_I2CM,//AT91SAM9260_BASE_TWI,
		.end		= AITC_BASE_PHY_I2CM1+SZ_256-1,//AITC_BASE_PHY_I2CM+ sizeof(AITS_I2CMS) - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_I2CM ,
		.end		= AITVSNV3_ID_I2CM ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource ait_vsnv3_phi2c_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_I2CM2,
		.end		= AITC_BASE_PHY_I2CM2+SZ_256-1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_I2CM ,
		.end		= AITVSNV3_ID_I2CM ,
		.flags	= IORESOURCE_IRQ,
	},
};

#if 0
static struct platform_device at91sam9260_twi_device = {
	.name		= "VsionV3_i2c",
	.id			= -1,
	.resource	= twi_resources,
	.num_resources	= ARRAY_SIZE(twi_resources),
};
#endif

#if 0
static struct platform_device ait_vsnv3_phi2c= {
	.name		= "VsionV3_phi2c",
	.id			= -1,
	.resource	= ait_vsnv3_phi2c_resources,
	.num_resources	= ARRAY_SIZE(ait_vsnv3_phi2c_resources),
};
#else

//static DEFINE_SPINLOCK(i2cm_i2c_0_lock);
//static DEFINE_SPINLOCK(i2cm_i2c_1_lock);
#define MAX_I2C_HW 2
static struct semaphore g_I2C_sem[MAX_I2C_HW];
static struct semaphore* g_pI2C_sem[MAX_I2C_HW] = {0};
static struct ait_i2c_extension ait_i2c_0_pad0_ext = {
		.uI2cmID = 0,
		.pad = 0,
		//.i2c_hw_spinlock = &i2cm_i2c_0_lock,
		.i2c_hw_lock = &g_pI2C_sem[0],
};
static struct ait_i2c_extension ait_i2c_0_pad1_ext = {
		.uI2cmID = 0,
		.pad = 1,
		//.i2c_hw_spinlock = &i2cm_i2c_0_lock,
		.i2c_hw_lock = &g_pI2C_sem[0],
};
static struct ait_i2c_extension ait_i2c_1_pad0_ext = {
		.uI2cmID = 1,
		.pad = 0,
		//.i2c_hw_spinlock = 0,
		.i2c_hw_lock = 0, //&m_pI2C_sem[1],
};

static struct platform_device ait_vsnv3_i2c[] = {
	{// phi2c , pad 1
		.name		= "VSNV3_I2C",
		.id			= 0,
		.resource	= twi_resources,
		.num_resources	= ARRAY_SIZE(twi_resources),
		.dev = {
			.platform_data = &ait_i2c_1_pad0_ext,
		},
	},
	{// phi2c , pad 0
		.name		= "VSNV3_I2C",
		.id			= 1,
		.resource	= ait_vsnv3_phi2c_resources,
		.num_resources	= ARRAY_SIZE(ait_vsnv3_phi2c_resources), ////resource conflict
		.dev = {
			.platform_data = &ait_i2c_0_pad0_ext,
		},
	},
	{// phi2c , pad 1
		.name		= "VSNV3_I2C",
		.id			= 2,
		.resource	= 0,//ait_vsnv3_phi2c_resources,
		.num_resources	= 0,//ARRAY_SIZE(ait_vsnv3_phi2c_resources),
		.dev = {
			.platform_data = &ait_i2c_0_pad1_ext,
		},
	},

};
#endif

#if 0

void __init at91_add_device_i2c(struct i2c_board_info *devices, int nr_devices)
{
#if 0
	at91_set_GPIO_periph(AT91_PIN_PA23, 1);		/* TWD (SDA) */
	at91_set_multi_drive(AT91_PIN_PA23, 1);

	at91_set_GPIO_periph(AT91_PIN_PA24, 1);		/* TWCK (SCL) */
	at91_set_multi_drive(AT91_PIN_PA24, 1);
#endif

	
	MMPF_VIF_SetPIODir(VIF_SIF_SEN, MMP_TRUE);
	MMPF_VIF_SetPIODir(VIF_SIF_RST, MMP_TRUE);
	
	i2c_register_board_info(0, devices, nr_devices);
	platform_device_register(&at91sam9260_twi_device);
}

void __init vsnv3_add_device_phi2c(struct i2c_board_info *devices, int nr_devices)
{
extern MMP_USHORT MMPF_Audio_EnableAFEClock(MMP_BOOL bEnableClk, MMP_ULONG SampleRate);
extern MMP_USHORT  MMPF_SetVoiceInPath(MMP_UBYTE path);
//int i = 0;
#if 0
	at91_set_GPIO_periph(AT91_PIN_PA23, 1);		/* TWD (SDA) */
	at91_set_multi_drive(AT91_PIN_PA23, 1);

	at91_set_GPIO_periph(AT91_PIN_PA24, 1);		/* TWCK (SCL) */
	at91_set_multi_drive(AT91_PIN_PA24, 1);
#endif

	i2c_register_board_info(1, devices, nr_devices);
	platform_device_register(&ait_vsnv3_phi2c);
}
#else
void __init vsnv3_add_adapter_i2c()
{
	int n;
	for(n=0;n<ARRAY_SIZE(ait_vsnv3_i2c);++n)
	{
		struct ait_i2c_extension* i2c_ext = (struct ait_i2c_extension*)ait_vsnv3_i2c[n].dev.platform_data;
		if(i2c_ext->i2c_hw_lock) //check semaphore is initialized or not
		{
			int hw_id = i2c_ext->uI2cmID;
			if(!g_pI2C_sem[hw_id]) //inti it
			{
				g_pI2C_sem[hw_id] = &g_I2C_sem[hw_id];
				sema_init(g_pI2C_sem[hw_id],1);
			}
		}
		platform_device_register(&ait_vsnv3_i2c[n]);
	}
}
void __init vsnv3_add_device_i2c(struct i2c_board_info *devices, int nr_devices,int bus)
{
	i2c_register_board_info(bus, devices, nr_devices);
	//platform_device_register(&ait_vsnv3_phi2c);
}
#endif

//#else
//void __init at91_add_device_i2c(struct i2c_board_info *devices, int nr_devices) {}
//#endif


/* --------------------------------------------------------------------
 *  SPI
 * -------------------------------------------------------------------- */

#if defined(CONFIG_SPI_AIT) 
static u64 spi_dmamask = DMA_BIT_MASK(32);
static u64 sif_dmamask = DMA_BIT_MASK(32);

static struct resource spi_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_PSPI,
		.end	= AITC_BASE_PHY_PSPI + SZ_64-1, //(0x6D00~0x6D3F)
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_PSPI,
		.end	= AITVSNV3_ID_PSPI,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource sif_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_SIF,
		.end	= AITC_BASE_PHY_SIF + SZ_128-1, //(0x6D00~0x6D3F)
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_SIF,
		.end	= AITVSNV3_ID_SIF,
		.flags	= IORESOURCE_IRQ,
	},
};


static struct platform_device vsnv3_pspi_device = {
	.name		= "vsnv3_pspi",
	.id		= 1,
	.dev		= {
				.dma_mask		= &spi_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	=spi_resources,
	.num_resources	= ARRAY_SIZE(spi_resources),
};

static struct platform_device vsnv3_sif_device = {
	.name		= "vsnv3_sif",
	.id		= 0,
	.dev		= {
				.dma_mask		= &sif_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	=sif_resources,
	.num_resources	= ARRAY_SIZE(sif_resources),
};


//static const unsigned spi0_standard_cs[4] = { AT91_PIN_PA3, AT91_PIN_PC11, AT91_PIN_PC16, AT91_PIN_PC17 };

void __init vsnv3_add_device_pspi(struct spi_board_info *devices, int nr_devices)
{

	AITPS_GBL pGBL = AITC_BASE_GBL;

	int i;
	for(i=0;i<nr_devices;++i)
	{
		if(devices[i].bus_num!=vsnv3_pspi_device.id)
		{
			pr_err("bus_num of devices is not match master id. \n");
		}
	}
spi_register_board_info(devices, nr_devices);


	platform_device_register(&vsnv3_pspi_device);
	pGBL->GBL_IO_CTL1 &= ~(GBL_PSPI_IO_PAD1_EN);
	pGBL->GBL_IO_CTL3 |= GBL_PSPI_IO_PAD0_EN;	// PSPI pad0 : PBGPIO10,11,12,14
	
}

void __init vsnv3_add_device_sif(struct spi_board_info *devices, int nr_devices)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;

	int i;
	for(i=0;i<nr_devices;++i)
	{
		if(devices[i].bus_num!=vsnv3_sif_device.id)
		{
			pr_err("bus_num of devices is not match master id. \n");
		}
	}
	
	spi_register_board_info(devices, nr_devices);

	platform_device_register(&vsnv3_sif_device);
	pGBL->GBL_IO_CTL1 |= GBL_SIF_PAD_EN;
	
}


#else
void __init vsnv3_add_device_pspi(struct spi_board_info *devices, int nr_devices) {}
void __init vsnv3_add_device_sif(struct spi_board_info *devices, int nr_devices) {}

#endif


/* --------------------------------------------------------------------
 *  Timer/Counter blocks
 * -------------------------------------------------------------------- */

#ifdef CONFIG_ATMEL_TCLIB

static struct resource tcb0_resources[] = {
	[0] = {
		.start	= AT91SAM9260_BASE_TCB0,
		.end	= AT91SAM9260_BASE_TCB0 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM9260_ID_TC0,
		.end	= AT91SAM9260_ID_TC0,
		.flags	= IORESOURCE_IRQ,
	},
	[2] = {
		.start	= AT91SAM9260_ID_TC1,
		.end	= AT91SAM9260_ID_TC1,
		.flags	= IORESOURCE_IRQ,
	},
	[3] = {
		.start	= AT91SAM9260_ID_TC2,
		.end	= AT91SAM9260_ID_TC2,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam9260_tcb0_device = {
	.name		= "atmel_tcb",
	.id		= 0,
	.resource	= tcb0_resources,
	.num_resources	= ARRAY_SIZE(tcb0_resources),
};

static struct resource tcb1_resources[] = {
	[0] = {
		.start	= AT91SAM9260_BASE_TCB1,
		.end	= AT91SAM9260_BASE_TCB1 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},

};

static struct platform_device at91sam9260_tcb1_device = {
	.name		= "atmel_tcb",
	.id		= 1,
	.resource	= tcb1_resources,
	.num_resources	= ARRAY_SIZE(tcb1_resources),
};

static void __init vsnv3_add_device_tc(void)
{
	platform_device_register(&at91sam9260_tcb0_device);
	platform_device_register(&at91sam9260_tcb1_device);
}
#else
static void __init vsnv3_add_device_tc(void) { }
#endif


/* --------------------------------------------------------------------
 *  RTT
 * -------------------------------------------------------------------- */
#if 0
static struct resource rtt_resources[] = {
	{
		.start	= AT91_BASE_SYS + AT91_RTT,
		.end	= AT91_BASE_SYS + AT91_RTT + SZ_16 - 1,
		.flags	= IORESOURCE_MEM,
	}
};

static struct platform_device at91sam9260_rtt_device = {
	.name		= "at91_rtt",
	.id		= 0,
	.resource	= rtt_resources,
	.num_resources	= ARRAY_SIZE(rtt_resources),
};

static void __init at91_add_device_rtt(void)
{
	platform_device_register(&at91sam9260_rtt_device);
}
#endif

/* --------------------------------------------------------------------
 *  Watchdog
 * -------------------------------------------------------------------- */

//#if defined(CONFIG_AT91SAM9X_WATCHDOG) || defined(CONFIG_AT91SAM9X_WATCHDOG_MODULE)
//static struct platform_device at91sam9260_wdt_device = {
#if defined(CONFIG_VSNV3_WATCHDOG)
static struct resource vsnv3_wdt_resources[] = {	
	[0] = {
		.start	= AITC_BASE_PHY_WD,
		.end		= AITC_BASE_PHY_WD+SZ_16 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_WDT,
		.end		= AITVSNV3_ID_WDT,
		.flags	= IORESOURCE_IRQ,
	},
	
};

static struct platform_device vsnv3_wdt_device = {
	.name		= "vsnv3_wdt",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(vsnv3_wdt_resources),
	.resource			=vsnv3_wdt_resources,
};

static void __init at91_add_device_watchdog(void)
{
	platform_device_register(&vsnv3_wdt_device);
}
#else
static void __init at91_add_device_watchdog(void) {}
#endif


/* --------------------------------------------------------------------
 *  SSC -- Synchronous Serial Controller
 * -------------------------------------------------------------------- */

#if defined(CONFIG_AIT_SSC) || defined(CONFIG_AIT_SSC_MODULE)

static u64 ssc_dmamask = DMA_BIT_MASK(32);

static struct resource ssc_resources[] = {
#if 1	
	[0] = {
		.start	= AITC_BASE_PHY_AUD,
		.end		= AITC_BASE_PHY_AUD+SZ_256 - 1,
		.flags	= IORESOURCE_MEM,
	},
#endif	
	[1] = {
		.start	= AITVSNV3_ID_I2S_FIFO,
		.end		= AITVSNV3_ID_I2S_FIFO,
		.flags	= IORESOURCE_IRQ,
	},
	
};


static struct resource ssc1_resources[] = {
#if 1	
	[0] = {
		.start	= AITC_BASE_PHY_AFE,
		.end		= AITC_BASE_PHY_AFE+SZ_128 - 1,
		.flags	= IORESOURCE_MEM,
	},
#endif	
	[1] = {
		.start	= AITVSNV3_ID_AFE_FIFO,
		.end		= AITVSNV3_ID_AFE_FIFO,
		.flags	= IORESOURCE_IRQ,
	},
	
};

static struct resource afe_resources[] = {
#if 1
	[0] = {
		.start	= AITC_BASE_PHY_AFE,
		.end		= AITC_BASE_PHY_AFE+SZ_128 - 1,
		.flags	= IORESOURCE_MEM,
	},
#endif	
	[1] = {
		.start	= AITVSNV3_ID_AFE_FIFO,
		.end		= AITVSNV3_ID_AFE_FIFO,
		.flags	= IORESOURCE_IRQ,
	},
	
};



static struct resource soc_audio_resources[] = {
#if 0	
	[0] = {
		.start	= AT91SAM9260_BASE_SSC+1,
		.end	= AT91SAM9260_BASE_SSC + 2,//SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
#endif
	[0] = {
		.start	= 14,
		.end	= 14,
		.flags	= IORESOURCE_IRQ,
	},

};

static struct resource wm8971_hifi_resources[] = {
	[0] = {
		.start	= AT91SAM9260_BASE_SSC+2,
		.end	= AT91SAM9260_BASE_SSC + 3,
		.flags	= IORESOURCE_MEM,
	},
	
	[1] = {
		.start	= 15,
		.end	= 15,
		.flags	= IORESOURCE_IRQ,
	},
	
};
#if 0
static struct resource wm8973_codec_resources[] = {
#if 0	
	[0] = {
		.start	= AT91SAM9260_BASE_SSC+3,
		.end	= AT91SAM9260_BASE_SSC + 4,//SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
#endif
	[0] = {
		.start	= 16,
		.end	= 16,
		.flags	= IORESOURCE_IRQ,
	},

	
};
static struct platform_device at91sam9260_soc_audio = {
	.name	= "soc-audio",
	.id	= 0,
	.dev	= {
		.dma_mask		= &ssc_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= soc_audio_resources,
	.num_resources = ARRAY_SIZE(soc_audio_resources),
};
#endif

static struct platform_device vsnv3_i2s_device = {
	.name	= "vsnv3aud",
	.id	= 0,
	.dev	= {
		.dma_mask		= &ssc_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= ssc_resources,
	.num_resources	= ARRAY_SIZE(ssc_resources),
};


static struct platform_device vsnv3_afe_device = {
	.name	= "vsnv3aud",
	.id	= 1,
	.dev	= {
		.dma_mask		= &ssc_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= ssc1_resources,
	.num_resources	= ARRAY_SIZE(ssc1_resources),
};



static struct platform_device vsnv3_afe_codec_device = {
	.name = "vsnv3-afe-codec",	
	.id	= -1,

	.dev	= {
		.dma_mask		= &ssc_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
		.platform_data 		=0,//(void*)&vsnv3afe_data,
	},
	.resource	= afe_resources,
	.num_resources	= ARRAY_SIZE(afe_resources),
};
#if 0
static struct platform_device wm8973_hifi = {
	.name	= "wm8973-hifi",
	.id	= 0,
	.dev	= {
		.dma_mask		= &ssc_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= wm8971_hifi_resources,
	.num_resources	= ARRAY_SIZE(wm8971_hifi_resources),
};

//struct ac97c_platform_data ac97_data;


static struct platform_device wm8973_codec = {
	.name	= "wm8973-codec",
	.id	= 0,
	.dev	= {
		.dma_mask		= &ssc_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
//		.platform_data 		=(void*)&ac97_data,
	},
	.resource	= wm8973_codec_resources,
	.num_resources	= ARRAY_SIZE(wm8973_codec_resources),
};
#endif
static inline void configure_ssc_pins(unsigned pins)
{
#if 0
	if (pins & ATMEL_SSC_TF)
		at91_set_A_periph(AT91_PIN_PB17, 1);
	if (pins & ATMEL_SSC_TK)
		at91_set_A_periph(AT91_PIN_PB16, 1);
	if (pins & ATMEL_SSC_TD)
		at91_set_A_periph(AT91_PIN_PB18, 1);
	if (pins & ATMEL_SSC_RD)
		at91_set_A_periph(AT91_PIN_PB19, 1);
	if (pins & ATMEL_SSC_RK)
		at91_set_A_periph(AT91_PIN_PB20, 1);
	if (pins & ATMEL_SSC_RF)
		at91_set_A_periph(AT91_PIN_PB21, 1);
#endif		
}

/*
 * SSC controllers are accessed through library code, instead of any
 * kind of all-singing/all-dancing driver.  For example one could be
 * used by a particular I2S audio codec's driver, while another one
 * on the same system might be used by a custom data capture driver.
 */
void __init vsnv3_add_device_ssc(unsigned id, unsigned pins)
{
	struct platform_device *pdev;
	/*
	 * NOTE: caller is responsible for passing information matching
	 * "pins" to whatever will be using each particular controller.
	 */
	switch (id) {
	case AITVSNV3_ID_I2S_FIFO:
		pdev = &vsnv3_i2s_device;
		configure_ssc_pins(pins);
		break;
	case AITVSNV3_ID_AFE_FIFO:
		pdev = &vsnv3_afe_device;
		configure_ssc_pins(pins);
		platform_device_register(&vsnv3_afe_codec_device);		
		break;
	default:
		return;
	}

	platform_device_register(pdev);

}

#else
void __init vsnv3_add_device_ssc(unsigned id, unsigned pins) {}
#endif


/* --------------------------------------------------------------------
 *  UART
 * -------------------------------------------------------------------- */
#if 1//defined(CONFIG_SERIAL_AIT)

static struct resource ait_uart0_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_UART0, //((__u32)AITC_BASE_UARTB),
		.end	= AITC_BASE_PHY_UART0 + sizeof(AITS_US) -1,//((__u32)AITC_BASE_UARTB) + sizeof(AITS_US) -1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_UART,//AITVSNV3_ID_UART,//AT91SAM9260_ID_US0,//AT91_ID_SYS,
		.end	= AITVSNV3_ID_UART,//AITVSNV3_ID_UART,//AT91SAM9260_ID_US0,//AT91_ID_SYS,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct resource ait_uart1_resources[] = {
	[0] = {
		.start	= AITC_BASE_PHY_UART1,
		.end	= AITC_BASE_PHY_UART1 + sizeof(AITS_US) -1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {  //resource conflict
		.start	= AITVSNV3_ID_UART,//AT91SAM9260_ID_US0,//AT91_ID_SYS,
		.end	= AITVSNV3_ID_UART,//AT91SAM9260_ID_US0,//AT91_ID_SYS,
		.flags	= IORESOURCE_IRQ,
	},
};
static struct atmel_uart_data ait_uart_data[] = {
	{
		.num = 0,
		.use_dma_tx	= 0,
		.use_dma_rx	= 0,		/* DBGU not capable of receive DMA */
		.uart_hw_id = 0,
		.pad = 0,
		.baudrate = 115200,
		.fifo_size = 0x80,
		.hw_status = UART_UNINIT,
		.pUS = (AITPS_US) AITC_BASE_UART0,
	},
	{
		.num = 1,
		.use_dma_tx	= 0,
		.use_dma_rx	= 0,		/* DBGU not capable of receive DMA */
		.uart_hw_id = 1,
		.pad = 0,
		.baudrate = 115200,
		.fifo_size = 0x20,
		.hw_status = UART_UNINIT,
		.pUS = (AITPS_US) AITC_BASE_UART1,
	},
};

static u64 dbgu_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam9260_dbgu_device[] = {
	{
		.name		= "atmel_usart",
		.id		= 0,
		.dev		= {
					.dma_mask		= &dbgu_dmamask,
					.coherent_dma_mask	= DMA_BIT_MASK(32),
					.platform_data		= &ait_uart_data[0],
		},
		.resource	= ait_uart0_resources,
		.num_resources	= ARRAY_SIZE(ait_uart0_resources),
	},
	{
		.name		= "atmel_usart",
		.id			= 1,
		.dev		= {
					.dma_mask		= &dbgu_dmamask,
					.coherent_dma_mask	= DMA_BIT_MASK(32),
					.platform_data		= &ait_uart_data[1],
		},
		.resource	= ait_uart1_resources,
		.num_resources	= ARRAY_SIZE(ait_uart1_resources),
	},
};

static inline void configure_dbgu_pins(void)
{
//	at91_set_A_periph(AT91_PIN_PB14, 0);		/* DRXD */
//	at91_set_A_periph(AT91_PIN_PB15, 1);		/* DTXD */
}
#if 0
static struct resource uart0_resources[] = {
	[0] = {
		.start	= AT91SAM9260_BASE_US0,
		.end	= AT91SAM9260_BASE_US0 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AITVSNV3_ID_UART,//AT91SAM9260_ID_US0,
		.end		= AITVSNV3_ID_UART,//AT91SAM9260_ID_US0,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct atmel_uart_data uart0_data = {
	.use_dma_tx	= 1,
	.use_dma_rx	= 1,
};

static u64 uart0_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam9260_uart0_device = {
	.name		= "atmel_usart",
	.id		= 1,
	.dev		= {
				.dma_mask		= &uart0_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &uart0_data,
	},
	.resource	= uart0_resources,
	.num_resources	= ARRAY_SIZE(uart0_resources),
};
#endif
static inline void configure_usart0_pins(unsigned pins)
{
#if 0
	at91_set_A_periph(AT91_PIN_PB4, 1);		/* TXD0 */
	at91_set_A_periph(AT91_PIN_PB5, 0);		/* RXD0 */

	if (pins & ATMEL_UART_RTS)
		at91_set_A_periph(AT91_PIN_PB26, 0);	/* RTS0 */
	if (pins & ATMEL_UART_CTS)
		at91_set_A_periph(AT91_PIN_PB27, 0);	/* CTS0 */
	if (pins & ATMEL_UART_DTR)
		at91_set_A_periph(AT91_PIN_PB24, 0);	/* DTR0 */
	if (pins & ATMEL_UART_DSR)
		at91_set_A_periph(AT91_PIN_PB22, 0);	/* DSR0 */
	if (pins & ATMEL_UART_DCD)
		at91_set_A_periph(AT91_PIN_PB23, 0);	/* DCD0 */
	if (pins & ATMEL_UART_RI)
		at91_set_A_periph(AT91_PIN_PB25, 0);	/* RI0 */
#endif	
}
#if 0
static struct resource uart1_resources[] = {
	[0] = {
		.start	= AT91SAM9260_BASE_US1,
		.end	= AT91SAM9260_BASE_US1 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM9260_ID_US1,
		.end	= AT91SAM9260_ID_US1,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct atmel_uart_data uart1_data = {
	.use_dma_tx	= 1,
	.use_dma_rx	= 1,
};

static u64 uart1_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam9260_uart1_device = {
	.name		= "atmel_usart",
	.id		= 2,
	.dev		= {
				.dma_mask		= &uart1_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &uart1_data,
	},
	.resource	= uart1_resources,
	.num_resources	= ARRAY_SIZE(uart1_resources),
};

static inline void configure_usart1_pins(unsigned pins)
{
	at91_set_A_periph(AT91_PIN_PB6, 1);		/* TXD1 */
	at91_set_A_periph(AT91_PIN_PB7, 0);		/* RXD1 */

	if (pins & ATMEL_UART_RTS)
		at91_set_A_periph(AT91_PIN_PB28, 0);	/* RTS1 */
	if (pins & ATMEL_UART_CTS)
		at91_set_A_periph(AT91_PIN_PB29, 0);	/* CTS1 */
}

#endif
static struct platform_device *__initdata at91_uarts[AIT_MAX_UART] = { /* the UARTs to use */
		&at91sam9260_dbgu_device[0],
		&at91sam9260_dbgu_device[1],
};
struct platform_device *ait_default_console_device;	/* the serial console device */

void __init vsnv3_register_uart(unsigned id, unsigned portnr, unsigned pins)
{
	struct platform_device *pdev;
	struct atmel_uart_data *pdata;

	//pdev = &at91sam9260_dbgu_device;
#if 0	//Vin				
	switch (id) {
		case 0:		/* DBGU */
			pdev = &at91sam9260_dbgu_device;
//			configure_dbgu_pins();
			break;

		case AT91SAM9260_ID_US0:
//			pdev = &at91sam9260_uart0_device;
			configure_usart0_pins(pins);
			break;
					
		case AT91SAM9260_ID_US1:
			pdev = &at91sam9260_uart1_device;
			configure_usart1_pins(pins);
			break;

		case AT91SAM9260_ID_US2:
			pdev = &at91sam9260_uart2_device;
			configure_usart2_pins(pins);
			break;
		case AT91SAM9260_ID_US3:
			pdev = &at91sam9260_uart3_device;
			configure_usart3_pins(pins);
			break;
		case AT91SAM9260_ID_US4:
			pdev = &at91sam9260_uart4_device;
			configure_usart4_pins();
			break;
		case AT91SAM9260_ID_US5:
			pdev = &at91sam9260_uart5_device;
			configure_usart5_pins();
			break;

		default:
			return;
	}
#endif				
	pdev = &at91sam9260_dbgu_device[id];
	pdata = (struct atmel_uart_data*)(at91sam9260_dbgu_device[id].dev.platform_data);

	if (portnr < AIT_MAX_UART)
		at91_uarts[portnr] = pdev;
}

void __init vsnv3_set_serial_console(unsigned portnr)
{
	if (portnr < AIT_MAX_UART) {
		ait_default_console_device = at91_uarts[0];
		at91sam9260_set_console_clock(at91_uarts[0]->id);
	}
}

void __init ait_add_device_serial(void)
{
	int i;

	for (i = 0; i < AIT_MAX_UART; i++) {
		if (at91_uarts[i])
			platform_device_register(at91_uarts[i]);
	}

	if (!ait_default_console_device)
		printk(KERN_INFO "AT91: No default serial console defined.\n");
}
#else
void __init vsnv3_register_uart(unsigned id, unsigned portnr, unsigned pins) {}
void __init vsnv3_set_serial_console(unsigned portnr) {}
void __init ait_add_device_serial(void) {}
#endif


/* -------------------------------------------------------------------- */
/*
 * These devices are always present and don't need any board-specific
 * setup.
 */
static int __init at91_add_standard_devices(void)
{
//	at91_add_device_rtt();
	at91_add_device_watchdog();
	//vsnv3_add_device_tc();
	return 0;
}

arch_initcall(at91_add_standard_devices);
