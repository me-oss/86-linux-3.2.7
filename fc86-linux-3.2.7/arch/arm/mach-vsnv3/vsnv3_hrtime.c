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
#include <linux/err.h>
#include <linux/clockchips.h>

#include <asm/mach/time.h>

#include <mach/at91_pit.h>
#include <mach/reg_retina.h>
#include <mach/mmp_register.h>


static long __init vsnv3_timer_get_clock_rate(const char *name)
{
        struct clk *clk;
        long rate;
        int err;
pr_err("vsnv3_timer_get_clock_rate\n");
        clk = clk_get_sys(NULL, name);
        if (IS_ERR(clk)) {
                pr_err("vsnv3_timer_ctl_clk: %s clock not found: %d\n", name,
                        (int)PTR_ERR(clk));
                return PTR_ERR(clk);
        }

        err = clk_prepare(clk);
        if (err) {
                pr_err("vsnv3_timer_ctl_clk: %s clock failed to prepare: %d\n", name, err);
                clk_put(clk);
                return err;
        }

        err = clk_enable(clk);
        if (err) {
                pr_err("vsnv3_timer_ctl_clk: %s clock failed to enable: %d\n", name, err);
                clk_unprepare(clk);
                clk_put(clk);
                return err;
        }

        rate = clk_get_rate(clk);
        if (rate < 0) {
                pr_err("vsnv3_timer_ctl_clk: %s clock failed to get rate: %ld\n", name, rate);
                clk_disable(clk);
                clk_unprepare(clk);
                clk_put(clk);
        }

        return rate;
}

static void __iomem *clkevt_base;
static unsigned long clkevt_reload;

void __init vsnv3_clocksource_init(void __iomem *base, const char *name)
{
        long rate = vsnv3_timer_get_clock_rate(name);

        if (rate < 0)
                return;

	AIT_TC_GET_SR(base); 			//Clear Status Register
	AIT_TC_SET_CCR(base,TC_CLKDIS);
	AIT_TC_SET_IDR(base,TC_CPCS);		
	AIT_TC_SET_CCR(base,TC_SWTRG);
	AIT_TC_SET_CMR(base,TC_CPCTRG | TC_CLK_MCK_D128);
	AIT_TC_SET_RC(base,0xffffffff);
	AIT_TC_SET_CCR(base,TC_CLKEN);
	AIT_TC_GET_SR(base);
		
	clocksource_mmio_init(((void __iomem *)(base) + TC_REG_OFFSET(TC_CVR)), "hrtimer-src",
		rate>>8, 200, 32,clocksource_mmio_readl_up);//clocksource_mmio_readl_down);
		
}




/*
 * IRQ handler for the timer
*/
static irqreturn_t vsnv3_timer_interrupt(int irq, void *dev_id)
{
#if 0
	extern void do_leds(void);

        struct clock_event_device *evt = dev_id;

	WARN_ON_ONCE(!irqs_disabled());

	int status = AIT_TC_GET_SR(clkevt_base);
#if 0
	pr_err("AIT_TC_GET_CMR: 0x%x\n\n",AIT_TC_GET_CMR(clkevt_base));
	pr_err("AIT_TC_GET_CVR: 0x%x\n\n",AIT_TC_GET_CVR(clkevt_base));
	pr_err("AIT_TC_GET_RC: 0x%x\n\n",AIT_TC_GET_RC(clkevt_base));
	pr_err("AIT_TC_GET_SR: 0x%x\n\n",AIT_TC_GET_SR(clkevt_base));
	pr_err("AIT_TC_GET_IMR: 0x%x\n\n",AIT_TC_GET_IMR(clkevt_base));	
#endif
pr_err(".\n");
        /* clear the interrupt */
        //writel(1, clkevt_base + TIMER_INTCLR);
        	do_leds();

	if(evt->event_handler)
	{
		pr_err("vsnv3_timer_interrupt: Event handler = %pf", evt->event_handler);

       	evt->event_handler(evt);
	}
        return IRQ_HANDLED;
#endif

	//extern void do_leds(void);
        struct clock_event_device *evt = dev_id;
	/*
	 * irqs should be disabled here, but as the irq is shared they are only
	 * guaranteed to be off if the timer irq is registered first.
	 */

	WARN_ON_ONCE(!irqs_disabled());
#if 0	
	pr_err("vsnv3_timer_interrupt\n");

	pr_err("AIT_TC_GET_CMR: 0x%x\n\n",AIT_TC_GET_CMR(clkevt_base));
	pr_err("AIT_TC_GET_CVR: 0x%x\n\n",AIT_TC_GET_CVR(clkevt_base));
	pr_err("AIT_TC_GET_RC: 0x%x\n\n",AIT_TC_GET_RC(clkevt_base));

	pr_err("AIT_TC_GET_IMR: 0x%x\n\n",AIT_TC_GET_IMR(clkevt_base));	
#endif	
	/* The PIT interrupt may be disabled, and is shared */
	{
		AIT_REG_D status;
		status= AIT_TC_GET_SR(clkevt_base);//pTC->TC_SR ;//at91_sys_read(0xfffe0020-AT91_BASE_SYS) ;

		AIT_TC_SET_SR(clkevt_base,status);

		if(status&0x10)
		{
#ifdef CONFIG_LEDS_TIMER
			do_leds();
#endif
			if(evt->event_handler)
			{
				//pr_err("vsnv3_timer_interrupt: Event handler = %pf", evt->event_handler);

		       	evt->event_handler(evt);
			}
		}

		return IRQ_HANDLED;
	}

	return IRQ_NONE;		
}

