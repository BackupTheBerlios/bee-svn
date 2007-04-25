#CFLAGS = -g -Wall -Werror -ansi -std=c99 -Wbad-function-cast  -Wmissing-declarations -Wmissing-prototypes \
           -Wnested-externs  -Wold-style-definition -Wstrict-prototypes \
		   -Wdeclaration-after-statement

TOP=.
files = base64.o str.o fileop.o baseclass.o basedb.o userdb.o testdb.o core.o sut.o sock.o scan.o debug.o #wall.o 
TARGETS=$(TOP)/lib/$(OS)/libtrpc.a $(TOP)/lib/$(OS)/libtbot.a bin/srph  bin/mngrd .tool
include system.mk

all: $(TARGETS)

help:
	echo "Targets are: srph lib clean tar indent install"



$(TOP)/lib/$(OS)/libtbot.a: $(objs)
	flex -oscan.c scan.l
	ar cr $(TOP)/lib/$(OS)/libtbot.a $(objs)
	ranlib $(TOP)/lib/$(OS)/libtbot.a

$(TOP)/lib/$(OS)/libtrpc.a:
	make -C rpc

bin/mngrd: $(TOP)/lib/$(OS)/libtbot.a
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) $(LDFLAGS)  mngrd.c  -o $@

bin/srph: $(TOP)/lib/$(OS)/libtbot.a $(TOP)/lib/$(OS)/libtrpc.a
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) $(LDFLAGS) srph.c  -o $@


.tool:
	make -C ./tools
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
	make -C tools install


clean:
	rm -rf $(TARGETS) $(objs) *.o *.lob *.debug scan.c
	make -C rpc clean
	make -C tools clean



#
jab:
	gcc -DHAVE_CONFIG_H  $(CFLAGS) -I./include -I.. -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  -g -O2 -Wall d_jabber.c \
	-o test-lm  -lglib-2.0   -lloudmouth-1 -lresolv -lnsl  -lssl -lcrypto

tar:
	cd .. && tar czvf srph-`date +%Y%m%d`.tar.gz src

indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -kr -i8 *.c

rmtinst:
	scp ./srph root@gigi:/home/srph
	make -C ./tools rmtinst
