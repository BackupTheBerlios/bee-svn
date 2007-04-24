#CFLAGS = -g -Wall -Werror -ansi -std=c99 -Wbad-function-cast  -Wmissing-declarations -Wmissing-prototypes \
           -Wnested-externs  -Wold-style-definition -Wstrict-prototypes \
		   -Wdeclaration-after-statement

TOP=.
files = strop.o fileop.o baseclass.o basedb.o userdb.o testdb.o core.o sut.o socket.o wall.o scan.o debug.o
TARGETS=lib/libtrpc.a lib/libtbot.a bin/seraph  bin/mngrd .tool
include system.mk

all: $(TARGETS)

help:
	echo "Targets are: seraph lib clean tar indent install"



lib/libtbot.a: $(objs)
	flex -oscan.c scan.l
	ar cr ./lib/libtbot.a $(objs)
	ranlib ./lib/libtbot.a

lib/libtrpc.a:
	make -C rpc

bin/mngrd: lib/libtbot.a
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) $(LDFLAGS)  mngrd.c  -o $@

bin/seraph: lib/libtbot.a lib/libtrpc.a
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) $(LDFLAGS) seraph.c  -o $@


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
	$(INSTALL) -m $(BIN_MODE) ./bin/seraph $(BINDIR)
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
	cd .. && tar czvf seraph-`date +%Y%m%d`.tar.gz src

indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -kr -i8 *.c

rmtinst:
	scp ./seraph root@gigi:/home/seraph
	make -C ./tools rmtinst
