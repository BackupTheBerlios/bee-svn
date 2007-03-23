CC  = gcc -g -I../include -Wall# -pedantic
LIB = ../lib/libtbot.a
BIN = ../bin/tools
TARGETS=rexec cp mkdir rm refresh start stop
# Solaris
# LDFLAGS= -lsocket -lnsl


all: $(TARGETS)

rexec:
	$(CC) $(LDFLAGS) rexec.c $(LIB) -o $(BIN)/rexec
cp:
	$(CC) $(LDFLAGS) cp.c $(LIB) -o $(BIN)/cp

mkdir:
	$(CC) $(LDFLAGS) mkdir.c $(LIB) -o $(BIN)/mkdir
rm:
	$(CC) $(LDFLAGS) rm.c $(LIB) -o $(BIN)/rm

refresh:
	$(CC) $(LDFLAGS) refresh.c $(LIB) -o $(BIN)/refresh

stop:
	$(CC) $(LDFLAGS) stop.c $(LIB) -o $(BIN)/stop

start:
	$(CC) $(LDFLAGS) start.c $(LIB) -o $(BIN)/start
clean:
	rm -rf $(BIN)/*


tar:
	tar cvf seraph.tar .

install:
	find ./bin/ -maxdepth 1 -type f -exec cp {} ../../engine \;

rmtinst:
	find ./bin/ -maxdepth 1 -type f -exec scp {} seraph@gigi:/home/seraph/current/engine/ \;
