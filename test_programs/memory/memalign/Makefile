CC := ppc-amigaos-gcc
CFLAGS := -mcrt=clib4 -O3 -gstabs
SRC = src

all:
	$(CC) $(CFLAGS) src/align.c src/byte.c src/measure.c src/half.c src/word.c src/double.c src/alloc.c src/print.c -Wall -o memaligntest

clean:
	rm -fr memaligntest
