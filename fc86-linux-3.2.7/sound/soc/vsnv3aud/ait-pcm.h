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


#ifndef _AIT_PCM_H
#define _AIT_PCM_H

struct ait_pcm_params {
	char *name;			/* stream identifier */
	int pdc_xfer_size;		/* PDC counter increment in bytes */
	struct ssc_device *ssc;		/* SSC device for stream */
	dma_addr_t xfer_ptr;
	struct snd_pcm_substream *substream;
	void (*dma_intr_handler)(u32, struct snd_pcm_substream *);
};


#endif /* _ATMEL_PCM_H */
