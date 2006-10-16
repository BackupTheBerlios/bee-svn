CC  = gcc -g -I../ -Wall
LIB = ../util.c ../socket.c
BIN = ./bin
TARGETS=axigen_action cp mkdir rm refresh_client


all: $(TARGETS)
	find ./bin/ -maxdepth 1 -type f -exec scp {} testbot@gigi:/home/testbot/current/engine/ \;
	find ./bin/ -maxdepth 1 -type f -exec cp {} ../../engine \;

axigen_action:
	$(CC) axigen_action.c $(LIB) -o $(BIN)/axigen_action
cp:
	$(CC) cp.c $(LIB) -o $(BIN)/cp

mkdir:
	$(CC) mkdir.c $(LIB) -o $(BIN)/mkdir
rm:
	$(CC) rm.c $(LIB) -o $(BIN)/rm

refresh_client:
	$(CC) refresh_client.c $(LIB) -o $(BIN)/refresh_client

clean:
	rm -rf $(BIN)/*


tar:
	tar cvf testbot.tar .
