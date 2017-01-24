/*
 * linux/arch/arm/mach-at91/gpio_file.c
 *
 * Copyright (C) 2005 HP Labs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include <mach/hardware.h>

#include "generic.h"
#include <mach/mmpf_pio.h>

#include <linux/io.h>


#include <asm/irq.h>
#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/uaccess.h>	/* copy_*_user */

#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>

#include <mach/ait_if.h>

#define AITGPIO_IOCTL_GET_DIRECTION 0x19761020
#define AITGPIO_IOCTL_SET_DIRECTION 0x19761021
#define AITGPIO_IOCTL_GET_DATA		0x19761022
#define AITGPIO_IOCTL_SET_DATA		0x19761023
#define AITGPIO_IOCTL_SET_ISPIRMODE 0x19761024

#define AITGPIO_INPUT_MODE			0
#define AITGPIO_OUTPUT_MODE			1

#define PCGPIO16		((AIT_REG_B*)	AIT8455_OPR_P2V(0x80005DDA))
#define PAGPIO02		((AIT_REG_B*)	AIT8455_OPR_P2V(0x800069A7))
#define PAGPIO03		((AIT_REG_B*)	AIT8455_OPR_P2V(0x800069A8))
#define PAGPIO04		((AIT_REG_B*)	AIT8455_OPR_P2V(0x800069A9))
#define PAGPIO05		((AIT_REG_B*)	AIT8455_OPR_P2V(0x800069AA))

#define PCGPIO06		((AIT_REG_B*)	AIT8455_OPR_P2V(0x80005DD0))
#define PCGPIO07		((AIT_REG_B*)	AIT8455_OPR_P2V(0x80005DD1))
#define PCGPIO08		((AIT_REG_B*)	AIT8455_OPR_P2V(0x80005DD2))



struct aitgpio_dev {
	struct semaphore	sem0;
	struct cdev			cdev;	// char device structure
};

struct aitGPIO_CTL {
	unsigned int port;
	unsigned int data;
};

static int aitgpio_major = 0;
static int aitgpio_minor = 0;

static struct aitgpio_dev aitgpio_device;

static int aitgpio_get_direction(unsigned int port, unsigned int *direction);
static int aitgpio_set_direction(unsigned int port, unsigned int direction);
static int aitgpio_get_data(unsigned int port, unsigned int *data);
static int aitgpio_set_data(unsigned int port, unsigned int data);

static int aitgpio_open (struct inode * inode, struct file * filp);
static int aitgpio_release (struct inode * inode, struct file * filp);
static long aitgpio_ioctl (struct file * filp, unsigned int cmd, unsigned long arg);

static void aitgpio_exit (void);
static int __init aitgpio_init (void);

struct file_operations aitgpio_fops = {
	.owner =    		THIS_MODULE,
	.unlocked_ioctl =	aitgpio_ioctl,
	.open = 			aitgpio_open,
	.release =			aitgpio_release,
};

static int aitgpio_get_direction(unsigned int port, unsigned int *data)
{
	MMP_ERR error = 0;

	//down(&aitgpio_device.sem0);	

	error = MMPF_PIO_GetOutputMode((MMPF_PIO_REG) port, (MMP_UBYTE *)data);

	//up(&aitgpio_device.sem0);

	//pr_info("port:%d data:%d\n", port, *data);

	return error;
}

static int aitgpio_set_direction(unsigned int port, unsigned int data)
{
	MMP_ERR error = 0;

	//down(&aitgpio_device.sem0);

	error = MMPF_PIO_EnableGpioMode((MMPF_PIO_REG) port, MMP_TRUE);
	
	error = MMPF_PIO_EnableOutputMode((MMPF_PIO_REG) port, (MMP_BOOL)data);

	//up(&aitgpio_device.sem0);
	//pr_info("port:%d data:%d\n", port, data);

	return error;
}

static int aitgpio_get_data(unsigned int port, unsigned int *data)
{
	MMP_ERR error = 0;

	//down(&aitgpio_device.sem0);
	
	error = MMPF_PIO_GetData((MMPF_PIO_REG) port, (MMP_UBYTE *)data);

	//up(&aitgpio_device.sem0);
	//pr_info("port:%d data:%d\n", port, *data);

	return error;
}

static int aitgpio_set_data(unsigned int port, unsigned int data)
{
	MMP_ERR error = 0;

	//down(&aitgpio_device.sem0);

	error = MMPF_PIO_SetData((MMPF_PIO_REG) port, (MMP_BOOL)data);

	//up(&aitgpio_device.sem0);

	//pr_info("port:%d data:%d\n", port, data);

	return error;
}

static int aitgpio_open (struct inode * inode, struct file * filp)
{
	int	result = 0;
	struct aitgpio_dev *dev; 

	dev = container_of(inode->i_cdev, struct aitgpio_dev, cdev);
	filp->private_data = dev;

	return (result);
}

static int aitgpio_release (struct inode * inode, struct file * filp)
{
	return (0);
}

