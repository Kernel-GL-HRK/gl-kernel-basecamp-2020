#!/bin/bash

dev_dir=/sys/firmware/devicetree/base/soc/i2c@1c2ac00/gl_mpu6050@68
driver_dir=driver
ls_log=$(ls $dev_dir)
log_bin=/dev/null

echo -e "Content of the directory $dev_dir\n"
ls -l $dev_dir

echo -e "\nDevice tree info of mpu6050:"
for i in $ls_log; do
        info=$(cat $dev_dir/$i)
        echo -e "\t$i: $info"
done

echo -e "\nDriver install started...\n"
make -C $driver_dir > $log_bin
sudo dmesg -C
make -C $driver_dir install > $log_bin
echo "This driver in lsmod log:"
lsmod | grep mpu
make -C $driver_dir uninstall > $log_bin
make -C $driver_dir clean > $log_bin
echo -e "\ndmesg log:"
dmesg

