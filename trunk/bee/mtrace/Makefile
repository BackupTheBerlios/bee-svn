CC=gcc
CFLAGS=-g -Wall -DDEBUG
OCFLAGS=-march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops
LDFLAGS=-lcunit -lm
TARGETS= memtrace test mmtrace mtrace buf gendebug test_ck
all: memtrace

%.o: %.c
	$(CC) $(CFLAGS) $< -c

memtrace: cuckoo.c
	$(CC) $(OCFLAGS) $(LDFLAGS) $< memtrace.c -o memtrace

gendebug: gendebug.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o gendebug


test: cuckoo.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< test_ck.c -o test_ck

mmtrace:
	$(CC) $(CFLAGS) $(LDFLAGS) cuckoo.c mmtrace.c -o mmtrace

mtrace:
	$(CC) $(LDFLAGS) cuckoo.c  mmtrace.c -o mtrace -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops

buf:
	$(CC) $(CFLAGS) $(LDFLAGS)  buffer.c -o buf

clean:
	rm -rf $(TARGETS) *.db a.out core
