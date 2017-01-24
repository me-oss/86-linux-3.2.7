/* /drivers/watchdog/vsnv3_wdt.c
 *
 * Copyright (c) 2013 Alpha Image Tech.
 *	Vincent Chen<vincent1103@a-i-t.com.tw>
 *
 *  AIT VSNV3 Watchdog Timer Support
 *
 * Base on linux/drivers/char/watchdog/s3c2410_wdt.c by Ben Dooks
 * Copyright (c) 2004 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
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

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h> 
#include <linux/watchdog.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cpufreq.h>
#include <linux/slab.h>
#include <linux/err.h>

#include <mach/reg_retina.h>
#include "mach/mmp_reg_gbl.h"
#include "vsnv3_wdt.h"
#include <mach/mmpf_wd.h>
#include "mach/mmpf_system.h"
#include <mach/mmp_reg_sf.h>
//#include <mach/mmpf_wd.h>

#define CONFIG_VSNV3_WATCHDOG_DEFAULT_TIME	(4)
/* Timer heartbeat (500ms) */
#define WDT_TIMEOUT	(1*HZ)




#define WRITE_IN_PROGRESS       0x01
#define WRITE_ENABLE_LATCH      0x02


#define WRITE_STATUS            0x01
#define PAGE_PROGRAM            0x02
#define READ_DATA               0x03
#define WRITE_DISABLE           0x04
#define EXIT_OTP_MODE           0x04
#define READ_STATUS             0x05
#define WRITE_ENABLE            0x06
#define FAST_READ_DATA          0x0B
#define SECTOR_ERASE            0x20
#define ENTER_OTP_MODE          0x3A
#define READ_UNIQUE_ID		0x4B
#define BLOCK_ERASE             0x52
#define CHIP_ERASE              0x60
#define EBSY                    0x70
#define DBSY                    0x80
#define READ_DEVICE_ID          0x9F
#define RELEASE_DEEP_POWER_DOWN 0xAB
#define ADDR_AUTO_INC_WRITE     0xAD
#define DEEP_POWER_DOWN         0xB9
#define OPCODE_RSTEN		0x66
#define OPCODE_RST		0x99
#define   WRITE_EAR      0xC5

static int nowayout	= WATCHDOG_NOWAYOUT;
static int tmr_margin	= CONFIG_VSNV3_WATCHDOG_DEFAULT_TIME;
static int wdt_on_atboot	= 1;//CONFIG_S3C2410_WATCHDOG_ATBOOT;
static int wdt_reset;
static int debug;

module_param(tmr_margin,  int, 0);
module_param(wdt_on_atboot,  int, 0);
module_param(nowayout,    int, 0);
module_param(wdt_reset, int, 0);
module_param(debug,	  int, 0);

MODULE_PARM_DESC(tmr_margin, "Watchdog tmr_margin in seconds. (default="
		__MODULE_STRING(CONFIG_VSNV3_WATCHDOG_DEFAULT_TIME) ")");

MODULE_PARM_DESC(wdt_on_atboot,"Watchdog is started at boot time if set to 1, default="
		__MODULE_STRING(CONFIG_S3C2410_WATCHDOG_ATBOOT));

MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default="
			__MODULE_STRING(WATCHDOG_NOWAYOUT) ")");
MODULE_PARM_DESC(wdt_reset, "Watchdog action, set to 1 to ignore reboots, "
			"0 to reboot (default 0)");
MODULE_PARM_DESC(debug, "Watchdog debug, set to >1 for debug (default 0)");

static struct device    *wdt_dev;	/* platform device attached to */
static struct resource	*wdt_mem;
static struct resource	*wdt_irq;
static struct clk	*vsnv3_wdt_clock;
static void __iomem	*wdt_base;
//static unsigned int	 wdt_count;
static DEFINE_SPINLOCK(vsnv3_wdt_lock);

/* watchdog control routines */

#define WDT_DBG(msg...) do { \
	if (debug) \
		printk(KERN_ALERT msg); \
	} while (0)

//static struct {
static unsigned long next_heartbeat;	/* the next_heartbeat for the timer */
//static unsigned long open;
//static char expect_close;
static struct timer_list timer;	/* The timer that pings the watchdog */
//static struct timer_list timer_start_wdt;	/* The timer that pings the watchdog */
//} vsnv3_wdt_private;


