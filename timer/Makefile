ifneq ($(KERNELRELEASE),)

obj-m += timer.o

else

.PHONY: all clean install uninstall format

all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) modules
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(CURDIR) clean
install:
	sudo insmod timer.ko
	lsmod | head -n 3
uninstall:
	sudo rmmod timer.ko
	lsmod | head -n 3
format:
	clang-format-9 -i timer.c
endif
