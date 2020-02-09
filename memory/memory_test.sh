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
log_file=./log.txt
start_upper=16384
curr_upper=$start_upper
curr_lower=0
page_size=4096

make format
make
make install
echo "Start output redirection. Write to file $log_file..."

echo "" > $log_file #Cleaning log file
exec 3>$log_file 4>&1 1>&3 #Open to write new file descriptor, save stdout, stdout > log.txt

echo "*--------------------*kmalloc & vmalloc (random buffer)*---------------------*"
printf "\n%-25s%-15s%-20s%-20s\n" "Buffer size (bytes)" "Memory" "Allocation (ns)" "Freeing (ns)"
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

echo -e "\n\n\n*------------------------*kmalloc & zsmalloc & vmalloc*------------------------*"
printf "\n%-25s%-15s%-20s%-20s\n" "Buffer size (bytes)" "Memory" "Allocation (ns)" "Freeing (ns)"
for (( i = 4; i <= $page_size; i *= 2)); do
        echo $i > ${sys_dir}/alloc_size
        kmalloc=( $(cat ${sys_dir}/kmalloc) )
        zsmalloc=( $(cat ${sys_dir}/zsmalloc) )
        vmalloc=( $(cat ${sys_dir}/vmalloc) )
        printf "\n%-25s%-15s%-20s%-20s\n" "" "kernel" ${kmalloc[0]} ${kmalloc[1]}
        printf "%-25lu%-15s%-20s%-20s\n" "$(cat ${sys_dir}/alloc_size)" "zram" ${zsmalloc[0]} ${zsmalloc[1]}
        printf "%-25s%-15s%-20s%-20s\n" "" "virtual" ${vmalloc[0]} ${vmalloc[1]}
done

echo -e "\n\n\n*------------------------*get_free_pages*---------------------------*"
printf "\n%-10s%-25s%-20s%-20s\n" "Order" "Buffer size (bytes)" "Allocation (ns)" "Freeing (ns)"
for (( i = 0; i < 10; ++i)); do
        echo $i > ${sys_dir}/page_order
        pages=( $(cat ${sys_dir}/pages) )
        printf "\n%-10u%-25s%-20s%-20s\n" "$(cat ${sys_dir}/page_order)" $page_size ${pages[0]} ${pages[1]}
        let "page_size *= 2"
done

exec 1>&4 3>&- 4>&- #Return stdout, close log.txt, close temporary stdout
echo "End of output redirection"
make uninstall

exit $RETURN_SUCCESS