static MMP_ERR MMPF_SF_CheckReady(void)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
    
    while((pSIF->SIF_INT_CPU_SR & SIF_CMD_DONE) == 0) ;
	
    return  MMP_ERR_NONE;
}

MMP_ERR MMPF_SF_DisableWrite(void)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
	
    pSIF->SIF_INT_CPU_SR = SIF_CLR_CMD_STATUS;
    
    pSIF->SIF_CMD = WRITE_DISABLE;

    pSIF->SIF_CTL = SIF_START;
    
    MMPF_SF_CheckReady();

    return  MMP_ERR_NONE;
}

static MMP_ERR MMPF_SF_EnableWrite(void)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
	
    pSIF->SIF_INT_CPU_SR = SIF_CLR_CMD_STATUS;
   
    pSIF->SIF_CMD = WRITE_ENABLE;
   
    pSIF->SIF_CTL = SIF_START;
    
    MMPF_SF_CheckReady();
    
    return  MMP_ERR_NONE;
}

static MMP_ERR MMPF_SF_ReadStatus(MMP_UBYTE *ubStatus)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
	
	pSIF->SIF_INT_CPU_SR = SIF_CLR_CMD_STATUS;
   
   	pSIF->SIF_CMD = READ_STATUS;
    
    pSIF->SIF_CTL = SIF_START | SIF_R | SIF_DATA_EN;

    MMPF_SF_CheckReady();
    
    *ubStatus = pSIF->SIF_DATA_RD;
    
    return  MMP_ERR_NONE;
}


static MMP_ERR MMPF_SF_WriteEAR(MMP_UBYTE ubData)
{
	AITPS_SIF pSIF = AITC_BASE_SIF;
	MMP_UBYTE ubStatus;
	
	MMPF_SF_EnableWrite();
	
	MMPF_SF_CheckReady();

	pSIF->SIF_INT_CPU_SR = SIF_CLR_CMD_STATUS;
    
    pSIF->SIF_CMD = WRITE_EAR;
    
    pSIF->SIF_DATA_WR = ubData;
    
    pSIF->SIF_CTL = SIF_START | SIF_W | SIF_DATA_EN;
    
    MMPF_SF_CheckReady();
   
    do {
        MMPF_SF_ReadStatus(&ubStatus);
    } while ((ubStatus & WRITE_IN_PROGRESS) || (ubStatus & WRITE_ENABLE_LATCH));
   
    MMPF_SF_DisableWrite();
    return  MMP_ERR_NONE;
}


static int vsnv3_wdt_start(struct watchdog_device *wdd)
{
//	AITPS_WD 	pWD = AITC_BASE_WD;

	AITPS_GBL pGBL = AITC_BASE_GBL;
	pGBL->GBL_CHIP_CTL |= GBL_WD_RST_ALL_MODULE;

	WDT_DBG("%s: %s\n wdt_reset = %d\n", __func__, wdd->info->identity,wdt_reset);

	spin_lock(&vsnv3_wdt_lock);

	ait_wd_writel(wdt_base,WD_MODE_CTL0,WD_CTL_ACCESS_KEY);

	ait_wd_writel(wdt_base,WD_RE_ST,WD_RESTART);	//Before enable, we shoudl set re-start first.

	if (wdt_reset) {
		ait_wd_writel(wdt_base,WD_MODE_CTL0,WD_CTL_ACCESS_KEY|WD_RT_EN|WD_EN);

	} else {
		ait_wd_writel(wdt_base,WD_MODE_CTL0,WD_CTL_ACCESS_KEY|WD_INT_EN|WD_EN);
	}

	spin_unlock(&vsnv3_wdt_lock);
	MMPF_WD_Kick();
//	mod_timer(&timer, jiffies + WDT_TIMEOUT);
//	mod_timer(&timer_start_wdt)
	return 0;
}

static int vsnv3_wdt_stop(struct watchdog_device *wdd)
{
	WDT_DBG("%s: %s\n", __func__, wdd->info->identity);

	printk(KERN_ALERT"%s: %s\n", __func__, wdd->info->identity);
	spin_lock(&vsnv3_wdt_lock);
	//ait_wd_writel(wdt_base,WD_MODE_CTL0,WD_CTL_ACCESS_KEY);
	spin_unlock(&vsnv3_wdt_lock);

	return 0;
}
int timeout=0;
static void vsnv3_wdt_timer_ping(unsigned long data)
{
	WDT_DBG("%s: \n", __func__);
	printk(KERN_ALERT"%s: %d \n", __func__,timeout);
	spin_lock(&vsnv3_wdt_lock);
	ait_wd_writel(wdt_base,WD_RE_ST, WD_RESTART);
	mod_timer(&timer, jiffies + WDT_TIMEOUT);
	spin_unlock(&vsnv3_wdt_lock);

	return;
}

