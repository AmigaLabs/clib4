#
# $Id: Makefile,v 1.14 2021-01-25 11:29:46 clib2devs Exp $
#
# :ts=8
#

##############################################################################

CC = ppc-amigaos-gcc
CXX = ppc-amigaos-g++
DELETE = rm -rf
MKDIR = mkdir -p
BUILDDIR = ./build
SOURCEDIR = .


##############################################################################

WARNINGS = -w

INCLUDE = -I../library/include
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
LFLAGS =

C_SOURCES = $(shell find $(SOURCEDIR) -maxdepth 2 -name '*.c' -type f -printf '%P\n')
CXX_SOURCES = $(shell find $(SOURCEDIR) -maxdepth 2 -name '*.cpp' -type f -printf '%P\n')
C_OBJECTS = $(patsubst %.c, %, $(C_SOURCES))
CXX_OBJECTS = $(patsubst %.cpp, %, $(CXX_SOURCES))

##############################################################################

LIBS = -lm -lpthread -lcrypt -lresolv

##############################################################################

all: builddir $(C_OBJECTS) $(CXX_OBJECTS)

builddir:
	$(MKDIR) $(BUILDDIR)

clean:
	$(DELETE) $(BUILDDIR)

##############################################################################

%: %.c
	@echo "Compiling $(BUILDDIR)/$@"
	@mkdir -p $(dir $(BUILDDIR)/$@)
	@$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $< $(LIBS) $(LFLAGS)

%: %.cpp
	@echo "Compiling $(BUILDDIR)/$@"
	@mkdir -p $(dir $(BUILDDIR)/$@)
	$(CXX) $(CFLAGS) -o $(BUILDDIR)/$@ $< $(LIBS) $(LFLAGS) -athread=native
