#
# $Id: Makefile,v 1.14 2021-01-25 11:29:46 apalmate Exp $
#
# :ts=8
#

##############################################################################

CC = ppc-amigaos-gcc
DELETE = rm -rf
MKDIR = mkdir -p
BUILDDIR = build
SOURCEDIR = .
##############################################################################

.c.o:
	@echo "Compiling $<"
	@$(CC) -c $(CFLAGS) $<

##############################################################################

WARNINGS = \
	-Wall -W -Wshadow -Wpointer-arith -Wsign-compare -Wmissing-prototypes \
	-Wundef -Wbad-function-cast -Wmissing-declarations -Wconversion \
	-Wno-unused-parameter -Wno-missing-prototypes

V = /V
INCLUDE = -I$(V)/include -I../library/include
#OPTIONS = -D__MEM_DEBUG
#OPTIONS = -DDEBUG
 OPTIONS = -DNDEBUG
#OPTIMIZE = -O3
#DEBUG = -ggdb

# Note: Because the matching startup code needs to be used for
#       correctly linking the test programs, you need to make sure
#       that the current development version of clib2 is visible
#       where the linker expects it (soft link). Some more tuning would be
#       required here because you really should not need to tinker
#       with the location of library and the options "-L. -L../library/lib"
#       should be sufficient.

CFLAGS = -mcrt=clib2 -fno-builtin $(WARNINGS) $(OPTIMIZE) $(DEBUG) $(OPTIONS) $(INCLUDE)
LFLAGS = -Wl,-d

SOURCES = $(shell find $(SOURCEDIR) -name '*.cpp' -or -name '*.c')
OBJECTS = $(patsubst %.c, %, $(notdir $(wildcard *.c)))

##############################################################################

LIBS = -ldebug -lm

##############################################################################

all: builddir $(OBJECTS)
	
builddir: ${BUILDDIR}

clean:
	$(DELETE) $(BUILDDIR)

##############################################################################

${BUILDDIR}:
	$(MKDIR) $(BUILDDIR)

%: %.c
	@echo "Linking $(BUILDDIR)/$@"
	@$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $< $(LIBS) $(LFLAGS) -Wl,--cref,-M,-Map=$(BUILDDIR)/$@.map

simple_sprintf : simple_sprintf
	@echo "Linking $(BUILDDIR)/$@"
	@$(CC) $(CFLAGS) -nostdlib -o $(BUILDDIR)/$@ simple_sprintf.c -lc -lgcc $(LFLAGS) -Wl,--cref,-M,-Map=$(BUILDDIR)/$@.map

semaphore_test : semaphore_test
	@echo "Linking $(BUILDDIR)/$@"
	@$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ semaphore_test.c -lpthread $(LFLAGS) -Wl,--cref,-M,-Map=$(BUILDDIR)/$@.map

pthread_join: pthread_join
	@echo "Linking $(BUILDDIR)/$@"
	@$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ pthread_join.c -lpthread $(LFLAGS) -Wl,--cref,-M,-Map=$(BUILDDIR)/$@.map