void vsnv3_wdt_force_timeout(void)
{

	spin_lock(&vsnv3_wdt_lock);
	//del_timer(&timer);
	spin_unlock(&vsnv3_wdt_lock);
	
	return;
}


EXPORT_SYMBOL(vsnv3_wdt_force_timeout);

static int vsnv3_wdt_ping(struct watchdog_device *wdd)
{
	WDT_DBG("%s: %s\n", __func__, wdd->info->identity);
	AITPS_WD        pWD = AITC_BASE_WD;
	printk(KERN_ALERT"%s: %s\n", __func__, wdd->info->identity);
	spin_lock(&vsnv3_wdt_lock);

	ait_wd_writel(wdt_base,WD_RE_ST, WD_RESTART);
	spin_unlock(&vsnv3_wdt_lock);

	return 0;
}

static inline int vsnv3_wdt_is_running(void)
{
	WDT_DBG("%s\n", __func__);

	return ait_wd_readl(wdt_base,WD_MODE_CTL0)&WD_EN;//readl(wdt_base + S3C2410_WTCON) & S3C2410_WTCON_ENABLE;
}

/*
 * Set the watchdog time interval in 1/256Hz (write-once)
 * Counter is 12 bit.
 */

static int vsnv3_wdt_settimeout(struct watchdog_device *wdd, unsigned int timeout)
{
	MMP_USHORT usCounter = 31, usMclkDiv = 1024;
        AITPS_WD        pWD = AITC_BASE_WD;
//	printk(KERN_EMERG">>>>>>>>>>>>>>>>>>>>%s: \n", __func__);
	if (unlikely(timeout<0))
		return -1;

	MMPF_WD_Kick();
	//ait_wd_writel(wdt_base,WD_RE_ST, WD_RESTART);

	usCounter = (timeout * ((MMPF_WD_GetFreq()*1000)/usMclkDiv)) / 16384;

	printk(KERN_EMERG"%s:%s: MMPF_WD_GetFreq:%d \n",__FILE__,__func__,MMPF_WD_GetFreq());
	printk(KERN_EMERG"%s:%s: usCounter:%d \n",__FILE__,__func__,usCounter);
	printk(KERN_EMERG"%s:%s: timeout:%d \n",__FILE__,__func__,timeout);		
	printk(KERN_EMERG"%s:%s: usMclkDiv:%d \n",__FILE__,__func__,usMclkDiv);		

	if(timeout>=60){
           printk(KERN_EMERG"\r\n>>>>> set timeout >= 60s ,Turn-off WD !!\r\n");
           pWD->WD_MODE_CTL0 = 0x2340;
	   del_timer(&timer);
	   return 0;
	}


	if(usCounter > 31) {
	    usCounter = 31 ;
	    WDT_DBG("[AIT] : WD window=31(max)\n");
	}
	//MMPF_SF_WriteEAR(0x00);
	MMPF_WD_SetTimeOut(usCounter, usMclkDiv);

	//mod_timer(&timer,jiffies + timeout*HZ);
	del_timer(&timer);
	printk(KERN_EMERG"after delete timer!!!!!!!!!!!\n");
	

	return 0;
}


static int vsnv3_wdt_initsettimeout(struct watchdog_device *wdd, unsigned int timeout)
{
	MMP_USHORT usCounter = 31, usMclkDiv = 1024;

	if (unlikely(timeout<0))
		return -1;

	MMPF_WD_Kick();
	//ait_wd_writel(wdt_base,WD_RE_ST, WD_RESTART);

	usCounter = (timeout * ((MMPF_WD_GetFreq()*1000)/usMclkDiv)) / 16384;

	printk(KERN_ALERT"%s:%s: MMPF_WD_GetFreq:%d \n",__FILE__,__func__,MMPF_WD_GetFreq());
	printk(KERN_ALERT"%s:%s: usCounter:%d \n",__FILE__,__func__,usCounter);
	printk(KERN_ALERT"%s:%s: timeout:%d \n",__FILE__,__func__,timeout);	
	printk(KERN_ALERT"%s:%s: usMclkDiv:%d \n",__FILE__,__func__,usMclkDiv);

	if(usCounter > 31) {
	    usCounter = 31 ;
	    WDT_DBG("[AIT] : WD window=31(max)\n");
	    printk(KERN_ALERT"%s:%s: [AIT] : WD window=31(max) \n",__FILE__,__func__);
	}
	MMPF_WD_SetTimeOut(usCounter, usMclkDiv);

	//mod_timer(&timer,jiffies + timeout*HZ);

	return 0;
}

