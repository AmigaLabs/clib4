/*
 * $Id: stdlib_swapstack.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/* Swap the current stack configuration out, call a function provided, swap the stack configuration back and return. */
int
__swap_stack_and_call(struct StackSwapStruct *stk, APTR function) {
    register int result;

    StackSwap(stk);
    result = ((int (*)(void)) function)();
    StackSwap(stk);

    return (result);
}
