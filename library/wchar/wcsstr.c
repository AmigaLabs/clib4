/*
 * $Id: wchar_wcsstr.c,v 1.0 2021-02-04 20:31:00 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wcsstr(const wchar_t *big, const wchar_t *little) {
#if 1
    unsigned int i;
    wchar_t *p = (wchar_t *) big;

    ENTER();

    if (!p)
        return NULL;

    // Always find the empty string
    if ((!little) || (!little[0]))
        return (wchar_t *) big;

    size_t flen = wcslen(little);

    for (i = 0; i < flen; i++) {
        if ((p = wcschr(p, little[i])) == NULL) {
            LEAVE();
            return NULL;
        }

        if (!wmemcmp(p, little, flen)) {
            LEAVE();
            return p;
        }

        big = p + 1;
    }

    LEAVE();
    return NULL;
#else
    const wchar_t *p;
    const wchar_t *q;
    const wchar_t *r;

    ENTER();

    if (!*little)
    {
        /* LINTED interface specification */
        return (wchar_t *)big;
    }
    if (wcslen(big) < wcslen(little))
        return NULL;

    p = big;
    q = little;
    while (*p)
    {
        q = little;
        r = p;
        while (*q)
        {
            if (*r != *q)
                break;
            q++;
            r++;
        }
        if (!*q)
        {
            /* LINTED interface specification */
            return (wchar_t *)p;
        }
        p++;
    }
    return NULL;
#endif
}
