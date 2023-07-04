/*
 * $Id: ctype_isdigit.c,v 1.5 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

#undef isdigit

int
isdigit(int c) {
    DECLARE_LOCALEBASE();
    int result;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();
    SHOWVALUE(c);

    __locale_lock(__clib2);

    if (__clib2->__locale_table[LC_CTYPE] != NULL) {
        assert(LocaleBase != NULL);

        /* The parameter must be either EOF or in the range of an
           'unsigned char'. If it's not, then the behaviour is
           undefined. */
        if (c != EOF && ((0 <= c && c <= UCHAR_MAX) || ((c + 256) <= UCHAR_MAX)))
            result = IsDigit(__clib2->__locale_table[LC_CTYPE], (ULONG)(c & 255));
        else
            result = FALSE;
    } else {
        result = ('0' <= c && c <= '9');
    }

    __locale_unlock(__clib2);

    RETURN(result);
    return (result);
}
