#CFLAGS = -g -Wall -Werror -ansi -std=c99 -Wbad-function-cast  -Wmissing-declarations -Wmissing-prototypes \
           -Wnested-externs  -Wold-style-definition -Wstrict-prototypes \
		   -Wdeclaration-after-statement

TOP=.
include system.mk
TARGETS=lib/libtrpc.a lib/libtbot.a $(BINDIR)/seraph  $(BINDIR)/mngrd

all: $(TARGETS)

help:
	echo "Targets are: seraph lib clean tar indent install"

files = strop.o fileop.o sut.o socket.o wall.o scan.o debug.o


objs=$(addprefix ./objs/,$(files))

./objs/%.o: %.c
	$(CC) -c $(CFLAGS) $(WARN) $< -o $@


lib/libtbot.a: $(objs)
	flex -oscan.c scan.l
	ar cr ./lib/libtbot.a $(objs)
	ranlib ./lib/libtbot.a

lib/libtrpc.a:
	make -C rpc

bin/mngrd: lib/libtbot.a
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) $(LDFLAGS) mngrd.c  -o $@
	make -C ./tools

bin/seraph: lib/libtbot.a lib/libtrpc.a
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) $(LDFLAGS) seraph.c  -o $@
	make -C ./tools


clean:
	rm -rf $(TARGETS) $(objs) *.o



#
jab:
	gcc -DHAVE_CONFIG_H  $(CFLAGS) -I./include -I.. -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  -g -O2 -Wall d_jabber.c \
	-o test-lm  -lglib-2.0   -lloudmouth-1 -lresolv -lnsl  -lssl -lcrypto

tar:
	tar cvf seraph.tar .

indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -kr -i8 *.c

install:
	cp ./seraph ..
	make -C ./tools install

rmtinst:
	scp ./seraph root@gigi:/home/seraph
	make -C ./tools rmtinst
