CC      = gcc
CFLAGS  = -O2 -Wall -Wextra
AR      = ar
PREFIX  = /usr/local

SRCS    = $(wildcard src/**/*.c)
OBJS    = $(patsubst src/%.c, bin/%.o, $(SRCS))
HEADERS = $(wildcard src/**/*.h)
TARGET  = bin/libladlib.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^

bin/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	sudo mkdir -p $(PREFIX)/include/ladlib
	sudo cp $(HEADERS) $(PREFIX)/include/ladlib/
	sudo cp $(TARGET) $(PREFIX)/lib/

uninstall:
	sudo rm -rf $(PREFIX)/include/ladlib
	sudo rm -f $(PREFIX)/lib/libladlib.a

clean:
	rm -rf bin/*

.PHONY: all install uninstall clean