TOP=..
include $(TOP)/system.mk

LIB = $(TOP)/lib/$(OS)/libtbot.a -lglib-2.0
BIN = $(TOP)/bin/tools
TARGETS=rexec cp mkdir rm refresh start stop


all: $(TARGETS)

rexec:
	$(CC) $(CFLAGS) rexec.c $(LDFLAGS) $(LIB) -o $(BIN)/rexec
	@echo ""

cp:
	$(CC) $(CFLAGS) cp.c $(LDFLAGS) $(LIB) -o $(BIN)/cp
	@echo ""

mkdir:
	$(CC) $(CFLAGS) mkdir.c $(LDFLAGS) $(LIB) -o $(BIN)/mkdir
	@echo ""

rm:
	$(CC) $(CFLAGS) rm.c $(LDFLAGS) $(LIB) -o $(BIN)/rm
	@echo ""

refresh:
	$(CC) $(CFLAGS) refresh.c $(LDFLAGS) $(LIB) -o $(BIN)/refresh
	@echo ""

stop:
	$(CC) $(CFLAGS) stop.c $(LDFLAGS) $(LIB) -o $(BIN)/stop
	@echo ""

start:
	$(CC) $(CFLAGS) start.c $(LDFLAGS) $(LIB) -o $(BIN)/start
	@echo ""

clean:
	rm -rf core *.debug
	rm -rf $(BIN)/rexec $(BIN)/cp $(BIN)/mkdir $(BIN)/rm $(BIN)/refresh $(BIN)/start $(BIN)/stop


tar:
	tar cvf seraph.tar .

install:
	$(INSTALLDIR) $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/rexec   $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/cp      $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/mkdir   $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/rm      $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/refresh $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/stop    $(DESTDIR)$(LIBDIR)
	$(INSTALL) -m $(BIN_MODE) $(BIN)/start   $(DESTDIR)$(LIBDIR)

rmtinst:
	find ./bin/ -maxdepth 1 -type f -exec scp {} seraph@gigi:/home/seraph/current/engine/ \;
