/*
 * linux/arch/arm/mach-at91/clock.c
 *
 * Copyright (C) 2005 David Brownell
 * Copyright (C) 2005 Ivan Kokshaysky
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <mach/hardware.h>
#include <mach/cpu.h>

#include "clock.h"
#include "generic.h"

#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_sd.h>
#include <mach/mmpf_audio_ctl.h>
#include <mach/mmpf_i2s_ctl.h>

#include <mach/mmpf_sd.h>
/*
 * There's a lot more which can be done with clocks, including cpufreq
 * integration, slow clock mode support (for system suspend), letting
 * PLLB be used at other rates (on boards that don't need USB), etc.
 */

#define clk_is_primary(x)	((x)->type & CLK_TYPE_PRIMARY)
#define clk_is_programmable(x)	((x)->type & CLK_TYPE_PROGRAMMABLE)
#define clk_is_peripheral(x)	((x)->type & CLK_TYPE_PERIPHERAL)
#define clk_is_sys(x)		((x)->type & CLK_TYPE_CLK_SYS)

/*
 * Chips have some kind of clocks : group them by functionality
 */
#define cpu_has_utmi()		(  cpu_is_at91cap9() \
				|| cpu_is_at91sam9rl() \
				|| cpu_is_at91sam9g45())

#define cpu_has_800M_plla()	(  cpu_is_at91sam9g20() \
				|| cpu_is_at91sam9g45())

#define cpu_has_300M_plla()	(cpu_is_at91sam9g10())

#define cpu_has_pllb()		(!(cpu_is_at91sam9rl() \
				|| cpu_is_at91sam9g45()))

#define cpu_has_upll()		(cpu_is_at91sam9g45())

/* USB host HS & FS */
#define cpu_has_uhp()		(!cpu_is_at91sam9rl())

/* USB device FS only */
#define cpu_has_udpfs()		(!(cpu_is_at91sam9rl() \
				|| cpu_is_at91sam9g45()))

static LIST_HEAD(clocks);
static DEFINE_SPINLOCK(clk_lock);

static u32 at91_pllb_usb_init;

#if 0
static struct clk clk32k = {
	.name		= "clk32k",
	.rate_hz	= AT91_SLOW_CLOCK,
	.users		= 1,		/* always on */
	.id		= 0,
	.type		= CLK_TYPE_PRIMARY,
};
#endif
static struct clk main_clk = {	//External Crystal Alwayse on
	.name		= "main",
	.rate_hz		= 12000000,
	.pmc_mask	= 0,
	.id		= 1,
	.type		= CLK_TYPE_PRIMARY,
};
#if 0
static struct clk plla = {
	.name		= "plla",
	.parent		= &main_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,	/* in PMC_SR */
	.id		= 2,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};
#endif

static struct clk pll0 = {
	.name		= "pll0",
	.rate_hz		= 600000000,		
	.parent		= &main_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 3,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};

static struct clk pll1 = {
	.name		= "pll1",
	.rate_hz		= 180000000,
	.parent		= &main_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 4,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};

static struct clk pll2 = {
	.name		= "pll2",
	//.rate_hz		= 264000000,						
	.parent		= &main_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 5,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};


static void pllb_mode(struct clk *clk, int is_on)
{
	u32	value;

	if (is_on) {
//		is_on = AT91_PMC_LOCKB;
//		value = at91_pllb_usb_init;
	} else
		value = 0;

	// REVISIT: Add work-around for AT91RM9200 Errata #26 ?
//	at91_sys_write(AT91_CKGR_PLLBR, value);

//	do {
//		cpu_relax();
//	} while ((at91_sys_read(AT91_PMC_SR) & AT91_PMC_LOCKB) != is_on);
}
#if 0
static struct clk pllb = {
	.name		= "pllb",
	.parent		= &main_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKB,	/* in PMC_SR */
	.mode		= pllb_mode,
	.id		= 3,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};
#endif

