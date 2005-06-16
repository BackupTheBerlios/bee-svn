CF = -llua -llualib -lconfuse -lpthread -L/usr/lib
CXXFLAGS += -g -fmessage-length=120
OBJS = socket.o test_socket.o
all: test_socket test_info
test_socket: socket.o test_socket.o
	g++ $(CFLAGS) socket.o test_socket.o $(CF) -o test_socket

test_info: info.o test_info.o
	g++ $(CFLAGS) info.o test_info.o $(CF) -o test_info

info.o: info.cpp info.h

socket.o: socket.cpp socket.h

test_socket.o: test_socket.cpp

test_info.o: test_info.cpp

clean:
	rm -rf *~ *.o core* test_socket test_info
