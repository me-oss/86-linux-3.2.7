/*
 *
 *  Based on drivers/tty/serial/atmel_serial.c
 *  Based on drivers/char/serial_sa1100.c, by Deep Blue Solutions Ltd.
 *  Based on drivers/char/serial.c, by Linus Torvalds, Theodore Ts'o.
 *
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
 *
 */
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/serial.h>
#include <linux/clk.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/tty_flip.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/dma-mapping.h>
#include <linux/atmel_pdc.h>
#include <linux/atmel_serial.h>
#include <linux/uaccess.h>

#include <asm/io.h>
#include <asm/ioctls.h>

#include <asm/mach/serial_at91.h>
#include <mach/board.h>

#ifdef CONFIG_ARM
#include <mach/cpu.h>
#include <asm/gpio.h>
#endif

#define PDC_BUFFER_SIZE		512
/* Revisit: We should calculate this based on the actual port settings */
#define PDC_RX_TIMEOUT		(3 * 10)		/* 3 bytes */

#if defined(CONFIG_SERIAL_AIT_CONSOLE) && defined(CONFIG_MAGIC_SYSRQ)
#define SUPPORT_SYSRQ
#endif

#include <linux/serial_core.h>

/*Vin@ add for debugging*/
#include <mach/includes_fw.h>
#include <mach/mmpf_uart.h>

static void ait_stop_rx(struct uart_port *port);

#ifdef CONFIG_SERIAL_AIT_TTYAT
x
/* Use device name ttyAT, major 204 and minor 154-169.  This is necessary if we
 * should coexist with the 8250 driver, such as if we have an external 16C550
 * UART. */
#define SERIAL_AIT_MAJOR	204
#define MINOR_START		154
#define AIT_CONSOLE_DEVICE	"ttyAT"

#else

/* Use device name ttyS, major 4, minor 64-68.  This is the usual serial port
 * name, but it is legally reserved for the 8250 driver. */
#define SERIAL_AIT_MAJOR	TTY_MAJOR
#define MINOR_START		64
#define AIT_CONSOLE_DEVICE  "ttyS"

#endif


#define AIT_US_RXPARE       (1 << 0)        //Rx Parity Error
#define AIT_US_RXFRME       (1 << 1)        //Rx Framing Error
#define AIT_US_RXFULL       (1 << 2)        //Rx FIFO Full
#define AIT_US_RXEMPTY      (1 << 3)        //Rx FIFO Empty
#define AIT_US_RXOVTH       (1 << 4)        //Rx FIFO Over Threshold
#define AIT_US_RXOVFW       (1 << 5)        //Rx Overflow Error
#define AIT_US_RXTIMEOUT    (1 << 6)        //Rx Timeout Error


/* UART registers. CR is write-only, hence no GET macro */
#define UART_PUT_CR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_US_CR)
#define UART_GET_MR(port)	//__raw_readl((port)->membase + ATMEL_US_MR)
#define UART_PUT_MR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_US_MR)
#define UART_PUT_IER(port,v)	//__raw_writel(v, (port)->membase + ATMEL_US_IER)
#define UART_PUT_IDR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_US_IDR)
#define UART_GET_IMR(port)	//__raw_readl((port)->membase + ATMEL_US_IMR)
#define UART_GET_CSR(port)	//(AITC_BASE_UART->US0.US_ISR)//__raw_readl((port)->membase + ATMEL_US_CSR)
#define UART_GET_CHAR(port)	 //(AITC_BASE_UART->US0.US_RXPR)//__raw_readl((port)->membase + ATMEL_US_RHR)
#define UART_PUT_CHAR(port,v)	//do{while(AITC_BASE_UART->US0.US_TX_FIFO_DATA_CNT<64); AITC_BASE_UART->US0.US_TXPR = v;}while(0)
#define UART_GET_BRGR(port)	//__raw_readl((port)->membase + ATMEL_US_BRGR)
#define UART_PUT_BRGR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_US_BRGR)
#define UART_PUT_RTOR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_US_RTOR)
#define UART_PUT_TTGR(port, v)	//__raw_writel(v, (port)->membase + ATMEL_US_TTGR)

 /* PDC registers */
#define UART_PUT_PTCR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_PDC_PTCR)
#define UART_GET_PTSR(port)	//__raw_readl((port)->membase + ATMEL_PDC_PTSR)

#define UART_PUT_RPR(port,v)//	__raw_writel(v, (port)->membase + ATMEL_PDC_RPR)
#define UART_GET_RPR(port)	//__raw_readl((port)->membase + ATMEL_PDC_RPR)
#define UART_PUT_RCR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_PDC_RCR)
#define UART_PUT_RNPR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_PDC_RNPR)
#define UART_PUT_RNCR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_PDC_RNCR)

#define UART_PUT_TPR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_PDC_TPR)
#define UART_PUT_TCR(port,v)	//__raw_writel(v, (port)->membase + ATMEL_PDC_TCR)
#define UART_GET_TCR(port)	//__raw_readl((port)->membase + ATMEL_PDC_TCR)
#define UART_GET_TXCNT(port) //(AITC_BASE_UART->US0.US_TX_FIFO_DATA_CNT)


static int (*ait_open_hook)(struct uart_port *);
static void (*ait_close_hook)(struct uart_port *);

