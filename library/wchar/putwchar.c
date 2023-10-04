/*
 * $Id: wchar_putwchar.c,v 1.4 2021-02-04 19:42:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wint_t
putwchar(wchar_t c) {
    return fputwc(c, stdout);
}