static void pmc_sys_mode(struct clk *clk, int is_on)
{
#if 0
	AITPS_GBL pGBL = AITC_BASE_GBL;

	if (is_on)
	{
		if(clk->pmc_mask&CLK_TYPE_CLK_CTL1)
		{
			pGBL->GBL_CLK_DIS0 &=~clk->pmc_mask;
		}
		if(clk->pmc_mask&CLK_TYPE_CLK_CTL2||clk->pmc_mask&CLK_TYPE_CLK_CTL3)
		{
			pGBL->GBL_CLK_DIS1 &=~clk->pmc_mask;
		}
		if(clk->pmc_mask&CLK_TYPE_CLK_CTL4)
		{
			pGBL->GBL_CLK_DIS2 &=~clk->pmc_mask;
		}

	}
	else
	{
		if(clk->pmc_mask&CLK_TYPE_CLK_CTL1)
		{
			pGBL->GBL_CLK_DIS0 |=clk->pmc_mask;
		}
		if(clk->pmc_mask&CLK_TYPE_CLK_CTL2||clk->pmc_mask&CLK_TYPE_CLK_CTL3)
		{
			pGBL->GBL_CLK_DIS1 |=clk->pmc_mask;
		}
		if(clk->pmc_mask&CLK_TYPE_CLK_CTL4)
		{
			pGBL->GBL_CLK_DIS2 |=clk->pmc_mask;
		}
//		at91_sys_write(AT91_PMC_PCDR, clk->pmc_mask);
	}
#endif
	
/*
	if (is_on)
		at91_sys_write(AT91_PMC_SCER, clk->pmc_mask);
	else
		at91_sys_write(AT91_PMC_SCDR, clk->pmc_mask);
*/		
}

static void pmc_uckr_mode(struct clk *clk, int is_on)
{
#if 0
	unsigned int uckr = at91_sys_read(AT91_CKGR_UCKR);

	if (cpu_is_at91sam9g45()) {
		if (is_on)
			uckr |= AT91_PMC_BIASEN;
		else
			uckr &= ~AT91_PMC_BIASEN;
	}

	if (is_on) {
		is_on = AT91_PMC_LOCKU;
		at91_sys_write(AT91_CKGR_UCKR, uckr | clk->pmc_mask);
	} else
		at91_sys_write(AT91_CKGR_UCKR, uckr & ~(clk->pmc_mask));

	do {
		cpu_relax();
	} while ((at91_sys_read(AT91_PMC_SR) & AT91_PMC_LOCKU) != is_on);
#endif	
}
#if 0
/* USB function clocks (PLLB must be 48 MHz) */
static const struct clk udpck = {
	.name		= "udpck",
	.parent		= &pllb,
	.mode		= pmc_sys_mode,
};

static const struct clk utmi_clk = {
	.name		= "utmi_clk",
	.parent		= &main_clk,
	.pmc_mask	= AT91_PMC_UPLLEN,	/* in CKGR_UCKR */
	.mode		= pmc_uckr_mode,
	.type		= CLK_TYPE_PLL,
};
static const struct clk uhpck = {
	.name		= "uhpck",
	/*.parent		= ... we choose parent at runtime */
	.mode		= pmc_sys_mode,
};
#endif

static struct clk cpu_clk = {
	.name		= "cpu_clk",
	.rate_hz		= 528000000,						
	.parent		= &main_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 6,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};