struct atmel_dma_buffer {
	unsigned char	*buf;
	dma_addr_t	dma_addr;
	unsigned int	dma_size;
	unsigned int	ofs;
};

struct ait_uart_chdesc {
	u16		status;
	u16		ch;
};

#define AIT_RX_RINGSIZE 1024

/*
 * We wrap our port structure around the generic uart_port.
 */
struct ait_uart_port {
	struct uart_port	uart;		/* uart */
	struct clk		*clk;		/* uart clock */
	int			may_wakeup;	/* cached value of device_may_wakeup for times we need to disable it */
	u32			backup_imr;	/* IMR saved during suspend */
	int			break_active;	/* break being received */

	short			pdc_rx_idx;	/* current PDC RX buffer */
	struct atmel_dma_buffer	pdc_rx[2];	/* PDC receier */

	struct atmel_dma_buffer	pdc_tx;		/* PDC transmitter */

	struct tasklet_struct	tasklet;

	struct circ_buf		rx_ring;

	struct serial_rs485	rs485;		/* rs485 settings */
	unsigned int		tx_done_mask;
};

static struct ait_uart_port ait_usports[ATMEL_MAX_UART];
static unsigned long ait_usports_in_use;

#ifdef SUPPORT_SYSRQ
static struct console ait_console;
#endif

#if defined(CONFIG_OF)
static const struct of_device_id ait_serial_dt_ids[] = {
	{ .compatible = "atmel,at91rm9200-usart" },
	{ .compatible = "atmel,at91sam9260-usart" },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, ait_serial_dt_ids);
#endif

static inline struct ait_uart_port *
to_ait_uart_port(struct uart_port *uart)
{
	return container_of(uart, struct ait_uart_port, uart);
}

static bool ait_use_dma_tx(struct uart_port *port)
{
	return false;
}

/*
 * Return TIOCSER_TEMT when transmitter FIFO and Shift register is empty.
 */
static u_int ait_tx_empty(struct uart_port *port)
{
    return MMPF_Uart_CheckState(DEBUG_UART_NUM, MMPF_UART_STAT_TXEMPTY)?
            TIOCSER_TEMT: 0;
}

/*
 * Set state of the modem control output lines
 */
static void ait_set_mctrl(struct uart_port *port, u_int mctrl)
{
	unsigned int control = 0;
//	unsigned int mode;
	struct ait_uart_port *ait_port = to_ait_uart_port(port);

#ifdef CONFIG_ARCH_AT91RM9200
x
	if (cpu_is_at91rm9200()) {
		/*
		 * AT91RM9200 Errata #39: RTS0 is not internally connected
		 * to PA21. We need to drive the pin manually.
		 */
		if (port->mapbase == AT91RM9200_BASE_US0) {
			if (mctrl & TIOCM_RTS)
				at91_set_gpio_value(AT91_PIN_PA21, 0);
			else
				at91_set_gpio_value(AT91_PIN_PA21, 1);
		}
	}
#endif

	if (mctrl & TIOCM_RTS)
		control |= ATMEL_US_RTSEN;
	else
		control |= ATMEL_US_RTSDIS;

	if (mctrl & TIOCM_DTR)
		control |= ATMEL_US_DTREN;
	else
		control |= ATMEL_US_DTRDIS;

	UART_PUT_CR(port, control);

	/* Local loopback mode? */
//	mode = UART_GET_MR(port) & ~ATMEL_US_CHMODE;
#if 0
	if (mctrl & TIOCM_LOOP)
		mode |= ATMEL_US_CHMODE_LOC_LOOP;
	else
		mode |= ATMEL_US_CHMODE_NORMAL;
#endif
	/* Resetting serial mode to RS232 (0x0) */
//	mode &= ~ATMEL_US_USMODE;

	if (ait_port->rs485.flags & SER_RS485_ENABLED) {
		dev_dbg(port->dev, "Setting UART to RS485\n");
		if ((ait_port->rs485.delay_rts_after_send) > 0)
			UART_PUT_TTGR(port,
			        ait_port->rs485.delay_rts_after_send);
//		mode |= ATMEL_US_USMODE_RS485;
	} else {
		dev_dbg(port->dev, "Setting UART to RS232\n");
	}
//	UART_PUT_MR(port, mode);
}

/*
 * Get state of the modem control input lines
 */
static u_int ait_get_mctrl(struct uart_port *port)
{
	return 0;
}

/*
 * Stop transmitting.
 */
static void ait_stop_tx(struct uart_port *port)
{
	//struct ait_uart_port *ait_port = to_ait_uart_port(port);

	if (ait_use_dma_tx(port)) {
		/* disable PDC transmit */
		UART_PUT_PTCR(port, ATMEL_PDC_TXTDIS);
	}
	/* Disable interrupts */
	UART_PUT_IDR(port, ait_port->tx_done_mask);

//	if ((ait_port->rs485.flags & SER_RS485_ENABLED) &&
//	    !(ait_port->rs485.flags & SER_RS485_RX_DURING_TX))
//		ait_start_rx(port);
}

/*
 * Start transmitting.
 */
static void ait_start_tx(struct uart_port *port)
{
	struct ait_uart_port *ait_port = to_ait_uart_port(port);

	tasklet_schedule(&ait_port->tasklet);
}

