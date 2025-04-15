SRC = src

all:
	ppc-amigaos-g++ -mcrt=clib4 -O3 -gstabs src/impl.c src/memcpy.cpp -Wall -o memcpy_bench -athread=native
	ppc-amigaos-g++ -mcrt=newlib -O3 -gstabs src/impl.c src/memcpy.cpp -Wall -o memcpy_bench_newlib -athread=native
	g++ -O3 -gstabs src/impl.c src/memcpy.cpp -Wall -o memcpy_bench_linux

clean:
	rm -fr memcpy_bench memcpy_bench_newlib memcpy_bench_linux
