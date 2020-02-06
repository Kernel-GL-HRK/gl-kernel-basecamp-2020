#!/bin/bash

make format
make
make install
cat /sys/class/timer/inter
for (( del = 0; del < 101; del += 5)); do
        sleep $del
        echo -e "delay = $del\t$(cat /sys/class/timer/inter)"
done
make uninstall

exit $RETURN_SUCCESS
