#!/bin/bash

start_log=$(ls /dev)

while true; do
    cur_log=$(ls /dev)
    diff --new-line-format=$'--> %l\n' --old-line-format=$'<-- %l\n' --unchanged-group-format='' <(echo "$start_log") <(echo "$cur_log") 
    start_log=$cur_log
done

