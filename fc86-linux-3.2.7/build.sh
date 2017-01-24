#!/bin/sh
#export CROSS_COMPILE=/opt/buildroot-vsnv3_2014q1/bin/arm-linux-
export CROSS_COMPILE=/home/tho/AIT_TOYCAM/buildroot-ait84_201402v1/bin/arm-linux-
#make distclean
#make ait8455evb_defconfig
make cvision_defconfig
#make uImage -j 3
make -j 2 uImage 
#make modules
sync
