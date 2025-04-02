/*
* $Id: backtrace.c,v 1.0 2025-03-02 12:06:14 clib4devs Exp $
*/

#include <execinfo.h>

// Get the current frame pointer
static void *get_frame_pointer() {
    void *fp;

    __asm__ volatile ("mr %0, 1" : "=r"(fp));

    return fp;
}

int
backtrace(void **buffer, int max_frames) {
    void **fp = (void **)get_frame_pointer();
    int count = 0;

    while (fp && count < max_frames) {
        // The return address is usually at fp[1]
        void *ret_addr = fp[1];
        if (!ret_addr) break;

        buffer[count++] = ret_addr;

        // Move to the previous frame
        fp = (void **)fp[0];
    }

    return count;
}