static struct clk g0_global_clk = {		//For Global
	.name		= "g0_global_clk",
	.parent		= 0,//&pll2,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 7,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};

static struct clk g0_global_clk_half = {		//For Global
	.name		= "g0_global_clk_half",
	.parent		= &g0_global_clk,
	.pmc_mask	= 0,// AT91_PMC_LOCKA,
	.id		= 7,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};

static struct clk g1_dram_clk = {		//For DRAM
	.name		= "g1_dram_clk",
	.rate_hz		= 180000000,	
	.parent		= &pll1,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 8,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};
#if 0
static struct clk g2_clk = {		//For USBPHY
	.name		= "g0_clk",
	.rate_hz		= 264000000,
	.parent		= &main_clk,
	.pmc_mask	= AT91_PMC_LOCKA,
	.id		= 9,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};
static struct clk g3_clk = {		//For RX_BIST
	.name		= "g0_clk",
	.rate_hz		= 264000000,	
	.parent		= &pll2,
	.pmc_mask	= AT91_PMC_LOCKA,
	.id		= 10,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};
#endif
static struct clk g4_sensor_clk = {		//For Sensor
	.name		= "g4_sensor_clk",
	//.rate_hz		= 264000000,
	.parent		= &g0_global_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 11,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};
static struct clk g5_audio_clk = {		//For Audio
	.name		= "g5_audio_clk",
	//.rate_hz		= 264000000,						
	.parent		= &g0_global_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 12,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};
static struct clk g6_isp_clk = {		//For ISP
	.name		= "g6_isp_clk",
	//.rate_hz		= 264000000,					
	.parent		= &g0_global_clk,
	.pmc_mask	= 0,//AT91_PMC_LOCKA,
	.id		= 13,
	.type		= CLK_TYPE_PRIMARY | CLK_TYPE_PLL,
};


static struct clk gpio_clk = {
	.name		= "gpio_clk",
	.parent		= &g0_global_clk_half,
	.pmc_mask	= GBL_CLK_GPIO_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL1,
};

static  struct clk isp_clk = {
	.name		= "isp_clk",
	.pmc_mask	= GBL_CLK_ISP_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL1,
};

static struct clk vif_clk = {
	.name		= "vif_clk",
	.pmc_mask	= GBL_CLK_VI_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL1,
};

struct clk audio_clk = {
	.name		= "audio_clk",
	.pmc_mask	= GBL_CLK_AUD_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL1,
};

static const struct clk jpeg_clk = {
	.name		= "jpeg_clk",
	.parent		= &g0_global_clk_half,		
	.pmc_mask	= GBL_CLK_JPG_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL1,
};

static const struct clk scaling_clk = {
	.name		= "scaling_clk",
	.parent		= &g0_global_clk_half,		
	.pmc_mask	= GBL_CLK_SCAL_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL1,
};
#if 0
static struct clk cpu_clk = {
	.name		= "adc_clk",
	.pmc_mask	= GBL_CLK_CPU_DIS,
	.type		= CLK_TYPE_CLK_CTL1,
};
#endif
static struct clk mci_clk = {
	.name		= "mci_clk",
	.pmc_mask	= GBL_CLK_MCI_DIS ,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL1,
};

static struct clk graphic_clk = {
	.name		= "graphic_clk",
	.pmc_mask	= GBL_CLK_GRA_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL2,
};

static struct clk icon_clk = {
	.name		= "icon_clk",
	.pmc_mask	= GBL_CLK_ICON_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL2,
};

static struct clk h264_clk = {
	.name		= "h264_clk",
	.pmc_mask	= GBL_CLK_H264_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL2,
};

const struct clk i2c_clk = {
	.name		= "i2c_clk",
	.parent		= &g0_global_clk_half,
	.pmc_mask	= GBL_CLK_I2C_DIS,
	.mode		= pmc_sys_mode,
	.type		= CLK_TYPE_CLK_CTL2,
};

static struct clk dma_clk = {
	.name		= "dma_clk",
	.pmc_mask	= GBL_CLK_DMA_DIS,
	.type		= CLK_TYPE_CLK_CTL2,
};

static struct clk raw_clk = {
	.name		= "raw_clk",
	.pmc_mask	= GBL_CLK_RAW_DIS,
	.type		= CLK_TYPE_CLK_CTL2,
};

struct clk bootspi_clk = {
	.name		= "bootspi_clk",
	.parent		=&g0_global_clk,
	.pmc_mask	= GBL_CLK_BS_SPI_DIS,
	.type		= CLK_TYPE_CLK_CTL2,
};

static struct clk dram_clk = {
	.name		= "dram_clk",
	.pmc_mask	= GBL_CLK_DRAM_DIS ,
	.type		= CLK_TYPE_CLK_CTL2,
};

struct clk cpu_peri_clk = {
	.name		= "cpu_peri_clk",
	.parent		= &g0_global_clk_half,
	.pmc_mask	= GBL_CLK_CPU_PHL_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

static struct clk mipi_clk = {
	.name		= "mipi_clk",
	.pmc_mask	= GBL_CLK_MIPI_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

struct clk pspi_clk = {
	.name		= "pspi_clk",
	.parent		= &g0_global_clk_half,	
	.pmc_mask	= GBL_CLK_PSPI_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

struct clk sd0_clk = {
	.name		= "sd0_clk",
	.parent		= &g0_global_clk,		
	.pmc_mask	= GBL_CLK_SD0_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

struct clk sd1_clk = {
	.name		= "sd1_clk",
	.parent		= &g0_global_clk,
	.pmc_mask	= GBL_CLK_SD1_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

static struct clk pwm_clk = {
	.name		= "pwm_clk",
	.parent		= &g0_global_clk_half,
	.pmc_mask	= GBL_CLK_PWM_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

static struct clk ibc_clk = {
	.name		= "ibc_clk",
	.pmc_mask	= GBL_CLK_IBC_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

struct clk usb_clk = {
	.name		= "usb_clk",
	.pmc_mask	= GBL_CLK_USB_DIS ,
	.type		= CLK_TYPE_CLK_CTL3,
};

static struct clk color_clk = {
	.name		= "color_clk",
	.pmc_mask	= GBL_CLK_COLOR_DIS,
	.type		= CLK_TYPE_CLK_CTL4,
};

static struct clk sm_clk = {
	.name		= "sm_clk",
	.pmc_mask	= GBL_CLK_SM_DIS,
	.type		= CLK_TYPE_CLK_CTL4,
};

static struct clk gnr_clk = {
	.name		= "gnr_clk",
	.pmc_mask	= GBL_CLK_GNR_DIS,
	.type		= CLK_TYPE_CLK_CTL4,
};

static struct clk cidc_clk = {
	.name		= "cidc_clk",
	.pmc_mask	= GBL_CLK_CIDC_DIS,
	.type		= CLK_TYPE_CLK_CTL4,
};

struct clk audio2_clk = {	//For codec, adc df, adc hbfs
	.name		= "audio2_clk",
	.pmc_mask	= GBL_CLK_AUD_CODEC_DIS ,
	.type		= CLK_TYPE_CLK_CTL4,
};

/*
 * The master clock is divided from the CPU clock (by 1-4).  It's used for
 * memory, interfaces to on-chip peripherals, the AIC, and sometimes more
 * (e.g baud rate generation).  It's sourced from one of the primary clocks.
 */
//struct clk mck = {
//	.name		= "mck",
//	.pmc_mask	= 0,//AT91_PMC_MCKRDY,	/* in PMC_SR */
//};

static void pmc_periph_mode(struct clk *clk, int is_on)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;

	

