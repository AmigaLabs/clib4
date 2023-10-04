/*
 * $Id: wctype_iswalnum.c,v 1.4 2021-02-04 00:16:27 clib4devs Exp $
*/

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

int
iswalnum(wint_t c) {
    return iswdigit(c) || iswalpha(c);
}
