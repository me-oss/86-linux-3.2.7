#!/bin/sh
#export CROSS_COMPILE=/opt/buildroot-vsnv3_2014q1/bin/arm-buildroot-linux-uclibcgnueabi-
make distclean
make ait8455evb_defconfig
make uImage
sync
