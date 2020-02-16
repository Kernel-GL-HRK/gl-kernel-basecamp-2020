#!/bin/bash

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