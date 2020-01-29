#!/bin/bash

check_usb_to_ttl() {
	usb_to_ttl=$(ls /dev | grep -E "^ttyUSB[0-9]+$")
	if [ -n "$usb_to_ttl" ];
	then
		echo "A USB to TTL convertor is connected."
	fi
}

check_flash_drives() {
	for l in "$(ls /dev/sd[a-z])";
	do
		if [ -n "$(udevadm info --query=all --name=${l} | grep 'ID_BUS=usb')" ];
		then
			echo "A USB flash drive is connected."
		fi
	done
}

check_usb_to_ttl
check_flash_drives
exit 0
