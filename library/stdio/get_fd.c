/*
 * $Id: stdio_get_fd.c,v 1.2 2023-07-04 09:17:04 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

struct _fd *
__get_fd(struct _clib4 *__clib4, int file_descriptor) {
    struct _fd *result;

    ENTER();
    SHOWVALUE(file_descriptor);

    result = (struct _fd *) __get_file_descriptor(__clib4, file_descriptor);

    LEAVE();
    return (result);
}
