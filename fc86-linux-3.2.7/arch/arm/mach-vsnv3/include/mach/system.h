/*
 * arch/arm/mach-at91/include/mach/system.h
 *
 *  Copyright (C) 2003 SAN People
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <mach/hardware.h>

static inline void arch_idle(void)
{
	/*
	 * Disable the processor clock.  The processor will be automatically
	 * re-enabled by an interrupt or by a reset.
	 */

#ifndef CONFIG_CPU_ARM920T
	/*
	 * Set the processor (CP15) into 'Wait for Interrupt' mode.
	 * Post-RM9200 processors need this in conjunction with the above
	 * to save power when idle.
	 */
//printk("%s,%d\r\n",__FUNCTION__,__LINE__);	
	 
	cpu_do_idle();
#endif
}

void (*vsnv3_arch_reset)(char mode);
static inline void arch_reset(char mode, const char *cmd)
{
pr_info("%s cmd = %s\n",__func__,cmd);
	/* call the CPU-specific reset function */
	if (vsnv3_arch_reset)
		(vsnv3_arch_reset)(mode);
}

#endif
