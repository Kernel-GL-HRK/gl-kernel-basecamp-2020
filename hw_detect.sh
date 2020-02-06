#!/bin/bash
#initially connected devices
in_c_dev=$(ls /dev)
while true
do
    con_dev=$(ls /dev)

    diff <(echo "$in_c_dev") <(echo "$con_dev")

    in_c_dev=$con_dev
done
