#!/bin/bash

make format
make
make install
for (( del = 0; del < 30; del += 5)); do
        sleep $del
        inter=$(cat /sys/class/timer/inter)
        time=$(cat /sys/class/timer/time)
        time=$(echo "scale=9;$time/1000000000" | bc)
        echo -e "delay = $del s\t${inter} s\t${time} s"
done
make uninstall

