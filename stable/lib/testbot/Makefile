#CFLAGS = -g -Wall -Werror -ansi -std=c99 -Wbad-function-cast  -Wmissing-declarations -Wmissing-prototypes \
           -Wnested-externs  -Wold-style-definition -Wstrict-prototypes \
		   -Wdeclaration-after-statement

CFLAGS = -g -Wall

CC = gcc -I/usr/include
LIB= strop.c fileop.c sut.c socket.c
TARGETS=testbot

all: $(TARGETS)


files = strop.o fileop.o sut.o socket.o

$(filter %.o,$(files)): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

testbot: $(files)
	$(CC) $(CFLAGS) $(files) main.c wall.c testbot.c rshd.c callbacks.c /usr/lib/libpcre.a -o testbot
	make -C ./tools

clean:
	rm -rf $(TARGETS)


tar:
	tar cvf testbot.tar .
indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -i8 *.c

install:
	scp ./testbot root@gigi:/home/testbot 
	cp ./testbot ..
	make -C ./tools install
