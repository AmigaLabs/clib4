/*
 * $Id: string_strlen.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

inline static size_t
__strlen(const char *str) {
    uint32 *s32, x, magic1, magic2;
    char *s = (char *) str;

    magic1 = 0xFEFEFEFF;
    magic2 = 0x7F7F7F7F;

    s32 = (uint32 * )((uint32) s & ~3);

    x = (s32 == (uint32 *) s)
        ? *s32
        : *s32 | (0xFFFFFFFF << (3 - ((uint32) s & 3) + 1) * 8);

    while (!((x + magic1) & ~(x | magic2))) {
        x = *++s32;
    }

    s = (char *) s32;
    if (x & 0xFF000000) {
        s++;
        if (x & 0x00FF0000) {
            s++;
            if (x & 0x0000FF00) {
                s++;
            }
        }
    }

    return s - str;
}

size_t
strlen(const char *s) {
    size_t result = 0;

    assert(s != NULL);

    if (NULL == s) {
        __set_errno(EFAULT);
        goto out;
    }

    if (__getclib2() != NULL && __getclib2()->optimizedCPUFunctions) {
        switch (__getclib2()->cpufamily) {
            case CPUFAMILY_4XX:
                result = __strlen440(s);
                break;
#ifdef SPE
            case CPUFAMILY_E500:
                result = __strlen_e500(s);
                break;
#endif
            default:
                result = __strlen(s);
        }
    } else {
        /* Fallback to standard function */
        result = __strlen(s);
    }
out:

    return (result);
}
