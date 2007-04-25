TOP=..
files=srv_xmlrpc.o svc_xmlrpc.o srv_tcp.o svc_tcp.o
include $(TOP)/system.mk


ifndef FLINT
all: $(objs)
	ar cr ../lib/libtrpc.a $(objs)
	ranlib ../lib/libtrpc.a
else
all: $(lobs)
	flint -oolibtrpc.lob $(lobs)
endif

clean:
	rm -rf objs/* ../lib/libtrpc.a *.lob
