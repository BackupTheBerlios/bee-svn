CC=gcc
CFLAGS=-g -Wall -DDEBUG
LDFLAGS=-lcunit -lm

all: memtrace


memtrace:
	$(CC) $(CFLAGS) $(LDFLAGS) cuckoo.c memtrace.c -o memtrace

mmtrace:
	$(CC) $(CFLAGS) $(LDFLAGS) cuckoo.c mmtrace.c -o mmtrace

mtrace:
	$(CC) $(LDFLAGS) cuckoo.c  mmtrace.c -o mtrace -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops

buf:
	$(CC) $(CFLAGS) $(LDFLAGS)  buffer.c -o buf

clean:
	rm -rf memtrace mmtrace mtrace buf
