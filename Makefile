CC      = gcc
TARGET  = build/jagertrace
CFLAGS  = -Wall -Wextra \
          -Isrc -Isrc/util -Isrc/trace -Isrc/syscall_map -Isrc/decode -Isrc/args

SRCS    = src/main.c \
          src/trace/trace.c \
          src/syscall_map/syscall_map.c \
          src/decode/decode.c \
          src/util/util.c \
          src/args/args.c

OBJS    = $(patsubst src/%.c, build/%.o, $(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build/trace build/syscall_map build/decode build/util build/args

clean:
	rm -rf build/

.PHONY: clean