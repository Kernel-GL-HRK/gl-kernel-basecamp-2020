SRC = userspace_time.c
USERSPACE_TARGET = userspace_time
KERNEL_TARGET = kernel_time
OBJ = $(SRC:%.c=%.o)
obj-m = mytimer.o
kern_dir ?= /lib/modules/$(shell uname -r)/build

.PHONY: clean

all: $(USERSPACE_TARGET) $(KERNEL_TARGET)

$(USERSPACE_TARGET): $(OBJ)

$(KERNEL_TARGET):
	$(MAKE) -C $(kern_dir) M=$(PWD) modules

clean:
	-rm -f $(OBJ)
	-rm -f $(USERSPACE_TARGET)
	$(MAKE) -C $(kern_dir) M=$(PWD) clean
