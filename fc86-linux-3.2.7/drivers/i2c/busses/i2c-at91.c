/*
    i2c Support for Atmel's AT91 Two-Wire Interface (TWI)

    Copyright (C) 2004 Rick Bronson
    Converted to 2.6 by Andrew Victor <andrew@sanpeople.com>

    Borrowed heavily from original work by:
    Copyright (C) 2000 Philip Edelbrock <phil@stimpy.netroedge.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <mach/at91_twi.h>
#include <mach/board.h>
#include <mach/cpu.h>

#include <mach/mmp_register.h>
#include <mach/mmp_reg_vif.h>
#include <mach/mmp_reg_i2cm.h>
#include <mach/mmpf_i2cm.h>
#include <mach/mmpf_vif.h>

#define TWI_CLOCK		100000		/* Hz. max 400 Kbits/sec */

static struct clk *twi_clk;
//static void __iomem *twi_base;

//#define at91_twi_read(reg)		__raw_readl(twi_base + (reg))
//#define at91_twi_write(reg, val)	__raw_writel((val), twi_base + (reg))

static MMPF_I2CM_ATTRIBUTE m_I2CMAttribute = {
	.uI2cmID			= MMPF_I2CM_ID_0,			//MMPF_I2CM_ID_0 ~ MMPF_I2CM_ID_2 stand for HW I2CM
	.ubSlaveAddr 		= 0x30,
	.ubRegLen		= 8,				//Indicate register as the 8 bit mode or 16 bit mode.
	.ubDataLen		= 8,				//Indicate data as the 8 bit mode or 16 bit mode.
	.ubDelayTime		= 0,			//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW or MMPF_I2CM_ID_SW_SENSOR)
   									//To Adjust the speed of software I2CM
	.bDelayWaitEn		= MMP_FALSE,  
	.bInputFilterEn		= MMP_FALSE,		//HW feature, to filter input noise
	.b10BitModeEn	= MMP_FALSE,			//HW I2CM supports 10 bit slave address, the bit8 and bit9 are in ubSlaveAddr1
	.bClkStretchEn	= MMP_FALSE, 		//HW support stretch clock
	.ubSlaveAddr1	= 0,
	.ubDelayCycle		= 0,  		//When bDelayWaitEn enable, set the delay cycle after each 8 bit transmission
	.ubPadNum		= 1,      		//HW pad map, the relate pad definition, please refer global register spec.
   									// ========= For Vision V2=========
									// If I2CM_ID = MMPF_I2CM_ID_0, ubPadNum = 0 stands for I2CM0_SCK = PHI2C_SCL
									//                              ubPadNum = 1 stands for I2CM0_SCK = PSNR_HSYNC
									// IF I2CM_ID = MMPF_I2CM_ID_1, only one pad(I2CM1_SCK = PSNR_SCK) is used   									
   									// ========= For Vision V2=========
	.uI2cmSpeed 		= MMPF_I2CM_SPEED_HW_250K, //HW I2CM speec control
	.SW_CLK_PIN 	= 0,  		//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW only), indicate the clock pin
	.SW_DAT_PIN		= 0,		
	.name 			= "ov9712",
	.deviceID			= 0x9711


};


/*
 * Initialize the TWI hardware registers.
 */
static void __devinit at91_twi_hwinit(void)
{
	unsigned long cdiv, ckdiv;
	AITPS_VIF	pVIF = AITC_BASE_VIF;
	int i;
	
	MMPF_I2cm_DisableInterrupt(&m_I2CMAttribute);

	pVIF->VIF_SENSR_CLK_FREQ[0] = VIF_SENSR_CLK_PLL_D11;
	pVIF->VIF_SENSR_CLK_CTL[0] = VIF_SENSR_CLK_EN;	

//The following should move to sensor driver

	MMPF_VIF_SetPIOOutput(VIF_SIF_RST, MMP_TRUE);

	msleep_interruptible(10);

	MMPF_VIF_SetPIOOutput(VIF_SIF_RST, MMP_FALSE);
	msleep_interruptible(10);

	MMPF_VIF_SetPIOOutput(VIF_SIF_RST, MMP_TRUE);
	msleep_interruptible(10);


	MMPF_I2cm_Initialize(&m_I2CMAttribute );
//	MMPF_I2c_ReadData(&m_I2CMAttribute );

//	for(i=0;i<255;++i)
	{
		MMP_BYTE usData ;
		int ret;
		usData = 0;
		ret = MMPF_I2cm_ReadReg(&m_I2CMAttribute, 0x0A, &usData);
		if(usData!=((m_I2CMAttribute.deviceID>>8)&0xff))
			printk(KERN_ERR "Get I2C ID Error Reg[%d] = 0x%02X\r\n",0x0A,usData);
		
		
		ret = MMPF_I2cm_ReadReg(&m_I2CMAttribute, 0x0B, &usData);		
		if(usData!=(m_I2CMAttribute.deviceID&0xff))		
			printk(KERN_ERR "Get I2C ID Error Reg[%d] = 0x%02X\r\n",0x0B,usData);		
		
	}

	
#if 0
	at91_twi_write(AT91_TWI_IDR, 0xffffffff);	/* Disable all interrupts */
	at91_twi_write(AT91_TWI_CR, AT91_TWI_SWRST);	/* Reset peripheral */
	at91_twi_write(AT91_TWI_CR, AT91_TWI_MSEN);	/* Set Master mode */

	
	
	/* Calcuate clock dividers */
	cdiv = (clk_get_rate(twi_clk) / (2 * TWI_CLOCK)) - 3;
	cdiv = cdiv + 1;	/* round up */
	ckdiv = 0;
	while (cdiv > 255) {
		ckdiv++;
		cdiv = cdiv >> 1;
	}

	if (cpu_is_at91rm9200()) {			/* AT91RM9200 Errata #22 */
		if (ckdiv > 5) {
			printk(KERN_ERR "AT91 I2C: Invalid TWI_CLOCK value!\n");
			ckdiv = 5;
		}
	}

	at91_twi_write(AT91_TWI_CWGR, (ckdiv << 16) | (cdiv << 8) | cdiv);
#endif	
}
#if 0
/*
 * Poll the i2c status register until the specified bit is set.
 * Returns 0 if timed out (100 msec).
 */
