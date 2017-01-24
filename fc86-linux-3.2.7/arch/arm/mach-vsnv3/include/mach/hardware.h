/*
 * arch/arm/mach-at91/include/mach/hardware.h
 *
 *  Copyright (C) 2003 SAN People
 *  Copyright (C) 2003 ATMEL
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <asm/sizes.h>

#define CHIP_INVAL  (0)
#define P_V2        (1)
#define VSN_V2      (2)
#define VSN_V3      (3)
#define MERCURY     (4)
#define VSN_V5      (5)
#define MCR_V2      (6)

#if defined(CONFIG_ARCH_VSNV3AIT845X)
#define CHIP        (VSN_V3)
#include <mach/vsnv3def.h>
#else
#error "Unsupported AT91 processor"
#define CHIP        (CHIP_INVAL)
#endif

/*
 * On all at91 except rm9200 and x40 have the System Controller starts
 * at address 0xffffc000 and has a size of 16KiB.
 *
 * On rm9200 it's start at 0xfffe4000 of 111KiB with non reserved data starting
 * at 0xfffff000
 *
 * Removes the individual definitions of AT91_BASE_SYS and
 * replaces them with a common version at base 0xfffffc000 and size 16KiB
 * and map the same memory space
 */
//#define AT91_BASE_SYS	0xffffc000
#define AT91_BASE_SYS	0xfffE0000


/*
 * Peripheral identifiers/interrupts.
 */
#define AT91_ID_FIQ		0	/* Advanced Interrupt Controller (FIQ) */
#define AT91_ID_SYS		1	/* System Peripherals */

/*
 * Remap the peripherals from address 0xFFF78000 .. 0xFFFFFFFF
 * to 0xFEF78000 .. 0xFF000000.  (544Kb)
 */

#define AT91_IO_PHYS_BASE	0xFFFE0000
#define AT91_IO_VIRT_BASE	(0xFF000000 - AT91_IO_SIZE)
#define AIT_OPR_VIRT_BASE 0xF0000000	//(AT91_IO_VIRT_BASE-AIT_OPR_VIRT_SIZE)
#define AIT_OPR_VIRT_SIZE AIT_OPR_PHYS_SIZE


#define AT91_IO_SIZE		(0xFFFFFFFF - AT91_IO_PHYS_BASE + 1)

 /* Convert a physical IO address to virtual IO address */
#define AT91_IO_P2V(x)		((x) - AT91_IO_PHYS_BASE + AT91_IO_VIRT_BASE)

/*
 * Virtual to Physical Address mapping for IO devices.
 */
#define AT91_VA_BASE_SYS	AT91_IO_P2V(AT91_BASE_SYS)
#define AT91_VA_BASE_EMAC	AT91_IO_P2V(AT91RM9200_BASE_EMAC)

#define AIT_DRAM_PHYS_BASE  (0x4400000)
#define AIT_DRAM_SIZE       (0xC00000)

#define AIT_RAM_P2V(x)      ((x) | 0xF0000000)
#define AIT_RAM_V2P(x)      (((x) < (0xF0000000+AIT_OPR_PHYS_SIZE))? \
                                ((x)&~0x70000000): ((x)&~0xF0000000))

#define AIT_OPR_PHYS_BASE   (0x80000000)
#define AIT_OPR_PHYS_SIZE   (SZ_64K)
#define AIT_SRAM_PHYS_BASE  (0x100000)
#define AIT_SRAM_SIZE       (0x26000)

#define AIT_SRAM_VIRT_BASE  (AIT_RAM_P2V(AIT_SRAM_PHYS_BASE))
#define AIT_DRAM_VIRT_BASE  (AIT_RAM_P2V(AIT_DRAM_PHYS_BASE))

 /* Internal SRAM is mapped below the IO devices */
#define AT91_SRAM_MAX		SZ_128K//SZ_1M
#define AT91_VIRT_BASE		(AIT_OPR_VIRT_BASE - AT91_SRAM_MAX)

#define AIT_OPR_P2V(x)      ((x- AIT_OPR_PHYS_BASE+AIT_OPR_VIRT_BASE))
#define AIT_OPR_V2P(x)      ((x- AIT_OPR_VIRT_BASE+AIT_OPR_PHYS_BASE))

//#define AT8453_OPR_P2V(x)		((x)-AIT8453_OPR_PHYS_BASE+AIT8453_OPR_VIRT_BASE )
/* Serial ports */
#define AIT_MAX_UART		2		/* 6 USART3's and one DBGU port (SAM9260) */

/* Clocks */
#define AT91_SLOW_CLOCK		32768		/* slow clock */


#endif
