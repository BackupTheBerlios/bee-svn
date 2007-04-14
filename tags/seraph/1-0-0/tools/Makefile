CC  = gcc -g -I../include -Wall# -pedantic
LIB = ../lib/libtbot.a -lglib-2.0
BIN = ../bin/tools
TARGETS=rexec cp mkdir rm refresh start stop
CFLAGS = `pkg-config --cflags glib-2.0`


all: $(TARGETS)

rexec:
	$(CC) $(CFLAGS) rexec.c $(LIB) -o $(BIN)/rexec
cp:
	$(CC) $(CFLAGS) cp.c $(LIB) -o $(BIN)/cp

mkdir:
	$(CC) $(CFLAGS) mkdir.c $(LIB) -o $(BIN)/mkdir
rm:
	$(CC) $(CFLAGS) rm.c $(LIB) -o $(BIN)/rm

refresh:
	$(CC) $(CFLAGS) refresh.c $(LIB) -o $(BIN)/refresh

stop:
	$(CC) $(CFLAGS) stop.c $(LIB) -o $(BIN)/stop

start:
	$(CC) $(CFLAGS) start.c $(LIB) -o $(BIN)/start
clean:
	rm -rf $(BIN)/*


tar:
	tar cvf seraph.tar .

install:
	find ./bin/ -maxdepth 1 -type f -exec cp {} ../../engine \;

rmtinst:
	find ./bin/ -maxdepth 1 -type f -exec scp {} seraph@gigi:/home/seraph/current/engine/ \;