	pr_debug("Periph clock %s ",clk->name);
#if 1	
	if (is_on)
	{
		pr_debug("turn on.\r\n");
		//if(clk->pmc_mask&1<<AITVSNV3_ID_I2S_FIFO)
		//{
		//	pGBL->GBL_CLK_DIS0 &= (~GBL_CLK_AUD_DIS);
		//}
		if(clk->pmc_mask& (1<<AITVSNV3_ID_SD))
		{
			AITPS_SD pSD = AITC_BASE_SD0;
			pSD->SD_CTL_1 |= CLK_EN;
		}
		else if(clk->pmc_mask&( 1<<AITVSNV3_ID_SD1))
		{
			AITPS_SD pSD = AITC_BASE_SD1;		
			pSD->SD_CTL_1 |= CLK_EN;
		}
		else if(clk->pmc_mask& (1<<AITVSNV3_ID_AFE_FIFO))
		{
			pGBL->GBL_CLK_DIS2 &=~GBL_CLK_AUD_CODEC_DIS;
		}

		else if(clk->pmc_mask&( 1<<AITVSNV3_ID_I2S_FIFO))
		{
			pGBL->GBL_CLK_DIS2 &=~GBL_CLK_AUD_CODEC_DIS;
			pGBL->GBL_CLK_DIS0 &=~GBL_CLK_AUD_DIS;
		}else if(clk->pmc_mask&(1<<AITVSNV3_ID_PSPI))
		{
			pGBL->GBL_CLK_DIS1 &= (~GBL_CLK_PSPI_DIS);
		}


	}
	else
	{

		pr_debug("turn off.\r\n");
		
		if(clk->pmc_mask&(1<<AITVSNV3_ID_I2S_FIFO))
		{
			pGBL->GBL_CLK_DIS0 |= GBL_CLK_AUD_DIS;
		}	

		if(clk->pmc_mask& (1<<AITVSNV3_ID_SD))
		{
			AITPS_SD pSD = AITC_BASE_SD0;
			pSD->SD_CTL_1 &= ~CLK_EN;
		}
		if(clk->pmc_mask& (1<<AITVSNV3_ID_SD1))
		{
			AITPS_SD pSD = AITC_BASE_SD1;	
			pSD->SD_CTL_1 &= ~CLK_EN;
		}

		if(clk->pmc_mask&( 1<<AITVSNV3_ID_AFE_FIFO))
		{
			pGBL->GBL_CLK_DIS2 |=GBL_CLK_AUD_CODEC_DIS;
		}
		if(clk->pmc_mask& (1<<AITVSNV3_ID_I2S_FIFO))
		{
			pGBL->GBL_CLK_DIS2 |=GBL_CLK_AUD_CODEC_DIS;
			pGBL->GBL_CLK_DIS0 |=GBL_CLK_AUD_DIS;
		}		
		
//		at91_sys_write(AT91_PMC_PCDR, clk->pmc_mask);
	}
#endif	
}


static void pmc_audio_mode(struct clk *clk, int is_on)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;

