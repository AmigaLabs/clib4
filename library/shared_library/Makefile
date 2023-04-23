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
DEBUG 		:= -gstabs -DDEBUG
CRTBEGIN 	?= /usr/ppc-amigaos/SDK/clib2/lib/crtbegin.o
CRTEND 		?= /usr/ppc-amigaos/SDK/clib2/lib/crtend.o
VERBOSE		:= @

###############################################################################

CFLAGS = $(WARNINGS) $(OPTIMIZE) $(DEBUG) $(CPU) $(OPTIONS) -I../library/include -I. -Iinclude -I../library

LIBC_SRCS = stubs/stubs.c
LIBC_OBJS = $(patsubst stubs/%.c, build/%.o, $(LIBC_SRCS))
LIBC_OBJS_PIC := $(patsubst stubs/%.c, build/pic/%.o, $(LIBC_SRCS))

###############################################################################

OBJS = \
	clib2.o \

###############################################################################

LIBS = -lgcc

###############################################################################

all: clib2.library.debug clib2.library libc.a libc.so

clib2.library.debug: $(OBJS)
	$(VERBOSE)$(CC) -o $@ $(OBJS) $(CFLAGS) $(C_LIB) $(C_MATH) $(PROFILE_LIB) $(CRYPT_LIB) -Wl,--cref,-M,-Map=$@.map -nostartfiles -nostdlib $(LIBS)

clib2.library: clib2.library.debug
	cp $< $@
	ppc-amigaos-strip -R.comment -R.sdata2 --strip-unneeded-rel-relocs $@

$(LIBC_OBJS_PIC): $(LIBC_SRCS)
	$(VERBOSE)$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(LIBC_OBJS): $(LIBC_SRCS)
	$(VERBOSE)$(CC) $(CFLAGS) -c $< -o $@

libc.so: $(LIBC_OBJS_PIC)
	$(VERBOSE)$(CC) -shared -Wl,-soname,libc.so -o $@ $(LIBC_OBJS_PIC) -nostdlib -Wl,-Map=$@.map

libc.a: $(LIBC_OBJS)
	$(VERBOSE)ppc-amigaos-ar cru $@ $^

clean:
	$(VERBOSE)rm -f *.o *.a *.so *.debug *.map clib2.library clib2.library.debug build/*.o build/pic/*.o