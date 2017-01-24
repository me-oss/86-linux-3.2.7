/*
 * Driver for Vision V3 SIF Controllers
 *
 * Copyright (C) 2006 Atmel Corporation
 * Copyright (C) 2013 Alpha Imageing Technology Corporation
 *
 * Author: Vincent Chen <vincent_chen@a-i-t.com.tw>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
//#define DEBUG
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>
#include <linux/slab.h>

#include <asm/io.h>
#include <mach/board.h>
#include <asm/gpio.h>
#include <mach/cpu.h>

#include <mach/mmp_reg_gbl.h>
#include <mach/mmp_reg_spi.h>

/*
 * The core SPI transfer engine just talks to a register bank to set up
 * DMA transfers; transfer queue progress is driven by IRQs.  The clock
 * framework provides the base clock, subdivided for each spi_device.
 */
struct ait_spi {
	spinlock_t		lock;

	void __iomem		*regs;
	int			irq;
	struct clk		*clk;
	struct platform_device	*pdev;
	struct spi_device	*stay;

	u8			stopping;
	struct list_head	queue;
	struct spi_transfer	*current_transfer;
	unsigned long		current_remaining_bytes;
	struct spi_transfer	*next_transfer;
	unsigned long		next_remaining_bytes;

	void			*buffer;
	dma_addr_t		buffer_dma;

	bool is_master;

		/* Timer for timeouts */
	struct timer_list timer;
	u8 cmd;
	u16 txlen;
	u16 rxlen;
	
	
};
#if 0

/**
 * struct s3c64xx_spi_driver_data - Runtime info holder for SPI driver.
 * @clk: Pointer to the spi clock.
 * @src_clk: Pointer to the clock used to generate SPI signals.
 * @master: Pointer to the SPI Protocol master.
 * @workqueue: Work queue for the SPI xfer requests.
 * @cntrlr_info: Platform specific data for the controller this driver manages.
 * @tgl_spi: Pointer to the last CS left untoggled by the cs_change hint.
 * @work: Work
 * @queue: To log SPI xfer requests.
 * @lock: Controller specific lock.
 * @state: Set of FLAGS to indicate status.
 * @rx_dmach: Controller's DMA channel for Rx.
 * @tx_dmach: Controller's DMA channel for Tx.
 * @sfr_start: BUS address of SPI controller regs.
 * @regs: Pointer to ioremap'ed controller registers.
 * @xfer_completion: To indicate completion of xfer task.
 * @cur_mode: Stores the active configuration of the controller.
 * @cur_bpw: Stores the active bits per word settings.
 * @cur_speed: Stores the active xfer clock speed.
 */
struct s3c64xx_spi_driver_data {
	void __iomem                    *regs;
	struct clk                      *clk;
	struct clk                      *src_clk;
	struct platform_device          *pdev;
	struct spi_master               *master;
	struct workqueue_struct	        *workqueue;
	struct s3c64xx_spi_info  *cntrlr_info;
	struct spi_device               *tgl_spi;
	struct work_struct              work;
	struct list_head                queue;
	spinlock_t                      lock;
	unsigned long                   sfr_start;
	struct completion               xfer_completion;
	unsigned                        state;
	unsigned                        cur_mode, cur_bpw;
	unsigned                        cur_speed;
	struct s3c64xx_spi_dma_data	rx_dma;
	struct s3c64xx_spi_dma_data	tx_dma;
	struct samsung_dma_ops		*ops;
};
#endif
/* Controller-specific per-slave state */
struct ait_spi_device {
	unsigned int		npcs_pin;
	u32			csr;
};

#define BUFFER_SIZE		PAGE_SIZE
#define INVALID_DMA_ADDRESS	0xffffffff

static void
ait_spi_msg_done(struct spi_master *master, struct ait_spi *as,
		struct spi_message *msg, int status, int stay);
#if 0		
/*
 * Version 2 of the SPI controller has
 *  - CR.LASTXFER
 *  - SPI_MR.DIV32 may become FDIV or must-be-zero (here: always zero)
 *  - SPI_SR.TXEMPTY, SPI_SR.NSSR (and corresponding irqs)
 *  - SPI_CSRx.CSAAT
 *  - SPI_CSRx.SBCR allows faster clocking
 *
 * We can determine the controller version by reading the VERSION
 * register, but I haven't checked that it exists on all chips, and
 * this is cheaper anyway.
 */
static bool ait_spi_is_v2(void)
{
pr_info("%s,%d\r\n",__func__,__LINE__);
	return !cpu_is_at91rm9200();
}

/*
 * Earlier SPI controllers (e.g. on at91rm9200) have a design bug whereby
 * they assume that spi slave device state will not change on deselect, so
 * that automagic deselection is OK.  ("NPCSx rises if no data is to be
 * transmitted")  Not so!  Workaround uses nCSx pins as GPIOs; or newer
 * controllers have CSAAT and friends.
 *
 * Since the CSAAT functionality is a bit weird on newer controllers as
 * well, we use GPIO to control nCSx pins on all controllers, updating
 * MR.PCS to avoid confusing the controller.  Using GPIOs also lets us
 * support active-high chipselects despite the controller's belief that
 * only active-low devices/systems exists.
 *
 * However, at91rm9200 has a second erratum whereby nCS0 doesn't work
 * right when driven with GPIO.  ("Mode Fault does not allow more than one
 * Master on Chip Select 0.")  No workaround exists for that ... so for
 * nCS0 on that chip, we (a) don't use the GPIO, (b) can't support CS_HIGH,
 * and (c) will trigger that first erratum in some cases.
 *
 * TODO: Test if the atmel_spi_is_v2() branch below works on
 * AT91RM9200 if we use some other register than CSR0. However, don't
 * do this unconditionally since AP7000 has an errata where the BITS
 * field in CSR0 overrides all other CSRs.
 */

