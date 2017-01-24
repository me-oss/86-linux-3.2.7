/*
 *
 * Copyright (C) 2014 Alpha Image Technology CORP.
 *
 * Author: Vincent Chen <vincent_chen@a-i-t.com>
 *         Alpha Image Technology CORP.
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


#ifndef _AIT_I2S_DAI_H
#define _AIT_I2S_DAI_H

#include <linux/types.h>
#include <linux/atmel-ssc.h>

//#include "ait-pcm.h"

#define SSC_DIR_MASK_UNUSED	0
#define SSC_DIR_MASK_PLAYBACK	1
#define SSC_DIR_MASK_CAPTURE	2

struct ait_afe_dai_ssc_info {
	char *name;
	struct ssc_device *ssc;
	spinlock_t lock;	/* lock for dir_mask */
	unsigned short dir_mask;	/* 0=unused, 1=playback, 2=capture */
	unsigned short initialized;	/* true if SSC has been initialized */
	unsigned short daifmt;
	unsigned short cmr_div;
	unsigned short tcmr_period;
	unsigned short rcmr_period;
	struct ait_pcm_params *dma_params[2];
	//struct atmel_ssc_state ssc_state;
};

#endif /* _AIT_I2S_DAI_H */
