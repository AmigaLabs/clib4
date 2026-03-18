/*
 * $Id: stdlib_mkstemp.c,v 1.6 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <fcntl.h>

int
mkstemp(char *name_template) {
    ENTER();

    SHOWSTRING(name_template);

    /* Delegate to mkostemps which uses O_EXCL for atomic creation */
    int result = mkostemps(name_template, 0, 0);

    RETURN(result);
    return (result);
}
