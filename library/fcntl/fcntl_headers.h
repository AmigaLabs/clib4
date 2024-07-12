/*
 * $Id: fcntl_headers.h,v 1.5 2006-01-08 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#define _FCNTL_HEADERS_H

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/file.h>

extern int getCurrentCliActionMode(void);
extern struct Window *getCurrentCliWindow(void);

extern ssize_t __write_r(struct _clib4 *__clib4, int file_descriptor, const void *buffer, size_t num_bytes);

#endif /* _FCNTL_HEADERS_H */