/*
 * Stop receiving - port is in process of being closed.
 */
static void ait_stop_rx(struct uart_port *port)
{
}

/*
 * Enable modem status interrupts
 */
static void ait_enable_ms(struct uart_port *port)
{
	UART_PUT_IER(port, ATMEL_US_RIIC | ATMEL_US_DSRIC
			| ATMEL_US_DCDIC | ATMEL_US_CTSIC);
}

/*
 * Control the transmission of a break signal
 */
static void ait_break_ctl(struct uart_port *port, int break_state)
{
	if (break_state != 0)
		;/* start break */
	else
		;/* stop break */
}

/*
 * Characters received (called from interrupt handler)
 */
static void ait_rx_chars(struct uart_port *port)
{
	struct ait_uart_port *ait_port = to_ait_uart_port(port);
    struct circ_buf *ring = &ait_port->rx_ring;
    struct ait_uart_chdesc *c;
	unsigned int nr;
	unsigned char ch;

    while ((nr = MMPF_Uart_TryRead(DEBUG_UART_NUM, &ch, 1)) != 0) {
        if (!CIRC_SPACE(ring->head, ring->tail, AIT_RX_RINGSIZE)) {
            /* Buffer overflow, ignore char */
            break;
        }

        c = &((struct ait_uart_chdesc *)ring->buf)[ring->head];
        c->status   = 0;//reserved ATMEL_US_RXRDY;
        c->ch       = ch;

        /* Make sure the character is stored before we update head. */
        smp_wmb();

        ring->head = (ring->head + 1) & (AIT_RX_RINGSIZE - 1);
        //atmel_buffer_rx_char(port, ATMEL_US_RXRDY, ch);
    }

	tasklet_schedule(&ait_port->tasklet);
}

/*
 * Transmit characters (called from tasklet with TXRDY interrupt
 * disabled)
 */
 static void ait_tx_chars(struct uart_port *port)
{
    struct circ_buf *xmit = &port->state->xmit;

    if (unlikely(port->x_char)) {
        MMPF_Uart_Write(DEBUG_UART_NUM, (const char*)&(port->x_char), 1);

        port->icount.tx++;
        port->x_char = 0;
    }

    if (uart_circ_empty(xmit) || uart_tx_stopped(port))
        return;

    while (!uart_circ_empty(xmit)) {
        //UART_PUT_CHAR(port, xmit->buf[xmit->tail]);
        MMPF_Uart_Write(DEBUG_UART_NUM, (const char*)(xmit->buf+xmit->tail), 1);

        xmit->tail = (xmit->tail + 1) & (UART_XMIT_SIZE - 1);
        port->icount.tx++;
    }

    if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
        uart_write_wakeup(port);

//	if (!uart_circ_empty(xmit))
//    /* Enable interrupts */
//		UART_PUT_IER(port, ait_port->tx_done_mask);
}

/*
 * Interrupt handler
 */
static irqreturn_t ait_uart_int_handler(int irq, void *dev_id)
{
    struct uart_port *port = dev_id;

//    MMPF_Uart_SetInterruptEnable(DEBUG_UART_NUM, MMPF_UART_EVENT_RXFIFO_OVERTH, MMP_FALSE);
    if (MMPF_Uart_CheckState(DEBUG_UART_NUM, MMPF_UART_STAT_RXOVERTH)) {

        ait_rx_chars(port);
//        tasklet_schedule(&ait_port->tasklet);
//        tasklet_schedule(&ait_port->tasklet);
    }
//    MMPF_Uart_SetInterruptEnable(DEBUG_UART_NUM, MMPF_UART_EVENT_RXFIFO_OVERTH, MMP_TRUE);

    return IRQ_HANDLED;
}

static void ait_rx_from_ring(struct uart_port *port)
{
	struct ait_uart_port *ait_port = to_ait_uart_port(port);
	struct circ_buf *ring = &ait_port->rx_ring;
	unsigned int flg;
	unsigned int status;

	while (ring->head != ring->tail) {
		struct ait_uart_chdesc c;

		/* Make sure c is loaded after head. */
		smp_rmb();

		c = ((struct ait_uart_chdesc *)ring->buf)[ring->tail];

		ring->tail = (ring->tail + 1) & (AIT_RX_RINGSIZE - 1);

		port->icount.rx++;
		status = c.status;
		flg = TTY_NORMAL;

		if (uart_handle_sysrq_char(port, c.ch))
			continue;

		uart_insert_char(port, status, AIT_US_RXOVFW, c.ch, flg);//ATMEL_US_RXRDY
	}

	/*
	 * Drop the lock here since it might end up calling
	 * uart_start(), which takes the lock.
	 */
	spin_unlock(&port->lock);
	tty_flip_buffer_push(port->state->port.tty);
	spin_lock(&port->lock);
}

/*
 * tasklet handling tty stuff outside the interrupt handler.
 */
static void ait_uart_tasklet_func(unsigned long data)
{
	struct uart_port *port = (struct uart_port *)data;

	/* The interrupt handler does not take the lock */
	spin_lock(&port->lock);

	ait_tx_chars(port);

	wake_up_interruptible(&port->state->port.delta_msr_wait);

    ait_rx_from_ring(port);

	spin_unlock(&port->lock);
}

/*
 * Perform initialization and enable port for reception
 */
