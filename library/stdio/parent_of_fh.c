/*
 * $Id: stdio_parent_of_fh.c,v 1.2 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

BPTR
__safe_parent_of_file_handle(BPTR file_handle) {
    BPTR result = ParentOfFH(file_handle);

    return (result);
}
