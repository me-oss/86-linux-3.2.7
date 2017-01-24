/*
* Customer code to add GPIO control during WLAN start/stop
* Copyright (C) 1999-2011, Broadcom Corporation
* 
*         Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2 (the "GPL"),
* available at http://www.broadcom.com/licenses/GPLv2.php, with the
* following added to such license:
* 
*      As a special exception, the copyright holders of this software give you
* permission to link this software with independent modules, and to copy and
* distribute the resulting executable under terms of your choice, provided that
* you also meet, for each linked independent module, the terms and conditions of
* the license of that module.  An independent module is a module which is not
* derived from this software.  The special exception does not apply to any
* modifications of the software.
* 
*      Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a license
* other than the GPL, without Broadcom's express prior written consent.
*
* $Id: dhd_custom_gpio.c,v 1.2.42.1 2010-10-19 00:41:09 Exp $
*/

#include <osl.h>
#include "include/bcmsdbus.h"

#if 1 //def 1//CUSTOMER_HW

#ifdef CONFIG_MACH_ODROID_4210

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#include <plat/sdhci.h>
#include <plat/devs.h>	// modifed plat-samsung/dev-hsmmcX.c EXPORT_SYMBOL(s3c_device_hsmmcx) added

//#include <asm-generic/gpio.h>	// modifed plat-samsung/dev-hsmmcX.c EXPORT_SYMBOL(s3c_device_hsmmcx) added

#include <linux/gpio.h>

#define	sdmmc_channel	s3c_device_hsmmc0
#endif

#ifdef CUSTOMER_OOB
int bcm_wlan_get_oob_irq(void)
{
	int host_oob_irq = 0;

#ifdef CONFIG_MACH_ODROID_4210
	printk("GPIO(WL_HOST_WAKE) = EXYNOS4_GPX0(7) = %d\n", EXYNOS4_GPX0(7));
	host_oob_irq = gpio_to_irq(EXYNOS4_GPX0(7));
	gpio_direction_input(EXYNOS4_GPX0(7));
	printk("host_oob_irq: %d \r\n", host_oob_irq);
#endif

	return host_oob_irq;
}
#endif
#include <mach/gpio.h>
extern uint gpio_reset_pin ;//= AIT_GPIO_CGPIO6_WIFI_RESET;
//module_param(gpio_reset_pin,  int, 0);


//MODULE_PARM_DESC(reset_pin, "WIFI Reset pin number from AIT84xx");
extern void __gpio_set_value(unsigned gpio, int value);


void bcm_wlan_power_on(int flag)
{

extern int
sdioh_start(sdioh_info_t *si, int stage);

		printk("======== PULL WL_REG_ON HIGH! ========\n");

	__gpio_set_value(gpio_reset_pin, 1);


}

void bcm_wlan_power_off(int flag)
{
		printk("======== PULL WL_REG_ON LOW! ========\n");

	__gpio_set_value(gpio_reset_pin, 0);

}

#endif /* CUSTOMER_HW */