static void cs_activate(struct atmel_spi *as, struct spi_device *spi)
{
	struct atmel_spi_device *asd = spi->controller_state;
	unsigned active = spi->mode & SPI_CS_HIGH;
	u32 mr;
pr_info("%s,%d\r\n",__func__,__LINE__);
	if (ait_spi_is_v2()) {
		/*
		 * Always use CSR0. This ensures that the clock
		 * switches to the correct idle polarity before we
		 * toggle the CS.
		 */
#if 0		 
		spi_writel(as, CSR0, asd->csr);
		spi_writel(as, MR, SPI_BF(PCS, 0x0e) | SPI_BIT(MODFDIS)
				| SPI_BIT(MSTR));
		mr = spi_readl(as, MR);
		
		gpio_set_value(asd->npcs_pin, active);
#endif		
	} else {
#if 0
		u32 cpol = (spi->mode & SPI_CPOL) ? SPI_BIT(CPOL) : 0;
		int i;
		u32 csr;

		/* Make sure clock polarity is correct */
		for (i = 0; i < spi->master->num_chipselect; i++) {

			csr = spi_readl(as, CSR0 + 4 * i);
			if ((csr ^ cpol) & SPI_BIT(CPOL))
				spi_writel(as, CSR0 + 4 * i,
						csr ^ SPI_BIT(CPOL));

		}

		mr = spi_readl(as, MR);
		mr = SPI_BFINS(PCS, ~(1 << spi->chip_select), mr);
		if (spi->chip_select != 0)
			gpio_set_value(asd->npcs_pin, active);
		spi_writel(as, MR, mr);
#endif		
	}

//	dev_dbg(&spi->dev, "activate %u%s, mr %08x\n",
	//		asd->npcs_pin, active ? " (high)" : "",
		//	mr);
}

static void cs_deactivate(struct ait_spi *as, struct spi_device *spi)
{
	struct ait_spi_device *asd = spi->controller_state;
	unsigned active = spi->mode & SPI_CS_HIGH;
	u32 mr;
pr_info("%s,%d\r\n",__func__,__LINE__);
	/* only deactivate *this* device; sometimes transfers to
	 * another device may be active when this routine is called.
	 */
#if 0	 
	mr = spi_readl(as, MR);
	if (~SPI_BFEXT(PCS, mr) & (1 << spi->chip_select)) {
		mr = SPI_BFINS(PCS, 0xf, mr);
		spi_writel(as, MR, mr);
	}
#endif
	dev_dbg(&spi->dev, "DEactivate %u%s, mr %08x\n",
			asd->npcs_pin, active ? " (low)" : "",
			mr);

//	if (ait_spi_is_v2() || spi->chip_select != 0)
//		gpio_set_value(asd->npcs_pin, !active);
}

static inline int ait_spi_xfer_can_be_chained(struct spi_transfer *xfer)
{
pr_info("%s,%d\r\n",__func__,__LINE__);
	return xfer->delay_usecs == 0 && !xfer->cs_change;
}


#endif
static inline int ait_spi_xfer_is_last(struct spi_message *msg,
					struct spi_transfer *xfer)
{
	pr_debug("%s,%d\r\n",__func__,__LINE__);
	return msg->transfers.prev == &xfer->transfer_list;
}

static void ait_spi_next_xfer_data(struct spi_master *master,
				struct spi_transfer *xfer,
				dma_addr_t *tx_dma,
				dma_addr_t *rx_dma,
				u32 *plen)
{
	struct ait_spi	*as = spi_master_get_devdata(master);
	u32			len = *plen;
	pr_debug("%s,%d\r\n",__func__,__LINE__);
	/* use scratch buffer only when rx or tx data is unspecified */
	if (xfer->rx_buf)
		*rx_dma = xfer->rx_dma + xfer->len - *plen;
	else {
		*rx_dma = as->buffer_dma;
		if (len > BUFFER_SIZE)
			len = BUFFER_SIZE;
	}
	if (xfer->tx_buf)
		*tx_dma = xfer->tx_dma + xfer->len - *plen;
	else {
		*tx_dma = as->buffer_dma;
		if (len > BUFFER_SIZE)
			len = BUFFER_SIZE;
		memset(as->buffer, 0, len);
		dma_sync_single_for_device(&as->pdev->dev,
				as->buffer_dma, len, DMA_TO_DEVICE);
	}

	*plen = len;
}

/*
 * Submit next transfer for DMA.
 * lock is held, spi irq is blocked
 */
static void ait_spi_next_xfer(struct spi_master *master,
				struct spi_message *msg)
{
	struct ait_spi	*as = spi_master_get_devdata(master);
	struct spi_transfer	*xfer;
	u32			len, remaining=0;
	u32			int_en=0,spi_cfg,spi_ctl;
	dma_addr_t		tx_dma, rx_dma;

	as->txlen = 0;
	as->rxlen = 0;	
	
