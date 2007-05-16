TOP=..
files=srv_xmlrpc.o svc_xmlrpc.o srv_tcp.o svc_tcp.o
include $(TOP)/system.mk


all: $(objs) _lints
	ar cr $(TOP)/lib/$(OS)/libtrpc.a $(objs)
	ranlib $(TOP)/lib/$(OS)/libtrpc.a

ifdef USEFLINT
_lints: $(lobs)
	flint $(lobs) -oo\($(TOP)/lib/$(OS)/libtrpc.lob\)
else
_lints:
endif

clean:
	rm -rf objs/*.o $(TOP)/lib/$(OS)/libtrpc.a *.lob *.lint