#if 0
static int vsnv3_wdt_settimeout(struct watchdog_device *wdd,unsigned int timeout)
{

	u8 ubCounter =31;
	u16 clockDiv = 1024;
	unsigned long ulValue;
	unsigned long	rate;
	
	WDT_DBG("%s: %s timeout = %d\n", __func__,wdd->info->identity, timeout);

	printk(KERN_ALERT"%s: %s timeout = %d\n", __func__,wdd->info->identity, timeout);
	if(unlikely( timeout<0))
		return -1;
	
	/* Check if disabled */
 
#if 0
//	if (mr & AT91_WDT_WDDIS) {
	if (!(wd_ctl & WD_EN)) {
//		printk(KERN_ERR DRV_NAME": sorry, watchdog is disabled\n");
		pr_err(KERN_ERR "VSNV3 WD:Watchdog is disabled\n");
		return -EIO;
	}
#endif	
	ait_wd_writel(wdt_base,WD_RE_ST, WD_RESTART);

	rate = clk_get_rate(vsnv3_wdt_clock);

	if((timeout*(rate/clockDiv))>(31*16384))
	{
		ubCounter = 31;

		printk(KERN_ALERT"%s:%d, counter-1 :%d\n",__func__,__LINE__,ubCounter);
	}
	else
	{
		ubCounter = max(1,(int)(timeout*(rate/clockDiv))/(16384));

		printk(KERN_ALERT"%s:%d, counter-2 :%d\n",__func__,__LINE__,ubCounter);
	}
	printk(KERN_ALERT"%s:%d\n",__func__,__LINE__);
	if(ubCounter >= 32) {
		WDT_DBG("Set CPU Watch dog counter Error\r\n");

		printk(KERN_ALERT"Set CPU Watch dog counter Error\r\n");
		return MMP_ERR_NONE;
	}
	
	printk(KERN_ALERT"%s:%d\n",__func__,__LINE__);
	ulValue = (ubCounter << 2);
	
	printk(KERN_ALERT"%s:%d\n",__func__,__LINE__);
	switch(clockDiv) {
		case 8:
			ulValue |= WD_CLK_MCK_D8;
			printk(KERN_ALERT"8 ulValue:%d\n",ulValue);
			break;
		case 32:
			ulValue |= WD_CLK_MCK_D32;
			printk(KERN_ALERT"32 ulValue:%d\n",ulValue);
			break;
		case 128:
			ulValue |= WD_CLK_MCK_D128;
			printk(KERN_ALERT"128 ulValue:%d\n",ulValue);
			break;
		case 1024:
			ulValue |= WD_CLK_MCK_D1024;
			printk(KERN_ALERT"1024 ulValue:%d\n",ulValue);
			break;
		default:
			WDT_DBG("Set CPU Watch dog CLK divider Error\r\n");
			printk(KERN_ALERT"Set CPU Watch dog counter Error\r\n");
			return MMP_ERR_NONE;
			break;
	}

	printk(KERN_ALERT"%s:%d\n",__func__,__LINE__);
	ait_wd_writel(wdt_base,WD_MODE_CTL1,WD_CLK_CTL_ACCESS_KEY|ulValue);
	
	printk(KERN_ALERT"%s:%d\n",__func__,__LINE__);
	return 0;
}
#endif

#define OPTIONS (WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE)

static const struct watchdog_info vsnv3_wdt_ident = {
	.options          =     OPTIONS,
	.firmware_version =	0,
	.identity         =	"VSNV3 Watchdog",
};

static struct watchdog_ops vsnv3_wdt_ops = {
	.owner = THIS_MODULE,
	.start = vsnv3_wdt_start,
	.stop = vsnv3_wdt_stop,
	.ping = vsnv3_wdt_ping,
	.set_timeout = vsnv3_wdt_settimeout,
};

