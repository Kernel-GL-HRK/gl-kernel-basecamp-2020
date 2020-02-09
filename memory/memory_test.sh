 #!/bin/bash

make format
make
make install
printf "\n*--------------------------*kmalloc()*--------------------------*\n"
printf "%-25s%-20s%-20s\n" "Buffer size (bytes)" "Allocation (ns)" "Freeing (ns)"
for (( i = 0; i < 10; ++i)); do
        printf "%-25lu%-40s\n" 1024 "$(cat /sys/class/alloc-test/kmalloc)"
done
make uninstall

exit $RETURN_SUCCESS
