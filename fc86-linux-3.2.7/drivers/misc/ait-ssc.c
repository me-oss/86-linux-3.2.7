/*
 * AIT SSC driver
 * Modify from drivers/misc/ait-ssc.c
 * Copyright (C) 2013 Alpha Image Corporation
 * Copyright (C) 2007 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
 //Vin
//#define DEBUG

#include <linux/platform_device.h>
#include <linux/list.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/atmel-ssc.h>
#include <linux/slab.h>
#include <linux/module.h>

#include <mach/mmp_reg_audio.h>

/* Serialize access to ssc_list and user count */
static DEFINE_SPINLOCK(user_lock);
static LIST_HEAD(ssc_list);

struct ssc_device *ssc_request(unsigned int ssc_num)
{
	int ssc_valid = 0;
	struct ssc_device *ssc;
	pr_debug( "%s: ssc_num = %d\n",__FUNCTION__,ssc_num);				

	spin_lock(&user_lock);
	list_for_each_entry(ssc, &ssc_list, list) {
		if (ssc->pdev->id == ssc_num) {
			ssc_valid = 1;
			break;
		}
	}

	if (!ssc_valid) {
		spin_unlock(&user_lock);
		pr_err("ssc: ssc%d platform device is missing\n", ssc_num);
		return ERR_PTR(-ENODEV);
	}

	if (ssc->user) {
		spin_unlock(&user_lock);
		dev_dbg(&ssc->pdev->dev, "module busy\n");
		return ERR_PTR(-EBUSY);
	}

	ssc->user++;
				
	spin_unlock(&user_lock);

	clk_enable(ssc->clk);

	return ssc;
}
EXPORT_SYMBOL(ssc_request);

void ssc_free(struct ssc_device *ssc)
{
	spin_lock(&user_lock);
	if (ssc->user) {
		ssc->user--;
		clk_disable(ssc->clk);
	} else {
		dev_dbg(&ssc->pdev->dev, "device already free\n");
	}
	spin_unlock(&user_lock);
}
EXPORT_SYMBOL(ssc_free);

static int __init ssc_probe(struct platform_device *pdev)
{
	int retval = 0;
	struct resource *regs;
	struct ssc_device *ssc;

	pr_debug( "%s: %s \n",__FUNCTION__,pdev->name);

	ssc = kzalloc(sizeof(struct ssc_device), GFP_KERNEL);
	if (!ssc) {
		dev_dbg(&pdev->dev, "out of memory\n");
		retval = -ENOMEM;
		goto out;
	}
#if 1
	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs) {
		dev_dbg(&pdev->dev, "no mmio resource defined\n");
		retval = -ENXIO;
		goto out_free;
	}
#endif
	ssc->clk = clk_get(&pdev->dev, "pclk");
	if (IS_ERR(ssc->clk)) {
		dev_dbg(&pdev->dev, "no pclk clock defined\n");
		retval = -ENXIO;
		goto out_free;
	}

	ssc->pdev = pdev;
	
	ssc->regs = ioremap(regs->start, resource_size(regs));
	if (!ssc->regs) {
		dev_dbg(&pdev->dev, "ioremap failed\n");
		retval = -EINVAL;
		goto out_clk;
	}

	dev_dbg(&pdev->dev, "reg addr = 0x%x\n",ssc->regs);

	/* disable all interrupts */
	clk_enable(ssc->clk);
	//ssc_writel(ssc->regs, IDR, ~0UL);

	if(ssc->regs==AITC_BASE_AUD)
	{
		i2sreg_writeb(ssc->regs,I2S_CPU_INT_EN,0);
		i2sreg_writeb(ssc->regs,I2S_FIFO_SR,i2sreg_readb(ssc->regs,I2S_FIFO_SR));
		i2sreg_writeb(ssc->regs,I2S_INT_CSR,i2sreg_readb(ssc->regs,I2S_INT_CSR));	
	}

	//ssc_readl(ssc->regs, SR);
	clk_disable(ssc->clk);

	ssc->irq = platform_get_irq(pdev, 0);
	if (!ssc->irq) {
		dev_dbg(&pdev->dev, "could not get irq\n");
		retval = -ENXIO;
		goto out_unmap;
	}

	spin_lock(&user_lock);
	list_add_tail(&ssc->list, &ssc_list);
	spin_unlock(&user_lock);

	platform_set_drvdata(pdev, ssc);

	dev_info(&pdev->dev, "AIT Audio device at 0x%p (irq %d)\n",
			ssc->regs, ssc->irq);

	goto out;

out_unmap:
	iounmap(ssc->regs);
out_clk:
	clk_put(ssc->clk);
out_free:
	kfree(ssc);
out:
	return retval;
}

static int __devexit ssc_remove(struct platform_device *pdev)
{
	struct ssc_device *ssc = platform_get_drvdata(pdev);

	spin_lock(&user_lock);
	iounmap(ssc->regs);
	clk_put(ssc->clk);
	list_del(&ssc->list);
	kfree(ssc);
	spin_unlock(&user_lock);

	return 0;
}

static struct platform_driver ssc_driver = {
	.remove		= __devexit_p(ssc_remove),
	.driver		= {
		.name		= "vsnv3aud",
		.owner		= THIS_MODULE,
	},
};

static int __init ssc_init(void)
{
	return platform_driver_probe(&ssc_driver, ssc_probe);
}
module_init(ssc_init);

static void __exit ssc_exit(void)
{
	platform_driver_unregister(&ssc_driver);
}
module_exit(ssc_exit);

//MODULE_AUTHOR("Hans-Christian Egtvedt <hcegtvedt@atmel.com>");
MODULE_DESCRIPTION("Audio driver for AIT VSNV3");
//MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:vsnv3 audio");
