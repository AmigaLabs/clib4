/*
 * $Id: stdio_fwalk.c,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Walk all open FILE streams and apply a callback function.
 * Used by fflush(NULL), exit cleanup, etc.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

/*
 * _fwalk_sglue — Walk all streams in a single glue block and its successors.
 *
 * Applies `func` to every stream that has IOBF_IN_USE set.
 * Returns 0 on total success, or the OR of all non-zero return values.
 */
int
_fwalk_sglue(struct _clib4 *__clib4, int (*func)(struct _clib4 *, struct iob *), struct _glue *g) {
    struct iob *fp;
    int n;
    int ret = 0;

    ENTER();

    for (; g != NULL; g = g->next) {
        for (n = 0; n < g->niobs; n++) {
            fp = g->iobs[n];
            if (fp != NULL && FLAG_IS_SET(fp->iob_Flags, IOBF_IN_USE)) {
                ret |= (*func)(__clib4, fp);
            }
        }
    }

    RETURN(ret);
    return ret;
}

/****************************************************************************/

/*
 * _fwalk — Walk all open streams starting from the global glue root.
 *
 * Calls `func` on every active FILE. Returns 0 on total success.
 */
int
_fwalk(struct _clib4 *__clib4, int (*func)(struct _clib4 *, struct iob *)) {
    return _fwalk_sglue(__clib4, func, __clib4->__sglue);
}
