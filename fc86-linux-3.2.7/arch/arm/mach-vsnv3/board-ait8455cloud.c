/*
 * linux/arch/arm/mach-at91/board-sam9260ek.c
 *
 *  Copyright (C) 2005 SAN People
 *  Copyright (C) 2006 Atmel
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

#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/at73c213.h>
#include <linux/clk.h>
#include <linux/i2c/at24.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/delay.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/hardware.h>
#include <mach/board.h>
#include <mach/system_rev.h>

#include "generic.h"

#include <mach/mmpf_pio.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_audio.h>
#include <mach/mmpf_i2s_ctl.h>
#include <mach/mmpf_pio.h>
#include <mach/mmpf_i2cm.h>

static void __init vsnv3_init_early(void)
{
#define PAGPIO03		((AIT_REG_B*)	AIT8455_OPR_P2V(0x800069A8))
#define PAGPIO04		((AIT_REG_B*)	AIT8455_OPR_P2V(0x800069A9))
#define PAGPIO05		((AIT_REG_B*)	AIT8455_OPR_P2V(0x800069AA))

	extern MMP_ERR MMPF_PIO_Initialize(void);
//	AITPS_GBL pGBL = AITC_BASE_GBL;
	printk("%s\r\n",__FUNCTION__);	
	MMPF_PIO_Initialize();

/**********************Enable GPIO Mode*****************************/
	*PAGPIO03 &= ~(0x02);	//disable pull down
	*PAGPIO03 |= (0x04);	//enable pull up	
	
	*PAGPIO04 &= ~(0x02);	//disable pull down
	*PAGPIO04 |= (0x04);		//enable pull up

	*PAGPIO05 &= ~(0x02);	//disable pull down
	*PAGPIO05 &= ~(0x04);	//enable pull up	

printk(KERN_ERR"========== before wifi reset =========\n");
	MMPF_PIO_EnableGpioMode(AIT_GPIO_CGPIO6_WIFI_RESET, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(AIT_GPIO_CGPIO6_WIFI_RESET, (MMP_BOOL)1);
	MMPF_PIO_SetData(AIT_GPIO_CGPIO6_WIFI_RESET, 1);
printk(KERN_ERR"========== after wifi reset =========\n");
	
/**********************Disable GPIO Mode*****************************/
//For I2S
	MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO24, MMP_FALSE);
	MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO25, MMP_FALSE);
	MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO26, MMP_FALSE);
	MMPF_PIO_EnableGpioMode(MMPF_PIO_REG_GPIO27, MMP_FALSE);	

	MMPF_PIO_EnableGpioMode(AIT_GPIO_CGPIO16,1);
	MMPF_PIO_EnableOutputMode(AIT_GPIO_CGPIO16,0);

	MMPF_PIO_EnableTrigMode(AIT_GPIO_CGPIO16,MMPF_PIO_TRIGMODE_EDGE_H2L, 1);
	MMPF_PIO_EnableTrigMode(AIT_GPIO_CGPIO16,MMPF_PIO_TRIGMODE_EDGE_L2H, 1);
	MMPF_PIO_EnableInterrupt(AIT_GPIO_CGPIO16, 1, 0, 0);	
	/* Initialize processor: 18.432 MHz crystal */
	vsnv3_initialize(12000000);

	/* DBGU on ttyS0. (Rx & Tx only) */
	//vsnv3_register_uart(0, 0, 0);

	vsnv3_set_serial_console(0);


}


/*
 * USB Device port
 */
static struct ait_udc_data __initdata ek_udc_data = {
	.pullup_pin	= 0,		/* pull-up driven by UDC */
};


/*
 * SPI devices.
 */
static struct spi_board_info ait8455_evb_pspi_devices[] = {
	//For Ethernet	
	{	/* DataFlash chip */
		.modalias	= "spidev",
		.chip_select	= 0,	// Must less than 	master->num_chipselect in sif driver probe.
		.max_speed_hz	= 25 * 1000 * 1000,
		.bus_num	= 1,
		.mode	= SPI_MODE_0,		//For AX88796C
		.irq = gpio_to_irq(AIT_GPIO_ETHERNET_IRQ),
	},

};

