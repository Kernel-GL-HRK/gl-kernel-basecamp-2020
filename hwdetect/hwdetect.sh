#!/bin/bash

check_usb_to_ttl() {
	usb_to_ttl=$(ls /dev | grep -E "^ttyUSB[0-9]+$")
	if [ -n "$usb_to_ttl" ];
	then
		echo "A USB to TTL convertor is connected."
	fi
}

check_usb_to_ttl
exit 0
