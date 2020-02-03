#!/bin/bash

USB_CVT=1
BLK_DEV=2
OTHER=3

STATUS_OK=0

function detrDevice
{
	local dev=$1

	if [[ "$dev" == *"ttyUSB"* ]]
	then
		return $USB_CVT
	fi

	if [[ "$dev" == *"sd"? ]]
	then
		return $BLK_DEV
	fi

	return $OTHER
}

function printInfo
{
	IFS=$'\n'
	cur=${1}
	for dev in $cur
	do
		echo $dev
		detrDevice $dev

		case $? in
			$USB_CVT)
			       	echo "Usb to ttl convertor"
				;;
			$BLK_DEV)
			       	echo "Block device"
				sudo lshw -short | grep ${dev//[<> ]} -m 1
				;;
			$OTHER)
			       	continue
				;;
		esac
        
	done
}

function main
{
    devs=`ls /dev/`
    difs=""

    while [ true ]
    do
	    difs=`diff <(echo "$devs" ) <(echo "$(ls /dev/)") | grep '[<>]'`

	
	    if [ -n "$difs" ]
    	then
	    	sleep .5
            printInfo "$difs"
		    devs=`ls /dev/`
	    fi	
    done
    return $STATUS_OK
}

main