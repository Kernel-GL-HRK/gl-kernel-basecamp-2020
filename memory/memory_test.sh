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
start_upper=1024
curr_upper=$start_upper
curr_lower=0

make format
make
make install
printf "\n*--------------------------*kmalloc()*--------------------------*\n"
printf "%-25s%-20s%-20s\n" "Buffer size (bytes)" "Allocation (ns)" "Freeing (ns)"
for (( i = 0; i < 10; ++i)); do
        rand_size=$(get_rand ${curr_upper} ${curr_lower})
        curr_lower=$curr_upper
        let "curr_upper += start_upper"
        echo $rand_size > ${sys_dir}/alloc_size
        kmalloc=( $(cat ${sys_dir}/kmalloc) )
        printf "%-25lu%-20s%-20s\n" "$(cat ${sys_dir}/alloc_size)" ${kmalloc[0]} ${kmalloc[1]}
done
echo ""
make uninstall

exit $RETURN_SUCCESS
