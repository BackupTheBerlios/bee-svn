TOP=.

files = notify_smtp.o notify.o str.o fileop.o baseclass.o basedb.o userdb.o testdb.o core.o sut.o sock.o scan.o dbg.o #wall.o 
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

bin/srph: $(TOP)/lib/$(OS)/libtrpc.a $(TOP)/lib/$(OS)/libtbot.a
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
	$(INSTALLDIR) $(DESTDIR)$(BINDIR)
	$(INSTALLDIR) $(DESTDIR)$(LOGDIR)
	$(INSTALLDIR) $(DESTDIR)$(MACHINES)
	$(INSTALLDIR) $(DESTDIR)$(NTFDIR)
	$(INSTALLDIR) $(DESTDIR)$(JOBS)
	$(INSTALLDIR) $(DESTDIR)$(JOBS)/complete
	$(INSTALLDIR) $(DESTDIR)$(JOBS)/running
	$(INSTALLDIR) $(DESTDIR)$(JOBS)/pending
	$(INSTALLDIR) $(DESTDIR)$(USERDB)
	$(INSTALL) -m $(DATA_MODE) ./cfg_template $(DESTDIR)$(MACHINES)
	$(INSTALL) -m $(BIN_MODE) $(TOP)/bin/mngrd $(DESTDIR)$(BINDIR)
	$(INSTALL) -m $(BIN_MODE) $(TOP)/bin/srph $(DESTDIR)$(BINDIR)
	$(MAKE) -C tools install


clean:
	rm -rf $(TARGETS) $(objs) *.o *.lob *.debug scan.c core *.lint
	$(MAKE) -C rpc clean
	$(MAKE) -C tools clean

jab:
	gcc -DHAVE_CONFIG_H  $(CFLAGS) -I./include -I.. -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  -g -O2 -Wall d_jabber.c \
	-o test-lm  -lglib-2.0   -lloudmouth-1 -lresolv -lnsl  -lssl -lcrypto

tar:
	cd ../.. && tar --exclude='CVS' --exclude='**/.svn/*' --exclude='**/.svn' -czvf seraph/srph-`date +%Y%m%d`.tar.gz seraph/src seraph/testAPI seraph/doc

indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -kr -i8 *.c

help:
	echo "Targets are: srph lib clean tar indent install"


#----------------------------
# Remote part of the Makefile
#----------------------------

HOST=$(shell echo $(DESTDIR) | cut -f1 -d: )
WDIR=$(shell echo $(DESTDIR) | cut -f2 -d: )

remote: remote-copy
	ssh $(HOST) "(cd $(WDIR)/src && $(MAKE))"
	touch remote

remote-copy:clean
	cd ../.. && tar --exclude='CVS' --exclude='**/.svn/*' --exclude='**/.svn' -cvf - seraph/src | ssh $(HOST) "( mkdir -p $(WDIR) && cd $(WDIR) && tar xvf - )"

remote-clean:
	ssh $(HOST) "(cd $(WDIR)/src && $(MAKE) clean)"

remote-startraw: remote
	ssh $(HOST) "(cd $(WDIR)/src && ./bin/mngrd -R 5000 )"

remote-stopraw:
	ssh $(HOST) "pkill mngrd"

setup:
	cat setup.nc | nc $(HOST) $(WDIR)
