
CC = cc
CFLAGS  = -fPIC -DDEBUG

.PHONY:
.PHONY: all clean install

.SUFFIXES:
.SUFFIXES: .c .h

BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

all: rbatchip rbatchipd

install: all
	install -d $(BINDIR)
	install -m 750 -s rbatchip rbatchipd $(BINDIR)
	install -d $(MANDIR)
	install -m 644 rbatchip.1 rbatchipd.1 $(MANDIR)

rbatchip: rbatchip.c register_clnt.o
	$(CC) $(CFLAGS) -o $@ $^

rbatchipd: rbatchipd.c register_svc.o
	$(CC) $(CFLAGS) -o $@ $^

register.h: register.x
	rpcgen -C $^

register_clnt.o: register.h register_clnt.c
register_svc.o: register.h register_svc.c

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
.SECONDARY: register_clnt.o register_svc.o

clean:
	rm -f register.h *.o rbatchip rbatchipd
