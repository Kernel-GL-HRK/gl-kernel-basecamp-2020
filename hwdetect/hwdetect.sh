#!/bin/bash

#ls directory
dir="/dev"

#grep option
opt="-w" #ignore slaves or holders

#grep paterns
ttyusb="ttyUSB['0'-'9']"
flash="sd['b'-'z']" #ignore the first device - "sda"
sdcard="mmcblk['0'-'9']"

echo "Device directory: $dir"
echo "1 - TTY USB:"
ls $dir | grep $ttyusb
echo "2 - Flash devices:"
ls $dir | grep $opt $flash
echo "3 - SD cards:"
ls $dir | grep $opt $sdcard

exit $RETURN_SUCCES
