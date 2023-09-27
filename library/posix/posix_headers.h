/*
 * $Id: posix_headers.h,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _POSIX_HEADERS_H
#define _POSIX_HEADERS_H

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/statvfs.h>

extern void __convert_info_to_statvfs(struct InfoData * id,struct statvfs * f);

#endif /* _MOUNT_HEADERS_H */
