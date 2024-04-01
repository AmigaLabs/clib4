/*
 * $Id: string_strlen.c,v 1.4 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

size_t
strlen(const char *str) {
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
#if  __GNUC__ >= 3
    s += (uint32)__builtin_clz(~(((x & magic2) + magic2) | x | magic2))/8;
#else
    if (x & 0xFF000000) {
        s++;
        if (x & 0x00FF0000) {
            s++;
            if (x & 0x0000FF00) {
                s++;
            }
        }
    }
#endif

    return s - str;
}
