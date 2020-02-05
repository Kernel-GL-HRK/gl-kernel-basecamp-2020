src = userspace_time.c
target = userspace_time
obj = $(src:%.c=%.o)

.PHONY: clean

$(target): $(obj)

clean:
	-rm -f $(obj)
	-rm -f $(target)
