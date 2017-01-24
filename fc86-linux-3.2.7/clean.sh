#!/bin/sh
#export CROSS_COMPILE=/home/vin/linux/kernel/buildroot-2013.11/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-
make distclean
make clean
make mrproper
sync
