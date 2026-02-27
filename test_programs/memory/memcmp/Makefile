CC = ppc-amigaos-gcc
CPPFLAGS = -DNDEBUG
CFLAGS = -O3 -mcrt=clib4 -std=c11 -Wno-error=array-bounds -Wall -Wextra -Werror -pedantic -O3
LDFLAGS =
LIBS = -lm

compile: memcmp

all: plot.png

plot.png: plot.gp timing_stdlib.dat timing_naive.dat timing_yamiez.dat
	gnuplot $<

timing_stdlib.dat timing_naive.dat timing_yamiez.dat: main
	./$<

memcmp: main.o memcmp_stdlib.o memcmp_yamiez.o memcmp_naive.o
	${CC} -o $@ ${CFLAGS} $^ ${LDFLAGS} ${LIBS}

main.o: main.c memcmp.h
	${CC} -c ${CPPFLAGS} ${CFLAGS} $<

memcmp_stdlib.o: memcmp_stdlib.c memcmp.h
	${CC} -c ${CPPFLAGS} ${CFLAGS} $<

memcmp_yamiez.o: memcmp_yamiez.c memcmp.h
	${CC} -c ${CPPFLAGS} ${CFLAGS} $<

memcmp_naive.o: memcmp_naive.c memcmp.h
	${CC} -c ${CPPFLAGS} ${CFLAGS} $<

mostlyclean:
	rm -f fit.log *.o

clean: mostlyclean
	rm -f memcmp *.dat plot.png

.PHONY: compile all mostlyclean clean