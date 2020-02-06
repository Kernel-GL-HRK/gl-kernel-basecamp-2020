#/usr/bin/env bash

SLEEP_TIME=2

st=(/dev/*)

while true
do  
    for dev in /dev/*
    do    
        found=0    
        for dev_st in "${st[@]}"    
        do [ "$dev" == "$dev_st" ] && found=1 && 
break; done    
        [ $found -eq 0 ] && echo "New device: $dev"  
    done  
    st=(/dev/*)  
    sleep $SLEEP_TIME
done
