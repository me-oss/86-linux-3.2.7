#
# Makefile for the linux kernel.
#

obj-y		:= vsnv3_gpio.o irq.o gpio_file.o setup.o
obj-m		:=
obj-n		:=
obj-		:=

obj-$(CONFIG_AT91_PMC_UNIT)	+= clock.o

obj-$(CONFIG_ARCH_VSNV3AIT845X)	+= vsnv3ait84.o vsnv3_time.o vsnv3ait84_devices.o 
obj-$(CONFIG_ARCH_VSNV3AIT845X)	+= vsnv3_afe.o mmpf_i2cm.o mmpf_pio.o mmpf_vif.o mmpf_wd.o mmpf_system.o  mmpf_i2s_ctl.o ait_if.o
obj-$(CONFIG_ARCH_VSNV3AIT845X)	+= mmpf_pll.o ait_export_arch.o #mmpf_timer.o
obj-$(CONFIG_ARCH_VSNV3AIT845X)	+= mmpf_uart.o
obj-$(CONFIG_BOARD_VSNV3_AIT8455EVB) += board-ait8455evb.o
obj-$(CONFIG_BOARD_VSNV3_AIT8455CLOUD) += board-ait8455cloud.o




# Drivers
obj-y				+= leds.o
obj-$(CONFIG_HIGH_RES_TIMERS)	+= vsnv3_hrtime.o 
# Power Management
obj-$(CONFIG_PM)		+= pm.o
obj-$(CONFIG_AT91_SLOW_CLOCK)	+= pm_slowclock.o
obj-$(CONFIG_CPU_IDLE)	+= cpuidle.o

ifeq ($(CONFIG_PM_DEBUG),y)
CFLAGS_pm.o += -DDEBUG
endif