static int ait_startup(struct uart_port *port)
{
	struct tty_struct *tty = port->state->port.tty;
	int retval;

	MMPF_Uart_SetInterruptEnable(DEBUG_UART_NUM, MMPF_UART_EVENT_RXFIFO_OVERTH,
	                                MMP_TRUE);
	MMPF_Uart_Reset(DEBUG_UART_NUM, MMPF_UART_DIRECTION_RX);
	/*
	 * Ensure that no interrupts are enabled otherwise when
	 * request_irq() is called we could get stuck trying to
	 * handle an unexpected interrupt
	 */
	//UART_PUT_IDR(port, -1);

	/*
	 * Allocate the IRQ
	 */
	retval = request_irq(port->irq, ait_uart_int_handler, IRQF_SHARED,
			tty ? tty->name : "ait_serial", port);
	if (retval) {
		printk("ait_serial: ait_startup - Can't get irq\n");
		return retval;
	}

	/*
	 * If there is a specific "open" function (to register
	 * control line interrupts)
	 */
	if (ait_open_hook) {
		retval = ait_open_hook(port);
		if (retval) {
			free_irq(port->irq, port);
			return retval;
		}
	}

	/* Save current CSR for comparison in ait_uart_tasklet_func() */

	/*
	 * Finally, enable the serial port
	 */

	return 0;
}

/*
 * Disable the port
 */
static void ait_shutdown(struct uart_port *port)
{	
	/*
	 * Ensure everything is stopped.
	 */
	ait_stop_rx(port);
	ait_stop_tx(port);

	/*
	 * Free the interrupt
	 */
	free_irq(port->irq, port);

	/*
	 * If there is a specific "close" function (to unregister
	 * control line interrupts)
	 */
	if (ait_close_hook)
	    ait_close_hook(port);
}

/*
 * Flush any TX data submitted for DMA. Called when the TX circular
 * buffer is reset.
 */
static void ait_flush_buffer(struct uart_port *port)
{
}

/*
 * Power / Clock management.
 */
static void ait_serial_pm(struct uart_port *port, unsigned int state,
			    unsigned int oldstate)
{
	struct ait_uart_port *ait_port = to_ait_uart_port(port);

	switch (state) {
	case 0:
		/*
		 * Enable the peripheral clock for this serial port.
		 * This is called on uart_open() or a resume event.
		 */
		clk_enable(ait_port->clk);

		/* re-enable interrupts if we disabled some on suspend */
		UART_PUT_IER(port, ait_port->backup_imr);
		break;
	case 3:
		/* Back up the interrupt mask and disable all interrupts */
//		ait_port->backup_imr = UART_GET_IMR(port);
//		UART_PUT_IDR(port, -1);

		/*
		 * Disable the peripheral clock for this serial port.
		 * This is called on uart_close() or a suspend event.
		 */
		clk_disable(ait_port->clk);
		break;
	default:
		printk(KERN_ERR "ait_serial: unknown pm %d\n", state);
	}
}

/*
 * Change the port parameters
 */
static void ait_set_termios(struct uart_port *port, struct ktermios *termios,
			      struct ktermios *old)
{
	unsigned long flags;
	unsigned int quot, baud;

	baud = uart_get_baud_rate(port, termios, old, 0, port->uartclk / 16);
	quot = uart_get_divisor(port, baud);
#if 0
	if (quot > 65535) {	/* BRGR is 16-bit, so switch to slower clock */
		quot /= 8;
		mode |= ATMEL_US_USCLKS_MCK_DIV8;
	}

	/* byte size */
	switch (termios->c_cflag & CSIZE) {
	case CS5:
		mode |= ATMEL_US_CHRL_5;
		break;
	case CS6:
		mode |= ATMEL_US_CHRL_6;
		break;
	case CS7:
		mode |= ATMEL_US_CHRL_7;
		break;
	default:
		mode |= ATMEL_US_CHRL_8;
		break;
	}

	/* stop bits */
	if (termios->c_cflag & CSTOPB)
		mode |= ATMEL_US_NBSTOP_2;

	/* parity */
	if (termios->c_cflag & PARENB) {
		/* Mark or Space parity */
		if (termios->c_cflag & CMSPAR) {
			if (termios->c_cflag & PARODD)
				mode |= ATMEL_US_PAR_MARK;
			else
				mode |= ATMEL_US_PAR_SPACE;
		} else if (termios->c_cflag & PARODD)
			mode |= ATMEL_US_PAR_ODD;
		else
			mode |= ATMEL_US_PAR_EVEN;
	} else
		mode |= ATMEL_US_PAR_NONE;

	/* hardware handshake (RTS/CTS) */
	if (termios->c_cflag & CRTSCTS)
		mode |= ATMEL_US_USMODE_HWHS;
	else
		mode |= ATMEL_US_USMODE_NORMAL;
#endif
	spin_lock_irqsave(&port->lock, flags);
#if 0
	port->read_status_mask = ATMEL_US_OVRE;
	if (termios->c_iflag & INPCK)
		port->read_status_mask |= (ATMEL_US_FRAME | ATMEL_US_PARE);
	if (termios->c_iflag & (BRKINT | PARMRK))
		port->read_status_mask |= ATMEL_US_RXBRK;

