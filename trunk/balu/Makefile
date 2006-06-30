CF = -llua -llualib -lconfuse -lpthread -L/usr/lib
CXXFLAGS += -g -fmessage-length=120
OBJS = debug.o socket.o config.o luaConfig.o wrapper.o luaWrapper.o util.o balu.o
all: balu
balu: $(OBJS)
	 g++ $(CFLAGS) $(OBJS) $(CF) /usr/lib/libpcre.a  -o balu

debug.o: debug.cpp debug.h

socket.o: socket.cpp socket.h


config.o: config.cpp config.h

luaConfig.o: luaConfig.cpp luaConfig.h


wrapper.o: wrapper.cpp wrapper.h debug.h

luaWrapper.o: luaWrapper.cpp luaWrapper.h

util.o : util.cpp util.h

balu.o: balu.cpp

clean:
	rm -rf *~ *.o core* balu tags
tags:
	ctags --recurse=yes --c-kinds=+cdefgmnpstuvx  --file-scope=no *
