/*
 * linux/arch/arm/mach-at91/irq.c
 *
 *  Copyright (C) 2004 SAN People
 *  Copyright (C) 2004 ATMEL
 *  Copyright (C) Rick Bronson
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/types.h>

#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/setup.h>

#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>

#include "generic.h"
static void at91_aic_mask_irq(struct irq_data *d)
{
    #if 0///aic_v2
	while (ait_sys_read(AT91_AIC_CISR))
	{
		ait_sys_read(AT91_AIC_IVR);
	}
	#endif

	/* Disable interrupt on AIC */
	ait_sys_write(AT91_AIC_IDCR, 1 << d->irq);

    #if 0///aic_v2
	ait_sys_write(AT91_AIC_ICCR, 1<< d->irq);
	ait_sys_write(AT91_AIC_EOICR, 0);
	#endif
}

static void at91_aic_unmask_irq(struct irq_data *d)
{
	/* Enable interrupt on AIC */
	ait_sys_write(AT91_AIC_IECR, 1 << d->irq);
}

unsigned int at91_extern_irq;

#define is_extern_irq(irq) ((1 << (irq)) & at91_extern_irq)

static int at91_aic_set_type(struct irq_data *d, unsigned type)
{
	unsigned int smr, srctype;

	switch (type) {
		/*Vin add*/
	case IRQ_TYPE_EDGE_BOTH:
	case IRQ_TYPE_LEVEL_HIGH:
		srctype = AT91_AIC_SRCTYPE_HIGH;
		break;
	case IRQ_TYPE_EDGE_RISING:
		srctype = AT91_AIC_SRCTYPE_RISING;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		if ((d->irq == AT91_ID_FIQ) || is_extern_irq(d->irq))		/* only supported on external interrupts */
			srctype = AT91_AIC_SRCTYPE_LOW;
		else
			return -EINVAL;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		if ((d->irq == AT91_ID_FIQ) || is_extern_irq(d->irq))		/* only supported on external interrupts */
			srctype = AT91_AIC_SRCTYPE_FALLING;
		else
			return -EINVAL;
		break;
	default:
		return -EINVAL;
	}

	smr = ait_sys_read(AT91_AIC_SMR(d->irq)) & ~AT91_AIC_SRCTYPE;
	ait_sys_write(AT91_AIC_SMR(d->irq), smr | srctype);
	return 0;
}

#ifdef CONFIG_PM

static u32 wakeups;
static u32 backups;

static int at91_aic_set_wake(struct irq_data *d, unsigned value)
{
printk(KERN_ERR "%s\r\n",__FUNCTION__);
return 0;
	if (unlikely(d->irq >= 32))
		return -EINVAL;

	if (value)
		wakeups |= (1 << d->irq);
	else
		wakeups &= ~(1 << d->irq);

	return 0;
}

void at91_irq_suspend(void)
{
//printk("%s\r\n",__FUNCTION__);
return ;
	backups = ait_sys_read(AT91_AIC_IMR);
	ait_sys_write(AT91_AIC_IDCR, backups);
	ait_sys_write(AT91_AIC_IECR, wakeups);
}

void at91_irq_resume(void)
{
//printk("%s\r\n",__FUNCTION__);
return;
	ait_sys_write(AT91_AIC_IDCR, wakeups);
	ait_sys_write(AT91_AIC_IECR, backups);
}

#else
#define at91_aic_set_wake	NULL
#endif

static struct irq_chip at91_aic_chip = {
	.name		= "AIC",
	#if 0///aic_v2
	.irq_ack	= at91_aic_mask_irq,
	#endif
	.irq_mask	= at91_aic_mask_irq,
	.irq_unmask	= at91_aic_unmask_irq,
	.irq_set_type	= at91_aic_set_type,
	.irq_set_wake	= at91_aic_set_wake,
};

/*
 * Initialize the AIC interrupt controller.
 */
void __init vsnv3_aic_init(unsigned int priority[NR_AIC_IRQS])
{
	unsigned int i;

	/*
	 * The IVR is used by macro get_irqnr_and_base to read and verify.
	 * The irq number is NR_AIC_IRQS when a spurious interrupt has occurred.
	 */
	for (i = 0; i < NR_AIC_IRQS; i++) {
		/* Put irq number in Source Vector Register: */
		ait_sys_write(AT91_AIC_SVR(i), i);
        ait_sys_write(AT91_AIC_SMR(i), AT91_AIC_SRCTYPE_HIGH| priority[i]);

		irq_set_chip_and_handler(i, &at91_aic_chip, handle_level_irq);
		set_irq_flags(i, IRQF_VALID | IRQF_PROBE);

		/* Perform 8 End Of Interrupt Command to make sure AIC will not Lock out nIRQ */
        #if 0///aic_v2
		if (i < 8) {
			ait_sys_write(AT91_AIC_EOICR, 0);
		}
        #endif
	}

	/*
	 * Spurious Interrupt ID in Spurious Vector Register is NR_AIC_IRQS
	 * When there is no current interrupt, the IRQ Vector Register reads the value stored in AIC_SPU
	 */
    #if 0///aic_v2
	ait_sys_write(AT91_AIC_SPU, NR_AIC_IRQS);
	#endif

	/* No debugging in AIC: Debug (Protect) Control Register */
	ait_sys_write(AT91_AIC_DCR, 0);

	/* Disable and clear all interrupts initially */
	ait_sys_write(AT91_AIC_IDCR, 0xFFFFFFFF);
	ait_sys_write(AT91_AIC_ICCR, 0xFFFFFFFF);

    #if 0///aic_v2
	ait_sys_write(AT91_AIC_IECR, 1<<AITVSNV3_ID_UART);
	#endif

    #if 0///aic_v2
	while(ait_sys_read(AT91_AIC_ISR))	//clear unknow ISR Status
	{
		ait_sys_read(AT91_AIC_IVR);
	}
	#endif
}


