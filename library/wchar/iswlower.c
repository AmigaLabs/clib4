/*
 * $Id: wctype_iswlower.c,v 1.4 2021-02-04 00:02:27 clib2devs Exp $
*/

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

int
iswlower(wint_t c) {
    return (towupper(c) != c);
}
