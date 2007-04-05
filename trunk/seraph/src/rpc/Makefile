
CFLAGS = -g -Wall -I../include -L../lib #`pkg-config --cflags glib-2.0`

all:
	gcc $(CFLAGS) -c srv_xmlrpc.c
	gcc $(CFLAGS) -c svc_xmlrpc.c
	gcc $(CFLAGS) -c srv_tcp.c
	gcc $(CFLAGS) -c svc_tcp.c
	ar r ../lib/libtrpc.a srv_xmlrpc.o svc_xmlrpc.o srv_tcp.o svc_tcp.o
clean:
	rm *.o ../lib/libtrpc.a
