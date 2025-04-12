PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
RESDIR ?= $(PREFIX)/share/sfetch

all: sFetch

sFetch: src/main.c
	$(CC) -o $@ $< -Wall -Wextra

install: sFetch
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 sFetch $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(RESDIR)
	install -m 644 res/void.txt $(DESTDIR)$(RESDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/sFetch
	rm -f $(DESTDIR)$(RESDIR)/void.txt
	-rmdir $(DESTDIR)$(RESDIR)

clean:
	rm -f sFetch

.PHONY: all install uninstall clean