static void vsnv3_timer_set_mode(enum clock_event_mode mode,
        struct clock_event_device *evt)
{

	int status;

        switch (mode) {
        case CLOCK_EVT_MODE_PERIODIC:
	//	AIT_TC_GET_SR(clkevt_base);
		AIT_TC_SET_IDR(clkevt_base,TC_CPCS);		
		AIT_TC_SET_SR(clkevt_base,AIT_TC_GET_SR(clkevt_base));		
		AIT_TC_SET_CCR(clkevt_base,TC_CLKDIS);	
		AIT_TC_SET_CCR(clkevt_base,TC_SWTRG);
		AIT_TC_SET_CMR(clkevt_base,TC_CPCTRG | TC_CLK_MCK);
		AIT_TC_SET_RC(clkevt_base,clkevt_reload);
		AIT_TC_SET_CCR(clkevt_base,TC_CLKEN);
		AIT_TC_SET_IER(clkevt_base,TC_CPCS);
		break;

        case CLOCK_EVT_MODE_ONESHOT:
		AIT_TC_SET_IDR(clkevt_base,TC_CPCS);		
		AIT_TC_SET_SR(clkevt_base,AIT_TC_GET_SR(clkevt_base));		
		AIT_TC_SET_CCR(clkevt_base,TC_CLKDIS);	
		AIT_TC_SET_CCR(clkevt_base,TC_SWTRG);
		AIT_TC_SET_CMR(clkevt_base,TC_CPCTRG | TC_CLK_MCK);
//		AIT_TC_SET_RC(clkevt_base,clkevt_reload);
//		AIT_TC_SET_CCR(clkevt_base,TC_CLKEN);

 
                break;

        case CLOCK_EVT_MODE_UNUSED:
        case CLOCK_EVT_MODE_SHUTDOWN:
        default:
		AIT_TC_SET_CCR(clkevt_base,TC_CLKDIS);
		AIT_TC_SET_IDR(clkevt_base,TC_CPCS);		
		AIT_TC_GET_SR(clkevt_base);			
                break;
        }

       // writel(ctrl, clkevt_base + TIMER_CTRL);
}

static int vsnv3_set_next_event(unsigned long next,
        struct clock_event_device *evt)
{

	AIT_TC_SET_RC(clkevt_base,next);
	AIT_TC_SET_CCR(clkevt_base,TC_SWTRG);
	AIT_TC_SET_CCR(clkevt_base,TC_CLKEN);
	AIT_TC_SET_IER(clkevt_base,TC_CPCS);

        return 0;
}
 
static struct clock_event_device vsnv3_clockevent = {
        .shift          = 32,
        .features       = CLOCK_EVT_FEAT_PERIODIC|CLOCK_EVT_FEAT_ONESHOT ,

        .set_mode       = vsnv3_timer_set_mode,
        .set_next_event = vsnv3_set_next_event,
        .rating         = 400,
        .cpumask        = cpu_all_mask,
};
 
static struct irqaction vsnv3_timer_irq = {
        .name           = "hrtimer_irq",
        .flags          = IRQF_SHARED | IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
        .handler        = vsnv3_timer_interrupt,
        .dev_id         = &vsnv3_clockevent,
};
 
void __init vsnv3_clockevents_init(void __iomem *base, unsigned int irq,
        const char *name)
{
        struct clock_event_device *evt = &vsnv3_clockevent;
        long rate = vsnv3_timer_get_clock_rate(name);
pr_err("vsnv3_clockevents_init\n");

        if (rate < 0)
                return;

        clkevt_base = base;
        clkevt_reload = DIV_ROUND_CLOSEST(rate>>1, HZ);

        evt->name = name;
        evt->irq = irq;
        evt->mult = div_sc(rate>>1, NSEC_PER_SEC, evt->shift);
        evt->max_delta_ns = clockevent_delta2ns(0xffffffff, evt);
        evt->min_delta_ns = clockevent_delta2ns(0xf, evt);

        setup_irq(irq, &vsnv3_timer_irq);
        clockevents_register_device(evt);
}


static int __init init_ait_clocksource(void)
{
#ifdef CONFIG_HIGH_RES_TIMERS
	vsnv3_clockevents_init(AT91_IO_P2V(AITC_BASE_PHY_TC2), AITVSNV3_ID_TC2,"tc2_clk");
	vsnv3_clocksource_init(AT91_IO_P2V(AITC_BASE_PHY_TC1), "tc1_clk");
#endif	
}

module_init(init_ait_clocksource);

