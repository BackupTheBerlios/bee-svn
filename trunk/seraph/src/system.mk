BIN_MODE=755
DATA_MODE=644

PREFIX=/home/groleo/sand
BINDIR=$(PREFIX)/bin
LIBDIR=$(PREFIX)/lib/seraph

VARDIR=/home/groleo/sand/var/db/seraph
MACHINES=$(VARDIR)/machines
JOBS=$(VARDIR)/jobs
USERDB=$(VARDIR)/users
INSTALL=$(TOP)/install-sh -c
INSTALLDIR=mkdir -p


ifndef FLINT
CC = gcc
# same as Wall but without Wunused
XOPEN_SOURCE=-D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED=1 -U_BSD_SOURCE

WARN=	-W -Wimplicit -Wreturn-type -Wswitch -Wcomment \
	-Wtrigraphs -Wformat -Wchar-subscripts \
	-Wparentheses -pedantic -Wuninitialized -std=c99

CFLAGS = $(WARN) $(XOPEN_SOURCE) -g -Os -DMACHINES=\"$(MACHINES)\" -DJOBS=\"$(JOBS)\" \
	-DUSE_DEBUG -DUSERDB=\"$(USERDB)\" -DLIBDIR=\"$(LIBDIR)\" -I$(TOP)/include `pkg-config --cflags glib-2.0`
LDFLAGS =-lglib-2.0 -Llib -Wl,-whole-archive -ltrpc -ltbot -Wl,-no-whole-archive -lxmlrpc
else
CC = flint
WARN=
CFLAGS = -I$(TOP)/include `pkg-config --cflags glib-2.0`
LDFLAGS =
endif

objs=$(addprefix ./objs/,$(files))

./objs/%.o: %.c
	$(CC) -c $(CFLAGS) $(WARN) $< -o $@

