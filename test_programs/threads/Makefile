tests:=pthread_join semaphore_test semaphore_test1 simple try_to_time call_once
.PHONY: all
all: $(tests)

ifneq ($(shell uname),AmigaOS)
CC:=ppc-amigaos-gcc
CXX:=ppc-amigaos-g++
else
CC:=gcc
CXX:=g++
endif

CFLAGS:=-mcrt=clib2
CXXFLAGS:=-mcrt=clib2

LD_C:=$(CC)
LD_CXX:=$(CXX)
LDFLAGS:=-mcrt=clib2 -athread=native
LDLIBS:=-lpthread

pthread_join: pthread_join.o
	$(LD_C) $(LDFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

semaphore_test: semaphore_test.o
	$(LD_C) $(LDFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

semaphore_test1: semaphore_test1.o
	$(LD_C) $(LDFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

simple: simple.o
	$(LD_C) $(LDFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

try_to_time: try_to_time.o
	$(LD_CXX) $(LDFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

call_once: call_once.o
	$(LD_CXX) $(LDFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)
	@echo "======================================================="
	@echo "Expected output for \"call_once\" is between --- lines:"
	@echo "---"
	@echo "Simple example: called once"
	@echo "throw: call_once will retry"
	@echo "throw: call_once will retry"
	@echo "Didn't throw, call_once will not attempt again"
	@echo "---"

clean:
	-rm -f $(tests) *.o
