TOP=..
include $(TOP)/system.mk
EXTRA=-c

ifdef FLINT
EXTRA=
endif

all:
	$(CC) $(CFLAGS) $(EXTRA) srv_xmlrpc.c
	$(CC) $(CFLAGS) $(EXTRA) svc_xmlrpc.c
	$(CC) $(CFLAGS) $(EXTRA) srv_tcp.c
	$(CC) $(CFLAGS) $(EXTRA) svc_tcp.c
	ar cr ../lib/libtrpc.a srv_xmlrpc.o svc_xmlrpc.o srv_tcp.o svc_tcp.o
	ranlib ../lib/libtrpc.a
clean:
	rm -rf *.o ../lib/libtrpc.a *.lob
