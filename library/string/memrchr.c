/*
 * $Id: string_memrchr.c,v 1.0 2021-02-23 10:32:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

/*
 * Reverse memchr()
 * Find the last occurrence of 'c' in the buffer 's' of size 'n'.
 */
void *
memrchr(const void *s, int c, size_t n) {
    const unsigned char *cp;

    ENTER();
    SHOWPOINTER(s);
    SHOWVALUE(c);
    SHOWVALUE(n);

    if (n != 0) {
        cp = (unsigned char *) s + n;
        do {
            if (*(--cp) == (unsigned char) c) {
                SHOWPOINTER(cp);
                LEAVE();
                return ((void *) cp);
            }
        } while (--n != 0);
    }
    LEAVE();
    return (NULL);
}