	if (!as->current_transfer)
		xfer = list_entry(msg->transfers.next,
				struct spi_transfer, transfer_list);
	else if (!as->next_transfer)
	{
		xfer = list_entry(as->current_transfer->transfer_list.next,
				struct spi_transfer, transfer_list);
		as->next_transfer = xfer;
	}
	else
		xfer = NULL;

	if(xfer==NULL)
	{
		ait_spi_msg_done(master, as, msg,0, 0);
		return;
	}else if(xfer->len==0)
	{
		ait_spi_msg_done(master, as, msg, 0, 0);
		return;
	}


	
	spi_cfg = SPI_GET_CFG(as)&(~(SPI_TX_EN|SPI_RX_EN|TX_NON_XCH_MODE));
	
	//SPI_SET_CFG(as,spi_cfg);	//Disable tx /rx
#if 0	// For slave support
       if (! (spi_cfg & SPI_MASTER_MODE) ){
                    SPI_SET_INT_SR(as,SPI_SLAVE_ERR);
	}
#endif	   
	SPI_SET_INT_SR(as,SPI_TXDMA_DONE | SPI_RXDMA_DONE);
	SPI_SET_CTL(as,SPI_TXFIFO_CLR | SPI_RXFIFO_CLR);
		                
	if (xfer&&xfer->tx_buf) {

		len= xfer->len;
		ait_spi_next_xfer_data(master, xfer, &tx_dma, &rx_dma, &len);
		remaining = xfer->len - len;

		as->txlen = xfer->len;

		memcpy(as->buffer,xfer->tx_buf,xfer->len);
		
		SPI_SET_TXDMA_ADDR(as,xfer->tx_dma);	

		spi_ctl |= SPI_TX_DMA_START;
		spi_cfg |=SPI_TX_EN;


		dev_dbg(&msg->spi->dev,
			"  start xfer %p: len %u tx %p/%08x rx %p/%08x\n",
			xfer, xfer->len, xfer->tx_buf, xfer->tx_dma,
			xfer->rx_buf, xfer->rx_dma);

	}


	
	as->current_transfer = xfer;

	as->current_remaining_bytes = remaining;

	if (remaining > 0)
		len = remaining;
	else if (!ait_spi_xfer_is_last(msg, xfer)){
		xfer = list_entry(xfer->transfer_list.next,
				struct spi_transfer, transfer_list);
		len = xfer->len;
	} else
		xfer = NULL;

		
	if (xfer&&xfer->tx_buf) {


		as->next_transfer = xfer;
		
		len= xfer->len;
		ait_spi_next_xfer_data(master, xfer, &tx_dma, &rx_dma, &len);
		remaining = xfer->len - len;
		as->txlen += xfer->len;
	}else if (xfer&&xfer->rx_buf) {
		u32	total;

		as->next_transfer = xfer;
		
		total = len;
		ait_spi_next_xfer_data(master, xfer, &tx_dma, &rx_dma, &len);
		as->next_remaining_bytes = total - len;
		dev_dbg(&msg->spi->dev,
			"  next xfer %p: len %u tx %p/%08x rx %p/%08x\n",
			xfer, xfer->len, xfer->tx_buf, xfer->tx_dma,
			xfer->rx_buf, xfer->rx_dma);

		as->rxlen = xfer->len;
		
		SPI_SET_RXDMA_ADDR(as,as->buffer_dma);

		spi_ctl |= SPI_RX_DMA_START;	
		spi_cfg |=SPI_RX_EN;		
		as->current_transfer = xfer;
//		pr_info("as->current_transfer =%x (%d)\n",as->current_transfer ,__LINE__);
		
	}

	SPI_SET_TXDMA_SIZE(as,as->txlen+as->rxlen);		
	SPI_SET_RXDMA_SIZE(as,as->txlen+as->rxlen);


	if(as->txlen&&as->rxlen)
	{
		int_en = SPI_RXDMA_DONE;
		SPI_SET_CTL(as,SPI_RX_DMA_START|SPI_TX_DMA_START);
	}else if(as->txlen)
	{
		int_en = SPI_TXDMA_DONE;	
		SPI_SET_CTL(as,SPI_TX_DMA_START);		
	}


	SPI_SET_INT_EN(as,int_en);
	mod_timer(&as->timer, jiffies +  msecs_to_jiffies(5000));
	
	SPI_SET_CFG(as,spi_cfg|TX_XCH_MODE);
	SPI_SET_XCH_CTL(as,XCH_START);

			
}

static void ait_spi_next_message(struct spi_master *master)
{
	struct ait_spi	*as = spi_master_get_devdata(master);
	struct spi_message	*msg;
	struct spi_device	*spi;
	
	BUG_ON(as->current_transfer);

	msg = list_entry(as->queue.next, struct spi_message, queue);
	spi = msg->spi;

	dev_dbg(master->dev.parent, "start message %p for %s\n",
			msg, dev_name(&spi->dev));
	ait_spi_next_xfer(master, msg);
}

/*
 * For DMA, tx_buf/tx_dma have the same relationship as rx_buf/rx_dma:
 *  - The buffer is either valid for CPU access, else NULL
 *  - If the buffer is valid, so is its DMA address
 *
 * This driver manages the dma address unless message->is_dma_mapped.
 */
