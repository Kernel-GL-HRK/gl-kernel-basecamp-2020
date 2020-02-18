ifneq ($(KERNELRELEASE),)

obj-m += kernel_memory.o

else

.PHONY: all clean install uninstall format

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean
install:
	sudo insmod kernel_memory.ko
	lsmod | head -n 3
uninstall:
	sudo rmmod kernel_memory.ko
	lsmod | head -n 3
format:
	clang-format-9 -i kernel_memory.c
#	 ./checkpatch.pl --no-tree -f ./kernel_memory.c
endif