static struct watchdog_device vsnv3_wdt_dev = {
	.info = &vsnv3_wdt_ident,
	.ops = &vsnv3_wdt_ops,
};

/* interrupt handler code */

static irqreturn_t vsnv3_wdt_irq(int irqno, void *param)
{
	unsigned long           flags;
	AITPS_WD 	pWD = AITC_BASE_WD;
//	unsigned long counter = 0x0;
	AITPS_GBL   pGBL = AITC_BASE_GBL;
	MMP_ULONG ulValue;
	spin_lock_irqsave(&vsnv3_wdt_lock,flags);
	//pr_err("watchdog timer expired \n");
	MMP_ULONG   counter = 0;
	printk(KERN_EMERG" > > > > > watchdog timer expired < < < < < \n");
	printk(KERN_EMERG"2. > > > > > %s: < < < < < \n",__func__);

//#if 0
  	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_VI, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_ISP, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_JPG, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_SCAL, MMP_TRUE);
	MMPF_SF_WriteEAR(0x00);
//	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_GPIO, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_AUD, MMP_TRUE);

	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_RAW, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_DMA, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_I2C, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_USB, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_H264, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_IBC, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_GRA, MMP_TRUE);

	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_SD, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_PWM, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_PSPI, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_USB_PHY, MMP_TRUE);
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_CPU_SRAM, MMP_TRUE);

	//MMPF_MMU_FlushDCache();
	//AT91F_DisableDCache();
	//AT91F_DisableICache();
	//AT91F_DisableMMU();

	pGBL->GBL_CLK_DIS0 = (MMP_UBYTE)(~(GBL_CLK_MCI_DIS | GBL_CLK_VI_DIS| GBL_CLK_JPG_DIS | GBL_CLK_CPU_DIS | GBL_CLK_GPIO_DIS));
	pGBL->GBL_CLK_DIS1 = (MMP_USHORT)(~(GBL_CLK_DRAM_DIS | GBL_CLK_PWM_DIS | GBL_CLK_I2C_DIS | GBL_CLK_DMA_DIS | GBL_CLK_USB_DIS | GBL_CLK_CPU_PHL_DIS));
	pGBL->GBL_CLK_DIS2 = (MMP_UBYTE)(~(GBL_CLK_CIDC_DIS | GBL_CLK_GNR_DIS | GBL_CLK_COLOR_DIS)) ;
	pGBL->GBL_CLK_DIS1 &= ~GBL_CLK_BS_SPI_DIS;

	//Change the Boot Strapping as ROM boot
	pGBL->GBL_CHIP_CTL |= MOD_BOOT_STRAP_EN;
	pGBL->GBL_CHIP_CFG = ROM_BOOT_MODE;
	pGBL->GBL_CHIP_CTL &= ~MOD_BOOT_STRAP_EN;
	
	//Turn-off watch dog
 	if((pWD->WD_MODE_CTL0 & WD_EN)!= 0x0) {
 		RTNA_DBG_Str(0, "\r\nTurn-off WD !!\r\n");
  	    pWD->WD_MODE_CTL0 = 0x2340;
 	}
	
	//VSN_V3, CPU access ROM code have HW bug, so we use reset ROM controller to archieve this purpose. 
	//Note: The busy waiting is necessary !!!  ROM controller need some time to re-load ROM code.
	MMPF_SYS_ResetHModule(MMPF_SYS_HMODULETYPE_CPU_PERIF, MMP_TRUE);

	#pragma O0
        for(counter = 0x3FFFF; counter > 0 ; counter --) {
        }
        #pragma

	
	//Finally, use watch-dog do timeout-reset, this purpose is to reset PLL as normal speed for serial-flash acessing issue.

	ulValue = WD_CLK_CTL_ACCESS_KEY|(15 << 2)|WD_CLK_MCK_D1024;

	ait_wd_writel(wdt_base,WD_MODE_CTL1,ulValue);

	pGBL->GBL_CHIP_CTL |= GBL_WD_RST_ALL_MODULE;

	pWD->WD_RE_ST = WD_RESTART; //Before enable, we shoudl set re-start first.
	
	ait_wd_writel(wdt_base,WD_MODE_CTL0,WD_CTL_ACCESS_KEY|WD_RT_EN|WD_EN);

	MMPF_SF_WriteEAR(0x00);

	spin_unlock_irqrestore(&vsnv3_wdt_lock,flags);

	return IRQ_HANDLED;
}


