/*
 * $Id: string_strxfrm.c,v 1.5 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

size_t
strxfrm(char *dest, const char *src, size_t len) {
    size_t result = 0;
    struct _clib4 *__clib4 = __CLIB4;
    DECLARE_LOCALEBASE_R(__clib4);

    ENTER();

    SHOWPOINTER(dest);
    SHOWSTRING(src);
    SHOWVALUE(len);

    assert(src != NULL);

    if (src == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    __locale_lock(__clib4);

    if (__clib4->__locale_table[LC_COLLATE] != NULL) {
        assert(LocaleBase != NULL);

        result = StrConvert(__clib4->__locale_table[LC_COLLATE], (STRPTR) src, dest, len, SC_COLLATE1);
    } else {
        if (len > 0 && dest != NULL) {
            char c;

            result = 0;

            while ((c = (*src++)) != '\0') {
                result++;

                if (len > 0) {
                    len--;

                    (*dest++) = c;
                }
            }

            (*dest) = '\0';
        } else {
            result = strlen(src);
        }
    }

    __locale_unlock(__clib4);

out:

    RETURN(result);
    return (result);
}
