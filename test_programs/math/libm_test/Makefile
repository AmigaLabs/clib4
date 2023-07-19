CC ?= ppc-amigaos-gcc
CXX ?= ppc-amigaos-g++
CFLAGS ?= -mcrt=clib2 -O3
LDFLAGS ?= -mcrt=clib2 -athread=native

all: test-double test-float # test-double-system test-float-system

bench: bench-libm

test-double: test-double.c libm-test.c libm-test-ulps.h
	$(CC) $(CFLAGS) $(LDFLAGS) $@.c -D__BSD_VISIBLE -o $@

test-float: test-float.c libm-test.c libm-test-ulps.h
	$(CC) $(CFLAGS) $(LDFLAGS) $@.c -D__BSD_VISIBLE -o $@

bench-libm: libm-bench.cpp
	$(CXX) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -fr test-double test-float test-double-system test-float-system bench-openlibm bench-syslibm
