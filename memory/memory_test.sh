 #!/bin/bash

#For getting a random number in a range from $1 to $2
function get_rand {
        local temp=$RANDOM
        local upper_bound=$1
        local lower_bound=$2
        let "temp = temp % (upper_bound - lower_bound) + lower_bound"
        echo $temp
}

RANDOM=$SECONDS
sys_dir=/sys/class/alloc_test
start_upper=8192
curr_upper=$start_upper
curr_lower=0

make format
make
make install
echo "" > log.txt #Cleaning log file
exec 3>log.txt 4>&1 1>&3 #Open to write new file descriptor, save stdout, stdout > log.txt
printf "%-25s%-15s%-20s%-20s\n" "Buffer size (bytes)" "Memory" "Allocation (ns)" "Freeing (ns)"
for (( i = 0; i < 10; ++i)); do
        rand_size=$(get_rand ${curr_upper} ${curr_lower})
        curr_lower=$curr_upper
        let "curr_upper += start_upper"
        echo $rand_size > ${sys_dir}/alloc_size
        kmalloc=( $(cat ${sys_dir}/kmalloc) )
        vmalloc=( $(cat ${sys_dir}/vmalloc) )
        printf "\n%-25lu%-15s%-20s%-20s\n" "$(cat ${sys_dir}/alloc_size)" "kernel" ${kmalloc[0]} ${kmalloc[1]}
        printf "%-25s%-15s%-20s%-20s\n" "" "virtual" ${vmalloc[0]} ${vmalloc[1]}
done
exec 1>&4 3>&- 4>&- #Return stdout, close log.txt, close temporary stdout
make uninstall

exit $RETURN_SUCCESS