	return;
	if (is_on)
	{
		if(clk->pmc_mask&1<<AITVSNV3_ID_I2S_FIFO)
		{
			pGBL->GBL_CLK_DIS0 &= (~GBL_CLK_AUD_DIS);
		}
	}
	else
	{
		if(clk->pmc_mask&1<<AITVSNV3_ID_I2S_FIFO)
		{
			pGBL->GBL_CLK_DIS0 |= GBL_CLK_AUD_DIS;
		}	

	}
}

static void __clk_enable(struct clk *clk)
{
	pr_debug("%s Enable\r\n",clk->name);	
	if (clk->parent)
		__clk_enable(clk->parent);
	
	if (clk->users++ == 0 && clk->mode)
	{
		clk->mode(clk, 1);
	}
}

int clk_enable(struct clk *clk)
{
	unsigned long	flags;

	spin_lock_irqsave(&clk_lock, flags);
	__clk_enable(clk);
	spin_unlock_irqrestore(&clk_lock, flags);
	return 0;
}
EXPORT_SYMBOL(clk_enable);

static void __clk_disable(struct clk *clk)
{
	BUG_ON(clk->users == 0);
	pr_debug("%s Disable\r\n",clk->name);
	if (--clk->users == 0 && clk->mode)
		clk->mode(clk, 0);
	if (clk->parent)
		__clk_disable(clk->parent);
}

void clk_disable(struct clk *clk)
{
	unsigned long	flags;

	spin_lock_irqsave(&clk_lock, flags);

	__clk_disable(clk);
	spin_unlock_irqrestore(&clk_lock, flags);
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate(struct clk *clk)
{
	unsigned long	flags;
	unsigned long	rate;

	spin_lock_irqsave(&clk_lock, flags);
	for (;;) {
		rate = clk->rate_hz;
		if (rate || !clk->parent)
			break;
		clk = clk->parent;
	}
	spin_unlock_irqrestore(&clk_lock, flags);
	return rate;
}
EXPORT_SYMBOL(clk_get_rate);

/*------------------------------------------------------------------------*/

#ifdef CONFIG_AT91_PROGRAMMABLE_CLOCKS

/*
 * For now, only the programmable clocks support reparenting (MCK could
 * do this too, with care) or rate changing (the PLLs could do this too,
 * ditto MCK but that's more for cpufreq).  Drivers may reparent to get
 * a better rate match; we don't.
 */

long clk_round_rate(struct clk *clk, unsigned long rate)
{
	unsigned long	flags;
	unsigned	prescale;
	unsigned long	actual;
	unsigned long	prev = ULONG_MAX;

	if (!clk_is_programmable(clk))
		return -EINVAL;
	spin_lock_irqsave(&clk_lock, flags);

	actual = clk->parent->rate_hz;
	for (prescale = 0; prescale < 7; prescale++) {
		if (actual > rate)
			prev = actual;

		if (actual && actual <= rate) {
			if ((prev - rate) < (rate - actual)) {
				actual = prev;
				prescale--;
			}
			break;
		}
		actual >>= 1;
	}

	spin_unlock_irqrestore(&clk_lock, flags);
	return (prescale < 7) ? actual : -ENOENT;
}
EXPORT_SYMBOL(clk_round_rate);

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	unsigned long	flags;
	unsigned	prescale;
	unsigned long	actual;

	if (!clk_is_programmable(clk))
		return -EINVAL;
	if (clk->users)
		return -EBUSY;
	spin_lock_irqsave(&clk_lock, flags);

	actual = clk->parent->rate_hz;
	for (prescale = 0; prescale < 7; prescale++) {
		if (actual && actual <= rate) {
			u32	pckr;
			pckr |= prescale << 2;
			clk->rate_hz = actual;
			break;
		}
		actual >>= 1;
	}

	spin_unlock_irqrestore(&clk_lock, flags);
	return (prescale < 7) ? actual : -ENOENT;
}
EXPORT_SYMBOL(clk_set_rate);

struct clk *clk_get_parent(struct clk *clk)
{
	return clk->parent;
}
EXPORT_SYMBOL(clk_get_parent);

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	unsigned long	flags;

