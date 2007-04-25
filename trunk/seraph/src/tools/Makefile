TOP=..
include $(TOP)/system.mk

LIB = ../lib/libtbot.a -lglib-2.0
BIN = ../bin/tools
TARGETS=rexec cp mkdir rm refresh start stop


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
	rm -rf $(BIN)/* core *.debug


tar:
	tar cvf seraph.tar .

install:
	$(INSTALLDIR) $(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/rexec $(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/cp $(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/mkdir $(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/rm $(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/refresh $(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/stop $(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/start $(LIBDIR)

rmtinst:
	find ./bin/ -maxdepth 1 -type f -exec scp {} seraph@gigi:/home/seraph/current/engine/ \;
