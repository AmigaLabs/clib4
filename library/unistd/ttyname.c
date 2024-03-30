/*
 * $Id: unistd_ttyname.c,v 1.5 2006-09-25 14:05:31 clib4devs Exp $
*/

#ifndef    _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char *
ttyname(int file_descriptor) {
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (ttyname_r(file_descriptor, __clib4->tty_file_name, sizeof(__clib4->tty_file_name)) != 0)
        goto out;

out:

    RETURN(__clib4->tty_file_name);
    return (__clib4->tty_file_name);
}
