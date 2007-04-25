TOP=..
include $(TOP)/system.mk

LIB = $(TOP)/lib/$(OS)/libtbot.a -lglib-2.0
BIN = $(TOP)/bin/tools
TARGETS=rexec cp mkdir rm refresh start stop


all: $(TARGETS)

rexec:
	$(CC) $(CFLAGS) rexec.c $(LDFLAGS) $(LIB) -o $(BIN)/rexec
cp:
	$(CC) $(CFLAGS) cp.c $(LDFLAGS) $(LIB) -o $(BIN)/cp

mkdir:
	$(CC) $(CFLAGS) mkdir.c $(LDFLAGS) $(LIB) -o $(BIN)/mkdir
rm:
	$(CC) $(CFLAGS) rm.c $(LDFLAGS) $(LIB) -o $(BIN)/rm

refresh:
	$(CC) $(CFLAGS) refresh.c $(LDFLAGS) $(LIB) -o $(BIN)/refresh

stop:
	$(CC) $(CFLAGS) stop.c $(LDFLAGS) $(LIB) -o $(BIN)/stop

start:
	$(CC) $(CFLAGS) start.c $(LDFLAGS) $(LIB) -o $(BIN)/start
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
