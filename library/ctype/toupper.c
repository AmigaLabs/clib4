/*
 * $Id: ctype_toupper.c,v 1.5 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

int
toupper(int c) {
    DECLARE_LOCALEBASE();
    int result;

    ENTER();
    SHOWVALUE(c);

    __locale_lock();

    if (__locale_table[LC_CTYPE] != NULL) {
        assert(LocaleBase != NULL);

        /* The parameter must be either EOF or in the range of an
           'unsigned char'. If it's not, then the behaviour is
           undefined. */
        if (c != EOF && ((0 <= c && c <= UCHAR_MAX) || ((c + 256) <= UCHAR_MAX)))
            result = ConvToUpper(__locale_table[LC_CTYPE], (ULONG)(c & 255));
        else
            result = c;
    } else {
        result = ('a' <= c && c <= 'z') ? (c - ('a' - 'A')) : c;
    }

    __locale_unlock();

    RETURN(result);
    return (result);
}
