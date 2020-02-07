SRCS = userspace_mm.c
OBJS = $(SRCS:%.c=%.o)
LDLIBS += -lm
TARGET = userspace_mm

.PHONY: clean

$(TARGET): $(OBJS)

clean:
	-rm $(TARGET)
	-rm $(OBJS)
