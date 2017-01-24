/*
 * Copyright (C) 2011 Jean-Christophe PLAGNIOL-VILLARD <plagnioj@jcrosoft.com>
 * Copyright (C) 2014 AIT
 *
 * Under GPLv2
 */

struct ait_init_soc {
	unsigned int *default_irq_priority;
	void (*map_io)(void);
	void (*register_clocks)(void);
	void (*init)(void);
};

extern struct ait_init_soc vsnv3ait84_soc;


static inline int vsnv3_soc_is_enabled(void)
{
	return vsnv3ait84_soc.init != NULL;
}


#if !defined(CONFIG_ARCH_VSNV3AIT845X)
#define vsnv3ait84_soc 	at91_boot_soc
#endif