static struct spi_board_info ait8455_evb_sif_devices[] = {
	{	/* DataFlash chip */
		.modalias	= "m25p80",
		.chip_select	= 0,	// Must less than 	master->num_chipselect in sif driver probe.
		.max_speed_hz	= 33 * 1000 * 1000,
		.bus_num	= 0,
		.mode	= SPI_MODE_0,		
	},
#if 0	
	{	/* DataFlash chip */
		.modalias	= "spidev",
		.chip_select	= 0,	// Must less than 	master->num_chipselect in sif driver probe.
		.max_speed_hz	= 33 * 1000 * 1000,
		.bus_num	= 0,
		.mode	= SPI_MODE_0,		
	},
#endif	
};

/*
 * MCI (SD/MMC)
 */
 static struct ait_sd_data /*__initdata*/ vsnv3_mmc_data_sdio = {
	.controller_id	= 0,
	.wire4		= 1,
	.pad_id		= 1,
	.det_pin	= 0,//... not connected	
	.wp_pin	= 0,//	= ... not connected
	.vcc_pin	= 0//... not connected
//	.det_pin	= ... not connected	
//	.wp_pin		= ... not connected
//	.vcc_pin	= ... not connected
};

static struct ait_sd_data /*__initdata*/ vsnv3_mmc_data_sd1 = {
	.controller_id	= 1,
	.wire4		= 1,
	.pad_id		= 1,	
	.det_pin	=AIT_GPIO_SD_DET_IRQ,//... not connected	

	.wp_pin	= 0,//	= ... not connected
	.vcc_pin	= 0,//... not connected
	.active_low = 1
};


 /*
  * LEDs and GPOs
  */
static struct gpio_led gpio_leds[] = {     
         {
                 .name                   = "LED-STATUS",
                 .gpio                   = AIT_GPIO_AGPIO03,
                 .active_low             = 0,
                 .default_trigger        = "none",
                 .default_state          = LEDS_GPIO_DEFSTATE_KEEP,
         },		 
         {
                 .name                   = "LED-WIFI",
                 .gpio                   = AIT_GPIO_AGPIO04,
                 .active_low             = 0,
                 .default_trigger        = "none",
                 .default_state          = LEDS_GPIO_DEFSTATE_KEEP,
         },

         {
                 .name                   = "LED-POWER",
                 .gpio                   = AIT_GPIO_AGPIO05,
                 .active_low             = 0,
                 .default_trigger        = "none",
                 .default_state          = LEDS_GPIO_DEFSTATE_KEEP,
         },
  
 };


/*
 * I2C devices
 */


static struct at24_platform_data at24c512 = {
	.byte_len	= SZ_512K / 8,
	.page_size	= 128,
	.flags		= AT24_FLAG_ADDR16,
};

static struct i2c_board_info __initdata ek_i2c_devices[] = {
	{
		I2C_BOARD_INFO("ov9710", 0x30),
		.platform_data = &at24c512,
	},

	/* more devices can be added using expansion connectors */
};


static struct i2c_board_info __initdata I2C_0_devices[] = { //I2C master 1, pad 0
	{
		I2C_BOARD_INFO("NULL", 0x00),
	},
	/* more devices can be added using expansion connectors */
};

static struct i2c_board_info __initdata I2C_1_devices[] = { //I2C master 0, pad 0
	{
		I2C_BOARD_INFO("rt5628", 0x18),
	},
	{
		I2C_BOARD_INFO("rt5627", 0x19),
	},
	{
		I2C_BOARD_INFO("wm8973", 0x1A),
	},
	{
		I2C_BOARD_INFO("rt5640", (0x38>>1)),
	},
	/* more devices can be added using expansion connectors */
};

static struct i2c_board_info __initdata I2C_2_devices[] = { //I2C master 0, pad 1
	{
		I2C_BOARD_INFO("NULL", 0x00),
	},
	/* more devices can be added using expansion connectors */
};

/*
 * GPIO Buttons
 */
#if defined(CONFIG_KEYBOARD_GPIO) || defined(CONFIG_KEYBOARD_GPIO_MODULE)
static struct gpio_keys_button ait8455evb_buttons[] = {

	{
		.gpio		= AIT_GPIO_BGPIO013,
		.code		= KEY_POWER2,
		.desc		= "Key Power off Detect",
		.debounce_interval = 20,
		.active_low	= 1,
		.wakeup		= 1,
	},