static int
ait_spi_dma_map_xfer(struct ait_spi *as, struct spi_transfer *xfer)
{
	struct device	*dev = &as->pdev->dev;
	
	xfer->tx_dma = xfer->rx_dma = INVALID_DMA_ADDRESS;
	if (xfer->tx_buf) {
		/* tx_buf is a const void* where we need a void * for the dma
		 * mapping */
		void *nonconst_tx = (void *)xfer->tx_buf;

		xfer->tx_dma = dma_map_single(dev,
				nonconst_tx, xfer->len,
				DMA_TO_DEVICE);
		if (dma_mapping_error(dev, xfer->tx_dma))
			return -ENOMEM;
	}
	if (xfer->rx_buf) {
		xfer->rx_dma = dma_map_single(dev,
				xfer->rx_buf, xfer->len,
				DMA_FROM_DEVICE);
		if (dma_mapping_error(dev, xfer->rx_dma)) {
			if (xfer->tx_buf)
				dma_unmap_single(dev,
						xfer->tx_dma, xfer->len,
						DMA_TO_DEVICE);
			return -ENOMEM;
		}
	}
	return 0;
}

static void ait_spi_dma_unmap_xfer(struct spi_master *master,
				     struct spi_transfer *xfer)
{
	if (xfer->tx_dma != INVALID_DMA_ADDRESS)
		dma_unmap_single(master->dev.parent, xfer->tx_dma,
				 xfer->len, DMA_TO_DEVICE);
	if (xfer->rx_dma != INVALID_DMA_ADDRESS)
		dma_unmap_single(master->dev.parent, xfer->rx_dma,
				 xfer->len, DMA_FROM_DEVICE);
}

static void
ait_spi_msg_done(struct spi_master *master, struct ait_spi *as,
		struct spi_message *msg, int status, int stay)
{
//	pr_debug("%s,%d\r\n",__func__,__LINE__);
#if 0
	if (!stay || status < 0)
	{
//		cs_deactivate(as, msg->spi);
	}
	else
		as->stay = msg->spi;
#endif
	list_del(&msg->queue);
	msg->status = status;

	dev_dbg(master->dev.parent,
		"xfer complete: %u bytes transferred\n",
		msg->actual_length);
//	spin_unlock(&as->lock);
	msg->complete(msg->context);
	del_timer(&as->timer);		
//	spin_lock(&as->lock);

	as->current_transfer = NULL;
	as->next_transfer = NULL;

	/* continue if needed */
	if (list_empty(&as->queue) || as->stopping)
	{}//spi_writel(as, PTCR, SPI_BIT(RXTDIS) | SPI_BIT(TXTDIS));
	else
		ait_spi_next_message(master);
}

static void ait_spi_timeout(unsigned long data)
{
	struct spi_message	*msg;

	struct spi_master *master = (struct spi_master *)data;

	struct ait_spi *as = spi_master_get_devdata(master);
	struct spi_transfer	*xfer;

	pr_warn("ait_spi_timeout\n");

	if(as)
	{
		int i;
		xfer = as->current_transfer;

		if(xfer->tx_buf)
		{
			pr_warn("tx_buf = 0x%x\n",(unsigned int)xfer->tx_buf);			
			for(i=0;i<xfer->len;++i)
			{
				pr_warn(" 0x%x ",(unsigned int)((char*)xfer->tx_buf)[i]);
			}
			pr_warn(" \n");
		}

		if(xfer->rx_buf)
			{
			pr_warn("rx_buf = %x\n",(unsigned int)xfer->rx_buf);			
			for(i=0;i<xfer->len;++i)
			{
				pr_warn(" 0x%x ",(unsigned int)((char*)xfer->rx_buf)[i]);
			}
			pr_warn(" \n");
		}
	}
	
	pr_warn("SPI_GET_INT_SR = 0x%04x\r\n",SPI_GET_INT_SR(as));
	pr_warn("SPI_GET_INT_EN = 0x%02x\r\n",SPI_GET_INT_EN(as));
	pr_warn("SPI_GET_TXDMA_SIZE = 0x%02x\r\n",SPI_GET_TXDMA_SIZE(as));
	pr_warn("SPI_GET_RXDMA_SIZE = 0x%02x\r\n",SPI_GET_RXDMA_SIZE(as));
	pr_warn("SPI_GET_CFG = 0x%02x\r\n",SPI_GET_CFG(as));

	msg = list_entry(as->queue.next, struct spi_message, queue);


	if (!msg->is_dma_mapped)
	{
		xfer = list_entry(msg->transfers.next,
				struct spi_transfer, transfer_list);
		ait_spi_dma_unmap_xfer(master, xfer);

		xfer = list_entry(xfer->transfer_list.next,
				struct spi_transfer, transfer_list);
		ait_spi_dma_unmap_xfer(master, xfer);		
	}

	ait_spi_msg_done(master, as, msg, -EIO, 0);
		
	return;
}


