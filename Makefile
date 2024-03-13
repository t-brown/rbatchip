
CC ?= cc
CFLAGS  = -fPIC

.PHONY:
.PHONY: all clean install

.SUFFIXES:
.SUFFIXES: .c .h

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

all: rbatchip rbatchipd

install: all
	install -d $(BINDIR)
	install -m 750 -s rbatchip rbatchipd $(BINDIR)
	install -d $(MANDIR)
	install -m 644 rbatchip.1 rbatchipd.1 $(MANDIR)

rbatchip: rbatchip.c register_clnt.o register_xdr.o
	$(CC) $(CFLAGS) -o $@ $^

rbatchipd: rbatchipd.c register_svc.o register_xdr.o
	$(CC) $(CFLAGS) -o $@ $^

register.h register_clnt.c register_svc.c register_xdr.c: register.x
	rpcgen -C $^

register_clnt.o: register.h register_clnt.c register_xdr.o
register_svc.o: register.h register_svc.c register_xdr.o

%.o: %.c register.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f register.h *.o rbatchip rbatchipd register_clnt.c register_svc.c register_xdr.c
