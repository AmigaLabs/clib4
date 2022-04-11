SRC = src

all:
	ppc-amigaos-g++ -mcrt=clib2 -O3 -gstabs src/impl.cpp src/memset.cpp -Wall -o memset_bench -athread=native
	ppc-amigaos-g++ -mcrt=newlib -O3 -gstabs src/impl.cpp src/memset.cpp -Wall -o memset_bench_newlib -athread=native
	g++ -O3 -gstabs src/impl.cpp src/memset.cpp -Wall -o memset_bench_linux

clean:
	rm -fr memset_bench memset_bench_newlib memset_bench_linux
