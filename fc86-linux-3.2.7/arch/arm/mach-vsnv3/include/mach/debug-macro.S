/*
 * arch/arm/mach-vsnv3/include/mach/debug-macro.S
 *
 *  Copyright (C) 2003-2005 SAN People
 *
 * Debugging macro include header
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <mach/hardware.h>

	.macro	addruart, rp, rv, tmp
	ldr	\rp, =(0x80006A00)	@ System peripherals (phys address)
	ldr	\rv, =AIT_OPR_P2V(0x80006A00)	@- AIT_OPR_PHYS_BASE+AIT_OPR_VIRT_BASE ))	@Vin:Todo
	#(AIT8455_OPR_VIRT_BASE- AIT8455_OPR_PHYS_BASE+0x80006A00)	@ System peripherals (virt address)
	.endm

	.macro	senduart,rd,rx
	strb	\rd, [\rx, #0x08]		@ Write to Transmitter Holding Register
	.endm

	.macro	waituart,rd,rx
1001:	ldr	\rd, [\rx, #0x24]		@ Read Status Register
	tst	\rd, #0x0000F000
	beq	1001b
	.endm

	.macro	busyuart,rd,rx
@1001:	ldr	\rd, [\rx, #(0x24)]		@ Read Status Register
@	tst	\rd, #0xff00					@ DBGU_TXEMPTY = 1 when transmission complete
@	beq	1001b	
	.endm
