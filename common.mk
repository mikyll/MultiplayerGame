OUT = bin

DEPS += src/core/defs.h
DEPS += src/core/structs.h

GAME_SOURCES := $(shell find src -name '*.c' | sort)

OBJS := $(addprefix $(OUT)/,$(GAME_SOURCES:%.c=%.o))

# Top-level rule to create the program.
all: $(PROG)

# Compiling other source files.
$(OUT)/%.o: %.c %.h $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

# Cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) -rf $(OUT) $(PROG)
