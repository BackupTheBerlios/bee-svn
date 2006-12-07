CC=gcc
CFLAGS=-g -Wall # -DDEBUG 
LDFLAGS=-lcunit -lm

all: mmtrace


mmtrace:
	$(CC) $(CFLAGS) $(LDFLAGS) cuckoo.c mmtrace.c -o mmtrace

mtrace:
	$(CC) $(LDFLAGS) cuckoo.c  mmtrace.c -o mtrace -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops

clean:
	rm -rf mmtrace mtrace
