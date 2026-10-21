PROG = multiplayer_game
CC = gcc

NPROCS = $(shell grep -c 'processor' /proc/cpuinfo)
MAKEFLAGS += -j$(NPROCS)

include common.mk

CXXFLAGS += `sdl2-config --cflags`
CXXFLAGS += -DVERSION=$(VERSION) -DREVISION=$(REVISION)
CXXFLAGS += -Wall -Wempty-body -Werror -Wstrict-prototypes -Werror=maybe-uninitialized -Warray-bounds
CXXFLAGS += -g -lefence

LDFLAGS += `sdl2-config --libs` -lSDL2_image -lm -lenet

# Linking the program.
$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# Pattern rule to compile .c files
%.o: %.c
	$(CC) $(CXXFLAGS) -c $< -o $@