static irqreturn_t
ait_spi_interrupt(int irq, void *dev_id)
{
	struct spi_master	*master = dev_id;
	struct ait_spi	*as = spi_master_get_devdata(master);
	struct spi_message	*msg;
	struct spi_transfer	*xfer;
	u32			status, pending, imr;
	int			ret = IRQ_NONE;

	BUG_ON(!as);
//	if(as)
	xfer = as->current_transfer;
//	else
	{
//		pr_err("vsnv3_sif_interrupt: as is NULL pointer\r\n");
	//	msg = list_entry(as->queue.next, struct spi_message, queue);
		
//		ait_spi_msg_done(master, as, msg, -EIO,0);		
//		return IRQ_HANDLED;
	}
	
	msg = list_entry(as->queue.next, struct spi_message, queue);

	imr = SPI_GET_INT_EN(as);
	status = SPI_GET_INT_SR(as)&imr;
	//pr_debug("EN: 0x%08x\r\n",imr);
	//pr_debug("SR 0x%08x\r\n",status);

	pending = status & imr;

	if(xfer==0)
	{
		pr_err("vsnv3_sif_interrupt: as->current_transfer is NULL pointer. as = %x\r\n",(unsigned int)as);

		pr_warn("SPI_GET_INT_SR = 0x%04x\r\n",SPI_GET_INT_SR(as));
		pr_warn("SPI_GET_INT_EN = 0x%02x\r\n",SPI_GET_INT_EN(as));
		pr_warn("SPI_GET_TXDMA_SIZE = 0x%02x\r\n",SPI_GET_TXDMA_SIZE(as));
		pr_warn("SPI_GET_RXDMA_SIZE = 0x%02x\r\n",SPI_GET_RXDMA_SIZE(as));
		pr_warn("SPI_GET_CFG = 0x%02x\r\n",SPI_GET_CFG(as));


		imr = SPI_GET_INT_EN(as);
		status = SPI_GET_INT_SR(as)&imr;
		pending = status & imr;
		pr_warn("EN: 0x%08x\r\n",imr);
		pr_warn("SR 0x%08x\r\n",status);
		pr_warn("pending 0x%08x\r\n",pending);

		ait_spi_msg_done(master, as, msg, -EIO,0);		
		
		return IRQ_NONE;

	}
	
	if(!(status&(SPI_TXDMA_DONE|SPI_RXDMA_DONE)))
	{
		return IRQ_NONE;
	}
	
	if(status & SPI_TXDMA_DONE){
		SPI_SET_INT_SR(as,SPI_TXDMA_DONE);		
	}
	
	if (status & (SPI_RXDMA_DONE/*|SPI_RXFIFO_GE*/)) {
		SPI_SET_INT_SR(as,SPI_RXDMA_DONE);	

		if(xfer->rx_buf&&xfer->len)
			memcpy(xfer->rx_buf,as->buffer+as->txlen, xfer->len);
	}

	ret = IRQ_HANDLED;

	if (as->current_remaining_bytes == 0) {
		msg->actual_length += xfer->len;

		if (!msg->is_dma_mapped)
			ait_spi_dma_unmap_xfer(master, xfer);
		
		/* REVISIT: udelay in irq is unfriendly */
		if (xfer->delay_usecs)
			udelay(xfer->delay_usecs);

		if (ait_spi_xfer_is_last(msg, xfer)) 
		{
			/* report completed message */
			SPI_SET_INT_SR(as,0xffff);	
			ait_spi_msg_done(master, as, msg, 0,
					xfer->cs_change);
		} else {

			/*
			 * Not done yet. Submit the next transfer.
			 *
			 * FIXME handle protocol options for xfer
			 */
			ait_spi_next_xfer(master, msg);
		}
	} else {
		/*
		 * Keep going, we still have data to send in
		 * the current transfer.
		 */
		ait_spi_next_xfer(master, msg);
	}

	
#if 0
	if (pending & SPI_BIT(OVRES)) {
		int timeout;

		ret = IRQ_HANDLED;

		//spi_writel(as, IDR, (SPI_BIT(RXBUFF) | SPI_BIT(ENDRX)
		//		     | SPI_BIT(OVRES)));

		/*
		 * When we get an overrun, we disregard the current
		 * transfer. Data will not be copied back from any
		 * bounce buffer and msg->actual_len will not be
		 * updated with the last xfer.
		 *
		 * We will also not process any remaning transfers in
		 * the message.
		 *
		 * First, stop the transfer and unmap the DMA buffers.
		 */
		spi_writel(as, PTCR, SPI_BIT(RXTDIS) | SPI_BIT(TXTDIS));
		if (!msg->is_dma_mapped)
			ait_spi_dma_unmap_xfer(master, xfer);

		/* REVISIT: udelay in irq is unfriendly */
		if (xfer->delay_usecs)
			udelay(xfer->delay_usecs);

//		dev_warn(master->dev.parent, "overrun (%u/%u remaining)\n",
//			 spi_readl(as, TCR), spi_readl(as, RCR));

		/*
		 * Clean up DMA registers and make sure the data
		 * registers are empty.
		 */
//		spi_writel(as, RNCR, 0);
//		spi_writel(as, TNCR, 0);
//		spi_writel(as, RCR, 0);
//		spi_writel(as, TCR, 0);
//		for (timeout = 1000; timeout; timeout--)
//			if (spi_readl(as, SR) & SPI_BIT(TXEMPTY))
//				break;
		if (!timeout)
			dev_warn(master->dev.parent,
				 "timeout waiting for TXEMPTY");
//		while (spi_readl(as, SR) & SPI_BIT(RDRF))
//			spi_readl(as, RDR);

		/* Clear any overrun happening while cleaning up */
		spi_readl(as, SR);

		ait_spi_msg_done(master, as, msg, -EIO, 0);
//	} else if (pending & (SPI_BIT(RXBUFF) | SPI_BIT(ENDRX))) {
	} else if (pending & SPI_RXDMA_DONE) {


		ret = IRQ_HANDLED;

		//spi_writel(as, IDR, pending);

		if (as->current_remaining_bytes == 0) {
			msg->actual_length += xfer->len;

			if (!msg->is_dma_mapped)
				ait_spi_dma_unmap_xfer(master, xfer);

			/* REVISIT: udelay in irq is unfriendly */
			if (xfer->delay_usecs)
				udelay(xfer->delay_usecs);

			if (ait_spi_xfer_is_last(msg, xfer)) {
				/* report completed message */
				ait_spi_msg_done(master, as, msg, 0,
						xfer->cs_change);
			} else {
				if (xfer->cs_change) {
					cs_deactivate(as, msg->spi);
					udelay(1);
					cs_activate(as, msg->spi);
				}

				/*
				 * Not done yet. Submit the next transfer.
				 *
				 * FIXME handle protocol options for xfer
				 */
				ait_spi_next_xfer(master, msg);
			}
		} else {
			/*
			 * Keep going, we still have data to send in
			 * the current transfer.
			 */
			ait_spi_next_xfer(master, msg);
		}
	}
#endif
//	spin_unlock(&as->lock);

	return ret;
}

