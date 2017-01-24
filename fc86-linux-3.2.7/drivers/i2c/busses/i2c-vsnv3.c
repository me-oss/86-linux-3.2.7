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
#include <linux/semaphore.h>

#include <mach/board.h>
#include <mach/cpu.h>

#include <mach/mmp_register.h>
#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_i2cm.h>
#include <mach/mmpf_i2cm.h>

#define TWI_CLOCK		100000		/* Hz. max 400 Kbits/sec */

static struct clk *i2cm_clk;

static void __iomem *twi_base;
#if 0// io mem mapping
#define at91_twi_read(reg)		__raw_readl(twi_base + (reg))
#define at91_twi_write(reg, val)	__raw_writel((val), twi_base + (reg))
#endif

#if 0
static MMPF_I2CM_ATTRIBUTE m_PHI2CMAttribute = {
	.uI2cmID		= MMPF_I2CM_ID_0,			//MMPF_I2CM_ID_0 ~ MMPF_I2CM_ID_2 stand for HW I2CM
	.ubSlaveAddr 	= 0x19,
	.ubRegLen		= 8,				//Indicate register as the 8 bit mode or 16 bit mode.
	.ubDataLen		= 16,				//Indicate data as the 8 bit mode or 16 bit mode.
	.ubDelayTime	= 0,			//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW or MMPF_I2CM_ID_SW_SENSOR)
   									//To Adjust the speed of software I2CM
	.bDelayWaitEn	= MMP_FALSE,  
	.bInputFilterEn	= MMP_FALSE,		//HW feature, to filter input noise
	.b10BitModeEn	= MMP_FALSE,			//HW I2CM supports 10 bit slave address, the bit8 and bit9 are in ubSlaveAddr1
	.bClkStretchEn	= MMP_FALSE, 		//HW support stretch clock
	.ubSlaveAddr1	= 0,
	.ubDelayCycle	= 0,  		//When bDelayWaitEn enable, set the delay cycle after each 8 bit transmission
	.ubPadNum		= 0,      		//HW pad map, the relate pad definition, please refer global register spec.
   									// ========= For Vision V2=========
									// If I2CM_ID = MMPF_I2CM_ID_0, ubPadNum = 0 stands for I2CM0_SCK = PHI2C_SCL
									//                              ubPadNum = 1 stands for I2CM0_SCK = PSNR_HSYNC
									// IF I2CM_ID = MMPF_I2CM_ID_1, only one pad(I2CM1_SCK = PSNR_SCK) is used   									
   									// ========= For Vision V2=========
	.uI2cmSpeed 	= MMPF_I2CM_SPEED_HW_250K, //HW I2CM speec control
	.SW_CLK_PIN 	= 0,  		//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW only), indicate the clock pin
	.SW_DAT_PIN		= 0,		
	.name 			= "VSNV3_PHI2CM",
	.deviceID		= 0


};
#endif


/*
 * Initialize the TWI hardware registers.
 */
