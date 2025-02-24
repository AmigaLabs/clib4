app = dlopen_sample
lib = libcat.so libdog.so

CC = ppc-amigaos-gcc
CRT ?= clib4
CFLAGS = -mcrt=$(CRT) -gstabs -Wall -pedantic
#LIBS=-ldl #-ldebug
LDFLAGS = -mcrt=$(CRT) -use-dynld -athread=native
ifeq ($(CRT), newlib)
	LDFLAGS += -ldl
endif

all: app libcat.so libdog.so

app: 
	$(CC) dlopen_sample.c $(CFLAGS) -o $(app) $< $(LDFLAGS) $(LIBS)

libcat.so:
	$(CC) -shared -fPIC cat.c $(CFLAGS) -o libcat.so -Wl,-soname libcat1.so -Wl,-rpath=/sobjs/clib4/

libdog.so:
	$(CC) -shared -fPIC dog.c $(CFLAGS) -o libdog.so -Wl,-soname libdog1.so -Wl,-rpath=/sobjs/clib4/

clean:
	$(RM) $(app) $(lib)

$(lib): animal.h

.PHONY: app
