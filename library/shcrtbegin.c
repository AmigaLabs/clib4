/*
 * $Id: shcrtbegin.c,v 1.1 2023-02-18 17:22:03 clib4devs Exp $
 */

#include <proto/exec.h>
#include "debug.h"

void __shlib_call_constructors(void);
void __shlib_call_destructors(void);

/* NOTE:  In order to be able to support SVR4 shared libraries, we arrange
 * to have one set of symbols { __CTOR_LIST__, __DTOR_LIST__, __CTOR_END__,
 * __DTOR_END__ } per root executable and also one set of these symbols
 * per shared library.  So in any given whole process image, we may have
 * multiple definitions of each of these symbols.  In order to prevent
 * these definitions from conflicting with one another, and in order to
 * ensure that the proper lists are used for the initialization/finalization
 * of each individual shared library (respectively), we give these symbols
 * only internal (i.e. `static') linkage, and we also make it a point to
 * refer to only the __CTOR_END__ symbol in sh/crtend.o and the __DTOR_LIST__
 * symbol in sh/crtbegin.o, where they are defined.
 */

static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors")));
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors")));

void __shlib_call_constructors(void) {
    extern void (*__CTOR_LIST__[])(void);
    int i = 0;

    ENTER();
    while (__CTOR_LIST__[i + 1]) {
        i++;
    }
    SHOWVALUE(i);
    while (i > 0) {
        D(("Calling constructor %ld", i));
        __CTOR_LIST__[i--]();
    }
    LEAVE();
}

void __shlib_call_destructors(void) {
    extern void (*__DTOR_LIST__[])(void);
    int i = 1;

    ENTER();
    while (__DTOR_LIST__[i]) {
        D(("Calling destructor %ld", i));
        __DTOR_LIST__[i++]();
    }
    LEAVE();
}
