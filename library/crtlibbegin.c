/*
 * crtlibbegin.c - the start object for a library that is linked statically
 * (freetype.library, for one). It brackets the constructor, destructor and
 * exception frame sections and nothing else: there is no _start, no
 * clib4_start and no reference to main, so a library object defines its own
 * entry point and the program start (crtbegin.o) stays a program's.
 *
 * Pair it with crtend.o at the end of the link line, as the program objects do.
 */

/* The dummy entries the linker script puts first in .ctors and .dtors. crtend.o
 * terminates the lists. Static linkage, so every object in the link keeps its
 * own pair. */
static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"))) = { (void *)~0 };
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"))) = { (void *)~0 };

/* Marks the beginning of the exception handling frames (crtend.o marks the end). */
static const char __EH_FRAME_BEGIN__[] __attribute__((used, section(".eh_frame"), aligned(4))) = { };
