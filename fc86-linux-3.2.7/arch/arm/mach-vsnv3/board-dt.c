/*
 *  Setup code for AT91SAM Evaluation Kits with Device Tree support
 *
 *  Covers: * AT91SAM9G45-EKES  board
 *          * AT91SAM9M10-EKES  board
 *          * AT91SAM9M10G45-EK board
 *
 *  Copyright (C) 2011 Atmel,
 *                2011 Nicolas Ferre <nicolas.ferre@atmel.com>
 *
 * Licensed under GPLv2 or later.
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/irqdomain.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>

#include <mach/hardware.h>
#include <mach/board.h>
#include <mach/system_rev.h>


#include <asm/setup.h>
#include <asm/irq.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>


#include "generic.h"


static void __init ek_init_early(void)
{
	/* Initialize processor: 12.000 MHz crystal */
	vsnv3_initialize(12000000);

	/* DGBU on ttyS0. (Rx & Tx only) */
	vsnv3_register_uart(0, 0, 0);

	/* set serial console to ttyS0 (ie, DBGU) */
	vsnv3_set_serial_console(0);
}

static const struct of_device_id aic_of_match[] __initconst = {
	{ .compatible = "atmel,at91rm9200-aic", },
	{},
};

static void __init at91_dt_init_irq(void)
{
	irq_domain_generate_simple(aic_of_match, 0xfffff000, 0);
	vsnv3_init_irq_default();
}

static void __init at91_dt_device_init(void)
{
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);

	/* NAND */
	//ek_add_device_nand();
}

static const char *at91_dt_board_compat[] __initdata = {
	"atmel,at91sam9m10g45ek",
	"calao,usb-a9g20",
	NULL
};

DT_MACHINE_START(at91sam_dt, "Atmel AT91SAM (Device Tree)")
	/* Maintainer: Atmel */
	.timer		= &at91sam926x_timer,
	.map_io		= vsnv3_map_io,
	.init_early	= ek_init_early,
	.init_irq	= at91_dt_init_irq,
	.init_machine	= at91_dt_device_init,
	.dt_compat	= at91_dt_board_compat,
MACHINE_END