#ifdef CONFIG_CPU_FREQ

static int vsnv3_wdt_cpufreq_transition(struct notifier_block *nb,
					  unsigned long val, void *data)
{
	int ret;

	if (!vsnv3_wdt_is_running())
		goto done;

	if (val == CPUFREQ_PRECHANGE) {
		/* To ensure that over the change we don't cause the
		 * watchdog to trigger, we perform an keep-alive if
		 * the watchdog is running.
		 */

		vsnv3_wdt_ping(&vsnv3_wdt_dev);
	} else if (val == CPUFREQ_POSTCHANGE) {
		vsnv3_wdt_stop(&vsnv3_wdt_dev);

		ret = vsnv3_wdt_settimeout(&vsnv3_wdt_dev, vsnv3_wdt_dev.timeout);

		if (ret >= 0)
			vsnv3_wdt_start(&vsnv3_wdt_dev);
		else
			goto err;
	}

done:
	return 0;

 err:
//	dev_err(wdt_dev, "cannot set new value for timeout %d\n",
//				s3c2410_wdd.timeout);
	return ret;
}

static struct notifier_block vsnv3_wdt_cpufreq_transition_nb = {
	.notifier_call	= vsnv3_wdt_cpufreq_transition,
};

static inline int vsnv3_wdt_cpufreq_register(void)
{
	return cpufreq_register_notifier(&vsnv3_wdt_cpufreq_transition_nb,
					 CPUFREQ_TRANSITION_NOTIFIER);
}

static inline void vsnv3_wdt_cpufreq_deregister(void)
{
	cpufreq_unregister_notifier(&vsnv3_wdt_cpufreq_transition_nb,
				    CPUFREQ_TRANSITION_NOTIFIER);
}

#else
static inline int vsnv3_wdt_cpufreq_register(void)
{
	return 0;
}

static inline void vsnv3_wdt_cpufreq_deregister(void)
{
}
#endif

static int __devinit vsnv3_wdt_probe(struct platform_device *pdev)
{
	struct device *dev;
	int started = 0;
	int ret;
	int size;

	WDT_DBG("%s: probe=%p\n", __func__, pdev->name);

	dev = &pdev->dev;
	wdt_dev = &pdev->dev;

	/* get the memory region for the watchdog timer */

	wdt_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (wdt_mem == NULL) {
		//dev_err(dev, "no memory resource specified\n");
		return -ENOENT;
	}

	size = resource_size(wdt_mem);
	if (!request_mem_region(wdt_mem->start, size, pdev->name)) {
		dev_err(dev, "failed to get memory region\n");
		return -EBUSY;
	}

	wdt_base = ioremap(wdt_mem->start, size);
	if (wdt_base == NULL) {
		dev_err(dev, "failed to ioremap() region\n");
		ret = -EINVAL;
		goto err_req;
	}

	WDT_DBG("probe: mapped wdt_base=%p\n", wdt_base);

	wdt_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (wdt_irq == NULL) {
		dev_err(dev, "no irq resource specified\n");
		ret = -ENOENT;
		goto err_map;
	}

	ret = request_irq(wdt_irq->start, vsnv3_wdt_irq, 0, pdev->name, pdev);
	if (ret != 0) {
		dev_err(dev, "failed to install irq (%d)\n", ret);
		goto err_map;
	}

	vsnv3_wdt_clock = clk_get(&pdev->dev, "wdt_ctl_clk");
	if (IS_ERR(vsnv3_wdt_clock)) {
		dev_err(dev, "failed to find watchdog clock source\n");
		ret = PTR_ERR(vsnv3_wdt_clock);
		goto err_irq;
	}

	clk_enable(vsnv3_wdt_clock);

	if (vsnv3_wdt_cpufreq_register() < 0) {
		printk(KERN_ERR "%s: failed to register cpufreq\n",dev_name(&pdev->dev));
		goto err_clk;
	}

	/* see if we can actually set the requested timer margin, and if
	 * not, try the default value */
/*
	if (vsnv3_wdt_initsettimeout(&vsnv3_wdt_dev, tmr_margin)) {
			dev_info(dev,
			   "timeout value out of range, default %d used\n", CONFIG_VSNV3_WATCHDOG_DEFAULT_TIME);

	}
*/
	ret = watchdog_register_device(&vsnv3_wdt_dev);
	if (ret) {
		dev_err(dev, "cannot register watchdog (%d)\n", ret);
		goto err_cpufreq;
	}

	next_heartbeat = jiffies + CONFIG_VSNV3_WATCHDOG_DEFAULT_TIME * HZ;
	//setup_timer(&timer, vsnv3_wdt_timer_ping, 0);

		
	if (wdt_on_atboot) {
		dev_info(dev, "starting watchdog timer\n");
		printk(KERN_EMERG"->-> starting watchdog timer!!!!\n");
		setup_timer(&timer, vsnv3_wdt_timer_ping, 0);
		//vsnv3_wdt_start(&vsnv3_wdt_dev);
		mod_timer(&timer, jiffies + WDT_TIMEOUT);
		

		//setup_timer(&timer_start_wdt, vsnv3_wdt_start, &vsnv3_wdt_dev);
		
		
	} else if (!wdt_on_atboot) {
		/* if we're not enabling the watchdog, then ensure it is
		 * disabled if it has been left running from the bootloader
		 * or other source */
		printk(KERN_EMERG"->-> no starting watchdog timer,go to vsnv3_wdt_stop !!!!\n");
		vsnv3_wdt_stop(&vsnv3_wdt_dev);
	}

	return 0;

err_cpufreq:
	vsnv3_wdt_cpufreq_deregister();

err_clk:
	clk_disable(vsnv3_wdt_clock);
	clk_put(vsnv3_wdt_clock);

err_irq:
	free_irq(wdt_irq->start, pdev);

err_map:
	iounmap(wdt_base);

err_req:
	release_mem_region(wdt_mem->start, size);
	wdt_mem = NULL;

	return ret;
}

