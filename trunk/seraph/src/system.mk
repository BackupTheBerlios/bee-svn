PREFIX=.
BINDIR=$(PREFIX)/bin
VARDIR=$(PREFIX)/db/seraph
MACHINES=/home/groleo/machines

CC = gcc
WARN=	-W -Wimplicit -Wreturn-type -Wswitch -Wcomment \
	-Wtrigraphs -Wformat -Wchar-subscripts \
	-Wparentheses -pedantic -Wuninitialized

CFLAGS = -g -Os -Wall -DMACHINES=\"$(MACHINES)\" -I$(TOP)/include `pkg-config --cflags glib-2.0`
LDFLAGS =-lglib-2.0 -Llib -Wl,-whole-archive -ltrpc -ltbot -Wl,-no-whole-archive -lxmlrpc 
