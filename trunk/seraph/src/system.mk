OS :=$(shell uname -s)
CPU:=$(shell uname -m)

BIN_MODE=755
DATA_MODE=644

PREFIX=/usr/local
DESTDIR=
BINDIR=$(PREFIX)/bin
LIBDIR=$(PREFIX)/lib/seraph
LOGDIR=$(PREFIX)/var/log/seraph

VARDIR=$(PREFIX)/var/db/seraph
MACHINES=$(VARDIR)/machines
NTFDIR=$(VARDIR)/notify
JOBS=$(VARDIR)/jobs
USERDB=$(VARDIR)/users
LOGDIR=$(PREFIX)/var/log
INSTALL=$(TOP)/install-sh -c
INSTALLDIR=mkdir -p


CC = gcc

# same as Wall but without Wunused
XOPEN_SOURCE=-D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED=1 -U_BSD_SOURCE
ALLSYMON=-whole-archive
ALLSYMOFF=-no-whole-archive

ifeq ($(OS),Linux)
#RH 8
export PKG_CONFIG_PATH=/usr/lib/pkgconfig
LDFLAGS=-L$(TOP)/lib/$(OS)/$(CPU)
endif

ifeq ($(OS),SunOS)
ALLSYMON=-zallextract
ALLSYMOFF=-zdefaultextract
XOPEN_SOURCE=-D__EXTENSIONS__
LDFLAGS = -lsocket -liconv -lnsl
endif

ifeq ($(OS),NetBSD)
LDFLAGS =-L/usr/pkg/lib -Wl,-rpath /usr/pkg/lib
MAKE := gmake
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

DEFINES=-DMACHINES=\"$(MACHINES)\" -DJOBS=\"$(JOBS)\" -DBINDIR=\"$(BINDIR)\" \
		-DUSE_DEBUG -DUSERDB=\"$(USERDB)\" -DLIBDIR=\"$(LIBDIR)\" -DLOGDIR=\"$(LOGDIR)\" \
		-DNTFDIR=\"$(NTFDIR)\" -DLOGDIR=\"$(LOGDIR)\"

CFLAGS += $(WARN) $(XOPEN_SOURCE) -g -ggdb3 -Os $(INCLUDES) $(DEFINES)
LDFLAGS += -L$(TOP)/lib/$(OS) -Wl,$(ALLSYMON) -ltrpc -ltbot -Wl,$(ALLSYMOFF) -lxmlrpc -lglib-2.0

ifeq ($(OS),CYGWIN_NT-5.1)
LDFLAGS += -liconv
endif

FLINT=flint -DUSERDB=\"$(USERDB)\" $(INCLUDES) $< -oo >> $<.lint

objs=$(addprefix ./objs/,$(files))
lobs=$(patsubst %.o,%.lob,$(files))

objs/%.o: %.c
	$(CC) -c $(CFLAGS) $(WARN) $< -o $@
	@echo ""

%.lob: %.c
	$(FLINT)
