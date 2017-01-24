/*
 * at91sam926x_time.c - Periodic Interval Timer (PIT) for at91sam926x
 *
 * Copyright (C) 2005-2006 M. Amine SAYA, ATMEL Rousset, France
 * Revision	 2005 M. Nicolas Diremdjian, ATMEL Rousset, France
 * Converted to ClockSource/ClockEvents by David Brownell.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/clockchips.h>

#include <asm/mach/time.h>

#include <mach/reg_retina.h>

#define PIT_CPIV(x)	((x) & AT91_PIT_CPIV)
#define PIT_PICNT(x)	(((x) & AT91_PIT_PICNT) >> 20)

static u32 pit_cycle;		/* write-once */
static u32 pit_cnt;		/* access only w/system irq blocked */


/*
 * Clocksource:  just a monotonic counter of MCK/16 cycles.
 * We don't care whether or not PIT irqs are enabled.
 */
static cycle_t read_pit_clk(struct clocksource *cs)
{
	unsigned long flags;
	u32 elapsed;

	raw_local_irq_save(flags);
	elapsed = pit_cnt;
	raw_local_irq_restore(flags);

	elapsed+=AIT_TC_GET_CVR(AITC_BASE_TC0);
//pr_err("read_pit_clk = %d\n",elapsed);
	return elapsed;
}

static struct clocksource pit_clk = {
	.name		= "pit",
	.rating		= 175,
	.read		= read_pit_clk,
	.flags		= CLOCK_SOURCE_IS_CONTINUOUS,
};


/*
 * Clockevent device:  interrupts every 1/HZ (== pit_cycles * MCK/16)
 */
static void
pit_clkevt_mode(enum clock_event_mode mode, struct clock_event_device *dev)
{

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		/* update clocksource counter */

		pit_cnt += AIT_TC_GET_CVR(AITC_BASE_TC0);//MMPF_Timer_ReadCount(MMPF_TIMER_0);//at91_sys_read(0xfffe0010-AT91_BASE_SYS) ;//pit_cycle>>1;// * PIT_PICNT(at91_sys_read(AT91_PIT_PIVR));


		AIT_TC_SET_CCR(AITC_BASE_TC0,TC_CLKDIS);
		AIT_TC_SET_IDR(AITC_BASE_TC0,TC_CPCS);		
		AIT_TC_SET_CCR(AITC_BASE_TC0,TC_SWTRG);
		AIT_TC_SET_CMR(AITC_BASE_TC0,TC_CPCTRG | TC_CLK_MCK_D1024);

		AIT_TC_SET_RC(AITC_BASE_TC0,pit_cycle);

		AIT_TC_SET_CCR(AITC_BASE_TC0,TC_CLKEN);
		
		AIT_TC_GET_SR(AITC_BASE_TC0);
		AIT_TC_SET_IER(AITC_BASE_TC0,TC_CPCS);

		

		/* Get number of ticks performed before irq, and ack it */
//		nr_ticks = PIT_PICNT(at91_sys_read(AT91_PIT_PIVR));
//		nr_ticks  = MMPF_Timer_ReadCount(0/*MMPF_TIMER_ID*/);

		break;
	case CLOCK_EVT_MODE_ONESHOT:
		BUG();
		/* FALLTHROUGH */
	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_UNUSED:
		/* disable irq, leaving the clocksource active */
		//at91_sys_write(AT91_PIT_MR, (pit_cycle - 1) | AT91_PIT_PITEN);
//		MMPF_Timer_EnableInterrupt(0, 0);
//		MMPF_Timer_Close(0);
		AIT_TC_SET_CCR(AITC_BASE_TC0,TC_CLKDIS);

		AIT_TC_SET_IDR(AITC_BASE_TC0,TC_CPCS);		
		AIT_TC_GET_SR(AITC_BASE_TC0);

		break;
	case CLOCK_EVT_MODE_RESUME:
		break;
	}
}

static struct clock_event_device pit_clkevt = {
	.name		= "pit",
	.features	= CLOCK_EVT_FEAT_PERIODIC,
	.shift		= 32,
	.rating		= 100,
	.set_mode	= pit_clkevt_mode,
};


/*
 * IRQ handler for the timer.
 */
static irqreturn_t at91sam926x_pit_interrupt(int irq, void *dev_id)
{
//	extern void do_leds(void);

	/*
	 * irqs should be disabled here, but as the irq is shared they are only
	 * guaranteed to be off if the timer irq is registered first.
	 */

	AITPS_TC pTC = AITC_BASE_TC0;
	WARN_ON_ONCE(!irqs_disabled());
	/* The PIT interrupt may be disabled, and is shared */
	if ((pit_clkevt.mode == CLOCK_EVT_MODE_PERIODIC)){
		AIT_REG_D status;
		status= pTC->TC_SR ;//at91_sys_read(0xfffe0020-AT91_BASE_SYS) ;
		pTC->TC_SR = status;

		if(status&0x10)
		{
#ifdef CONFIG_LEDS_TIMER
//			do_leds();
#endif
			pit_cnt +=(pit_cycle+AIT_TC_GET_CVR(AITC_BASE_TC0));//pit_cycle/10;	//pit_cnt shold get from register 0xfffe0010 which is really counter			

			if(likely(pit_clkevt.event_handler))
				pit_clkevt.event_handler(&pit_clkevt);
			else
				printk(KERN_ERR "%pf", pit_clkevt.event_handler);
		}

		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static struct irqaction at91sam926x_pit_irq = {
	.name		= "at91_tick",
	.flags		= IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= at91sam926x_pit_interrupt
};

static void vsnv3_timer_reset(void)
{
	AIT_TC_SET_IDR(AITC_BASE_TC0,TC_CPCS);		
	AIT_TC_GET_SR(AITC_BASE_TC0);
}

/*
 * Set up both clocksource and clockevent support.
 */
static void __init vsnv3_ait8455_pit_init(void)
{
	unsigned long	pit_rate;

	/*
	 * Use our actual MCK to figure out how many MCK/16 ticks per
	 * 1/HZ period (instead of a compile-time constant LATCH).
	 */
 	pit_rate = clk_get_rate(clk_get(NULL, "cpu_peri_clk"));
 	pit_rate >>=11; 	// div 2*1024
	pit_cycle = (pit_rate + HZ/2) / HZ;
	/* Initialize and enable the timer */
	vsnv3_timer_reset();

	/*
	 * Register clocksource.  The high order bits of PIV are unused,
	 * so this isn't a 32-bit counter unless we get clockevent irqs.
	 */

	pit_clk.mask = 0xffffffff;
	clocksource_register_hz(&pit_clk, pit_rate);

	/* Set up irq handler */
	setup_irq(AITVSNV3_ID_TC0, &at91sam926x_pit_irq);
	

	/* Set up and register clockevents */
	pit_clkevt.mult = div_sc(pit_rate, NSEC_PER_SEC, pit_clkevt.shift);
	pit_clkevt.cpumask = cpumask_of(0);
	clockevents_register_device(&pit_clkevt);

//	printk(KERN_ERR "vsnv3_ait8455_pit_init:%pf", pit_clkevt.event_handler);



  
}

static void at91sam926x_pit_suspend(void)
{
	/* Disable timer */
//	at91_sys_write(AT91_PIT_MR, 0);
}

struct sys_timer at91sam926x_timer = {
	.init		= vsnv3_ait8455_pit_init,
	.suspend	= at91sam926x_pit_suspend,
	.resume		= vsnv3_timer_reset,
};

