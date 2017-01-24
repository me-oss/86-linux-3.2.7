/*
 * Copyright (C) 2007 Atmel Corporation.
 * Copyright (C) 2011 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 * Copyright (C) 2013 Alpha Image Corporation.
 *
 * Under GPLv2
 */

#include <linux/module.h>
#include <linux/io.h>
#include <linux/mm.h>

#include <asm/mach/map.h>

#include <mach/hardware.h>
#include <mach/cpu.h>

#include "soc.h"
#include "generic.h"
#include "asm/setup.h"

struct ait_init_soc __initdata vsnv3_boot_soc;

struct ait_socinfo vsnv3_soc_initdata;
EXPORT_SYMBOL(vsnv3_soc_initdata);

void __init vsnv3_set_type(int type)
{
#if 0
	if (type == ARCH_REVISON_9200_PQFP)
		at91_soc_initdata.subtype = AT91_SOC_RM9200_PQFP;
	else
		at91_soc_initdata.subtype = AT91_SOC_RM9200_BGA;

	pr_info("AT91: filled in soc subtype: %s\n",
		at91_get_soc_subtype(&at91_soc_initdata));
#endif	
}

void __init vsnv3_init_irq_default(void)
{
	vsnv3_init_interrupts(vsnv3_boot_soc.default_irq_priority);
}

void __init vsnv3_init_interrupts(unsigned int *priority)
{
	/* Initialize the AIC interrupt controller */
	vsnv3_aic_init(priority);

	vsnv3_gpio_irq_setup();
}

static struct map_desc sram_desc[2] __initdata;

void __init vsnv3_init_sram(int bank, unsigned long base, unsigned int length)
{
	struct map_desc *desc = &sram_desc[bank];

	desc->virtual = AIT_OPR_VIRT_BASE - length;
	if (bank > 0)
		desc->virtual -= sram_desc[bank - 1].length;

	desc->pfn = __phys_to_pfn(base);
	desc->length = length;
	desc->type = MT_DEVICE;

	pr_info("AIT: sram at 0x%lx of 0x%x mapped at 0x%lx\n",
		base, length, desc->virtual);

	iotable_init(desc, 1);
}

	/* Reserver for H264 Encode */
static struct map_desc video_dram_desc __initdata;

void __init vsnv3_init_video_dram(int bank, unsigned long base, unsigned int length)
{
	struct map_desc *desc = &video_dram_desc;

	desc->virtual = AIT_RAM_P2V(base);
	desc->pfn = __phys_to_pfn(base),
	desc->length = length;
	desc->type = MT_DEVICE;

	pr_info("AIT: Video DRAM at 0x%lx of 0x%x mapped at 0x%lx\n",
		base, length, desc->virtual);

	iotable_init(desc, 1);
}



static struct map_desc vsnv3_io_desc[] __initdata = {
	{
	.virtual	= AIT_OPR_VIRT_BASE,
	.pfn		= __phys_to_pfn(0x80000000),
	.length	= SZ_64K,//AIT8455_OPR_VIRT_SIZE ,
	.type	= MT_DEVICE,//MT_DEVICE,
	},

	{
	.virtual	= AIT_SRAM_VIRT_BASE,
	.pfn		= __phys_to_pfn(AIT_SRAM_PHYS_BASE),
	.length	    = AIT_SRAM_SIZE,
	.type	    = MT_DEVICE,
	},

#if 0///julian dbg only, temp solution
	{
	.virtual	= AIT_DRAM_VIRT_BASE,
	.pfn		= __phys_to_pfn(AIT_DRAM_PHYS_BASE),
	.length	    = AIT_DRAM_SIZE,
	.type	    = MT_DEVICE,
	},
#endif

	{
	.virtual	= AT91_VA_BASE_SYS,
	.pfn		= __phys_to_pfn(AT91_BASE_SYS),
	.length		= SZ_128K,//SZ_16K,
	.type		= MT_DEVICE,
	}
};

