#CFLAGS = -g -Wall -Werror -ansi -std=c99 -Wbad-function-cast  -Wmissing-declarations -Wmissing-prototypes \
           -Wnested-externs  -Wold-style-definition -Wstrict-prototypes \
		   -Wdeclaration-after-statement

CFLAGS = -g -static -Wall -I./include -L./lib #`pkg-config --cflags glib-2.0`

CC = gcc

all: libtrpc lib seraph  mngrd

help:
	echo "Targets are: seraph lib clean tar indent install"

files = strop.o fileop.o sut.o socket.o wall.o scan.o debug.o

$(filter %.o,$(files)): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o ./objs/$@

objs=$(addprefix ./objs/,$(files))

seraph: lib libtrpc
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) -ltrpc -ltbot -lxmlrpc seraph.c  -o ./bin/seraph -u useDebug_g -u start_rawrpc -u str_isEnv -u start_xmlrpc -u srph_parseCfg
	make -C ./tools

mngrd: lib
	mkdir -p ./bin/tools
	$(CC) $(CFLAGS) -ltrpc -ltbot -lxmlrpc mngrd.c  -o ./bin/mngrd -u useDebug_g -u start_rawrpc -u str_isEnv -u start_xmlrpc -u srph_parseCfg
	make -C ./tools

libtrpc:
	make -C rpc

lib: $(files)
	flex -oscan.c scan.l
	ar r ./lib/libtbot.a $(objs)

clean:
	rm -rf ./bin ./lib/libtbot.a $(objs)

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
