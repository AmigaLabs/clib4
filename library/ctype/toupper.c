/*
* $Id: ctype_toupper.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

// TODO - We have to change this also on limits.h but at moment will break the compatibility
#undef MB_LEN_MAX
#define MB_LEN_MAX 8

#undef toupper

int
toupper(int c) {
	if ((unsigned char) c <= 0x7f)
		return islower(c) ? c - 'a' + 'A' : c;
	if (c != EOF && MB_CUR_MAX == 1 && islower(c)) {
		char s[MB_LEN_MAX] = {c, '\0'};
		wchar_t wc;
		if (mbtowc(&wc, s, 1) >= 0 && wctomb(s, (wchar_t) towupper(wc)) == 1)
			c = (unsigned char) s[0];
	}
	return c;
}