	/*
	 * Characters to ignore
	 */
	port->ignore_status_mask = 0;
	if (termios->c_iflag & IGNPAR)
		port->ignore_status_mask |= (ATMEL_US_FRAME | ATMEL_US_PARE);
	if (termios->c_iflag & IGNBRK) {
		port->ignore_status_mask |= ATMEL_US_RXBRK;
		/*
		 * If we're ignoring parity and break indicators,
		 * ignore overruns too (for real raw support).
		 */
		if (termios->c_iflag & IGNPAR)
			port->ignore_status_mask |= ATMEL_US_OVRE;
	}
	/* TODO: Ignore all characters if CREAD is set.*/

	/* update the per-port timeout */
	uart_update_timeout(port, termios->c_cflag, baud);

	/*
	 * save/disable interrupts. The tty layer will ensure that the
	 * transmitter is empty if requested by the caller, so there's
	 * no need to wait for it here.
	 */
//	imr = UART_GET_IMR(port);
//	UART_PUT_IDR(port, -1);

	/* disable receiver and transmitter */
//	UART_PUT_CR(port, ATMEL_US_TXDIS | ATMEL_US_RXDIS);

	/* Resetting serial mode to RS232 (0x0) */
	mode &= ~ATMEL_US_USMODE;

	/* set the parity, stop bits and data size */
	UART_PUT_MR(port, mode);

	/* set the baud rate */
	UART_PUT_BRGR(port, quot);
	UART_PUT_CR(port, ATMEL_US_RSTSTA | ATMEL_US_RSTRX);
	UART_PUT_CR(port, ATMEL_US_TXEN | ATMEL_US_RXEN);

	/* restore interrupts */
	UART_PUT_IER(port, imr);
#endif
	/* CTS flow-control and modem-status interrupts */
	if (UART_ENABLE_MS(port, termios->c_cflag))
		port->ops->enable_ms(port);

	spin_unlock_irqrestore(&port->lock, flags);
}

static void ait_set_ldisc(struct uart_port *port, int new)
{
	int line = port->line;

	if (line >= port->state->port.tty->driver->num)
		return;

	if (port->state->port.tty->ldisc->ops->num == N_PPS) {
		port->flags |= UPF_HARDPPS_CD;
		ait_enable_ms(port);
	} else {
		port->flags &= ~UPF_HARDPPS_CD;
	}
}

/*
 * Return string describing the specified port
 */
static const char *ait_type(struct uart_port *port)
{
	return (port->type == PORT_ATMEL) ? "ATMEL_SERIAL" : NULL;
}

/*
 * Release the memory region(s) being used by 'port'.
 */
static void ait_release_port(struct uart_port *port)
{
	struct platform_device *pdev = to_platform_device(port->dev);
	int size = pdev->resource[0].end - pdev->resource[0].start + 1;

	release_mem_region(port->mapbase, size);

	if (port->flags & UPF_IOREMAP) {
		iounmap(port->membase);
		port->membase = NULL;
	}
}

/*
 * Request the memory region(s) being used by 'port'.
 */
static int ait_request_port(struct uart_port *port)
{
	struct platform_device *pdev = to_platform_device(port->dev);
	int size = pdev->resource[0].end - pdev->resource[0].start + 1;

	if (!request_mem_region(port->mapbase, size, "ait_serial"))
		return -EBUSY;

	if (port->flags & UPF_IOREMAP) {
		port->membase = ioremap(port->mapbase, size);
		if (port->membase == NULL) {
			release_mem_region(port->mapbase, size);
			return -ENOMEM;
		}
	}

	return 0;
}

/*
 * Configure/autoconfigure the port.
 */
static void ait_config_port(struct uart_port *port, int flags)
{
	if (flags & UART_CONFIG_TYPE) {
		port->type = PORT_ATMEL;
		ait_request_port(port);
	}
}

/*
 * Verify the new serial_struct (for TIOCSSERIAL).
 */
static int ait_verify_port(struct uart_port *port, struct serial_struct *ser)
{
	int ret = 0;
	if (ser->type != PORT_UNKNOWN && ser->type != PORT_ATMEL)
		ret = -EINVAL;
	if (port->irq != ser->irq)
		ret = -EINVAL;
	if (ser->io_type != SERIAL_IO_MEM)
		ret = -EINVAL;
	if (port->uartclk / 16 != ser->baud_base)
		ret = -EINVAL;
	if ((void *)port->mapbase != ser->iomem_base)
		ret = -EINVAL;
	if (port->iobase != ser->port)
		ret = -EINVAL;
	if (ser->hub6 != 0)
		ret = -EINVAL;
	return ret;
}

#ifdef CONFIG_CONSOLE_POLL
static int ait_poll_get_char(struct uart_port *port)
{
    MMP_UBYTE ch = 0;

    //while (!(UART_GET_CSR(port) & ATMEL_US_RXRDY))
    while (MMPF_Uart_TryRead(DEBUG_UART_NUM, &ch, 1) == 0) {
		cpu_relax();
    }
	return ch;//UART_GET_CHAR(port);
}

static void ait_poll_put_char(struct uart_port *port, unsigned char ch)
{
    //while (!(UART_GET_CSR(port) & ATMEL_US_TXRDY))
    while (!MMPF_Uart_CheckState(DEBUG_UART_NUM, MMPF_UART_STAT_TXUNDERTH))
	    cpu_relax();

	//UART_PUT_CHAR(port, ch);
    MMPF_Uart_Write(DEBUG_UART_NUM, &ch, 1);
}
#endif

