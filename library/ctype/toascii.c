/*
 * $Id: ctype_toascii.c,v 1.0 2021-02-10 23:51:22 clib4devs Exp $
*/

#undef __STRICT_ANSI__

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

int
toascii(int c) {

    ENTER();
    SHOWVALUE(c);

    int result = (c) & 0177;

    RETURN(result);
    return result;
}
