CC=gcc
CFLAGS=-g -DDEBUG
LDFLAGS=-lcunit

all: mmtrace


mmtrace:
	$(CC) $(CFLAGS) $(LDFLAGS) mmtrace.c -o mmtrace

mtrace:
	$(CC) -march=i686 -mtune=i686 -O9 -finline-functions -funroll-loops -mno-push-args -fomit-frame-pointer -m3dnow -mmmx -minline-all-stringops mmtrace.c -o mtrace
