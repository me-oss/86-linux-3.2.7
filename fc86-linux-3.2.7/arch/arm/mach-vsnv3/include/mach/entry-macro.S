/*
 * arch/arm/mach-at91/include/mach/entry-macro.S
 *
 *  Copyright (C) 2003-2005 SAN People
 *
 * Low-level IRQ helper macros for AT91RM9200 platforms
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <mach/hardware.h>
#include <mach/at91_aic.h>

	.macro	disable_fiq
	.endm

	.macro  get_irqnr_preamble, base, tmp
	ldr	\base, =(AT91_VA_BASE_SYS + AT91_AIC)		@ base virtual address of AIC peripheral
	.endm

	.macro  arch_ret_to_user, tmp1, tmp2
	.endm

	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
#if 1//Julian's modify
	ldr     \irqstat, [\base, #(AT91_AIC_CISR - AT91_AIC)]  @ read interrupt source number
	teq     \irqstat, #0                                    @ ISR is 0 when no current interrupt
	ldrne   \irqnr, [\base, #(AT91_AIC_IVR - AT91_AIC)]     @ read IRQ vector register: de-asserts nIRQ to processor (and clears interrupt)
#else	//Vin:Keep the following till above is no problem.
	ldr	\irqnr, [\base, #(AT91_AIC_IVR - AT91_AIC)]	@ read IRQ vector register: de-asserts nIRQ to processor (and clears interrupt)
	@ldr	\irqstat, [\base, #(AT91_AIC_ISR - AT91_AIC)]	@ read interrupt source number
	teq	\irqnr, #0					@ ISR is 0 when no current interrupt, or spurious interrupt
	streq	\tmp, [\base, #(AT91_AIC_EOICR - AT91_AIC)]	@ not going to be handled further, then ACK it now.
#endif
	.endm