static int ait_spi_setup(struct spi_device *spi)
{
	struct ait_spi	*as;
//	struct ait_spi_device	*asd;
//	u32			scbr, csr;
	unsigned int		bits = spi->bits_per_word;
//	unsigned long		bus_hz;
	//unsigned int		npcs_pin;
//	int			ret;
	int			clk_mode;
	//pr_debug("%s,%d\r\n",__func__,__LINE__);
	as = spi_master_get_devdata(spi->master);

	if (as->stopping)
		return -ESHUTDOWN;

	if (spi->chip_select > spi->master->num_chipselect) {
		dev_dbg(&spi->dev,
				"setup: invalid chipselect %u (%u defined)\n",
				spi->chip_select, spi->master->num_chipselect);
		return -EINVAL;
	}

	
	if (bits < 8 || bits > 16) {
		dev_dbg(&spi->dev,
				"setup: invalid bits_per_word %u (8 to 16)\n",
				bits);
		return -EINVAL;
	}
	SPI_SET_WORD_LEN(as,8);
#if 0
	/* see notes above re chipselect */
	if (!ait_spi_is_v2()
			&& spi->chip_select == 0
			&& (spi->mode & SPI_CS_HIGH)) {
		dev_dbg(&spi->dev, "setup: can't be active-high\n");
		return -EINVAL;
	}
#endif
	/* v1 chips start out at half the peripheral bus speed. */
	//bus_hz = clk_get_rate(as->clk);
///pr_info("bus_hz = %d\r\n",bus_hz);


	if (spi->max_speed_hz) {
		u8  divisor=1;
		u32	bus_hz = clk_get_rate(as->clk)/2;

		if(spi->max_speed_hz<=(bus_hz/(2*(divisor+1))))
		{		
			for(divisor=1;divisor<0x80;divisor++)
			{
					pr_info("bus_hz = %d    target clock=%d divisor = %d \r\n",bus_hz,(bus_hz/(2*(divisor+1))),divisor);
					if((bus_hz/(2*(divisor+1)))<=spi->max_speed_hz)
					break;
			}
		}

		spi->max_speed_hz = bus_hz/(2*(divisor+1));

		SPI_SET_CLK_DIV(as,divisor);
		pr_info("%s:max_speed_hz = %d\r\n",dev_name(&spi->dev),spi->max_speed_hz);	
		pr_info("Wanted speed: %d Hz , provided %d Hz\n",spi->max_speed_hz,bus_hz/(2*(divisor+1)));
		//return -ENOPROTOOPT;
	}else
		SPI_SET_CLK_DIV(as,1);

	
#if 0
	if (!ait_spi_is_v2())
		bus_hz /= 2;

	if (spi->max_speed_hz) {
		/*
		 * Calculate the lowest divider that satisfies the
		 * constraint, assuming div32/fdiv/mbz == 0.
		 */
		scbr = DIV_ROUND_UP(bus_hz, spi->max_speed_hz);

		/*
		 * If the resulting divider doesn't fit into the
		 * register bitfield, we can't satisfy the constraint.
		 */
		if (scbr >= (1 << SPI_SCBR_SIZE)) {
			dev_dbg(&spi->dev,
				"setup: %d Hz too slow, scbr %u; min %ld Hz\n",
				spi->max_speed_hz, scbr, bus_hz/255);
			return -EINVAL;
		}
	} else
		/* speed zero means "as slow as possible" */
		scbr = 0xff;

	csr = SPI_BF(SCBR, scbr) | SPI_BF(BITS, bits - 8);
	if (spi->mode & SPI_CPOL)
		csr |= SPI_BIT(CPOL);
	if (!(spi->mode & SPI_CPHA))
		csr |= SPI_BIT(NCPHA);

	/* DLYBS is mostly irrelevant since we manage chipselect using GPIOs.
	 *
	 * DLYBCT would add delays between words, slowing down transfers.
	 * It could potentially be useful to cope with DMA bottlenecks, but
	 * in those cases it's probably best to just use a lower bitrate.
	 */
	csr |= SPI_BF(DLYBS, 0);
	csr |= SPI_BF(DLYBCT, 0);
#endif
	clk_mode = SPI_GET_CFG(as)&~(SCLK_POL_HIGH|SCLK_PHA_HIGH);

	if (spi->mode & SPI_CPOL)
		SPI_SET_CFG(as,clk_mode|SCLK_POL_HIGH);
	if (spi->mode & SPI_CPHA)
		SPI_SET_CFG(as,clk_mode|SCLK_PHA_HIGH);		
	pr_info("%s:clk_mode = %d\r\n",dev_name(&spi->dev),spi->mode&(SPI_CPOL|SPI_CPHA));	

#if 0

	/* chipselect must have been muxed as GPIO (e.g. in board setup) */
	npcs_pin = (unsigned int)spi->controller_data;
	asd = spi->controller_state;
	if (!asd) {
		asd = kzalloc(sizeof(struct ait_spi_device), GFP_KERNEL);
		if (!asd)
			return -ENOMEM;

		ret = gpio_request(npcs_pin, dev_name(&spi->dev));
		if (ret) {
			kfree(asd);
			return ret;
		}

		asd->npcs_pin = npcs_pin;
		spi->controller_state = asd;
		gpio_direction_output(npcs_pin, !(spi->mode & SPI_CS_HIGH));
	
		spi->controller_state = asd;
	} else {
		unsigned long		flags;

		spin_lock_irqsave(&as->lock, flags);
		if (as->stay == spi)
			as->stay = NULL;
		cs_deactivate(as, spi);
		spin_unlock_irqrestore(&as->lock, flags);
	}
	
	dev_dbg(&spi->dev,
		"setup: %lu Hz bpw %u mode 0x%x -> csr%d %08x\n",
		bus_hz , bits, spi->mode, spi->chip_select, csr);		
//		bus_hz / scbr, bits, spi->mode, spi->chip_select, csr);

	if (!ait_spi_is_v2())
		spi_writel(as, CSR0 + 4 * spi->chip_select, csr);
#endif
	return 0;
}

