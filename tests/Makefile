#
# Makefile for clib4 test suite
#
# Note: These tests are designed to test the system's C library
# They do not test against clib4 directly since this is meant to be
# a cross-platform test suite that can validate C library implementations

# Compiler and flags
CC = ppc-amigaos-gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE -mcrt=clib4
LDFLAGS = -lm

# Test programs
TESTS = test_string test_stdlib test_stdio test_math test_time test_shm test_mmap test_mlock
TEST_RUNNER = test_runner

# All targets
all: $(TESTS) $(TEST_RUNNER)

# Individual test targets
test_string: test_string.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_stdlib: test_stdlib.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_stdio: test_stdio.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_math: test_math.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_time: test_time.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_shm: test_shm.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_mmap: test_mmap.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_mlock: test_mlock.c test_framework.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Test runner
test_runner: test_runner.c
	$(CC) $(CFLAGS) -o $@ $<

# Run all tests
run: all
	./$(TEST_RUNNER)

# Run individual tests
run-string: test_string
	./test_string

run-stdlib: test_stdlib
	./test_stdlib

run-stdio: test_stdio
	./test_stdio

run-math: test_math
	./test_math

run-time: test_time
	./test_time

run-shm: test_shm
	./test_shm

run-mmap: test_mmap
	./test_mmap

run-mlock: test_mlock
	./test_mlock

# Clean build artifacts
clean:
	rm -f $(TESTS) $(TEST_RUNNER)
	rm -f *.o
	rm -f /tmp/test_clib4*

# Help target
help:
	@echo "Clib4 Test Suite Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all         - Build all test programs (default)"
	@echo "  run         - Build and run all tests"
	@echo "  run-string  - Run string tests only"
	@echo "  run-stdlib  - Run stdlib tests only"
	@echo "  run-stdio   - Run stdio tests only"
	@echo "  run-math    - Run math tests only"
	@echo "  run-time    - Run time tests only"	@echo "  run-shm     - Run shared memory/mmap tests"	@echo "  clean       - Remove all build artifacts"
	@echo "  help        - Display this help message"

.PHONY: all run run-string run-stdlib run-stdio run-math run-time run-shm run-mmap clean help
