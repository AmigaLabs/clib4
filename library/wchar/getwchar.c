/*
 * $Id: wchar_getwchar.c,v 1.1 2022-03-05 19:36:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wint_t
getwchar(void) {
    return fgetwc(stdin);
}
