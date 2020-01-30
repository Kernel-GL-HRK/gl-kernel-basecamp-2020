#!/bin/bash

echo "For monitoring hardware, choose please \n 1 - USB devices \n 2 - I2C devices"
read value
case $value in
       1) lsusb;;
       2) i2cdetect -l;;
esac
break
