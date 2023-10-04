/*
 * $Id: string_strcpy.c,v 1.5 2022-03-29 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
strcpy(char *dest, const char *src) {
    char *result = dest;
    struct _clib4 *__clib4 = __CLIB4;

    assert(dest != NULL && src != NULL);

    if (dest == NULL || src == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    if (dest != src) {
        if (__clib4->__optimizedCPUFunctions) {
            switch (__clib4->cpufamily) {
                case CPUFAMILY_4XX:
                    result = __strcpy440(dest, src);
                    break;
#ifdef __SPE__
                case CPUFAMILY_E500:
                    result = __strcpy_e500(dest, src);
                    break;
#endif
                default: {
                    if (__clib4->hasAltivec) {
                        vec_strcpy(dest, src);
                    } else {
                        /* Fallback to standard function */
                        while (((*dest++) = (*src++)) != '\0')
                            DO_NOTHING;
                    }
                }
            }
        } else {
            /* Fallback to standard function */
            while (((*dest++) = (*src++)) != '\0')
                DO_NOTHING;
        }
    }
    out:

    return (result);
}
