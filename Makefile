PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
RESDIR ?= $(PREFIX)/share/sfetch

all: sFetch

sFetch: src/main.c
	$(CC) -o $@ $< -Wall -Wextra -DRES_PATH=\"$(RESDIR)/ascii_art\"

install: sFetch
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 sFetch $(DESTDIR)$(BINDIR)
	install -d $(DESTDIR)$(RESDIR)/ascii_art
	cp -r res/* $(DESTDIR)$(RESDIR)/ascii_art/

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/sFetch
	rm -rf $(DESTDIR)$(RESDIR)/ascii_art
	-rmdir $(DESTDIR)$(RESDIR)

clean:
	rm -f sFetch

.PHONY: all install uninstall clean
