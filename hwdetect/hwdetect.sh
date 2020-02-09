#!/bin/bash

usb0=$(mktemp)
flash0=$(mktemp)
sd0=$(mktemp)
i2c0=$(mktemp)

usb1=$(mktemp)
flash1=$(mktemp)
sd1=$(mktemp)
i2c1=$(mktemp)

ALL_TMP_FILES="$usb0 $usb1 $flash0 $flash1 $i2c0 $i2c1 $sd0 $sd1"

CON_MSG=">connected> %l"

DISCON_MSG="<disconnected< %l"

END_MSG="***********Detection ended***********"

echo -e "Hardware that is detected from start\n"
echo "USB to ttl:"
ls /dev | grep ttyUSB* > $usb0
if [[ -n $(cat $usb0) ]]; then
    cat $usb0
else 
    echo "none"
fi
echo

echo "USB flash drives:"
ls /dev/sd[a-z] > $flash0
if [[ -n $(cat $flash0) ]]; then
    cat $flash0
else
    echo "none"    
fi
echo

echo "I2C devices: "
i2cdetect -l > $i2c0
if [[ -n $(cat $i2c0) ]]; then
    cat $i2c0
else
    echo "none"    
fi
echo

echo "SD cards: "
ls /dev | grep -E "mmcblk[0-9]+$" > $sd0
if [[ -n $(cat $sd0) ]]; then
    cat $sd0
else
    echo "none"    
fi
echo

trap 'rm -f $ALL_TMP_FILES && echo -e "\n$END_MSG" && exit' 2 3 15

echo "Active detector"
while true; do
    ls /dev | grep ttyUSB* > $usb1
    usb_to_ttl=$(diff --new-line-format=$"$CON_MSG" --old-line-format=$"$DISCON_MSG" --unchanged-group-format='' $usb0 $usb1 )

    ls /dev/sd[a-z] > $flash1
    flash_drives=$(diff --new-line-format=$"$CON_MSG" --old-line-format=$"$DISCON_MSG" --unchanged-group-format='' $flash0 $flash1)

    i2cdetect -l > $i2c1
    i2c_devices=$(diff --new-line-format=$"$CON_MSG" --old-line-format=$"$DISCON_MSG" --unchanged-group-format='' $i2c0 $i2c1)

    ls /dev | grep -E "mmcblk[0-9]+$" > $sd1
    sd_cards=$(diff --new-line-format=$"$CON_MSG" --old-line-format=$"$DISCON_MSG" --unchanged-group-format='' $sd0 $sd1)

    if [[ -n  "$usb_to_ttl" ]]; then
        for i in $(echo "$usb_to_ttl"); do 
            echo "$i"
        done  
        echo
        cat $usb1 > $usb0
    fi

    if [[ -n  "$flash_drives" ]]; then
        for j in $(echo "$flash_drives"); do 
            echo "$j"
        done
        echo
        cat $flash1 > $flash0 | 2>/dev/null
    fi

    if [[ -n  "$i2c_devices" ]]; then
        for k in $(echo "$i2c_devices"); do 
            echo "$k"
        done
        echo
        cat $i2c1 > $i2c0
    fi

    if [[ -n  "$sd_cards" ]]; then
        for l in $(echo "$sd_cards"); do 
            echo "$l"
        done
        echo
        cat $sd1 > $sd0
    fi

    sleep 1
done