static long aitgpio_ioctl (struct file * filp, unsigned int cmd, unsigned long arg)
{
	int error = 0;

	struct aitGPIO_CTL gpioCtl;

	switch (cmd) {
		case AITGPIO_IOCTL_GET_DIRECTION:
			error = copy_from_user(&gpioCtl, (void __user *)arg, sizeof(struct aitGPIO_CTL));
			if (error) {
				error = -EIO;
				break;
			}
			error = aitgpio_get_direction(gpioCtl.port, &gpioCtl.data);
			if (error) {
				break;
			}
			error = copy_to_user((void __user *)arg, &gpioCtl, sizeof(struct aitGPIO_CTL));
			if (error) {
				error = -EIO;
				break;
			}
			break;
		case AITGPIO_IOCTL_SET_DIRECTION:
			error = copy_from_user(&gpioCtl, (void __user *)arg, sizeof(struct aitGPIO_CTL));
			if (error) {
				error = -EIO;
				break;
			}
			error = aitgpio_set_direction(gpioCtl.port, gpioCtl.data);
			break; 
		case AITGPIO_IOCTL_GET_DATA:
			error = copy_from_user(&gpioCtl, (void __user *)arg, sizeof(struct aitGPIO_CTL));
			if (error) {
				error = -EIO;
				break;
			}
			error = aitgpio_get_data(gpioCtl.port, &gpioCtl.data);
			if (error) {
				break;
			}
			error = copy_to_user((void __user *)arg, &gpioCtl, sizeof(struct aitGPIO_CTL));
			if (error) {
				error = -EIO;
				break;
			}
			break; 
		case AITGPIO_IOCTL_SET_DATA:
			error = copy_from_user(&gpioCtl, (void __user *)arg, sizeof(struct aitGPIO_CTL));
			if (error) {
				error = -EIO;
				break;
			}
			error = aitgpio_set_data(gpioCtl.port, gpioCtl.data);
			break;
		case AITGPIO_IOCTL_SET_ISPIRMODE:
			error = copy_from_user(&gpioCtl, (void __user *)arg, sizeof(struct aitGPIO_CTL));
			struct ait_camif_ir_ctrl ir_ctl;
			ir_ctl.ir_on = gpioCtl.data;
			error = ait_param_ctrl(AIT_PARAM_CAM_IR_S_CTRL, &ir_ctl);
			if (error) {
				error = -EIO;
				break;
			}
			break;
	}

	return error;
}

static int __init aitgpio_init (void)
{
	int		result = 0;
	dev_t	dev = 0;
#if 0
	pr_info("Loading and setting up ait8455 GPIO ...\n");


	result = alloc_chrdev_region(&dev, 0, aitgpio_minor, "aitgpio");
	aitgpio_major = MAJOR(dev);

	if (result < 0) {
		pr_info("aitgpio: can't get major %d\n", aitgpio_major);
		goto FUNC_EXIT;
	}

	// Initialize the device
	cdev_init(&aitgpio_device.cdev, &aitgpio_fops);
	aitgpio_device.cdev.owner = THIS_MODULE;
	aitgpio_device.cdev.ops = &aitgpio_fops;
	//mutex_init(&aitgpio_device.sem0);
	result = cdev_add(&aitgpio_device.cdev,MKDEV(aitgpio_major ,aitgpio_minor), 1);
	if (result != 0) {
		pr_info("aitgpio: error %d adding aitgpio device\n", result);
		goto FUNC_EXIT;
	}
#endif
	*PCGPIO16 &= ~(0x02);	//disable pull down
	*PCGPIO16 |= (0x04);	//enable pull up
#if 0	
	*PAGPIO04 &= ~(0x02);	//disable pull down
	*PAGPIO04 |= (0x04);	//enable pull up
	*PAGPIO03 &= ~(0x02);	//disable pull down
	*PAGPIO03 &= ~(0x04);	//enable pull up	

	*PAGPIO05 &= ~(0x02);	//disable pull down
	*PAGPIO05 &= ~(0x04);	//enable pull up	
#endif
	*PCGPIO08 &= ~(0x02);	//disable pull down
	*PCGPIO08 &= ~(0x04);	//enable pull up	
	

	*PCGPIO06 &= ~(0x02);	//disable pull down
	*PCGPIO06 |= (0x04);	//enable pull up

	*PCGPIO07 &= ~(0x02);	//disable pull down
	*PCGPIO07 |= (0x04);	//enable pull up
	
	*PAGPIO02 &= ~(0x02);	//disable pull down

//	aitgpio_set_direction(3, AITGPIO_INPUT_MODE);//GPIO3(HOSTAP, WPS) input mode
//	aitgpio_set_direction(4, AITGPIO_INPUT_MODE);//GPIO4(RESET TO DEFAULT) input mode
	aitgpio_set_direction(58, AITGPIO_INPUT_MODE);//GPIO58(SD DETECT) input mode

FUNC_EXIT:
	if (result != 0) {
		aitgpio_exit();
	}
	return (result);
}

static void aitgpio_exit (void)
{
	dev_t devno = MKDEV(aitgpio_major, aitgpio_minor);

	cdev_del(&aitgpio_device.cdev);

	unregister_chrdev_region(devno,1);
}

module_init(aitgpio_init);
module_exit(aitgpio_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("AIT 8455 GPIO file driver");

