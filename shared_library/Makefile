#
# $Id: Makefile,v 1.0 2023-04-16 09:30:25 clib2devs Exp $
#

WARNINGS := \
	-Wall -W -Wextra -Wpointer-arith -Wsign-compare -Wmissing-prototypes \
	-Wundef -Wmissing-declarations -Wunused -Wwrite-strings -Wno-array-bounds -Wno-missing-braces -Wno-unused-value -Wno-comment \
	-Wno-deprecated-declarations -Wno-sign-compare -Wno-cast-function-type -Wno-unused-variable -Wno-parentheses -Wno-missing-prototypes \
	-Wstrict-aliasing -Wno-shadow -Wno-implicit-fallthrough -Wno-discarded-qualifiers -Wno-unused-function -Wno-unused-parameter -Wno-strict-aliasing \
    -Wno-type-limits -Wno-prio-ctor-dtor # -Wbad-function-cast -Wconversion -Wformat

CC 			:= ppc-amigaos-gcc
CPU 		:=
OPTIONS 	:= -mcrt=clib2 -DHAVE_SYSV -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D__CLIB2__ -Wa,-mregnames -fno-builtin -nostdlib -D_GNU_SOURCE -D_XOPEN_SOURCE -D_USE_GNU
OPTIMIZE 	:= -O
DEBUG 		:= -gstabs
CRTBEGIN 	?= /usr/ppc-amigaos/SDK/clib2/lib/crtbegin.o
CRTEND 		?= /usr/ppc-amigaos/SDK/clib2/lib/crtend.o
VERBOSE		:= @

###############################################################################

CFLAGS = $(WARNINGS) $(OPTIMIZE) $(DEBUG) $(CPU) $(OPTIONS) -I../library/include -I. -Iinclude -I../library

LIBC_SRCS = $(wildcard stubs/*.c)
STATIC_LIBC_SRCS = $(wildcard stubs/static/*.c)

LIBC_OBJS = $(patsubst %.c, build/%.o, $(LIBC_SRCS))
STATIC_LIBC_OBJS = $(patsubst %.c, build/%.o, $(STATIC_LIBC_SRCS))

LIBC_OBJS_PIC = $(LIBC_SRCS:.c=.o)
LIBC_OBJS_PIC := $(patsubst %.c, build/pic/%.o, $(LIBC_SRCS))
LIBC_OBJS_PIC += $(patsubst %.c, build/pic/%.o, $(STATIC_LIBC_SRCS))

###############################################################################

OBJS = \
	clib2.o \

###############################################################################

LIBS = -lc -lm -lprofile -lcrypt -lgcc

###############################################################################

all: clib2.library libc.a libc.so

clib2.library: $(OBJS)
	$(info $(LIBC_OBJS))
	$(VERBOSE)$(CC) -o $@ $(OBJS) $(CFLAGS) -Wl,--cref,-M,-Map=$@.map -nostartfiles -nostdlib $(LIBS)

libc.a: build/$(LIBC_OBJS) build/$(STATIC_LIBC_OBJS)
	rm -f $@
	ppc-amigaos-ar cru $@ $^

$(LIBC_OBJS_PIC): $(LIBC_SRCS)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

libc.so: $(LIBC_OBJS_PIC)
	rm -f $@
	$(CC) -shared -Wl,-soname,libc.so -o $@ $(LIBC_OBJS_PIC) -nostdlib -Wl,-Map=$@.map

clean:
	-rm -f *.o *.a *.debug *.map clib2.library build/stubs/*.o build/stubs/static/*.o build/pic/stubs/*.o build/pic/stubs/static/*.o