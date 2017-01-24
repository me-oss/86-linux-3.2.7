#
# Makefile for the Linux Bluetooth HCI device drivers.
#

obj-$(CONFIG_BT_HCIUART)	+= hci_uart.o
hci_uart-y				:= hci_ldisc.o
hci_uart-$(CONFIG_BT_HCIUART_H4)	+= hci_h4.o
hci_uart-$(CONFIG_BT_HCIUART_RTKH5)	+= hci_rtk_h5.o rtk_coex.o
hci_uart-objs				:= $(hci_uart-y)

#ccflags-y += -D__CHECK_ENDIAN__
