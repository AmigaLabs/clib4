/*
* $Id: backtrace.c,v 1.0 2025-03-02 12:06:14 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <execinfo.h>

typedef struct StackFrame {
    struct Frame *prev;  // Pointer to previous frame
    void *return_address; // The return address (stored in LR)
} StackFrame;

int
backtrace(void **buffer, int max_frames) {
    StackFrame *frame;
    void **bp;
    unsigned int count = 0;

    // Get the frame pointer, which is usually stored in r1 on PowerPC
    __asm__ volatile("mr %0, r1" : "=r"(bp));

    // Walk the stack by following the saved frame pointer
    while (bp != NULL && count < max_frames) {
        frame = (StackFrame *) bp;
	    if (frame->return_address != 0)
            buffer[count++] = frame->return_address;
	    else
            count++;
        // Move to the previous frame (the saved frame pointer is at bp[0])
        bp = (void **)frame->prev;
    }

    return count;
}