//static int
//ait_ioctl(struct uart_port *port, unsigned int cmd, unsigned long arg)
//{
//	struct serial_rs485 rs485conf;
//
//	switch (cmd) {
//	case TIOCSRS485:
//		if (copy_from_user(&rs485conf, (struct serial_rs485 *) arg,
//					sizeof(rs485conf)))
//			return -EFAULT;
//		break;
//
//	case TIOCGRS485:
//		if (copy_to_user((struct serial_rs485 *) arg,
//					&(to_ait_uart_port(port)->rs485),
//					sizeof(rs485conf)))
//			return -EFAULT;
//		break;
//
//	default:
//		return -ENOIOCTLCMD;
//	}
//	return 0;
//}

static struct uart_ops ait_pops = {
	.tx_empty	= ait_tx_empty,
	.set_mctrl	= ait_set_mctrl,
	.get_mctrl	= ait_get_mctrl,
	.stop_tx	= ait_stop_tx,
	.start_tx	= ait_start_tx,//used
	.stop_rx	= ait_stop_rx,
	.enable_ms	= ait_enable_ms,
	.break_ctl	= ait_break_ctl,
	.startup	= ait_startup,//used
	.shutdown	= ait_shutdown,//used
	.flush_buffer	= ait_flush_buffer,
	.set_termios	= ait_set_termios,//used
	.set_ldisc	= ait_set_ldisc,
	.type		= ait_type,
	.release_port	= ait_release_port,
	.request_port	= ait_request_port,
	.config_port	= ait_config_port,
	.verify_port	= ait_verify_port,
	.pm		    = ait_serial_pm,
	//.ioctl		= ait_ioctl,
#ifdef CONFIG_CONSOLE_POLL
	.poll_get_char	= ait_poll_get_char,
	.poll_put_char	= ait_poll_put_char,
#endif
};

static void __devinit ait_of_init_port(struct ait_uart_port *ait_port,
					 struct device_node *np)
{
	u32 rs485_delay[2];

	/* DMA/PDC usage specification */
	//if (of_get_property(np, "atmel,use-dma-rx", NULL))
	//    ait_port->use_dma_rx	= 1;
	//else
	//    ait_port->use_dma_rx	= 0;
	//if (of_get_property(np, "atmel,use-dma-tx", NULL))
	//    ait_port->use_dma_tx	= 1;
	//else
	//    ait_port->use_dma_tx	= 0;

	/* rs485 properties */
	if (of_property_read_u32_array(np, "rs485-rts-delay",
					    rs485_delay, 2) == 0) {
		struct serial_rs485 *rs485conf = &ait_port->rs485;

		rs485conf->delay_rts_before_send = rs485_delay[0];
		rs485conf->delay_rts_after_send = rs485_delay[1];
		rs485conf->flags = 0;

		if (of_get_property(np, "rs485-rx-during-tx", NULL))
			rs485conf->flags |= SER_RS485_RX_DURING_TX;

		if (of_get_property(np, "linux,rs485-enabled-at-boot-time", NULL))
			rs485conf->flags |= SER_RS485_ENABLED;
	}
}

/*
 * Configure the port from the platform device resource info.
 */
static void __devinit ait_init_port(struct ait_uart_port *ait_port,
				      struct platform_device *pdev)
{
	struct uart_port *port = &ait_port->uart;
	struct atmel_uart_data *pdata = pdev->dev.platform_data;

	if (pdev->dev.of_node) {
		ait_of_init_port(ait_port, pdev->dev.of_node);
	} else {
	    ait_port->rs485	= pdata->rs485;
	}

	port->iotype    = UPIO_MEM;
	port->flags		= UPF_BOOT_AUTOCONF;
	port->ops		= &ait_pops;
	port->fifosize  = 1;
	port->dev		= &pdev->dev;
	port->mapbase	= pdev->resource[0].start;
	port->irq	    = pdev->resource[1].start;

	tasklet_init(&ait_port->tasklet, ait_uart_tasklet_func,
			(unsigned long)port);

	memset(&ait_port->rx_ring, 0, sizeof(ait_port->rx_ring));

	if (pdata && pdata->regs) {
		/* Already mapped by setup code */
		port->membase = pdata->regs;
	} else {
		port->flags	|= UPF_IOREMAP;
		port->membase	= NULL;
	}

	/* for console, the clock could already be configured */

	if (!ait_port->clk) {
	    ait_port->clk = clk_get(&pdev->dev, "usart");
		clk_enable(ait_port->clk);
		port->uartclk = clk_get_rate(ait_port->clk);
		clk_disable(ait_port->clk);
		/* only enable clock when USART is in use */
	}

	/* Use TXEMPTY for interrupt when rs485 else TXRDY or ENDTX|TXBUFE */
//	if (ait_port->rs485.flags & SER_RS485_ENABLED)
//	    ait_port->tx_done_mask = ATMEL_US_TXEMPTY;
//	else if (ait_use_dma_tx(port)) {
//		port->fifosize = PDC_BUFFER_SIZE;
//		ait_port->tx_done_mask = ATMEL_US_ENDTX | ATMEL_US_TXBUFE;
//	} else {
//	    ait_port->tx_done_mask = ATMEL_US_TXRDY;
//	}

