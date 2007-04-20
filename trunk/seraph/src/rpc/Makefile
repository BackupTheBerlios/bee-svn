TOP=..
files=srv_xmlrpc.o svc_xmlrpc.o srv_tcp.o svc_tcp.o
include $(TOP)/system.mk
EXTRA=-c

ifdef FLINT
EXTRA=
endif

all: $(objs)
	ar cr ../lib/libtrpc.a $(objs)
	ranlib ../lib/libtrpc.a
clean:
	rm -rf objs/* ../lib/libtrpc.a *.lob
