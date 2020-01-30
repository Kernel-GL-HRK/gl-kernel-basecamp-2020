#!/bin/bash

check_usb_to_ttl() {
	usb_to_ttl=$(ls /dev | grep -E "^ttyUSB[0-9]+$")
	if [ -n "$usb_to_ttl" ];
	then
		echo "A USB to TTL convertor is connected:"
		echo "${usb_to_ttl}"
	fi
}

check_flash_drives() {
	for l in $(ls /dev/sd[a-z]);
	do
		if [ -n "$(udevadm info --query=all --name=${l} | grep 'ID_BUS=usb')" ];
		then
			echo "A USB flash drive is connected: ${l}"
		fi
	done
}

check_sd_card() {
	sd_card=$(ls /dev | grep -E "^mmcblk[0-9]+$")
	if [ -n "$sd_card" ];
	then
		echo "An SD card is connected:"
		echo "${sd_card}"
	fi
}

check_i2c_devices() {
	for ((i=1; i<=$(i2cdetect -l | wc -l); i++));
	do
		i2c_bus=$(i2cdetect -y ${i} | awk '{if (NR!=1) {print}}' | awk '{$1=""}1')
		if [ -n "$(echo $i2c_bus | grep '[0-9]')" ];
		then
			echo "An i2c device is connected"
		fi
	done
}

check_usb_to_ttl
check_flash_drives
check_sd_card
check_i2c_devices
exit 0
