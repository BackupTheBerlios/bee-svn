TOP=..
include $(TOP)/system.mk

all:
	gcc $(CFLAGS) -c srv_xmlrpc.c
	gcc $(CFLAGS) -c svc_xmlrpc.c
	gcc $(CFLAGS) -c srv_tcp.c
	gcc $(CFLAGS) -c svc_tcp.c
	ar cr ../lib/libtrpc.a srv_xmlrpc.o svc_xmlrpc.o srv_tcp.o svc_tcp.o
	ranlib ../lib/libtrpc.a
clean:
	rm -rf *.o ../lib/libtrpc.a
