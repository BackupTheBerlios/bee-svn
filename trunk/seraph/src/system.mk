OS:=$(shell uname -s)

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


CC = gcc
# same as Wall but without Wunused
XOPEN_SOURCE=-D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED=1 -U_BSD_SOURCE

WARN=	-W -Wimplicit -Wreturn-type -Wswitch -Wcomment \
	-Wtrigraphs -Wformat -Wchar-subscripts \
	-Wparentheses -pedantic -Wuninitialized -std=c99

INCLUDES=-I$(TOP)/include `pkg-config --cflags glib-2.0`

DEFINES=-DMACHINES=\"$(MACHINES)\" -DJOBS=\"$(JOBS)\" \
	-DUSE_DEBUG -DUSERDB=\"$(USERDB)\" -DLIBDIR=\"$(LIBDIR)\"

CFLAGS = $(WARN) $(XOPEN_SOURCE) -g -Os $(INCLUDES) $(DEFINES)
LDFLAGS = -lglib-2.0 -L$(TOP)/lib/$(OS) -Wl,-whole-archive -ltrpc -ltbot -Wl,-no-whole-archive -lxmlrpc


ifeq ($(OS),NetBSD)
LDFLAGS +=-L/usr/pkg/lib
endif

FLINT=flint $(INCLUDES) $< -oo >> $<.lint

objs=$(addprefix ./objs/,$(files))
lobs=$(patsubst %.o,%.lob,$(files))

objs/%.o: %.c
	$(CC) -c $(CFLAGS) $(WARN) $< -o $@

%.lob: %.c
	$(FLINT)
