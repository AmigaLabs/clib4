/*
 * $Id: stdio_get_fd.c,v 1.1 2006-11-15 09:17:04 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

struct _fd *
__get_fd(int file_descriptor) {
    struct _fd *result;

    ENTER();
    SHOWVALUE(file_descriptor);

    result = (struct _fd *) __get_file_descriptor(file_descriptor);

    LEAVE();
    return (result);
}
