#!/bin/bash

dev0=$(mktemp)
ls /dev > $dev0
dev1=$(mktemp)

while true; do
    ls /dev > $dev1
    usb_to_ttl=$(diff $dev0 $dev1 | grep ttyUSB* )
    if [[ -n  "$usb_to_ttl" ]]; then
        if [[ "$(diff $dev0 $dev1 | grep ttyUSB* | head -c1)" = ">" ]]; then
            echo "connected"
        else
            echo "disconnected"
        fi  
        echo "$usb_to_ttl"
        echo
        ls /dev > $dev0
    fi
done

trap "rm -f $dev0" 0 2 3 15
trap "rm -f $dev1" 0 2 3 15

# flash drives

# SD cards

# i2c devices