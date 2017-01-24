#include "ax88796c_spi.h"

/* driver bus management functions */
void axspi_wakeup (struct axspi_data *ax_spi)
{
	u8 tx_buf;
	int ret;
//printk (KERN_INFO": %s\n", __func__);

	tx_buf = AX_SPICMD_EXIT_PWD;	// OP
	ret = spi_write(ax_spi->spi, &tx_buf, 1);
	if (ret)
		printk (KERN_ERR": %s() failed: ret = %d\n", __func__, ret);
}

void axspi_read_status (struct axspi_data *ax_spi, struct spi_status *status)
{
	u8 tx_buf;
	int ret;
//printk (KERN_INFO": %s\n", __func__);
	/* OP */
	tx_buf = AX_SPICMD_READ_STATUS;
	ret = spi_write_then_read (ax_spi->spi, &tx_buf, 1, (u8 *)&status, 3);
	if (ret)
		printk (KERN_ERR": %s() failed: ret = %d\n", __func__, ret);
	else
		le16_to_cpus (&status->isr);
}

int axspi_read_rxq (struct axspi_data *ax_spi, void *data, int len)
{
	struct spi_transfer *xfer = ax_spi->spi_rx_xfer;
	int ret;
//printk (KERN_INFO": %s\n", __func__);
//printk (KERN_INFO": data = %x\n", data);
//printk (KERN_INFO": ax_spi->cmd_buf = %x\n", ax_spi->cmd_buf);

	memcpy (ax_spi->cmd_buf, rx_cmd_buf, 5);
//printk (KERN_INFO": xfer = %x\n", xfer);

	xfer->tx_buf = ax_spi->cmd_buf;
	xfer->rx_buf = NULL;
	xfer->len = ax_spi->comp ? 2 : 5;
	xfer->bits_per_word = 8;
	//printk (KERN_INFO": xfer->tx_buf  = %x\n", xfer->tx_buf );
	
//	printk (KERN_INFO": xfer->len  = %d\n", xfer->len );

	spi_message_add_tail(xfer, &ax_spi->rx_msg);

	xfer++;
//printk (KERN_INFO": xfer = %x\n", xfer);
	
	xfer->rx_buf = data;
	xfer->tx_buf = NULL;
	xfer->len = len;
	xfer->bits_per_word = 8;
	//printk (KERN_INFO": xfer->rx_buf  = %x\n", xfer->rx_buf );
	
	spi_message_add_tail(xfer, &ax_spi->rx_msg);
//	printk (KERN_INFO": rx_msg  = %x\n",ax_spi->rx_msg);

	ret = spi_sync(ax_spi->spi, &ax_spi->rx_msg);
	if (ret)
		printk (KERN_ERR": %s() failed: ret = %d\n", __func__, ret);
//printk (KERN_INFO": %s-\n", __func__);

	return ret;
}

int axspi_write_txq (struct axspi_data *ax_spi, void *data, int len)
{
//printk (KERN_INFO": %s\n", __func__);
	return spi_write(ax_spi->spi, data, len);
}

u16 axspi_read_reg (struct axspi_data *ax_spi, u8 reg)
{
	u8 tx_buf[4];
	u16 rx_buf = 0;
	int ret;
	int len = ax_spi->comp ? 3 : 4;
//printk (KERN_INFO": %s\n", __func__);
	tx_buf[0] = 0x03;	// OP code read register
	tx_buf[1] = reg;	// register address
	tx_buf[2] = 0xFF;	// dumy cycle
	tx_buf[3] = 0xFF;	// dumy cycle
	ret = spi_write_then_read (ax_spi->spi, tx_buf, len, (u8 *)&rx_buf, 2);
	if (ret)
		printk (": %s() failed: ret = %d\n", __func__, ret);
	else
		le16_to_cpus (&rx_buf);

	return rx_buf;
}

void axspi_write_reg (struct axspi_data *ax_spi, u8 reg, u16 value)
{
	u8 tx_buf[4];
	int ret;
//printk (KERN_INFO": %s\n", __func__);
	tx_buf[0] = AX_SPICMD_WRITE_REG;	// OP code read register
	tx_buf[1] = reg;			// register address
	tx_buf[2] = value;
	tx_buf[3] = value >> 8;

	ret = spi_write(ax_spi->spi, tx_buf, 4);
	if (ret)
		printk (": %s() failed: ret = %d\n", __func__, ret);

}

