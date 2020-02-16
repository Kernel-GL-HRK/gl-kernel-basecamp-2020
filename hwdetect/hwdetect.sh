#!/bin/bash

USB_CVT=1
BLK_DEV=2
OTHER=3

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

STATUS_OK=0

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
		    devs=`ls /dev/`
	    fi	
    done
    return $STATUS_OK
}

main