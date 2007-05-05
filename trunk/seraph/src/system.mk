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


CC =gcc
MAKE=make
AR=ar
RANLIB=ranlib

# same as Wall but without Wunused
XOPEN_SOURCE=-D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED=1 -U_BSD_SOURCE
ALLSYMON=-whole-archive
ALLSYMOFF=-no-whole-archive

ifeq ($(OS),SunOS)
ALLSYMON=-zallextract
ALLSYMOFF=-zdefaultextract
XOPEN_SOURCE=-D__EXTENSIONS__
LDFLAGS = -lsocket -liconv -lnsl
endif

ifeq ($(OS),NetBSD)
LDFLAGS =-L/usr/pkg/lib
endif

ifeq ($(OS),OpenBSD)
CFLAGS =-I/usr/local/include/glib-2.0
LDFLAGS =-L/usr/local/lib
endif

ifeq ($(OS),FreeBSD)
CFLAGS =-I/usr/local/include/glib-2.0
LDFLAGS =-L/usr/local/lib
endif

WARN=-W -Wimplicit -Wreturn-type -Wswitch -Wcomment \
	-Wtrigraphs -Wformat -Wchar-subscripts \
	-Wparentheses -pedantic -Wuninitialized -std=c99

INCLUDES=-I$(TOP)/include `pkg-config --cflags glib-2.0` 

DEFINES=-DMACHINES=\"$(MACHINES)\" -DJOBS=\"$(JOBS)\" \
	-DUSE_DEBUG -DUSERDB=\"$(USERDB)\" -DLIBDIR=\"$(LIBDIR)\" -U__STRICT_ANSI__

CFLAGS += $(WARN) $(XOPEN_SOURCE) -g -ggdb3 -Os $(INCLUDES) $(DEFINES)

LDFLAGS += -L$(TOP)/lib/$(OS) -Wl,$(ALLSYMON) -ltrpc -ltbot -Wl,$(ALLSYMOFF) -lxmlrpc -lglib-2.0

ifeq ($(OS),CYGWIN_NT-5.1)
LDFLAGS += -liconv2
endif

FLINT=flint -DUSERDB=\"$(USERDB)\" $(INCLUDES) $< -oo >> $<.lint

objs=$(addprefix ./objs/,$(files))
lobs=$(patsubst %.o,%.lob,$(files))

objs/%.o: %.c
	$(CC) -c $(CFLAGS) $(WARN) $< -o $@

%.lob: %.c
	$(FLINT)
