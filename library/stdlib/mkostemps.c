/*
 * $Id: stdlib_mkostemps.c,v 1.0 2023-07-13 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

#include <fcntl.h>

int
mkostemps(char *template, int len, int flags) {
    size_t l = strlen(template);
    struct name_translation_info name_template_nti;
    struct _clib4 *__clib4 = __CLIB4;

    if (l < 6 || (size_t) len > l - 6 || memcmp(template + l - len - 6, "XXXXXX", 6)) {
        __set_errno(EINVAL);
        return -1;
    }

    flags -= flags & O_ACCMODE;
    int fd, retries = 100;
    do {
        __randname(template + l - len - 6);
        if (__clib4->__unix_path_semantics) {
            if (__translate_unix_to_amiga_path_name((char const **) &template, &name_template_nti) != 0)
                return -1;
        }

        if ((fd = open(template, flags | O_RDWR | O_CREAT | O_EXCL, 0600)) >= 0)
            return fd;
    } while (--retries && errno == EEXIST);

    memcpy(template + l - len - 6, "XXXXXX", 6);
    return -1;
}