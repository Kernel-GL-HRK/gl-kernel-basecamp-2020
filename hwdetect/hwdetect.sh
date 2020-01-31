#!/bin/bash

#ls directory
dev_dir="/dev"

#grep option
opt="-w" #ignore slaves or holders

#grep paterns
ttyusb="ttyUSB['0'-'9']"
sd="sd['b'-'z']" #ignore the first device - "sda"
mmcblk="mmcblk['0'-'9']"

#bin directory
bin_dir=./bin

#ls logs
tty_log=""
flash_log=""
mmc_log=""

function refresh_logs {
        tty_log=$(ls $dev_dir | grep $ttyusb)
        flash_log=$(ls $dev_dir | grep $opt $sd)
        mmc_log=$(ls $dev_dir | grep $opt $mmcblk)
}

#Current device info
refresh_logs
echo "Device directory: $dev_dir"
echo "1 - TTY USB:"
[[ -n $tty_log ]] && echo "$tty_log" || echo "none"
echo "2 - Flash devices:"
[[ -n $flash_log ]] && echo "$flash_log" || echo "none"
echo "3 - SD cards:"
[[ -n $mmc_log ]] && echo "$mmc_log" || echo "none"

#temp file init
mkdir -p $bin_dir
current_file=~temp1.txt
echo -e "$tty_log\n$flash_log\n$mmc_log" > $bin_dir/$current_file

echo -e "\nPlug/unplug actions:"

while :; do
        refresh_logs
        if [[ $current_file = ~temp1.txt ]]; then
                current_file=~temp2.txt
                echo -e "$tty_log\n$flash_log\n$mmc_log" > $bin_dir/$current_file
                diff -u $bin_dir/~temp1.txt $bin_dir/~temp2.txt | grep [+-]['a'-'z']
        else
                current_file=~temp1.txt
                echo -e "$tty_log\n$flash_log\n$mmc_log" > $bin_dir/$current_file
                diff -u $bin_dir/~temp2.txt $bin_dir/~temp1.txt | grep [+-]['a'-'z']
        fi
done

exit $RETURN_SUCCES
