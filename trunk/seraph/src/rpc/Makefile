TOP=..
files=srv_xmlrpc.o svc_xmlrpc.o srv_tcp.o svc_tcp.o
include $(TOP)/system.mk


all: $(objs) $(lobs)
	ar cr $(TOP)/lib/$(OS)/libtrpc.a $(objs)
	ranlib $(TOP)/lib/$(OS)/libtrpc.a
	flint $(lobs) -oo\($(TOP)/lib/$(OS)/libtrpc.lob\)

clean:
	rm -rf objs/* $(TOP)/lib/$(OS)/libtrpc.a *.lob
