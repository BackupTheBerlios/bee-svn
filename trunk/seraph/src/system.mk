PREFIX=.
BINDIR=$(PREFIX)/bin
VARDIR=/home/groleo/var/db/seraph
MACHINES=$(VARDIR)/machines
JOBS=$(VARDIR)/jobs
SCHEDULES=$(VARDIR)/schedules

ifndef FLINT
CC = gcc
# same as Wall but without Wunused
XOPEN_SOURCE=-D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=600 -D_XOPEN_SOURCE_EXTENDED=1 -U_BSD_SOURCE

WARN=	-W -Wimplicit -Wreturn-type -Wswitch -Wcomment \
	-Wtrigraphs -Wformat -Wchar-subscripts \
	-Wparentheses -pedantic -Wuninitialized -std=c99

CFLAGS = $(WARN) $(XOPEN_SOURCE) -g -Os -DMACHINES=\"$(MACHINES)\" -DJOBS=\"$(JOBS)\" \
	-I$(TOP)/include `pkg-config --cflags glib-2.0`
LDFLAGS =-lglib-2.0 -Llib -Wl,-whole-archive -ltrpc -ltbot -Wl,-no-whole-archive -lxmlrpc
else
CC = flint
WARN=
CFLAGS = -I$(TOP)/include `pkg-config --cflags glib-2.0`
LDFLAGS =
endif