	ait_port->tx_done_mask = ATMEL_US_TXRDY;
}

/*
 * Register board-specific modem-control line handlers.
 */
void __init ait_register_uart_fns(struct atmel_port_fns *fns)
{
	if (fns->enable_ms)
	    ait_pops.enable_ms = fns->enable_ms;
	if (fns->get_mctrl)
	    ait_pops.get_mctrl = fns->get_mctrl;
	if (fns->set_mctrl)
	    ait_pops.set_mctrl = fns->set_mctrl;
	ait_open_hook		= fns->open;
	ait_close_hook	    = fns->close;
	ait_pops.pm		    = fns->pm;
	ait_pops.set_wake	= fns->set_wake;
}

#ifdef CONFIG_SERIAL_AIT_CONSOLE

extern void putstr(const char *ptr);

static void ait_console_putchar(struct uart_port *port, int ch)
{
    MMP_UBYTE c = ch;

    while (!MMPF_Uart_CheckState(DEBUG_UART_NUM, MMPF_UART_STAT_TXEMPTY)) {
        cpu_relax();
    }

    MMPF_Uart_Write(DEBUG_UART_NUM, &c, 1);
}


/*
 * Interrupts are disabled on entering
 */
static void ait_console_write(struct console *co, const char *s, u_int count)
{
	struct uart_port *port = &ait_usports[co->index].uart;

	/*
	 * First, save IMR and then disable interrupts
	 */
    #if 0
	imr = UART_GET_IMR(port);
	UART_PUT_IDR(port, ATMEL_US_RXRDY | ait_port->tx_done_mask);

	/* Store PDC transmit status and disable it */
	pdc_tx = UART_GET_PTSR(port) & ATMEL_PDC_TXTEN;
	UART_PUT_PTCR(port, ATMEL_PDC_TXTDIS);
    #endif
	uart_console_write(port, s, count, ait_console_putchar);

	/*
	 * Finally, wait for transmitter to become empty
	 * and restore IMR
	 */
    #if 0
	do {
		status = UART_GET_CSR(port);
	} while (!(status & ATMEL_US_TXRDY));

	/* Restore PDC transmit status */
	if (pdc_tx)
		UART_PUT_PTCR(port, ATMEL_PDC_TXTEN);

	/* set interrupts back the way they were */
	UART_PUT_IER(port, imr);
    #endif
}

static int __init ait_console_setup(struct console *co, char *options)
{
	struct uart_port *port = &ait_usports[co->index].uart;
	int baud = 115200;
	int bits = 8;
	int parity = 'n';
	int flow = 'n';

	if (port->membase == NULL) {
		/* Port not initialized yet - delay setup */
		return -ENODEV;
	}

//	clk_enable(ait_usports[co->index].clk);

//	UART_PUT_IDR(port, -1);
//	UART_PUT_CR(port, ATMEL_US_RSTSTA | ATMEL_US_RSTRX);
//	UART_PUT_CR(port, ATMEL_US_TXEN | ATMEL_US_RXEN);

	if (options) {
		uart_parse_options(options, &baud, &parity, &bits, &flow);
	} else {
        /*
         * If the port was already initialised (eg, by a boot loader),
         * we may determine from the current HW setup. TBD.
         */
	}

	return uart_set_options(port, co, baud, parity, bits, flow);
}

static struct uart_driver ait_uart;

static struct console ait_console = {
	.name		= AIT_CONSOLE_DEVICE,
	.write		= ait_console_write,
	.device		= uart_console_device,
	.setup		= ait_console_setup,
	.flags		= CON_PRINTBUFFER|CON_PRINTBUFFER|CON_CONSDEV|CON_ENABLED/*|CON_BOOT*/|CON_ANYTIME|CON_BRL,
	.index		= -1,
	.data		= &ait_uart,
};

/*
 * Early console initialization (before VM subsystem initialized).
 */
static int __init ait_console_init(void)
{
	if (ait_default_console_device) {
		struct atmel_uart_data *pdata =
			ait_default_console_device->dev.platform_data;
		int id = pdata->num;
		struct ait_uart_port *port = &ait_usports[id];

		port->backup_imr = 0;
		port->uart.line = id;

		add_preferred_console(AIT_CONSOLE_DEVICE, id, NULL);
		ait_init_port(port, ait_default_console_device);
		register_console(&ait_console);
	}

	return 0;
}

console_initcall(ait_console_init);

/*
 * Late console initialization.
 */
static int __init ait_late_console_init(void)
{
	if (ait_default_console_device
	    && !(ait_console.flags & CON_ENABLED))
		register_console(&ait_console);

	return 0;
}

core_initcall(ait_late_console_init);

static inline bool ait_is_console_port(struct uart_port *port)
{
	return port->cons && port->cons->index == port->line;
}

#else
x

static inline bool ait_is_console_port(struct uart_port *port)
{
	return false;
}
#endif

static struct uart_driver ait_uart = {
	.owner		= THIS_MODULE,
	.driver_name	= "ait_serial",//"atmel_serial",
	.dev_name	= AIT_CONSOLE_DEVICE,
	.major		= SERIAL_AIT_MAJOR,
	.minor		= MINOR_START,
	.nr		    = ATMEL_MAX_UART,
	.cons		= (&ait_console),
};

