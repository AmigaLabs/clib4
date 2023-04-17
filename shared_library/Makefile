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

STUBS = $(wildcard stubs/*.c)
STATIC_STUBS = $(wildcard stubs/static/*.c)

###############################################################################

OBJS = \
	clib2.o \
	$(STUBS:.c=.o) \
	$(STATIC_STUBS:.c=.o)

###############################################################################

LIBS = $(CRTBEGIN) -lm -lprofile -lcrypt -lgcc $(CRTEND)

###############################################################################

all: clib2.library

###############################################################################

clib2.library: $(OBJS)
	$(VERBOSE)$(CC) -o $@.debug $(OBJS) $(CFLAGS) -Wl,--cref,-M,-Map=$@.map \
		-nostartfiles -nostdlib $(LIBS)
	ppc-amigaos-strip -R.comment -o $@ $@.debug

clean:
	-rm -f *.o *.debug *.map clib2.library stubs/*.o