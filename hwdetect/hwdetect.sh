#!/bin/bash

usb0=$(mktemp)
flash0=$(mktemp)
sd0=$(mktemp)

usb1=$(mktemp)
flash1=$(mktemp)
sd1=$(mktemp)

echo "Hardware that is detected from start"
echo "USB to ttl:"
ls /dev | grep ttyUSB* > $usb0
if [[ -n $(cat $usb0) ]]; then
    cat $usb0
else 
    echo "none"
fi

echo "USB flash drives that are connected"
ls /dev/sd[a-z] > $flash0
if [[ -n $(cat $flash0) ]]; then
    cat $flash0
else
    echo "none"    
fi

echo 
echo "Active detector"
while true; do
    ls /dev | grep ttyUSB* > $usb1
    usb_to_ttl=$(diff --new-line-format=$'>connected> %l\n' --old-line-format=$'<disconnected< %l\n' --unchanged-group-format='' $usb0 $usb1 )

    ls /dev/sd[a-z] > $flash1
    flash_drives=$(diff --new-line-format=$'>connected> %l\n' --old-line-format=$'<disconnected< %l\n' --unchanged-group-format='' $flash0 $flash1)

    if [[ -n  "$usb_to_ttl" ]]; then
        for i in $(echo "$usb_to_ttl"); do 
            echo "$i"
        done  
        echo
        ls /dev | grep ttyUSB* > $usb0
    fi

    if [[ -n  "$flash_drives" ]]; then
        for j in $(echo "$flash_drives"); do 
            echo "$j"
        done
        echo
        ls /dev/sd[a-z] > $flash0 | 2>/dev/null
    fi

    sleep 1
done

trap "rm -f $usb0" 0 2 3 15
trap "rm -f $usb1" 0 2 3 15
trap "rm -f $flash0" 0 2 3 15
trap "rm -f $flash1" 0 2 3 15



# SD cards

# i2c devicesecho "USB flash drives that are connected:"
ls /dev/sd[a-z] > $flash0
if [[ -n $(cat $flash0) ]]; then
    cat $flash0
else 
    echo "none"
fi

echo 
echo "Active detector"
while true; do
    ls /dev | grep ttyUSB* > $usb1
    usb_to_ttl=$(diff $usb0 $usb1 )

    ls /dev/sd[a-z] > $flash1
    flash_drives=$(diff $flash0 $flash1)

    if [[ -n  "$usb_to_ttl" ]]; then
        if [[ "$(diff $usb0 $usb1 | head -c1)" = ">" ]]; then
            echo "connected"
        else
            echo "disconnected"
        fi  
        echo "$usb_to_ttl"
        echo
        ls /dev > $usb0
    fi

    if [[ -n  "$flash_drives" ]]; then
        if [[ "$( diff $flash0 $flash1 | head -c1)" = ">" ]]; then
            echo "connected"
            echo "$flash_drives"
        elif [[ "$(echo "$flash_drives" | head -c1)" = "<" ]]; then
            echo "disconnected"
            echo "$flash_drives"
        fi  
        cat $flash1 > $flash0
    fi
done

trap "rm -f $usb0" 0 2 3 15
trap "rm -f $usb1" 0 2 3 15
trap "rm -f $flash0" 0 2 3 15
trap "rm -f $flash1" 0 2 3 15



# SD cards

# i2c devices