static int ait_spi_transfer(struct spi_device *spi, struct spi_message *msg)
{
	struct ait_spi	*as;
	struct spi_transfer	*xfer;
	unsigned long		flags;
//	struct device		*controller = spi->master->dev.parent;
//	u8			bits;
//	struct ait_spi_device	*asd;
//	spin_lock(&as->lock);
	as = spi_master_get_devdata(spi->master);
//	spin_unlock(&as->lock);

//	dev_dbg(controller, "new message %p submitted for %s\n",
//			msg, dev_name(&spi->dev));

	if (unlikely(list_empty(&msg->transfers)))
		return -EINVAL;

	if (as->stopping)
		return -ESHUTDOWN;

	list_for_each_entry(xfer, &msg->transfers, transfer_list) {
		if (!(xfer->tx_buf || xfer->rx_buf) && xfer->len) {
			dev_err(&spi->dev, "missing rx or tx buf\n");
			return -EINVAL;
		}

		if (xfer->tx_buf&&xfer->bits_per_word) {		
			SPI_SET_WORD_LEN(as,xfer->bits_per_word);
		}

		if (xfer->speed_hz) {
			u32 target_speed;
			u8  divisor;
			u32	bus_hz = clk_get_rate(as->clk)/2;

		//	pr_info("bus_hz = %d\r\n",bus_hz);
			if(xfer->speed_hz>spi->max_speed_hz)
			{
				divisor = 10;
				target_speed = spi->max_speed_hz; 
			}
			else
			{
				for(divisor=1;divisor<0x80;divisor++)
					if((bus_hz/(2*(divisor+1)))<=xfer->speed_hz)
						break;
			}

			SPI_SET_CLK_DIV(as,divisor);
			
			pr_info( "%s:Wanted speed: %d Hz , provided %d Hz   divisor = %d\n",dev_name(&spi->dev),xfer->speed_hz,bus_hz/(2*(divisor+1)),divisor);
			//return -ENOPROTOOPT;
		}
		
		/*
		 * DMA map early, for performance (empties dcache ASAP) and
		 * better fault reporting.  This is a DMA-only driver.
		 *
		 * NOTE that if dma_unmap_single() ever starts to do work on
		 * platforms supported by this driver, we would need to clean
		 * up mappings for previously-mapped transfers.
		 */
		if (!msg->is_dma_mapped) {
			if (ait_spi_dma_map_xfer(as, xfer) < 0)
				return -ENOMEM;
		}
	}

#if 0//def VERBOSE
	list_for_each_entry(xfer, &msg->transfers, transfer_list) {
		pr_info(	"  xfer %p: len %u tx %p/%08x rx %p/%08x\n",
			xfer, xfer->len,
			xfer->tx_buf, xfer->tx_dma,
			xfer->rx_buf, xfer->rx_dma);
	}
#endif

	msg->status = -EINPROGRESS;
	msg->actual_length = 0;

	spin_lock_irqsave(&as->lock, flags);
	list_add_tail(&msg->queue, &as->queue);
	if (!as->current_transfer)
		ait_spi_next_message(spi->master);
	else
		pr_info(	"1 current_transfer = 0x%x\n",(unsigned int)as->current_transfer);

	spin_unlock_irqrestore(&as->lock, flags);

	return 0;
}

static void ait_spi_cleanup(struct spi_device *spi)
{
#if 0
	struct ait_spi	*as = spi_master_get_devdata(spi->master);
	struct ait_spi_device	*asd = spi->controller_state;
	unsigned		gpio = (unsigned) spi->controller_data;
	unsigned long		flags;

	if (!asd)
		return;

	spin_lock_irqsave(&as->lock, flags);
	if (as->stay == spi) {
		as->stay = NULL;
		cs_deactivate(as, spi);
	}
	spin_unlock_irqrestore(&as->lock, flags);

	spi->controller_state = NULL;
	gpio_free(gpio);
	kfree(asd);
#endif	
}

/*-------------------------------------------------------------------------*/

