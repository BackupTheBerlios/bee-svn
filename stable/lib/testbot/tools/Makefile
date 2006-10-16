CC  = gcc -g -I../ -Wall
LIB = ../util.c ../socket.c
BIN = ./bin
TARGETS=action cp mkdir rm refresh


all: $(TARGETS)
#	find ./bin/ -maxdepth 1 -type f -exec scp {} testbot@gigi:/home/testbot/current/engine/ \;
#	find ./bin/ -maxdepth 1 -type f -exec cp {} ../../engine \;

action:
	$(CC) action.c $(LIB) -o $(BIN)/action
cp:
	$(CC) cp.c $(LIB) -o $(BIN)/cp

mkdir:
	$(CC) mkdir.c $(LIB) -o $(BIN)/mkdir
rm:
	$(CC) rm.c $(LIB) -o $(BIN)/rm

refresh:
	$(CC) refresh.c $(LIB) -o $(BIN)/refresh

clean:
	rm -rf $(BIN)/*


tar:
	tar cvf testbot.tar .
