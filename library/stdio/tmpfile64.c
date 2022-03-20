/*
 * $Id: stdio_tmpfile64.c,v 1.0 2021-02-05 18:00:41 clib2devs Exp $
*/

#define _GNU_SOURCE

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */


FILE *
tmpfile64(void)
{
    return tmpfile();
}
