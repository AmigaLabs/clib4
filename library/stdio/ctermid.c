/*
 * $Id: stdio_ctermid.c,v 1.2 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
ctermid(char *buf) {
    static char cname[] = "CONSOLE:";

    ENTER();
    SHOWSTRING(buf);

    if (buf != NULL)
        strcpy(buf, cname);
    else
        buf = cname;

    RETURN(buf);
    return (buf);
}