static int __devinit ait_spi_probe(struct platform_device *pdev)
{
	struct resource		*regs;
	int			irq;
	struct clk		*clk;
	int			ret;
	struct spi_master	*master;
	struct ait_spi	*as;
	
	regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!regs)
		return -ENXIO;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	clk = clk_get(&pdev->dev, "pspi_ctl_clk");
	if (IS_ERR(clk))
		return PTR_ERR(clk);

	ret = -ENOMEM;
	master = spi_alloc_master(&pdev->dev, sizeof *as);
	if (!master)
		goto alloc_master_fail;
	else
	{
	}
	/* the spi->mode bits understood by this driver: */
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_CS_HIGH;

	master->bus_num = pdev->id;
	master->num_chipselect = 1;
	master->setup = ait_spi_setup;
	master->transfer = ait_spi_transfer;
	master->cleanup = ait_spi_cleanup;
	platform_set_drvdata(pdev, master);

	as = spi_master_get_devdata(master);

	/*
	 * Scratch buffer is used for throwaway rx and tx data.
	 * It's coherent to minimize dcache pollution.
	 */
	as->buffer = dma_alloc_coherent(&pdev->dev, BUFFER_SIZE,
					&as->buffer_dma, GFP_KERNEL);
	if (!as->buffer)
		goto alloc_master_fail;

	spin_lock_init(&as->lock);
	INIT_LIST_HEAD(&as->queue);
	as->pdev = pdev;
	as->regs = ioremap(regs->start, resource_size(regs));
	if (!as->regs)
		goto ioremap_fail;
	as->irq = irq;
	as->clk = clk;

	ret = request_irq(irq, ait_spi_interrupt, 0,
			dev_name(&pdev->dev), master);
	if (ret)
		goto req_irq_fail;

	/* Initialize the hardware */
	clk_enable(clk);

	SPI_SET_CFG(as,MASTER_RX_USE_INCLK|/*MASTER_RX_PAD_CLK|*/SPI_MASTER_MODE|SS_POLAR_LOW|RX_CLK_ADJ_EN|RX_PHA_ADJ|RX_POL_ADJ);

	SPI_SET_CTL(as,SPI_TXFIFO_CLR | SPI_RXFIFO_CLR);

	SPI_SET_INT_EN(as, 0);
	SPI_SET_INT_SR(as, 0);
	SPI_SET_DLY_CYCLE(as, 0);
	SPI_SET_WAIT_CYCLE(as, 0);

	dev_info(&pdev->dev, "AIT SPI Controller at 0x%08lx (irq %d)\n",
			(unsigned long)regs->start, irq);

	setup_timer(&as->timer, ait_spi_timeout, (unsigned long)master);


	ret = spi_register_master(master);
	if (ret)
		goto reg_spi_master_fail;

	return 0;

reg_spi_master_fail:
	clk_disable(clk);
	free_irq(irq, master);
req_irq_fail:
	iounmap(as->regs);
ioremap_fail:
	dma_free_coherent(&pdev->dev, BUFFER_SIZE, as->buffer,
			as->buffer_dma);
alloc_master_fail:
	clk_put(clk);
	spi_master_put(master);
	return ret;
}

static int __devexit ait_spi_remove(struct platform_device *pdev)
{
	struct spi_master	*master = platform_get_drvdata(pdev);
	struct ait_spi	*as = spi_master_get_devdata(master);
	struct spi_message	*msg;

	/* reset the hardware and block queue progress */
	spin_lock_irq(&as->lock);
	as->stopping = 1;

	/* Timer for timeouts */
	del_timer_sync(&as->timer);
	
	spin_unlock_irq(&as->lock);

	/* Terminate remaining queued transfers */
	list_for_each_entry(msg, &as->queue, queue) {
		/* REVISIT unmapping the dma is a NOP on ARM and AVR32
		 * but we shouldn't depend on that...
		 */
		msg->status = -ESHUTDOWN;
		msg->complete(msg->context);
	}

	dma_free_coherent(&pdev->dev, BUFFER_SIZE, as->buffer,
			as->buffer_dma);

	clk_disable(as->clk);
	clk_put(as->clk);
	free_irq(as->irq, master);
	iounmap(as->regs);

	spi_unregister_master(master);

	return 0;
}

#ifdef	CONFIG_PM

static int ait_spi_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	struct spi_master	*master = platform_get_drvdata(pdev);
	struct ait_spi	*as = spi_master_get_devdata(master);

	clk_disable(as->clk);
	return 0;
}

static int ait_spi_resume(struct platform_device *pdev)
{
	struct spi_master	*master = platform_get_drvdata(pdev);
	struct ait_spi	*as = spi_master_get_devdata(master);

	clk_enable(as->clk);
	return 0;
}

#else
#define	ait_spi_suspend	NULL
#define	ait_spi_resume	NULL
#endif


static struct platform_driver ait_spi_driver = {
	.driver		= {
		.name	= "vsnv3_pspi",
		.owner	= THIS_MODULE,
	},
	.probe		= ait_spi_probe,
	.remove		= __exit_p(ait_spi_remove),
	.shutdown	= NULL,
	.suspend	= ait_spi_suspend,
	.resume		= ait_spi_resume,
};
module_platform_driver(ait_spi_driver);

MODULE_DESCRIPTION("AIT VSNV3 PSPI Controller driver");
MODULE_AUTHOR("Vincent (AIT)");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ait_vsnv3_pspi");
