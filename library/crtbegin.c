/*
* $Id: crtbegin.c,v 1.3 2022-03-09 21:07:25 clib2devs Exp $
  */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/elf.h>
#include <libraries/elf.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

/*
 * Dummy constructor and destructor array. The linker script will put these at the
 * very beginning of section ".ctors" and ".dtors". crtend.o contains a similar entry
 * with a NULL pointer entry and is put at the end of the sections. This way, the init
 * code can find the global constructor/destructor pointers.
 *
 * WARNING:
 * This hack does not work correctly with GCC 5 and higher. The optimizer
 * will see a one element array and act appropriately. The current workaround
 * is to use -fno-aggressive-loop-optimizations when compiling this file.
 *
 * NOTE:  In order to be able to support SVR4 shared libraries, we arrange
 * to have one set of symbols { __CTOR_LIST__, __DTOR_LIST__, __CTOR_END__,
 * __DTOR_END__ } per root executable and also one set of these symbols
 * per shared library.  So in any given whole process image, we may have
 * multiple definitions of each of these symbols.  In order to prevent
 * these definitions from conflicting with one another, and in order to
 * ensure that the proper lists are used for the initialization/finalization
 * of each individual shared library (respectively), we give these symbols
 * only internal (i.e. `static') linkage, and we also make it a point to
 * refer to only the __CTOR_END__ symbol in sh/crtend.o and the __DTOR_LIST__
 * symbol in sh/crtbegin.o, where they are defined.  */

typedef void (*func_ptr) (void);

static func_ptr __CTOR_LIST__[1] __attribute__((used, section(".ctors"), aligned(sizeof(func_ptr)))) = { (func_ptr) (-1) };;
static func_ptr __DTOR_LIST__[1] __attribute__((used, section(".dtors"), aligned(sizeof(func_ptr)))) = { (func_ptr) (-1) };;

void _init(void) {
    int num_ctors, i;
    int j;

    /* The shared objects need to be set up before any local constructors are invoked. */
    shared_obj_init();

    for (i = 1, num_ctors = 0; __CTOR_LIST__[i] != NULL; i++)
        num_ctors++;

    for (j = 0; j < num_ctors; j++)
        __CTOR_LIST__[num_ctors - j]();
}

void _fini(void) {
    int num_dtors, i;
    static int j = 0;

    for (i = 1, num_dtors = 0; __DTOR_LIST__[i] != NULL; i++)
        num_dtors++;

    while (j++ < num_dtors)
        __DTOR_LIST__[j]();

    /* The shared objects need to be cleaned up after all local
       destructors have been invoked. */
    shared_obj_exit();
}
