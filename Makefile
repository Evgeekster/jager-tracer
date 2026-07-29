CC      = gcc
TARGET  = build/jagertrace
CFLAGS  = -Wall -Wextra \
          -Isrc -Isrc/util -Isrc/trace -Isrc/syscall_map -Isrc/decode -Isrc/net -Isrc/args

SRCS    = src/main.c \
          src/trace/trace.c \
          src/syscall_map/syscall_map.c \
          src/net/network.c \
          src/decode/decode.c \
          src/args/sockopt_names.c \
          src/util/util.c \
          src/args/args.c 

OBJS    = $(patsubst src/%.c, build/%.o, $(SRCS))

GEN_SCRIPT = scripts/py/syscall_map.py
GEN_TARGET = src/syscall_map/syscall_map.c

.PHONY: all clean gen

all: gen $(TARGET)

gen: $(GEN_TARGET)

# regenerate only if the script itself changed, or the file doesn't exist yet
$(GEN_TARGET): $(GEN_SCRIPT)
	python3 $(GEN_SCRIPT) --out $(GEN_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/