static short at91_poll_status(unsigned long bit)
{
	int loop_cntr = 10000;

	do {
		udelay(10);
	} while (!(at91_twi_read(AT91_TWI_SR) & bit) && (--loop_cntr > 0));

	return (loop_cntr > 0);
}

static int xfer_read(struct i2c_adapter *adap, unsigned char *buf, int length)
{
	/* Send Start */
	at91_twi_write(AT91_TWI_CR, AT91_TWI_START);

	/* Read data */
	while (length--) {
		if (!length)	/* need to send Stop before reading last byte */
			at91_twi_write(AT91_TWI_CR, AT91_TWI_STOP);
		if (!at91_poll_status(AT91_TWI_RXRDY)) {
			dev_dbg(&adap->dev, "RXRDY timeout\n");
			return -ETIMEDOUT;
		}
		*buf++ = (at91_twi_read(AT91_TWI_RHR) & 0xff);
	}

	return 0;
}

static int xfer_write(struct i2c_adapter *adap, unsigned char *buf, int length)
{
	/* Load first byte into transmitter */
	at91_twi_write(AT91_TWI_THR, *buf++);

	/* Send Start */
	at91_twi_write(AT91_TWI_CR, AT91_TWI_START);

	do {
		if (!at91_poll_status(AT91_TWI_TXRDY)) {
			dev_dbg(&adap->dev, "TXRDY timeout\n");
			return -ETIMEDOUT;
		}

		length--;	/* byte was transmitted */

		if (length > 0)		/* more data to send? */
			at91_twi_write(AT91_TWI_THR, *buf++);
	} while (length);

	/* Send Stop */
	at91_twi_write(AT91_TWI_CR, AT91_TWI_STOP);

	return 0;
}
#endif
/*
 * Generic i2c master transfer entrypoint.
 *
 * Note: We do not use Atmel's feature of storing the "internal device address".
 * Instead the "internal device address" has to be written using a separate
 * i2c message.
 * http://lists.arm.linux.org.uk/pipermail/linux-arm-kernel/2004-September/024411.html
 */
static int at91_xfer(struct i2c_adapter *adap, struct i2c_msg *pmsg, int num)
{
	int i, ret;

	dev_dbg(&adap->dev, "at91_xfer: processing %d messages:\n", num);

//	for (i = 0; i < num; i++) 
		{
		dev_dbg(&adap->dev, " #%d: %sing %d byte%s %s 0x%02x\n", i,
			pmsg->flags & I2C_M_RD ? "read" : "writ",
			pmsg->len, pmsg->len > 1 ? "s" : "",
			pmsg->flags & I2C_M_RD ? "from" : "to",	pmsg->addr);

//		at91_twi_write(AT91_TWI_MMR, (pmsg->addr << 16)
//			| ((pmsg->flags & I2C_M_RD) ? AT91_TWI_MREAD : 0));

		m_I2CMAttribute.ubSlaveAddr = pmsg->addr;
		if (pmsg->len && pmsg->buf) {	/* sanity check */
			if (pmsg->flags & I2C_M_RD)
			{
				MMP_USHORT reg, data;
				dev_dbg(&adap->dev, "Reg = %d   buf = %x \r\n",pmsg->buf[0], pmsg->buf[1]);
				reg = pmsg->buf[0];
				ret = MMPF_I2cm_ReadReg(&m_I2CMAttribute, reg, &data);
				
				pmsg->buf[1] = data;
//				ret = MMPF_I2cm_ReadXfr(&m_I2CMAttribute, pmsg->buf, pmsg->len);
//				ret = xfer_read(adap, pmsg->buf, pmsg->len);
			}
			else
			{
//				ret = MMPF_I2cm_WriteXfr(&m_I2CMAttribute, pmsg->buf, pmsg->len);
				ret = MMPF_I2cm_WriteReg(&m_I2CMAttribute, pmsg->buf[0], pmsg->buf[1]);
//				ret = xfer_write(adap, pmsg->buf, pmsg->len);
			}

			if(ret ==MMP_I2CM_ERR_SLAVE_NO_ACK )
				return -ENXIO;
			
			if (ret)
//				return ret;
				return -ETIMEDOUT;


				
			/* Wait until transfer is finished */
//			if (!at91_poll_status(AT91_TWI_TXCOMP)) {
//				dev_dbg(&adap->dev, "TXCOMP timeout\n");
//				return -ETIMEDOUT;
			//}
		}
		dev_dbg(&adap->dev, "transfer complete\n");
//		pmsg++;		/* next message */
	}
	return ret;//i;
}

