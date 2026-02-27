/*
 * $Id: string_strcoll.c,v 1.5 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

int
strcoll(const char *s1, const char *s2) {
    int result = 0;
    struct _clib4 *__clib4 = __CLIB4;
    DECLARE_LOCALEBASE_R(__clib4);

    assert(s1 != NULL && s2 != NULL);

    if (s1 == NULL || s2 == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    __locale_lock(__clib4);

    if (__clib4->__locale_table[LC_COLLATE] != NULL) {
        assert(LocaleBase != NULL);

        result = StrnCmp(__clib4->__locale_table[LC_COLLATE], (STRPTR) s1, (STRPTR) s2, -1, SC_COLLATE1);
    } else {
        result = strcmp(s1, s2);
    }

    __locale_unlock(__clib4);

out:

    return (result);
}