	{
		.gpio		= AIT_GPIO_CGPIO17,
		.code		= KEY_RESTART,
		.desc		= "Button Reset",
		.debounce_interval = 20,
		.active_low	= 1,
		.wakeup		= 1,
	},		
/*
	{
		.gpio		= AIT_GPIO_CGPIO09,
		.code		= KEY_MODE,
		.desc		= "KEY_MODE",
		.debounce_interval = 20,
		.active_low	= 1,
		.wakeup		= 1,
	},
	{
		.gpio		= AIT_GPIO_CGPIO08,
		.code		= KEY_TITLE,
		.desc		= "KEY_PIR",
		.debounce_interval = 20,
		.active_low	= 1,
		.wakeup		= 1,
	},		
*/


};

static struct gpio_keys_platform_data ait8455_button_data = {
	.buttons	= ait8455evb_buttons,
	.nbuttons	= ARRAY_SIZE(ait8455evb_buttons),
	.name = "skypecam-keys"
};

static struct platform_device ait8455evb_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &ait8455_button_data,
	}
};

static void __init ait8455evb_button_devices(void)
{
	int i;
	for(i=0;i<ARRAY_SIZE(ait8455evb_buttons);++i)
	{
		int gpio = ait8455evb_buttons[i].gpio;
		MMPF_PIO_EnableGpioMode(gpio,1);
		MMPF_PIO_EnableOutputMode(gpio,0);
		MMPF_PIO_EnableTrigMode(gpio, MMPF_PIO_TRIGMODE_EDGE_L2H, 1);
		MMPF_PIO_EnableTrigMode(gpio,MMPF_PIO_TRIGMODE_EDGE_H2L, 1);
		MMPF_PIO_EnableInterrupt(gpio, 1, 0, 0);	
	}

	platform_device_register(&ait8455evb_button_device);

}
#else
static void __init ait8455evb_button_devices(void) {}
#endif


static struct platform_device ait_camera_device = {
	.name   = "ait-cam",
	.id     = -1,

};
extern void __init vsnv3_init_video_dram(int bank, unsigned long base, unsigned int length);
static void __init ait8455evb_board_init(void)
{
	AITPS_GBL pGBL = AITC_BASE_GBL;
	pr_info("## Board Device Initial ##");
	
	/* Serial */
	ait_add_device_serial();
#if 0	
	/* USB Host */
	at91_add_device_usbh(&ek_usbh_data);
	/* USB Device */
	at91_add_device_udc(&ek_udc_data);
#endif

	vsnv3_add_device_udc(&ek_udc_data);

	/* SPI */
	vsnv3_add_device_sif(ait8455_evb_sif_devices,ARRAY_SIZE(ait8455_evb_sif_devices));
	
	vsnv3_add_device_pspi(ait8455_evb_pspi_devices, ARRAY_SIZE(ait8455_evb_pspi_devices));

	/* MMC */
	vsnv3_add_device_mmc(0, &vsnv3_mmc_data_sdio );
	vsnv3_add_device_mmc(1, &vsnv3_mmc_data_sd1);

	/* I2C */
	vsnv3_add_adapter_i2c();
	vsnv3_add_device_i2c(I2C_0_devices, ARRAY_SIZE(I2C_0_devices),0); //add i2c device to i2c_1 , pad 0
	vsnv3_add_device_i2c(I2C_1_devices, ARRAY_SIZE(I2C_1_devices),1); //add i2c device to i2c_0 , pad 0
	vsnv3_add_device_i2c(I2C_2_devices, ARRAY_SIZE(I2C_2_devices),2); //add i2c device to i2c_0 , pad 1


//Enable I2S MCLK
	pGBL->GBL_CLK_DIS0 &= (~GBL_CLK_AUD_DIS);
	
	vsnv3_add_device_ssc(AITVSNV3_ID_I2S_FIFO, 0);
	vsnv3_add_device_ssc(AITVSNV3_ID_AFE_FIFO, 0);	



	platform_device_register(&ait_camera_device);

	/* LEDs */
	vsnv3_gpio_leds(gpio_leds, ARRAY_SIZE(gpio_leds));

	/* Buttons */
	ait8455evb_button_devices();
	

}


/*Vin: Modify for matching unmber which pass from u-boot MACHINE_START(AT91SAM9260EK, "Atmel AT91SAM9260-EK")*/
MACHINE_START(AT91SAM9X5EK, "AIT VSNV3 AIT8455 CLOUD")

	/* Maintainer: Atmel */
	.timer		= &at91sam926x_timer,
	.map_io		= vsnv3_map_io,
	.init_early	= vsnv3_init_early,
	.init_irq	= vsnv3_init_irq_default,
	.init_machine	= ait8455evb_board_init,
#if 0
	.video_start 	= ,
	.video_end 	= ,
#endif		
MACHINE_END
