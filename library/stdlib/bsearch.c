/*
 * $Id: stdlib_bsearch.c,v 1.4 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void *
bsearch(const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value)) {
    void *result = NULL;

    ENTER();

    SHOWPOINTER(key);
    SHOWPOINTER(base);
    SHOWVALUE(count);
    SHOWVALUE(size);
    SHOWPOINTER(compare);

    if (count > 0 && size > 0) {
        void *current;
        size_t lower = 0;
        size_t upper = count;
        size_t position;
        int delta;

        assert(key != NULL && base != NULL && compare != NULL);

        if (key == NULL || base == NULL || compare == NULL) {
            SHOWMSG("invalid parameters");

            __set_errno(EFAULT);
            goto out;
        }

        while (lower < upper) {
            position = (lower + upper) / 2;

            current = (void *) (((ULONG) base) + (position * size));

            delta = (*compare)(key, current);
            if (delta == 0) {
                result = current;
                break;
            }

            if (delta < 0)
                upper = position;
            else
                lower = position + 1;
        }
    }

    out:

    RETURN(result);
    return (result);
}
