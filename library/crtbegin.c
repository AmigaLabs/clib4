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
 */

static void (*__CTOR_LIST__[1])(void) __attribute__((used, section(".ctors"), aligned(sizeof(void (*)(void)))));
static void (*__DTOR_LIST__[1])(void) __attribute__((used, section(".dtors"), aligned(sizeof(void (*)(void)))));

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
    static int j;

    for (i = 1, num_dtors = 0; __DTOR_LIST__[i] != NULL; i++)
        num_dtors++;

    while (j++ < num_dtors)
        __DTOR_LIST__[j]();

    /* The shared objects need to be cleaned up after all local
       destructors have been invoked. */
    shared_obj_exit();
}
