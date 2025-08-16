/*
 * $Id: iconv_priv.c,v 1.0 2022-08-08 12:04:25 clib4devs Exp $
*/

#include "iconv_priv.h"

int fuzzycmp(const unsigned char *a, const unsigned char *b) {
    for (; *a && *b; a++, b++) {
        while (*a && (*a | 32U) - 'a' > 26 && *a - '0' > 10) a++;
        if ((*a | 32U) != *b) return 1;
    }
    return *a != *b;
}

size_t find_charmap(const void *name) {
    const unsigned char *s;
    if (!*(char *) name) name = charmaps; /* "utf8" */
    for (s = charmaps; *s;) {
        if (!fuzzycmp(name, s)) {
            for (; *s; s += strlen((void *) s) + 1);
            return s + 1 - charmaps;
        }
        s += strlen((void *) s) + 1;
        if (!*s) {
            if (s[1] > 0200) s += 2;
            else s += 2 + (64U - s[1]) * 5;
        }
    }
    return -1;
}

iconv_t combine_to_from(size_t t, size_t f) {
    return (void *) (f << 16 | t << 1 | 1);
}

size_t extract_from(iconv_t cd) {
    return (size_t) cd >> 16;
}

size_t extract_to(iconv_t cd) {
    return (size_t) cd >> 1 & 0x7fff;
}