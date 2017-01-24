/*
 * arch/arm/mach-at91/include/mach/at91sam9260.h
 *
 * (C) 2006 Andrew Victor
 *
 * Common definitions.
 * Based on AT91SAM9260 datasheet revision A (Preliminary).
 *
 * Includes also definitions for AT91SAM9XE and AT91SAM9G families
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef AT91SAM9260_H
#define AT91SAM9260_H

/*
 * Peripheral identifiers/interrupts.
 */
#define AT91SAM9260_ID_VIF	0	/* Image Sensor Interface */
#define AT91SAM9260_ID_ADC	5	/* Analog-to-Digital Converter */
#define AITVSNV3_ID_AFE   	6
#define AITVSNV3_ID_UART   	8		//#define AT91SAM9260_ID_US0	8	/* USART 0 */
#define AITVSNV3_ID_TC0		9	/* Timer Counter 0 */
#define AITVSNV3_ID_TC1		10	/* Timer Counter 1 */

#define AITVSNV3_ID_TC2		11	/* Timer Counter 2 */
#define AITVSNV3_ID_USB		12	/* USB */
#define AITVSNV3_ID_WDT		15	/*  Watch Dog*/
#define AITVSNV3_ID_GPIO		16	/* GPIO */
#define AITVSNV3_ID_SD   		18	/* Multimedia Card Interface */
#define AITVSNV3_ID_AFE_FIFO  19
//#define AT91SAM9260_ID_UHP	20	/* USB Host port */

//#define AT91SAM9260_ID_EMAC	21	/* Ethernet */
//#define AT91SAM9260_ID_SSC		22	/* Serial Synchronous Controller */
#define AITVSNV3_ID_I2S_FIFO   	22	//22	//
#define AITVSNV3_ID_I2S   		23
#define AITVSNV3_ID_I2CM 			24		//#define AT91SAM9260_ID_TWI	22	/* Two-Wire Interface */
#define AITVSNV3_ID_SIF			25	/* Serial flash interface */
#define AITVSNV3_ID_PSPI			26	/* Programming SPI Interface */


/* The following peripheral id can't use in AIC*/
#define AITVSNV3_ID_SD1   		31	/* Multimedia Card 1 Interface */





/*
 * User Peripheral physical base addresses.
 */

#define AT91_SDRAMC0	(0xffffea00 - AT91_BASE_SYS)
#define AT91_RSTC	(0xfffffd00 - AT91_BASE_SYS)

#define AT91_DBGU	(0xfffff200 - AT91_BASE_SYS)

#define AT91SAM9260_BASE_SSC		0xffff1000

#define AT91_AIC	(0xfffff000 - AT91_BASE_SYS)
#if 0
#define AT91_USART1	AT91SAM9260_BASE_US1
#define AT91_USART2	AT91SAM9260_BASE_US2
#define AT91_USART3	AT91SAM9260_BASE_US3
#define AT91_USART4	AT91SAM9260_BASE_US4
#define AT91_USART5	AT91SAM9260_BASE_US5
#endif

/*
 * Internal Memory.
 */

/* For AIT8453 */
#define AT91SAM9260_SRAM0_BASE	0x00100000	/* Internal SRAM 0 base address */
#define AT91SAM9260_SRAM0_SIZE	SZ_32K		/* Internal SRAM 0 size (256Kb) */
#define AT91SAM9260_SRAM1_BASE	0x00108000	/* Internal SRAM 1 base address */
#define AT91SAM9260_SRAM1_SIZE	0x126000 - 0x108000	//SZ_32K		/* Internal SRAM 1 size (256Kb) */
										

#endif
