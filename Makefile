.POSIX:
.SUFFIXES:

VERSION = 1.0
TARGET = bit
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

CFLAGS = -Os -march=native -mtune=native -pipe -s -std=c99 -flto -pedantic -Wall -lm -D_DEFAULT_SOURCE

SRC = $(TARGET).c

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $@ $(CFLAGS)

dist:
	mkdir -p $(TARGET)-$(VERSION)
	cp -R README.md $(TARGET) $(TARGET)-$(VERSION)
	tar -cf $(TARGET)-$(VERSION).tar $(TARGET)-$(VERSION)
	gzip $(TARGET)-$(VERSION).tar
	rm -rf $(TARGET)-$(VERSION)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(BINDIR)
	cp -p $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	chmod 755 $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	rm $(DESTDIR)$(BINDIR)/$(TARGET)

clean:
	rm $(TARGET)

all: $(TARGET)

.PHONY: all dist install uninstall clean