static int __devexit vsnv3_wdt_remove(struct platform_device *dev)
{
//	printk(KERN_ALERT"%s,%s: \n",__func__,__LINE__);
	watchdog_unregister_device(&vsnv3_wdt_dev);

	vsnv3_wdt_cpufreq_deregister();

	clk_disable(vsnv3_wdt_clock);
	clk_put(vsnv3_wdt_clock);
	vsnv3_wdt_clock = NULL;

	free_irq(wdt_irq->start, dev);
	wdt_irq = NULL;

	iounmap(wdt_base);

	release_mem_region(wdt_mem->start, resource_size(wdt_mem));
	wdt_mem = NULL;
	return 0;
}

static void vsnv3_wdt_shutdown(struct platform_device *dev)
{
//	printk(KERN_ALERT"%s,%s: \n",__func__,__LINE__);
	vsnv3_wdt_stop(&vsnv3_wdt_dev);
}

#ifdef CONFIG_PM

//static unsigned long wtcon_save;
//static unsigned long wtdat_save;

static int vsnv3_wdt_suspend(struct platform_device *dev, pm_message_t state)
{
//printk(KERN_ALERT"%s,%s: \n",__func__,__LINE__);
	return 0;
}

static int vsnv3_wdt_resume(struct platform_device *dev)
{
//printk(KERN_ALERT"%s,%s: \n",__func__,__LINE__);
	return 0;
}

#else
#define vsnv3_wdt_suspend NULL
#define vsnv3_wdt_resume  NULL
#endif /* CONFIG_PM */


static struct platform_driver vsnv3_wdt_driver = {
	.probe		= vsnv3_wdt_probe,
	.remove		= __devexit_p(vsnv3_wdt_remove),
	.shutdown	= vsnv3_wdt_shutdown,
	.suspend	= vsnv3_wdt_suspend,
	.resume		= vsnv3_wdt_resume,
	.driver		= {
		.name	= "vsnv3_wdt",
		.owner	= THIS_MODULE,
	},
};

static int __init watchdog_init(void)
{
	printk("AIT VSNV3 Watchdog Timer (c) 2014 Alpha Image Technology Corp.");
	return platform_driver_register(&vsnv3_wdt_driver);
}

static void __exit watchdog_exit(void)
{
	platform_driver_unregister(&vsnv3_wdt_driver);
}

module_init(watchdog_init);
module_exit(watchdog_exit);

MODULE_AUTHOR("AIT <ait@a-i-t.com.tw>");
MODULE_DESCRIPTION("AIT VSNV3 Watchdog Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
MODULE_ALIAS("platform:aitvsnv3-wdt");