/*
 * Return list of supported functionality.
 */
static u32 at91_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm at91_algorithm = {
	.master_xfer	= at91_xfer,
	.functionality	= at91_func,
};

/*
 * Main initialization routine.
 */
static int __devinit at91_i2c_probe(struct platform_device *pdev)	//at91sam9260_twi_device
{
	struct i2c_adapter *adapter;
	struct resource *res;
	int rc;


printk("%s: Todo\r\n",__FUNCTION__);
#if 0// io mem mapping
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENXIO;

	if (!request_mem_region(res->start, resource_size(res), "VsionV3_i2c"))
		return -EBUSY;

	twi_base = ioremap(res->start, resource_size(res));
	if (!twi_base) {
		rc = -ENOMEM;
		goto fail0;
	}
#endif
	twi_clk = clk_get(NULL, "i2c_clk");
	if (IS_ERR(twi_clk)) {
		dev_err(&pdev->dev, "no clock defined\n");
		rc = -ENODEV;
		goto fail1;
	}

	adapter = kzalloc(sizeof(struct i2c_adapter), GFP_KERNEL);
	if (adapter == NULL) {
		dev_err(&pdev->dev, "can't allocate inteface!\n");
		rc = -ENOMEM;
		goto fail2;
	}
	snprintf(adapter->name, sizeof(adapter->name), "VisionV3I2CM_Adapter");
	adapter->algo = &at91_algorithm;
	adapter->class = I2C_CLASS_HWMON;
	adapter->dev.parent = &pdev->dev;
	/* adapter->id == 0 ... only one TWI controller for now */

	platform_set_drvdata(pdev, adapter);

	clk_enable(twi_clk);		/* enable peripheral clock */
	at91_twi_hwinit();		/* initialize TWI controller */

	rc = i2c_add_numbered_adapter(adapter);
	if (rc) {
		dev_err(&pdev->dev, "Adapter %s registration failed\n",
				adapter->name);
		goto fail3;
	}

	dev_info(&pdev->dev, "AT91 i2c bus driver.\n");
	return 0;

fail3:
	platform_set_drvdata(pdev, NULL);
	kfree(adapter);
	clk_disable(twi_clk);
fail2:
	clk_put(twi_clk);
fail1:
	//Vin iounmap(twi_base);
fail0:
	//Vin	release_mem_region(res->start, resource_size(res));

	return rc;
}

static int __devexit at91_i2c_remove(struct platform_device *pdev)
{
	struct i2c_adapter *adapter = platform_get_drvdata(pdev);
	struct resource *res;
	int rc;

	rc = i2c_del_adapter(adapter);
	platform_set_drvdata(pdev, NULL);
#if 0
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	iounmap(twi_base);
	release_mem_region(res->start, resource_size(res));
#endif
	clk_disable(twi_clk);		/* disable peripheral clock */
	clk_put(twi_clk);

	return rc;
}

#ifdef CONFIG_PM

/* NOTE: could save a few mA by keeping clock off outside of at91_xfer... */

static int at91_i2c_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	clk_disable(twi_clk);
	return 0;
}

static int at91_i2c_resume(struct platform_device *pdev)
{
	return clk_enable(twi_clk);
}

#else
#define at91_i2c_suspend	NULL
#define at91_i2c_resume		NULL
#endif

/* work with "modprobe at91_i2c" from hotplugging or coldplugging */
MODULE_ALIAS("platform:VsionV3_i2c");

static struct platform_driver at91_i2c_driver = {
	.probe		= at91_i2c_probe,
	.remove		= __devexit_p(at91_i2c_remove),
	.suspend		= at91_i2c_suspend,
	.resume		= at91_i2c_resume,
	.driver		= {
		.name	= "VsionV3_i2c",
		.owner	= THIS_MODULE,
	},
};

static int __init at91_i2c_init(void)
{
	return platform_driver_register(&at91_i2c_driver);
}

static void __exit at91_i2c_exit(void)
{
	platform_driver_unregister(&at91_i2c_driver);
}

module_init(at91_i2c_init);
module_exit(at91_i2c_exit);

MODULE_AUTHOR("Vincent Chen");
MODULE_DESCRIPTION("I2CM driver for AIT Vision V3");
//MODULE_LICENSE("GPL");
