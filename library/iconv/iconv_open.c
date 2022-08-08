/*
 * $Id: iconv_open.c,v 1.0 2021-03-09 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "iconv_priv.h"

iconv_t
iconv_open(const char *to, const char *from) {
    size_t f, t;
    struct stateful_cd *scd;

    if ((t = find_charmap(to)) == -1
        || (f = find_charmap(from)) == -1
        || (charmaps[t] >= 0330)) {
        errno = EINVAL;
        return (iconv_t) - 1;
    }
    iconv_t cd = combine_to_from(t, f);

    switch (charmaps[f]) {
        case UTF_16:
        case UTF_32:
        case UCS2:
        case ISO2022_JP:
            scd = malloc(sizeof *scd);
            if (!scd)
                return (iconv_t) - 1;
            scd->base_cd = cd;
            scd->state = 0;
            cd = (iconv_t) scd;
    }

    return cd;
}
