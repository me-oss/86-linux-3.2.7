/*
 * arch/arm/mach-vsnv3/include/mach/gpio.h
 *
 *  Copyright (C) 2005 HP Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef __ASM_ARCH_VSNV3_GPIO_H
#define __ASM_ARCH_VSNV3_GPIO_H

#include <linux/kernel.h>
#include <asm/irq.h>


#define PIN_BASE		NR_AIC_IRQS

#define MAX_GPIO_BANKS		4
#define NR_BUILTIN_GPIO		(PIN_BASE + (MAX_GPIO_BANKS * 32))

#define AIT_GPIO_AGPIO02  MMPF_PIO_REG_GPIO2
#define AIT_GPIO_AGPIO03  3
#define AIT_GPIO_AGPIO04  4
#define AIT_GPIO_AGPIO05  5
#define AIT_GPIO_CGPIO6  48
//#define AIT_GPIO_CGPIO8  50
#define AIT_GPIO_CGPIO16 MMPF_PIO_REG_GPIO58
#define AIT_GPIO_CGPIO17  MMPF_PIO_REG_GPIO59
#define AIT_GPIO_CGPIO08 MMPF_PIO_REG_GPIO50
#define AIT_GPIO_CGPIO09 MMPF_PIO_REG_GPIO51
/* GPIO Button*/
#define AIT_GPIO_BGPIO013  MMPF_PIO_REG_GPIO21
#define AIT_GPIO_CGPIO09  MMPF_PIO_REG_GPIO51

/* GPIO LED */
//#define AIT_GPIO_AGPIO6  6
//#define AIT_GPIO_CGPIO17  59


/* GPIO */
#define AIT_GPIO_CGPIO6_WIFI_RESET  48

#define AIT_GPIO_SD_DET_IRQ AIT_GPIO_CGPIO16
#define AIT_GPIO_ETHERNET_IRQ AIT_GPIO_AGPIO02
	

#ifndef __ASSEMBLY__
/* setup setup routines, called from board init or driver probe() */
extern int __init_or_module at91_set_GPIO_periph(unsigned pin, int use_pullup);
extern int __init_or_module at91_set_A_periph(unsigned pin, int use_pullup);
extern int __init_or_module at91_set_B_periph(unsigned pin, int use_pullup);
extern int __init_or_module at91_set_gpio_input(unsigned pin, int use_pullup);
extern int __init_or_module vsnv3_set_gpio_output(unsigned pin, int value);
extern int __init_or_module at91_set_deglitch(unsigned pin, int is_on);
extern int __init_or_module at91_set_multi_drive(unsigned pin, int is_on);

/* callable at any time */
extern int vsnv3_set_gpio_value(unsigned pin, int value);
extern int vsnv3_get_gpio_value(unsigned pin);

/* callable only from core power-management code */
extern void vsnv3_gpio_suspend(void);
extern void at91_gpio_resume(void);

/*-------------------------------------------------------------------------*/

/* wrappers for "new style" GPIO calls. the old AT91-specific ones should
 * eventually be removed (along with this errno.h inclusion), and the
 * gpio request/free calls should probably be implemented.
 */

#include <asm/errno.h>

#define ARCH_NR_GPIOS (66)
#define gpio_to_irq(gpio) (NR_AIC_IRQS+gpio)
#define irq_to_gpio(irq)  (irq>32?(irq-NR_AIC_IRQS):-1)

#endif	/* __ASSEMBLY__ */

#endif