void __iomem *vsnv3_ioremap(unsigned long p, size_t size, unsigned int type)
{
	pr_debug("vsnv3_ioremap:0x%08x  size: %d\n",(unsigned int)p,size);

	if (p >= AT91_BASE_SYS && p <= (AT91_BASE_SYS + SZ_16K - 1))
		return (void __iomem *)AT91_IO_P2V(p);

	if (p >= AIT_OPR_PHYS_BASE && p <= (AIT_OPR_PHYS_BASE + AIT_OPR_PHYS_SIZE - 1))
		return (void __iomem *)AIT_OPR_P2V(p);

//	if (p >= AIT_SRAM_PHYS_BASE && p <= (AIT_SRAM_PHYS_BASE + AIT_SRAM_SIZE - 1))
//		return (void __iomem *)AIT_RAM_P2V(p);
	
	return __arm_ioremap_caller(p, size, type, __builtin_return_address(0));
}
EXPORT_SYMBOL(vsnv3_ioremap);

void vsnv3_iounmap(volatile void __iomem *addr)
{
	unsigned long virt = (unsigned long)addr;

	if (virt >= VMALLOC_START && virt < VMALLOC_END)
		__iounmap(addr);
}
EXPORT_SYMBOL(vsnv3_iounmap);

#define AT91_DBGU0	0xfffff200
#define AT91_DBGU1	0xffffee00

static void __init soc_detect(u32 dbgu_base)
{
	u32 cidr, socid;
	

/*Vin@ Force to SAM9260 setting*/
	socid = ARCH_ID_AT91SAM9260;
	vsnv3_soc_initdata.type = AIT_SOC_VSN_V3;
	vsnv3_boot_soc = vsnv3ait84_soc ;

	if (!vsnv3_soc_is_detected())
		return;

	vsnv3_soc_initdata.cidr = cidr;

}

static const char *soc_name[] = {
	
	[AIT_SOC_VSN_V3]	= "ait_vsnv3",

	[AIT_SOC_NONE]		= "Unknown"
};

const char *vsnv3_get_soc_type(struct ait_socinfo *c)
{
	return soc_name[c->type];
}
EXPORT_SYMBOL(vsnv3_get_soc_type);

static const char *soc_subtype_name[] = {

	[AIT_VSN_V3_8453]	= "ait_vsnv3_8453",
	[AIT_VSN_V3_8455]	= "ait_vsnv3_8455",
	[AIT_SOC_SUBTYPE_NONE]	= "Unknown"
};
const char *vsnv3_get_soc_subtype(struct ait_socinfo *c)
{
	return soc_subtype_name[c->subtype];
}
EXPORT_SYMBOL(vsnv3_get_soc_subtype);
extern struct meminfo meminfo;


void __init vsnv3_map_io(void)
{
      unsigned long DramSize = AIT_DRAM_SIZE;

	/* Map peripherals */
/* Reserver for H264 Encode */

	pr_err("PHY mem start 0x%x\n",(unsigned int)meminfo.bank[0].size);
	
	//vsnv3_init_video_dram(0, (meminfo.bank[0].size)+CONFIG_PHYS_OFFSET, AIT_DRAM_SIZE);	

	DramSize = (64 * 1024 * 1024 - meminfo.bank[0].size);  // calculate memory size for video driver
	vsnv3_init_video_dram(0, (meminfo.bank[0].size)+CONFIG_PHYS_OFFSET, DramSize);	
	printk(KERN_ERR"%s,%s,%d Debug for kernel panic ",__FILE__,__func__,__LINE__);


	iotable_init(&vsnv3_io_desc, ARRAY_SIZE(vsnv3_io_desc));

	vsnv3_soc_initdata.type = AIT_SOC_NONE;
	vsnv3_soc_initdata.subtype = AIT_SOC_SUBTYPE_NONE;

	soc_detect(AT91_DBGU0);
	if (!vsnv3_soc_is_detected())
		soc_detect(AT91_DBGU1);

	if (!vsnv3_soc_is_detected())
		panic("AIT: Impossible to detect the SOC type");

	pr_info("AIT: Detected soc type: %s\n",
		vsnv3_get_soc_type(&vsnv3_soc_initdata));
	pr_info("AIT: Detected soc subtype: %s\n",
		vsnv3_get_soc_subtype(&vsnv3_soc_initdata));

	if (!vsnv3_soc_is_enabled())
		panic("AIT: Soc not enabled");

	if (vsnv3_boot_soc.map_io)
		vsnv3_boot_soc.map_io();
}

void __init vsnv3_initialize(unsigned long main_clock)
{

	/* Init clock subsystem */
	vsnv3_clock_init(main_clock);

	/* Register the processor-specific clocks */
	vsnv3_boot_soc.register_clocks();

	vsnv3_boot_soc.init();

}
