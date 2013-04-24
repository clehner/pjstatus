P=pjstatus
OBJECTS=
CFLAGS = -g -Wall -O3
LDLIBS=
CC=gcc
PREFIX=/usr/local

$(P): $(OBJECTS)

install: $(P)
	install -m 755 $(P) $(PREFIX)/bin

.PHONY: install