static void __devinit ait_vsnv3_phi2c_hwinit(MMPF_I2CM_ATTRIBUTE *i2c_attr)
{
	MMPF_I2cm_DisableInterrupt(i2c_attr);

#if 0
	//MMPF_I2cm_Initialize(&m_PHI2CMAttribute );
//	MMPF_I2c_ReadData(&m_I2CMAttribute );
while(1)
{
	for(i=0;i<0x2a;++i)
	{
		MMP_BYTE usData ;
		int ret;
		usData = 0;
		ret = MMPF_I2cm_ReadReg(&m_PHI2CMAttribute, i, &usData);

	}
}
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
#if 0
static int ait_vsnv3_phi2c_xfer(struct i2c_adapter *adap, struct i2c_msg *pmsg, int num)
{
	int  ret=0;

	dev_dbg(&adap->dev, "ait_vsnv3_phi2c_xfer: processing %d messages:\n", num);
	{

	dev_dbg(&adap->dev, "%sing %d byte%s %s 0x%02x\n",
			pmsg->flags & I2C_M_RD ? "read" : "writ",
			pmsg->len, pmsg->len > 1 ? "s" : "",
			pmsg->flags & I2C_M_RD ? "from" : "to",	pmsg->addr);
		m_PHI2CMAttribute.ubSlaveAddr = pmsg->addr;
		
		if (pmsg->len && pmsg->buf) {	/* sanity check */
			if (pmsg->flags & I2C_M_RD)
			{
				MMP_USHORT reg, data;
				reg = pmsg->buf[0];
				ret = MMPF_I2cm_ReadReg(&m_PHI2CMAttribute, reg, &data);
				pmsg->buf[1] = data;
			}
			else
			{
				m_PHI2CMAttribute.ubDataLen = (pmsg->len-1)*8;
				if(pmsg->len ==3)
					ret = MMPF_I2cm_WriteReg(&m_PHI2CMAttribute, pmsg->buf[0], pmsg->buf[1] << 8 | pmsg->buf[2]);
				else if(pmsg->len ==2)
					ret = MMPF_I2cm_WriteReg(&m_PHI2CMAttribute, pmsg->buf[0], pmsg->buf[1]);
			}

			if(ret ==MMP_I2CM_ERR_SLAVE_NO_ACK )
				return -ENXIO;
			if (ret)
				return -ETIMEDOUT;

		}
		dev_dbg(&adap->dev, "transfer complete ret = %d\n",ret);

	}
	return ret;
}
#else

typedef struct _ait_i2c_adapter
{
	struct i2c_adapter i2c_adp;
	MMPF_I2CM_ATTRIBUTE i2c_attr;
	struct ait_i2c_extension i2c_ext;
}ait_i2c_adapter;
int i2cdev_ioctl_aitdelaycycle(struct i2c_adapter *adapter, unsigned long arg)
{
	//int datasize, res;

	ait_i2c_adapter* ait_adap = (ait_i2c_adapter*) dev_get_drvdata(&adapter->dev);
	MMPF_I2CM_ATTRIBUTE *i2c_attr = &ait_adap->i2c_attr;

	/* Note that command values are always valid! */

	if (arg > 0xff ||  arg < 0 ) {
		dev_dbg(&adapter->dev,
			"delay out of range (%x) in ioctl I2C_AIT.\n",
			arg);
		return -EINVAL;
	}else{

		//printk(KERN_ERR"data_arg:%04x\n",data_arg.byte);
		i2c_attr->ubDelayCycle = arg;
        i2c_attr->bDelayWaitEn = MMP_TRUE;
		//printk(KERN_ERR"i2c_attr->ubDelayCycle:%04x\n",i2c_attr->ubDelayCycle);
        }
	return 1;

}
EXPORT_SYMBOL(i2cdev_ioctl_aitdelaycycle);
int i2cdev_ioctl_aitdelaytime(struct i2c_adapter *adapter, unsigned long arg)
{
	//int datasize, res;

	ait_i2c_adapter* ait_adap = (ait_i2c_adapter*) dev_get_drvdata(&adapter->dev);
	MMPF_I2CM_ATTRIBUTE *i2c_attr = &ait_adap->i2c_attr;

	/* Note that command values are always valid! */

	if (arg > 0xff ||  arg < 0 ) {
		dev_dbg(&adapter->dev,
			"delay out of range (%x) in ioctl I2C_AIT.\n",
			arg);
		return -EINVAL;
	}else{
		i2c_attr->ubDelayTime=arg;
		printk(KERN_ERR"%s: i2c_attr->ubDelayTime:%04x\n",__FUNCTION__, i2c_attr->ubDelayTime);
        }
	return 1;

}
EXPORT_SYMBOL(i2cdev_ioctl_aitdelaytime);
int i2cdev_ioctl_aitclockctl(struct i2c_adapter *adapter, unsigned long arg)
{
    ait_i2c_adapter* ait_adap = (ait_i2c_adapter*) dev_get_drvdata(&adapter->dev);
    MMPF_I2CM_ATTRIBUTE *i2c_attr = &ait_adap->i2c_attr;
    MMP_USHORT  usSckDiv = 0;
    MMP_ULONG   ulSysGroupClk = 0;
    AITPS_I2CM  pI2CM = AITC_BASE_I2CM;
    ulSysGroupClk = 0x40740;
    if (arg > 0x190 ||  arg < 0 ) {

        dev_dbg(&adapter->dev,
            "i2c out of range (%x) in ioctl I2C_AIT.\n",
            arg);
        return -EINVAL;
    }else{
        //printk(KERN_ERR"SET CLOCK :%d\n",arg);
        /*arg= {0,0,10,20,30,...,100,150,200,250,...},check gI2CM_SPEED_CONTROL*/
        if((arg/10)==15)
        {
            i2c_attr->uI2cmSpeed=MMPF_I2CM_SPEED_HW_150K;
        }
        else if ((arg/10)==20)
        {
            i2c_attr->uI2cmSpeed=MMPF_I2CM_SPEED_HW_200K;

        }
        else if((arg/10)==25)
        {
            i2c_attr->uI2cmSpeed=MMPF_I2CM_SPEED_HW_250K;
        }else
            i2c_attr->uI2cmSpeed=arg/10+1;
        }
    return 1;   
}
EXPORT_SYMBOL(i2cdev_ioctl_aitclockctl);
#if 0
int AIT_I2cm_ReadReg_spinlock(ait_i2c_adapter *adap, MMP_USHORT usReg, MMP_USHORT *usData)
{
	if(adap->i2c_ext.i2c_hw_spinlock)
	{
		MMP_ERR r;
		unsigned long flags;
		spin_lock_irqsave(adap->i2c_ext.i2c_hw_spinlock, flags);
		r = MMPF_I2cm_ReadReg(&adap->i2c_attr,usReg,usData);
		spin_unlock_irqrestore(adap->i2c_ext.i2c_hw_spinlock,flags);
		return r;
	}else{
		return MMPF_I2cm_ReadReg(&adap->i2c_attr,usReg,usData);
	}
}

int	AIT_I2cm_WriteReg_spinlock(ait_i2c_adapter *adap, MMP_USHORT usReg, MMP_USHORT usData)
{
	if(adap->i2c_ext.i2c_hw_spinlock)
	{
		MMP_ERR r;
		unsigned long flags;
		spin_lock_irqsave(adap->i2c_ext.i2c_hw_spinlock, flags);
		r = MMPF_I2cm_WriteReg(&adap->i2c_attr,usReg,usData);
		spin_unlock_irqrestore(adap->i2c_ext.i2c_hw_spinlock,flags);
		return r;
	}else{
		return MMPF_I2cm_WriteReg(&adap->i2c_attr,usReg,usData);
	}
}
#endif

int AIT_I2cm_ReadReg_safe(ait_i2c_adapter *adap, MMP_USHORT usReg,MMP_ULONG *usData )//MMP_USHORT *usData)
{
	if(adap->i2c_ext.i2c_hw_lock)
	{
		MMP_ERR r;
		down(*adap->i2c_ext.i2c_hw_lock);
		r = MMPF_I2cm_ReadReg(&adap->i2c_attr,usReg,usData);
		up(*adap->i2c_ext.i2c_hw_lock);
		return r;
	}else{
		return MMPF_I2cm_ReadReg(&adap->i2c_attr,usReg,usData);
	}
}

int	AIT_I2cm_WriteReg_safe(ait_i2c_adapter *adap, MMP_USHORT usReg, MMP_USHORT usData)
{
	if(adap->i2c_ext.i2c_hw_lock)
	{
		MMP_ERR r;
		down(*adap->i2c_ext.i2c_hw_lock);
		r = MMPF_I2cm_WriteReg(&adap->i2c_attr,usReg,usData);
		up(*adap->i2c_ext.i2c_hw_lock);
		return r;
	}else{
		return MMPF_I2cm_WriteReg(&adap->i2c_attr,usReg,usData);
	}
}

static int ait_vsnv3_phi2c_xfer(struct i2c_adapter *adap, struct i2c_msg *pmsg, int num)
{
	int     ret=0;
    int     i = 0;


	dev_dbg(&adap->dev, "ait_vsnv3_phi2c_xfer: processing %d messages:\n", num);
	{

		ait_i2c_adapter* ait_adap = (ait_i2c_adapter*) dev_get_drvdata(&adap->dev);
		MMPF_I2CM_ATTRIBUTE *i2c_attr = &ait_adap->i2c_attr;


		dev_dbg(&adap->dev, "%sing %d byte%s %s 0x%02x\n",
			pmsg->flags & I2C_M_RD ? "read" : "write",
			pmsg->len, pmsg->len > 1 ? "s" : "",
			pmsg->flags & I2C_M_RD ? "from" : "to",	pmsg->addr);
			i2c_attr->ubSlaveAddr = pmsg->addr;
#if 0
            if(i2c_attr->ubSlaveAddr == 0x14) //N79E8341A
            {
                i2c_attr->bDelayWaitEn = MMP_TRUE;
                i2c_attr->ubDelayCycle = 4;
            }else{
                i2c_attr->bDelayWaitEn = MMP_FALSE;
                i2c_attr->ubDelayCycle = 0;
            }
#endif
		if(num>1)
		{
			int n=0;
			MMP_USHORT reg, data;

            //printk(KERN_ERR"read: num1=%d\n",num);
			if(pmsg[0].flags == 0)
			{
				reg = pmsg[0].buf[0];
				n=1;
			}else{
				printk(KERN_ERR"ait_vsnv3_phi2c_xfer unexpect case.\r\n");
				n=0;
				//return 0;
			}

			for(n;n<num;++n)
			{
				if (pmsg[n].flags & I2C_M_RD)
				{
					MMP_ULONG data;//MMP_USHORT data;
					//ret = MMPF_I2cm_ReadReg(&m_PHI2CMAttribute, reg, &data);
					//ret = AIT_I2cm_ReadReg_spinlock(ait_adap, reg, &data);
					
					ret = AIT_I2cm_ReadReg_safe(ait_adap, reg, &data);

					if(pmsg[n].len == 1 )
						pmsg[n].buf[0] = data & 0xFF;
					else if(pmsg[n].len == 2)
						*(MMP_USHORT*)pmsg[n].buf = data;
				}else{
					//m_PHI2CMAttribute.ubDataLen = (pmsg[n].len-1)*8;
					i2c_attr->ubDataLen = (pmsg[n].len-1)*8;
					if(pmsg->len ==3)
					{
						//ret = MMPF_I2cm_WriteReg(&m_PHI2CMAttribute, pmsg[n].buf[0], pmsg->buf[1] << 8 | pmsg->buf[2]);
						//ret = AIT_I2cm_WriteReg_spinlock(ait_adap, pmsg[n].buf[0], pmsg->buf[1] << 8 | pmsg->buf[2]);
						ret = AIT_I2cm_WriteReg_safe(ait_adap, pmsg[n].buf[0], pmsg->buf[1] << 8 | pmsg->buf[2]);
					}
					else if(pmsg->len ==2)
					{
						//ret = MMPF_I2cm_WriteReg(&m_PHI2CMAttribute, pmsg[n].buf[0], pmsg->buf[1]);
						//ret = AIT_I2cm_WriteReg_spinlock(ait_adap, pmsg[n].buf[0], pmsg->buf[1]);
						ret = AIT_I2cm_WriteReg_safe(ait_adap, pmsg[n].buf[0], pmsg->buf[1]);
					}
					else if(pmsg->len ==1)
					{
						//ret = MMPF_I2cm_WriteReg(&m_PHI2CMAttribute, pmsg[n].buf[0], pmsg->buf[1]);
						//ret = AIT_I2cm_WriteReg_spinlock(ait_adap, pmsg[n].buf[0], pmsg->buf[1]);
						ret = AIT_I2cm_WriteReg_safe(ait_adap, pmsg[n].buf[0], NULL);
					}
				}

				if(ret < 0)
				{
					if(ret ==MMP_I2CM_ERR_SLAVE_NO_ACK )
						return -ENXIO;
					if (ret)
						return -ETIMEDOUT;
				}
				else
					ret = n+1;
			}

		}else{
            //printk(KERN_ERR"read: num=%d\n",num);
			if (pmsg->len && pmsg->buf) {	/* sanity check */
				if (pmsg->flags & I2C_M_RD)
				{
					MMP_USHORT reg;
					MMP_ULONG data;
					i2c_attr->ubDataLen = (pmsg->len-1)*8;
					ait_adap->i2c_attr.ubDataLen = (pmsg->len-1)*8;
					reg = pmsg->buf[0];
                    //printk(KERN_ERR"pmsg r:%d->",pmsg->len);
                    //for(i = 0; i < pmsg->len; i++)
                    //    printk(KERN_ERR"%02x:",pmsg->buf[i]);
                    //printk(KERN_ERR"reg=%04X\n", reg);
					ret = AIT_I2cm_ReadReg_safe(ait_adap, reg, &data);
					if(pmsg->len == 1 )
						pmsg->buf[0] = data & 0xFF;
					else if(pmsg->len == 2){
						*(MMP_USHORT*)pmsg->buf = data;
                                                pmsg->buf[1]=pmsg->buf[0];
                                                pmsg->buf[0]=reg;
					}
					else if(pmsg->len == 3){
						*(MMP_ULONG*)pmsg->buf = data;
						pmsg->buf[2]=pmsg->buf[1];
						pmsg->buf[1]=pmsg->buf[0];
						pmsg->buf[0]=reg;
					}
					else if(pmsg->len == 4){
						*(MMP_ULONG*)pmsg->buf = data;
						pmsg->buf[3]=pmsg->buf[2];
						pmsg->buf[2]=pmsg->buf[1];
						pmsg->buf[1]=pmsg->buf[0];
						pmsg->buf[0]=reg;

					}

				}
				else
				{
                    //printk(KERN_ERR"pmsg w:%d->",pmsg->len);
                    //printk(KERN_ERR"reg=%04X\n", pmsg->buf[0]);
					i2c_attr->ubDataLen = (pmsg->len-1)*8;
					if(pmsg->len ==3)
					{
						ret = AIT_I2cm_WriteReg_safe(ait_adap, pmsg->buf[0], pmsg->buf[1] << 8 | pmsg->buf[2]);
					}
					else if(pmsg->len ==2)
					{
						ret = AIT_I2cm_WriteReg_safe(ait_adap, pmsg->buf[0], pmsg->buf[1]);
					}
					else if(pmsg->len ==1)
					{
						ret = AIT_I2cm_WriteReg_safe(ait_adap, pmsg->buf[0], NULL);
					}
				}

				if(ret ==MMP_I2CM_ERR_SLAVE_NO_ACK )
					return -ENXIO;
				if (ret)
					return -ETIMEDOUT;
                ///printk(KERN_ERR"----------\n");
				ret = 1;
			}
		}

		//printk(KERN_ERR"transfer complete ret = %d ----\n",ret);
		//dev_dbg(&adap->dev, "transfer complete ret = %d\n",ret);

	}
	return ret;
}
#endif
/*
 * Return list of supported functionality.
 */
static u32  ait_vsnv3_phi2c_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm ait_vsnv3_phi2c_algorithm = {
	.master_xfer	= ait_vsnv3_phi2c_xfer,
	.functionality	= ait_vsnv3_phi2c_func,
};

/*
 * Main initialization routine.
 */
static int __devinit  ait_vsnv3_phi2c_probe(struct platform_device *pdev)	//at91sam9260_twi_device
{
	struct i2c_adapter *adapter;
	ait_i2c_adapter *ait_adapter;
	MMPF_I2CM_ATTRIBUTE *i2c_attr;
	struct resource *res;
	int rc;
	struct ait_i2c_extension* i2c_ext;
	//AITPS_GBL pGBL = AITC_BASE_GBL;
	//pGBL->GBL_CLK_DIS0 &=~GBL_CLK_AUD_DIS;
	//pGBL->GBL_CLK_DIS2 &= (~GBL_CLK_AUD_CODEC_DIS);

	i2c_ext = (struct ait_i2c_extension*) pdev->dev.platform_data;
#if 0	//to remove resource chack is just a tempoary solution for one i2c bus hw engine used for two i2c bus driver.
		//the best solution is to make a i2c engine driver to avoid resource conflict
	// io mem mapping
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -ENXIO;

	if (!request_mem_region(res->start, resource_size(res), "VsionV3_phi2c"))
		return -EBUSY;

	twi_base = ioremap(res->start, resource_size(res));
	if (!twi_base) {
		rc = -ENOMEM;
		goto fail0;
	}
#endif
#if 0
	if(i2c_ext->uI2cmID == MMPF_I2CM_ID_0)
	{
		i2cm_clk = clk_get(NULL, "i2c_clk");
	}else
#endif	
		i2cm_clk = clk_get(NULL, "i2c_clk");

	if (IS_ERR(i2cm_clk)) {
		dev_err(&pdev->dev, "no clock defined\n");
		rc = -ENODEV;
		goto fail1;
	}

	//adapter = kzalloc(sizeof(struct i2c_adapter), GFP_KERNEL);
	ait_adapter = kzalloc(sizeof(ait_i2c_adapter), GFP_KERNEL);
	if (ait_adapter == NULL) {
		dev_err(&pdev->dev, "can't allocate inteface, kzalloc failed!\n");
		rc = -ENOMEM;
		goto fail2;
	}
	adapter = &(ait_adapter->i2c_adp);
	i2c_attr = &(ait_adapter->i2c_attr);
	//

	i2c_attr->uI2cmID 		= i2c_ext->uI2cmID;			//MMPF_I2CM_ID_0 ~ MMPF_I2CM_ID_2 stand for HW I2CM
	i2c_attr->ubSlaveAddr 	= 0;
	i2c_attr->ubRegLen		= 8;				//Indicate register as the 8 bit mode or 16 bit mode.
	i2c_attr->ubDataLen		= 16;				//Indicate data as the 8 bit mode or 16 bit mode.
	i2c_attr->ubDelayTime	= 0;			//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW or MMPF_I2CM_ID_SW_SENSOR)
	   									//To Adjust the speed of software I2CM
	i2c_attr->bDelayWaitEn	= MMP_FALSE;
	i2c_attr->bInputFilterEn= MMP_FALSE;		//HW feature, to filter input noise
	i2c_attr->b10BitModeEn	= MMP_FALSE;			//HW I2CM supports 10 bit slave address, the bit8 and bit9 are in ubSlaveAddr1
	i2c_attr->bClkStretchEn	= MMP_FALSE; 		//HW support stretch clock
	i2c_attr->ubSlaveAddr1	= 0;
	i2c_attr->ubDelayCycle	= 0;  				//When bDelayWaitEn enable, set the delay cycle after each 8 bit transmission
	i2c_attr->ubPadNum		= i2c_ext->pad;      		//HW pad map, the relate pad definition, please refer global register spec.
	   									// ========= For Vision V2=========
										// If I2CM_ID = MMPF_I2CM_ID_0, ubPadNum = 0 stands for I2CM0_SCK = PHI2C_SCL
										//                              ubPadNum = 1 stands for I2CM0_SCK = PSNR_HSYNC
										// IF I2CM_ID = MMPF_I2CM_ID_1, only one pad(I2CM1_SCK = PSNR_SCK) is used
	   									// ========= For Vision V2=========
	i2c_attr->uI2cmSpeed 	= MMPF_I2CM_SPEED_HW_250K; //HW I2CM speec control
	i2c_attr->SW_CLK_PIN 	= 0;  		//Used in SW I2CM (uI2cmID = MMPF_I2CM_ID_SW only), indicate the clock pin
	i2c_attr->SW_DAT_PIN	= 0;
	i2c_attr->name 			= 0;
	i2c_attr->deviceID		= 0;

	ait_adapter->i2c_ext.uI2cmID = i2c_ext->uI2cmID;
	ait_adapter->i2c_ext.pad = i2c_ext->pad;
	//ait_adapter->i2c_ext.i2c_hw_spinlock = i2c_ext->i2c_hw_spinlock;
	ait_adapter->i2c_ext.i2c_hw_lock = i2c_ext->i2c_hw_lock;
	//

	//snprintf(adapter->name, sizeof(adapter->name), "VisionV3_PHI2C_Adapter");
	snprintf(adapter->name, sizeof(adapter->name), "VisionV3_I2C_%d_pad%d",i2c_ext->uI2cmID,i2c_ext->pad);
	printk("probe i2c bus %s \r\n",adapter->name);

	adapter->algo = &ait_vsnv3_phi2c_algorithm;
	adapter->class = I2C_CLASS_HWMON;
	adapter->dev.parent = &pdev->dev;
	/* adapter->id == 0 ... only one TWI controller for now */

//	clk_enable(i2cm_clk);		/* enable peripheral clock */
	ait_vsnv3_phi2c_hwinit(i2c_attr);		/* initialize TWI controller */

	//adapter->nr = 1;
	adapter->nr = pdev->id;

	rc = i2c_add_numbered_adapter(adapter);
	if (rc) {
		dev_err(&pdev->dev, "Adapter %s registration failed\n",
				adapter->name);
		goto fail3;
	}

	platform_set_drvdata(pdev, (void*)ait_adapter);
	dev_set_drvdata(&adapter->dev, (void*)ait_adapter);

	dev_info(&pdev->dev, "VSNV3 i2c bus driver.\n");
	return 0;

fail3:
	platform_set_drvdata(pdev, NULL);
	kfree(ait_adapter);
///	clk_disable(twi_clk);
fail2:
	clk_put(i2cm_clk);
fail1:
//Vin	iounmap(twi_base);
fail0:
//Vin	release_mem_region(res->start, resource_size(res));

	return rc;
}

static int __devexit  ait_vsnv3_phi2c_remove(struct platform_device *pdev)
{
	struct i2c_adapter *adapter = platform_get_drvdata(pdev);
//	struct resource *res;
	int rc;

	rc = i2c_del_adapter(adapter);
	platform_set_drvdata(pdev, NULL);
#if 0
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	iounmap(twi_base);
	release_mem_region(res->start, resource_size(res));
#endif
//	clk_disable(twi_clk);		/* disable peripheral clock */
//	clk_put(twi_clk);

	return rc;
}

#ifdef CONFIG_PM

/* NOTE: could save a few mA by keeping clock off outside of at91_xfer... */

static int  ait_vsnv3_phi2c_suspend(struct platform_device *pdev, pm_message_t mesg)
{
//	clk_disable(twi_clk);
	return 0;
}

static int  ait_vsnv3_phi2c_resume(struct platform_device *pdev)
{
	return 0;//clk_enable(twi_clk);
}

#else
#define at91_i2c_suspend	NULL
#define at91_i2c_resume		NULL
#endif

/* work with "modprobe at91_i2c" from hotplugging or coldplugging */
MODULE_ALIAS("platform:VsionV3_i2c_sw");

static struct platform_driver  ait_vsnv3_phi2c_driver = {
	.probe		= ait_vsnv3_phi2c_probe,
	.remove		= __devexit_p(ait_vsnv3_phi2c_remove),
	.suspend		= ait_vsnv3_phi2c_suspend,
	.resume		= ait_vsnv3_phi2c_resume,
	.driver		= {
		//.name	= "VsionV3_phi2c",
		.name = "VSNV3_I2C",
		.owner	= THIS_MODULE,
	},
};

static int __init  ait_vsnv3_phi2c_init(void)
{
	return platform_driver_register(&ait_vsnv3_phi2c_driver );
}

static void __exit  ait_vsnv3_phi2c_exit(void)
{
	platform_driver_unregister(&ait_vsnv3_phi2c_driver );
}

module_init(ait_vsnv3_phi2c_init);
module_exit(ait_vsnv3_phi2c_exit);

MODULE_AUTHOR("Vincent Chen");
MODULE_DESCRIPTION("I2CM driver for AIT Vision V3");
//MODULE_LICENSE("GPL");
