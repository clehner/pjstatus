BIN = pjstatus
CFLAGS = -Wall -Wextra -O3
CC ?= gcc
PREFIX ?= /usr/local

all: $(BIN)

install: $(BIN)
	install -m 755 $(BIN) $(PREFIX)/bin

clean:
	rm $(BIN)

.PHONY: install clean
