#!/bin/bash

make format
make
make install
cat /sys/class/timer/inter
for (( del = 0; del < 30; del += 5)); do
        sleep $del
        iter=$(cat /sys/class/timer/inter)
        time=$(cat /sys/class/timer/time)
        echo -e "delay = $del\t${iter}\t${time}"
done
make uninstall

exit $RETURN_SUCCESS
