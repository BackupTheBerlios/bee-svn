CC = gcc -g
LIB= strop.c fileop.c sut.c socket.c
TARGETS=testbot #testhost

all: $(TARGETS)

testbot:
	$(CC) testbot.c rshd.c callbacks.c $(LIB) /usr/lib/libpcre.a -o testbot
	make -C ./tools

clean:
	rm -rf $(TARGETS)


tar:
	tar cvf testbot.tar .
indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -i8 *.c

install:
	scp ./testbot testbot@gigi:/home/testbot/current/testbot
	cp ./testbot ..
	make -C ./tools install
