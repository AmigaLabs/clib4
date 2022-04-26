/*
 * $Id: ctype_isblank.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

#undef isblank

int
isblank(int c) {
    int result;

    ENTER();
    SHOWVALUE(c);

    result = (c == '\t' || c == ' ');

    RETURN(result);
    return (result);
}
