/*
 * backtrace.c - the return addresses of the calling frames, for ARM.
 *
 * The PowerPC version walks the back chain in r1. AAPCS keeps no back chain, so
 * a walk needs the unwind tables, which this build does not link. The result is
 * therefore the caller's return address alone, as one frame; that is reported
 * honestly as the count, and never more than the buffer holds.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <execinfo.h>

int
backtrace(void **buffer, int max_frames)
{
    if (buffer == 0 || max_frames < 1)
        return 0;
    buffer[0] = __builtin_return_address(0);
    return 1;
}
