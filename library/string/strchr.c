/*
 * $Id: string_strchr.c,v 1.7 2024-08-05 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
glibc_strchr(const char *s, int c_in) {
    const unsigned char *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int longword, magic_bits, charmask;
    register unsigned char c;
    c = (unsigned char) c_in;
    for (char_ptr = (const unsigned char *) s;
         ((unsigned long int) char_ptr & (sizeof(longword) - 1)) != 0;
         ++char_ptr)
        if (*char_ptr == c)
            return (char *) char_ptr;
        else if (*char_ptr == '\0')
            return NULL;
    longword_ptr = (unsigned long int *) char_ptr;
    magic_bits = 0x7efefeffL;
    charmask = c | (c << 8);
    charmask |= charmask << 16;
    charmask |= (charmask << 16) << 16;
    for (;;) {
        longword = *longword_ptr++;
        if ((((longword + magic_bits) ^ ~longword) & ~magic_bits) != 0 ||
            ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask)) & ~magic_bits) != 0) {
            const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);
            if (*cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (sizeof(longword) > 4) {
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
            }
        }
    }
    return NULL;
}

char *
strchr(const char *s, int c) {
    return __strchr_ppc(s, c);
}
