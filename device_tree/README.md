# device_tree
The module and the dts file were tested on Raspberry Pi Zero W with Raspbian installed.
#### dmesg output
`[ 6542.829839] i2c world driver: init`  
`[ 6542.830005] world_probe() function is called.`  
`[ 6542.830304] world: got minor 56`  
`[ 6545.571496] i2c world driver: exit`  
`[ 6545.571576] world_remove() function is called.`  
#### sysfs driver
`pi@raspberrypi:~/ $ sudo find /sys -name "*i2c_ol*"`  
`/sys/bus/i2c/drivers/i2c_ol`  
`/sys/module/i2c_ol`  
`/sys/module/i2c_ol/drivers/i2c:i2c_ol`  
#### procfs device tree
`pi@raspberrypi:~ $ sudo find /proc/device-tree/ -name "*world*"`  
`/proc/device-tree/soc/i2c@7e804000/world@69`
