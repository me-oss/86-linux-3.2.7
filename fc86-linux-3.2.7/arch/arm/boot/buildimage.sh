#!/bin/sh
~/customer/u-boot/u-boot-2013.01/tools/mkimage -A arm -O linux -C gzip -n 'linux-3.2.7_AIT8453' -T kernel -a 0x1000000 -e 0x1000000 -d zImage uImage