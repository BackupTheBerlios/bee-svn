CC = gcc -g -std=c99
LIB= strop.c fileop.c sut.c socket.c
TARGETS=testbot #testhost

all: $(TARGETS)

testbot:
	$(CC) main.c wall.c testbot.c rshd.c callbacks.c $(LIB) /usr/lib/libpcre.a -o testbot
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
