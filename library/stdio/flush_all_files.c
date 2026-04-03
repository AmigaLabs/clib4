/*
 * $Id: stdio_flush_all_files.c,v 1.5 2025-01-01 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/*
 * Helper callback for _fwalk — flushes a stream if it matches
 * the target buffer mode.
 */
static int __flush_target_mode = -1;

static int
__flush_if_matching(struct _clib4 *__clib4, struct iob *fp) {
    if (FLAG_IS_SET(fp->iob_Flags, IOBF_WRITE) &&
        (__flush_target_mode < 0 || (fp->iob_Flags & IOBF_BUFFER_MODE) == (ULONG) __flush_target_mode) &&
        fp->iob_BufferWriteBytes > 0) {
        return __sflush(__clib4, fp);
    }
    return 0;
}

int
__flush_all_files(struct _clib4 *__clib4, int buffer_mode) {
    int result;

    ENTER();
    SHOWVALUE(buffer_mode);

    __stdio_lock(__clib4);

    __flush_target_mode = buffer_mode;
    result = _fwalk(__clib4, __flush_if_matching);

    __stdio_unlock(__clib4);

    RETURN(result == 0 ? OK : ERROR);
    return (result == 0 ? OK : ERROR);
}
