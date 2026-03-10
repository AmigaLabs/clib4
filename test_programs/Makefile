#
# $Id: makefile,v 1.14 2021-01-25 11:29:46 clib4devs Exp $
#
# :ts=8
#

##############################################################################

CC := ppc-amigaos-gcc
CXX := ppc-amigaos-g++

# On AmigaOS use native commands
ifeq ($(UNAME), AmigaOS)
    COPY := copy
    DELETE := delete all quiet force
    MAKEDIR := makedir all force
else
    COPY := cp -r
    DELETE := rm -rf
    MAKEDIR := mkdir -p
endif

BUILDDIR = ./build
SOURCEDIR = .
DEPDIR = $(BUILDDIR)/.deps


##############################################################################

WARNINGS = -w

INCLUDE = -I../library/include
#OPTIONS = -DDEBUG
 OPTIONS = -DNDEBUG
#OPTIMIZE = -O3
#DEBUG = -ggdb

# Note: Because the matching startup code needs to be used for
#       correctly linking the test programs, you need to make sure
#       that the current development version of clib4 is visible
#       where the linker expects it (soft link). Some more tuning would be
#       required here because you really should not need to tinker
#       with the location of library and the options "-L. -L../library/lib"
#       should be sufficient.

DEPFLAGS = -MMD -MP -MF $(DEPDIR)/$*.d
CFLAGS = -mcrt=clib4 -gstabs -fno-builtin $(WARNINGS) $(OPTIMIZE) $(DEBUG) $(OPTIONS) $(INCLUDE) $(DEPFLAGS)
LFLAGS = -Wl,--export-dynamic

C_SOURCES_ALL = $(shell find $(SOURCEDIR) -maxdepth 2 -name '*.c' -type f -printf '%P\n')
CXX_SOURCES_ALL = $(shell find $(SOURCEDIR) -maxdepth 2 -name '*.cpp' -type f -printf '%P\n')
# Exclude misc/start.cpp from normal build (needs -nostartfiles)
CXX_SOURCES = $(filter-out misc/start.cpp, $(CXX_SOURCES_ALL))
C_SOURCES = $(filter-out threads/curl_share.c, $(C_SOURCES_ALL))
C_OBJECTS = $(patsubst %.c, %, $(C_SOURCES))
CXX_OBJECTS = $(patsubst %.cpp, %, $(CXX_SOURCES))

##############################################################################

LIBS = -L../build/lib -lm -L../build/lib -lrt -L../build/lib -lpthread -L../build/lib -lcrypt -L../build/lib -ldebug

##############################################################################

all: builddir $(C_OBJECTS) $(CXX_OBJECTS) misc/start

builddir:
	@$(MAKEDIR) $(BUILDDIR)
	@$(MAKEDIR) $(DEPDIR)

clean:
	$(DELETE) $(BUILDDIR)

##############################################################################

%: %.c | builddir
	@mkdir -p $(dir $(BUILDDIR)/$@)
	@mkdir -p $(dir $(DEPDIR)/$@)
	@if [ ! -f $(BUILDDIR)/$@ ] || [ $< -nt $(BUILDDIR)/$@ ]; then \
		echo "Compiling $(BUILDDIR)/$@"; \
		$(CC) $(CFLAGS) -o $(BUILDDIR)/$@ $< $(LIBS) $(LFLAGS) -athread=native; \
	else \
		echo "$(BUILDDIR)/$@ is up to date"; \
	fi

%: %.cpp | builddir
	@mkdir -p $(dir $(BUILDDIR)/$@)
	@mkdir -p $(dir $(DEPDIR)/$@)
	@if [ ! -f $(BUILDDIR)/$@ ] || [ $< -nt $(BUILDDIR)/$@ ]; then \
		echo "Compiling $(BUILDDIR)/$@"; \
		$(CXX) $(CFLAGS) -o $(BUILDDIR)/$@ $< $(LIBS) $(LFLAGS) -athread=native; \
	else \
		echo "$(BUILDDIR)/$@ is up to date"; \
	fi

# Special rule for misc/start.cpp - needs -nostartfiles
misc/start: misc/start.cpp | builddir
	@mkdir -p $(dir $(BUILDDIR)/$@)
	@mkdir -p $(dir $(DEPDIR)/$@)
	@if [ ! -f $(BUILDDIR)/$@ ] || [ $< -nt $(BUILDDIR)/$@ ]; then \
		echo "Compiling $(BUILDDIR)/$@ (with -nostartfiles)"; \
		$(CXX) -nostartfiles $(CFLAGS) -o $(BUILDDIR)/$@ $< $(LIBS) $(LFLAGS) -athread=native; \
	else \
		echo "$(BUILDDIR)/$@ is up to date"; \
	fi

# Include dependency files
-include $(patsubst %.c,$(DEPDIR)/%.d,$(C_SOURCES))
-include $(patsubst %.cpp,$(DEPDIR)/%.d,$(CXX_SOURCES))
