#!/bin/bash

start_log=$(ls /dev)

while true; do
    cur_log=$(ls /dev)
    diff <(echo "$start_log") <(echo "$cur_log") 
    start_log=$cur_log
done
