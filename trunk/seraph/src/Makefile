TOP=.

files = str.o fileop.o baseclass.o basedb.o userdb.o testdb.o core.o sut.o sock.o scan.o dbg.o #wall.o 
TARGETS=$(TOP)/lib/$(OS)/libtrpc.a $(TOP)/lib/$(OS)/libtbot.a bin/srph  bin/mngrd .tool

include system.mk

all: $(TARGETS)


$(TOP)/lib/$(OS)/libtbot.a: $(objs) _lints
	ar cr $(TOP)/lib/$(OS)/libtbot.a $(objs)
	ranlib $(TOP)/lib/$(OS)/libtbot.a

ifdef USEFLINT
_lints: $(lobs)
	flint $(lobs) -oo\($(TOP)/lib/$(OS)/libtbot.lob\) >> libtbot.lint
else
_lints:
endif

objs/scan.o:
	flex -oscan.c scan.l
	$(CC) -c $(CFLAGS) $(WARN) scan.c -o $@

$(TOP)/lib/$(OS)/libtrpc.a:
	$(MAKE) -C rpc

bin/mngrd: $(TOP)/lib/$(OS)/libtbot.a
	$(CC) $(CFLAGS) $(LDFLAGS)  mngrd.c  -o $@
ifdef USEFLINT
	flint $(TOP)/lib/$(OS)/libtbot.lob mngrd.c >> mngrd.lint
endif

bin/srph: $(TOP)/lib/$(OS)/libtbot.a $(TOP)/lib/$(OS)/libtrpc.a
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) $(LDFLAGS) srph.c  -o $@
ifdef USEFLINT
	flint $(TOP)/lib/$(OS)/libtbot.lob $(TOP)/lib/$(OS)/libtrpc.lob  srph.c >> srph.lint
endif


.tool:
	mkdir -p ./bin/tools
	$(MAKE) -C ./tools
	touch .tool

install:
	$(INSTALLDIR) $(BINDIR)
	$(INSTALLDIR) $(MACHINES)
	$(INSTALLDIR) $(JOBS)
	$(INSTALLDIR) $(JOBS)/complete
	$(INSTALLDIR) $(JOBS)/running
	$(INSTALLDIR) $(JOBS)/pending
	$(INSTALLDIR) $(USERDB)
	$(INSTALL) -m $(DATA_MODE) ./cfg_template $(MACHINES)
	$(INSTALL) -m $(BIN_MODE) ./bin/mngrd $(BINDIR)
	$(INSTALL) -m $(BIN_MODE) ./bin/srph $(BINDIR)
	$(MAKE) -C tools install


clean:
	rm -rf $(TARGETS) $(objs) *.o *.lob *.debug scan.c core *.lint
	$(MAKE) -C rpc clean
	$(MAKE) -C tools clean

jab:
	$(CC) -DHAVE_CONFIG_H  $(CFLAGS) -I./include -I.. -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  -g -O2 -Wall d_jabber.c \
	-o test-lm  -lglib-2.0   -lloudmouth-1 -lresolv -lnsl  -lssl -lcrypto

tar:
	cd .. && tar czvf srph-`date +%Y%m%d`.tar.gz src

indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -kr -i8 *.c

rmtinst:
	scp ./srph root@gigi:/home/srph
	$(MAKE) -C ./tools rmtinst

help:
	echo "Targets are: srph lib clean tar indent install"



