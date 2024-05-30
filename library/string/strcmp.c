/*
 * $Id: string_strcmp.c,v 1.6 2022-03-22 15:33:36 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

inline static
int
__strcmp(const char *s1, const char *s2) {
    int result = 0;

    if (s1 != s2) {
        int c1, c2;

        while ((*s1) == (*s2)) {
            if ((*s1) == '\0')
                break;

            s1++;
            s2++;
        }

        /* The comparison must be performed as if the
           characters were unsigned characters. */
        c1 = *(unsigned char *) s1;
        c2 = *(unsigned char *) s2;

        result = c1 - c2;
    }

    return result;
}

int
strcmp(const char *s1, const char *s2) {
    return __strcmp(s1, s2);
}