#ifdef CONFIG_PM

static int ait_serial_suspend(struct platform_device *pdev,
				pm_message_t state)
{
	struct uart_port *port = platform_get_drvdata(pdev);
	struct ait_uart_port *ait_port = to_ait_uart_port(port);

    #if 0
	if (ait_is_console_port(port) && console_suspend_enabled) {
		/* Drain the TX shifter */
	}
    #endif
	/* we can not wake up if we're running on slow clock */
	ait_port->may_wakeup = device_may_wakeup(&pdev->dev);
	if (false)
		device_set_wakeup_enable(&pdev->dev, 0);

	uart_suspend_port(&ait_uart, port);

	return 0;
}

static int ait_serial_resume(struct platform_device *pdev)
{
	struct uart_port *port = platform_get_drvdata(pdev);
	struct ait_uart_port *ait_port = to_ait_uart_port(port);

    #if 0
	uart_resume_port(&ait_uart, port);
    #endif
	device_set_wakeup_enable(&pdev->dev, ait_port->may_wakeup);

	return 0;
}
#else
#define ait_serial_suspend NULL
#define ait_serial_resume NULL
#endif

static int __devinit ait_serial_probe(struct platform_device *pdev)
{
	struct ait_uart_port *port;
	struct device_node *np = pdev->dev.of_node;
	struct atmel_uart_data *pdata = pdev->dev.platform_data;
	void *data;
	int ret = -ENODEV;

	BUILD_BUG_ON(AIT_RX_RINGSIZE & (AIT_RX_RINGSIZE - 1));

	if (np)
		ret = of_alias_get_id(np, "serial");
	else
		if (pdata)
			ret = pdata->num;

	if (ret < 0)
		/* port id not found in platform data nor device-tree aliases:
		 * auto-enumerate it */
		ret = find_first_zero_bit(&ait_usports_in_use,
				sizeof(ait_usports_in_use));

	if (ret > ATMEL_MAX_UART) {
		ret = -ENODEV;
		goto err;
	}

	if (test_and_set_bit(ret, &ait_usports_in_use)) {
		/* port already in use */
		ret = -EBUSY;
		goto err;
	}

	port = &ait_usports[ret];
	port->backup_imr = 0;
	port->uart.line = ret;

	ait_init_port(port, pdev);

    ret = -ENOMEM;
    data = kmalloc(sizeof(struct ait_uart_chdesc)
            * AIT_RX_RINGSIZE, GFP_KERNEL);
    if (!data)
        goto err_alloc_ring;
    port->rx_ring.buf = data;

	ret = uart_add_one_port(&ait_uart, &port->uart);
	if (ret)
		goto err_add_port;

    #ifdef CONFIG_SERIAL_AIT_CONSOLE
	if (ait_is_console_port(&port->uart)
			&& ait_console.flags & CON_ENABLED) {
		/*
		 * The serial core enabled the clock for us, so undo
		 * the clk_enable() in ait_console_setup()
		 */
		clk_disable(port->clk);
	}
    #endif

	device_init_wakeup(&pdev->dev, 1);
	platform_set_drvdata(pdev, port);

	return 0;

err_add_port:
	kfree(port->rx_ring.buf);
	port->rx_ring.buf = NULL;
err_alloc_ring:
	if (!ait_is_console_port(&port->uart)) {
		clk_put(port->clk);
		port->clk = NULL;
	}
err:
	return ret;
}

static int __devexit ait_serial_remove(struct platform_device *pdev)
{
	struct uart_port *port = platform_get_drvdata(pdev);
	struct ait_uart_port *ait_port = to_ait_uart_port(port);
	int ret = 0;

	device_init_wakeup(&pdev->dev, 0);
	platform_set_drvdata(pdev, NULL);

	ret = uart_remove_one_port(&ait_uart, port);

	tasklet_kill(&ait_port->tasklet);
	kfree(ait_port->rx_ring.buf);

	/* "port" is allocated statically, so we shouldn't free it */

	clear_bit(port->line, &ait_usports_in_use);

	clk_put(ait_port->clk);

	return ret;
}

static struct platform_driver ait_serial_driver = {
	.probe		= ait_serial_probe,
	.remove		= __devexit_p(ait_serial_remove),
	.suspend	= ait_serial_suspend,
	.resume		= ait_serial_resume,
	.driver		= {
		.name	= "ait_usart",
		.owner	= THIS_MODULE,
		.of_match_table	= of_match_ptr(ait_serial_dt_ids),
	},
};

static int __init ait_serial_init(void)
{
	int ret;

	ret = uart_register_driver(&ait_uart);
	if (ret)
		return ret;

	ret = platform_driver_register(&ait_serial_driver);
	if (ret)
		uart_unregister_driver(&ait_uart);

	return ret;
}

static void __exit ait_serial_exit(void)
{
	platform_driver_unregister(&ait_serial_driver);
	uart_unregister_driver(&ait_uart);
}

module_init(ait_serial_init);
module_exit(ait_serial_exit);

MODULE_AUTHOR("Vincent Chen");
MODULE_DESCRIPTION("AIT Vision V3 serial port driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ait_usart");