	if (clk->users)
		return -EBUSY;
	if (!clk_is_primary(parent) || !clk_is_programmable(clk))
		return -EINVAL;

	if (cpu_is_at91sam9rl() && parent->id == AT91_PMC_CSS_PLLB)
		return -EINVAL;

	spin_lock_irqsave(&clk_lock, flags);

	clk->rate_hz = parent->rate_hz;
	clk->parent = parent;
	spin_unlock_irqrestore(&clk_lock, flags);
	return 0;
}
EXPORT_SYMBOL(clk_set_parent);

/* establish PCK0..PCKN parentage and rate */
static void __init init_programmable_clock(struct clk *clk)
{
	struct clk	*parent;
	u32		pckr;

	clk->parent = parent;
	clk->rate_hz = parent->rate_hz / (1 << ((pckr & AT91_PMC_PRES) >> 2));
}

#endif	/* CONFIG_AT91_PROGRAMMABLE_CLOCKS */

/*------------------------------------------------------------------------*/

#ifdef CONFIG_DEBUG_FS

static int at91_clk_show(struct seq_file *s, void *unused)
{
	u32		scsr, pcsr, uckr = 0, sr;
	struct clk	*clk;

	list_for_each_entry(clk, &clocks, node) {
		char	*state;

		if (clk->mode == pmc_sys_mode)
			state = (scsr & clk->pmc_mask) ? "on" : "off";
		else if (clk->mode == pmc_periph_mode)
			state = (pcsr & clk->pmc_mask) ? "on" : "off";
		else if (clk->mode == pmc_uckr_mode)
			state = (uckr & clk->pmc_mask) ? "on" : "off";
		else if (clk->pmc_mask)
			state = (sr & clk->pmc_mask) ? "on" : "off";
//		else if (clk == &clk32k || clk == &main_clk)
		else if (clk == &main_clk)
			state = "on";
		else
			state = "";

		seq_printf(s, "%-10s users=%2d %-3s %9ld Hz %s\n",
			clk->name, clk->users, state, clk_get_rate(clk),
			clk->parent ? clk->parent->name : "");
	}
	return 0;
}

static int at91_clk_open(struct inode *inode, struct file *file)
{
	return single_open(file, at91_clk_show, NULL);
}

static const struct file_operations at91_clk_operations = {
	.open		= at91_clk_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init at91_clk_debugfs_init(void)
{
	/* /sys/kernel/debug/at91_clk */
	(void) debugfs_create_file("vsnv3_clk", S_IFREG | S_IRUGO, NULL, NULL, &at91_clk_operations);

	return 0;
}
postcore_initcall(at91_clk_debugfs_init);

#endif

/*------------------------------------------------------------------------*/

/* Register a new clock */
static void __init at91_clk_add(struct clk *clk)
{
	list_add_tail(&clk->node, &clocks);

	clk->cl.con_id = clk->name;
	clk->cl.clk = clk;
	clkdev_add(&clk->cl);
}

int __init clk_register(struct clk *clk)
{
	pr_debug("reg clk: %s\r\n",clk->name);
	if (clk_is_peripheral(clk)) {
		if (!clk->parent)
			clk->parent = &g0_global_clk;
		clk->mode = pmc_periph_mode;
	}
	else if (clk_is_sys(clk)) {
		if (!clk->parent)
			clk->parent = &main_clk;
		clk->mode = pmc_sys_mode;
	}

	at91_clk_add(clk);

	return 0;
}

/*------------------------------------------------------------------------*/

static u32 __init at91_pll_rate(struct clk *pll, u32 freq, u32 reg)
{
	unsigned mul, div;

	div = reg & 0xff;
	mul = (reg >> 16) & 0x7ff;
	if (div && mul) {
		freq /= div;
		freq *= mul + 1;
	} else
		freq = 0;

	return freq;
}

static struct clk *const standard_pmc_clocks[] __initdata = {
	/* four primary clocks */
//	&clk32k,
	&main_clk,
//	&plla,

	/* MCK */
//	&mck

};

static struct clk *const vsnv3_sys__clocks[] __initdata = {
&gpio_clk,
&isp_clk,
&vif_clk,
&audio_clk,
&jpeg_clk,
&scaling_clk,
#if 0
&cpu_clk,
#endif
&mci_clk,
&graphic_clk,
&icon_clk,
&h264_clk,
&i2c_clk,
&dma_clk,
&raw_clk,
&bootspi_clk,
&dram_clk,
&cpu_peri_clk ,
&mipi_clk ,
&pspi_clk ,
&sd0_clk ,
&sd1_clk ,
&pwm_clk,
&ibc_clk,
&usb_clk ,
&color_clk,
&sm_clk,
&gnr_clk,
&cidc_clk,
&audio2_clk,

};


/** @brief The function get the Group frequence.

@param[out] ulGroupFreq (GX frequency) (ex: 264MHz -> 264000) 

@return It reports the status of the operation.

*/

unsigned long MMPF_PLL_GetGroup0Freq(void)//unsigned long *ulGroupFreq)

{

	AITPS_GBL pGBL = AITC_BASE_GBL;

	unsigned char m_pll_src_temp;

	unsigned char ubCfgDivid = 0x8;

	unsigned char ubPostDiv = 0x0;

	unsigned long pll_out_clk;



	if(pGBL->GBL_CLK_SEL & GBL_CLK_SEL_MUX0) { //Use 0x6905 bit 6~7 as source selection

		m_pll_src_temp = ((pGBL->GBL_CLK_DIV >> PLL_SEL_PLL_OFFSET) & 0x03);

	}
	else {

		m_pll_src_temp = ((pGBL->GBL_CLK_SEL >> PLL_SEL_PLL_OFFSET1) & 0x03);

	}

 

	switch(m_pll_src_temp)

	{

		case 0:

			pll_out_clk = (clk_get_rate(&main_clk)*pGBL->GBL_DPLL0_N)/(pGBL->GBL_DPLL0_K + 1); 

			break;

		case 1:

			ubCfgDivid = (ubCfgDivid >> (pGBL->GBL_DPLL1_CFG01 & 0x07));

			pll_out_clk = (((clk_get_rate(&main_clk))/pGBL->GBL_DPLL1_M)*(pGBL->GBL_DPLL1_N+2))/ubCfgDivid; 

			break;

		case 2:

			ubCfgDivid = (ubCfgDivid >> (pGBL->GBL_DPLL2_CFG2 & 0x07));

			pll_out_clk = (((clk_get_rate(&main_clk))/pGBL->GBL_DPLL2_M)*(pGBL->GBL_DPLL2_N+2))/ubCfgDivid;

			break;

		default:

			ubCfgDivid = (ubCfgDivid >> (pGBL->GBL_DPLL2_CFG2 & 0x07));

			pll_out_clk = (((clk_get_rate(&main_clk))/pGBL->GBL_DPLL2_M)*(pGBL->GBL_DPLL2_N+2))/ubCfgDivid;

			break;

    	}

 

	ubPostDiv = (pGBL->GBL_CLK_DIV & 0x1f) + 1;

	ubPostDiv = (ubPostDiv << 1);



	return pll_out_clk/ubPostDiv;



	//return MMP_ERR_NONE;

}



int __init vsnv3_clock_init(unsigned long main_clock)
{

	unsigned freq;
	int i;


	/*
	 * When the bootloader initialized the main oscillator correctly,
	 * there's no problem using the cycle counter.  But if it didn't,
	 * or when using oscillator bypass mode, we must be told the speed
	 * of the main clock.
	 */

	main_clk.rate_hz = main_clock;

//	pll2.rate_hz = MMPF_PLL_GetGroup0Freq();//150000000;//264000000;//132000000;
	g0_global_clk.rate_hz = MMPF_PLL_GetGroup0Freq();
	/*
	 * MCK and CPU derive from one of those primary clocks.
	 * For now, assume this parentage won't change.
	 */
	freq = clk_get_rate(&cpu_clk);
	
	/* Register the PMC's standard clocks */
	for (i = 0; i < ARRAY_SIZE(standard_pmc_clocks); i++)
		at91_clk_add(standard_pmc_clocks[i]);

	for (i = 0; i < ARRAY_SIZE(vsnv3_sys__clocks); i++)
		at91_clk_add(vsnv3_sys__clocks[i]);


	clk_register(&sd0_clk);
	clk_register(&sd1_clk);
	clk_register(&cpu_peri_clk);

	/* MCK and CPU clock are "always on" */
	//clk_enable(&mck);

	printk("Clocks: CPU %u MHz, master %u MHz, main %u.%03u MHz\n",
		freq / 1000000, (unsigned) clk_get_rate(& g0_global_clk)/ 1000000,
		(unsigned) main_clock / 1000000,
		((unsigned) main_clock % 1000000) / 1000);

	return 0;
}

/*
 * Several unused clocks may be active.  Turn them off.
 */
static int __init at91_clock_reset(void)
{
	unsigned long pcdr = 0;
	unsigned long scdr = 0;
	struct clk *clk;

	list_for_each_entry(clk, &clocks, node) {
		if (clk->users > 0)
			continue;

		if (clk->mode == pmc_periph_mode)
			pcdr |= clk->pmc_mask;

		if (clk->mode == pmc_sys_mode)
			scdr |= clk->pmc_mask;

		pr_debug("Clocks: disable unused %s\n", clk->name);
	}

	return 0;
}
late_initcall(at91_clock_reset);
