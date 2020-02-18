BUILDDIR := ./build
OBJDIR   := $(BUILDDIR)/obj
SRCDIR   := ./src
INCDIR   := ./include

EXECUTABLE := timer
TARGET     := $(BUILDDIR)/$(EXECUTABLE)

SRC := $(shell find $(SRCDIR) -name *.c 2>/dev/null)
OBJ := $(SRC:%.c=$(OBJDIR)/%.o)
INC := $(shell find $(INCDIR) -name *.h 2>/dev/null)

CLANG  := clang
FORMAT := clang-format -i -style=file
MKDIR  := mkdir -p

INCFLAGS := -I$(INCDIR)

CC       := $(CLANG)
CFLAGS   := -Wall -Wextra -Werror
CPPFLAGS := $(INCFLAGS)
LDFLAGS  :=

all: release

$(OBJDIR)/%.o: %.c
	$(MKDIR) $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJ)
	$(MKDIR) $(@D)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

.PHONY: debug

debug: CFLAGS  += -fsanitize=address -g
debug: LDFLAGS += -lasan
debug: $(TARGET)

.PHONY: release

release: CFLAGS += -DNDEBUG -O3
release: $(TARGET)

.PHONY: run

run: $(TARGET)
	$(TARGET) $(ARGS)

.PHONY: format

format:
ifneq ($(SRC),)
	@$(FORMAT) $(SRC)
endif

ifneq ($(INC),)
	@$(FORMAT) $(INC)
endif

.PHONY: clean

clean:
	$(RM) -r $(BUILDDIR)
