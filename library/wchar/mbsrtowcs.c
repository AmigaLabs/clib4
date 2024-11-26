/*
 * $Id: wchar_mbsrtowcs.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
mbsrtowcs(wchar_t *ws, const char **src, size_t wn, mbstate_t *st) {
    const unsigned char *s = (const void *) *src;
    size_t wn0 = wn;
    unsigned c = 0;

    ENTER();

    if (st && (c = *(unsigned *) st)) {
        if (ws) {
            *(unsigned *) st = 0;
            goto resume;
        } else {
            goto resume0;
        }
    }

    if (!ws) {
        for (;;) {
            if (((*s - 1U) < 0x7f) && (((uintptr_t) s % 2) == 0)) {
                unsigned short w;
                while (
                        (memcpy(&w, s, sizeof(w))) &&
                        !((w | (w - 0x01010101U)) & (0x80808080U))) {
                    s += 2;
                    wn -= 2;
                }
            }
            if (*s - 1u < 0x7f) {
                s++;
                wn--;
                continue;
            }
            if (*s - __SA > __SB - __SA)
                break;
            c = bittab[*s++ - __SA];
            resume0:
            if (__OOB(c, *s)) {
                s--;
                break;
            }
            s++;
            if (c & (1U << 25)) {
                if (*s - 0x80u >= 0x40) {
                    s -= 2;
                    break;
                }
                s++;
                if (c & (1U << 19)) {
                    if (*s - 0x80u >= 0x40) {
                        s -= 3;
                        break;
                    }
                    s++;
                }
            }
            wn--;
            c = 0;
        }
    } else {
        for (;;) {
            if (!wn) {
                RETURN(wn0);
                return wn0;
            }
            if (((*s - 1U) < 0x7f) && (((uintptr_t) s % 2) == 0)) {
                unsigned short w;
                while (
                        (memcpy(&w, s, sizeof(w))) &&
                        (wn >= 2) &&
                        !((w | (w - 0x01010101U)) & (0x80808080U))) {
                    *ws++ = *s++;
                    *ws++ = *s++;
                    wn -= 2;
                }
            }
            if (*s - 1U < 0x7f) {
                *ws++ = *s++;
                wn--;
                continue;
            }
            if (*s - __SA > __SB - __SA)
                break;
            c = bittab[*s++ - __SA];
            resume:
            if (__OOB(c, *s)) {
                s--;
                break;
            }
            c = ((c << 6) | (*s++ - 0x80U));
            if (c & (1U << 31)) {
                if (*s - 0x80U >= 0x40) {
                    s -= 2;
                    break;
                }
                c = ((c << 6) | (*s++ - 0x80U));
                if (c & (1U << 31)) {
                    if (*s - 0x80U >= 0x40) {
                        s -= 3;
                        break;
                    }
                    c = ((c << 6) | (*s++ - 0x80U));
                }
            }
            *ws++ = (wchar_t) c;
            wn--;
            c = 0;
        }
    }

    if (!c && !*s) {
        if (ws) {
            *ws = 0;
            *src = 0;
        }
        RETURN(wn0 - wn);
        return (wn0 - wn);
    }
    __set_errno(EILSEQ);
    if (ws)
        *src = (const void *) s;

    RETURN(-1);
    return (size_t) - 1;
}
