CF = -llua -llualib -lconfuse -lpthread -L/usr/lib
CXXFLAGS += -g -fmessage-length=120
OBJS = socket.o test.o
all: test
test: $(OBJS)
	 g++ $(CFLAGS) $(OBJS) $(CF) -o test


socket.o: socket.cpp socket.h

test.o: test.cpp

clean:
	rm -rf *~ *.o core* test
