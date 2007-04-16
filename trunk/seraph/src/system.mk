PREFIX=.
BINDIR=$(PREFIX)/bin
VARDIR=$(PREFIX)/db/seraph
MACHINES=/home/groleo/machines

ifndef FLINT
CC = gcc
# same as Wall but without Wunused
XOPEN_SOURCE=-D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED
WARN=	-W -Wimplicit -Wreturn-type -Wswitch -Wcomment \
	-Wtrigraphs -Wformat -Wchar-subscripts \
	-Wparentheses -pedantic -Wuninitialized -std=c99

CFLAGS = $(WARN) $(XOPEN_SOURCE) -D_POSIX_SOURCE -g -Os -DMACHINES=\"$(MACHINES)\" -I$(TOP)/include `pkg-config --cflags glib-2.0`
LDFLAGS =-lglib-2.0 -Llib -Wl,-whole-archive -ltrpc -ltbot -Wl,-no-whole-archive -lxmlrpc
else
CC = flint
WARN=
CFLAGS = -I$(TOP)/include `pkg-config --cflags glib-2.0`
LDFLAGS =
endif
