#!/bin/bash

printf "USB to TTL\n"

for device in $(find /dev -name "ttyUSB*"); do
    dev_model=$(udevadm info ${device} | grep "ID_MODEL_FROM_DATABASE" | cut -d "=" -f2)
    echo ${device} ${dev_model}
done
