/*
 * $Id: stdlib_stack_usage.c,v 1.8 2010-10-20 13:50:17 clib2devs Exp $
*/

#ifndef NDEBUG

#include "stdlib_headers.h"

#define STACK_FILL_COOKIE 0xA1

void
__stack_usage_init(struct StackSwapStruct *stk) {
    struct _clib2 *__clib2 = __CLIB2;

    if (stk != NULL) {
        size_t stack_size = ((ULONG) stk->stk_Upper - (ULONG) stk->stk_Lower);

        memset(stk->stk_Lower, STACK_FILL_COOKIE, stack_size);

        __clib2->stack_swap_struct = (*stk);
    }
}

void
__stack_usage_exit(void) {
    struct _clib2 *__clib2 = __CLIB2;

    if (__clib2->stack_swap_struct.stk_Lower != NULL && __clib2->stack_swap_struct.stk_Upper != 0) {
        const UBYTE *m = (const UBYTE *) __clib2->stack_swap_struct.stk_Lower;
        size_t stack_size = ((ULONG) __clib2->stack_swap_struct.stk_Upper - (ULONG) __clib2->stack_swap_struct.stk_Lower);
        size_t total, i;

        total = 0;

        /* Figure out how much of the stack was used by checking
           if the fill pattern was overwritten. */
        for (i = 0; i < stack_size; i++) {
            /* Strangely, the first long word is always trashed,
               even if the stack doesn't grow down this far... */
            if (i > sizeof(LONG) && m[i] != STACK_FILL_COOKIE)
                break;

            total++;
        }

        kprintf("[%s] total amount of stack space used = %ld bytes\n", __clib2->__progname, stack_size - total);

        __clib2->stack_swap_struct.stk_Lower = NULL;
        __clib2->stack_swap_struct.stk_Upper = 0;
    }
}

#endif /* NDEBUG */
