DRIVER=mpu6050

ifneq ($(KERNELRELEASE),)

obj-m += $(DRIVER).o

else

.PHONY: all clean install uninstall format

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules

clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean

install:
	sudo insmod $(DRIVER).ko

uninstall:
	sudo rmmod $(DRIVER).ko

endif
