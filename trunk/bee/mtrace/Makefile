CC=gcc
CFLAGS=-g -ggdb3 -Wall -DDEBUG -dr
#OCFLAGS=-march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops
LDFLAGS= -lm
TARGETS= memtrace test mmtrace mtrace buf gendebug test_ck
all: memtrace

%.o: %.c
	$(CC) $(CFLAGS) $< -c

memtrace: cuckoo.o memtrace.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< memtrace.o -o memtrace

gendebug: gendebug.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o gendebug


test: cuckoo.o test_ck.o
	$(CC) $(CFLAGS) $(LDFLAGS) $< test_ck.o -o test_ck
	./test_ck

mmtrace:
	$(CC) $(CFLAGS) $(LDFLAGS) cuckoo.c mmtrace.c -o mmtrace

mtrace:
	$(CC) $(LDFLAGS) cuckoo.c  mmtrace.c -o mtrace -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops

buf:
	$(CC) $(CFLAGS) $(LDFLAGS)  buffer.c -o buf

clean:
	rm -rf $(TARGETS) *.db a.out core *.o
