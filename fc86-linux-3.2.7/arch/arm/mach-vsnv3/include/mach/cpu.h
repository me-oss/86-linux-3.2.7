/*
 * arch/arm/mach-vsnv3/include/mach/cpu.h
 *
 * Copyright (C) 2006 SAN People
 * Copyright (C) 2011 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
  * Copyright (C) 2013 AIT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef __MACH_CPU_H__
#define __MACH_CPU_H__

#define ARCH_ID_AT91RM9200	0x09290780
#define ARCH_ID_AT91SAM9260	0x019803a0


enum ait_soc_type {
	AIT_SOC_VSN_V3,

	/* SAM92xx */
	//AT91_SOC_SAM9260,
	
	/* Unknown type */
	AIT_SOC_NONE
};

enum ait_soc_subtype {
	AIT_VSN_V3_8453,
	AIT_VSN_V3_8455,		
	/* Unknown subtype */
	AIT_SOC_SUBTYPE_NONE
};

struct ait_socinfo {
	unsigned int type, subtype;
	unsigned int cidr, exid;
};

extern struct ait_socinfo vsnv3_soc_initdata;
//const char *at91_get_soc_type(struct at91_socinfo *c);
//const char *at91_get_soc_subtype(struct at91_socinfo *c);

static inline int vsnv3_soc_is_detected(void)
{
	return vsnv3_soc_initdata.type != AIT_SOC_NONE;
}



#ifdef CONFIG_ARCH_VSNV3AIT845X
//#define cpu_is_at91sam9xe()	(vsnv3_soc_initdata.subtype == AT91_SOC_SAM9XE)
//#define cpu_is_at91sam9260()	(vsnv3_soc_initdata.type == AT91_SOC_SAM9260)

#define cpu_is_ait_vsnv3_8453()	(vsnv3_soc_initdata.subtype == AIT_VSN_V3_8453)
#define cpu_is_ait_vsnv3()		(vsnv3_soc_initdata.type == AIT_VSN_V3)

#else
//#define cpu_is_at91sam9xe()	(0)
//#define cpu_is_at91sam9260()	(0)

#define cpu_is_ait_vsnv3_8453()	(0)
#define cpu_is_ait_vsnv3()	(0)

#endif

/*
 * Since this is ARM, we will never run on any AVR32 CPU. But these
 * definitions may reduce clutter in common drivers.
 */
#define cpu_is_at32ap7000()	(0)

#endif /* __MACH_CPU_H__ */
