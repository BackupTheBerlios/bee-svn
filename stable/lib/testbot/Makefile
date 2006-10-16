CC = gcc -g -Wall
LIB= util.c socket.c
TARGETS=testbot #testhost

all: $(TARGETS)

#testhost:
#	$(CC) testhost.c $(LIB) /usr/lib/libpcre.a -o testhost

testbot:
	$(CC) testbot.c rshd.c $(LIB) /usr/lib/libpcre.a -o testbot
#	scp ./testbot testbot@gigi:/home/testbot/current/testbot
#	cp ./testbot ..
	make -C ./tools

clean:
	rm -rf $(TARGETS)


tar:
	tar cvf testbot.tar .
indent:
	indent -br -brs -cdw -ce -l80 -nut -ncs -prs -nsai -npcs -nsaf -nsaw -i8 *.c
