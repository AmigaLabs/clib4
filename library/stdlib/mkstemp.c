/*
 * $Id: stdlib_mkstemp.c,v 1.6 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <fcntl.h>

int
mkstemp(char *name_template) {
    int result = ERROR;

    ENTER();

    SHOWSTRING(name_template);

    assert(name_template != NULL);

    if (name_template == NULL) {
        SHOWMSG("invalid name template");

        __set_errno(EFAULT);
        goto out;
    }

    if (mktemp(name_template) == NULL) {
        SHOWMSG("could not pick temp name");
        goto out;
    }

    SHOWSTRING(name_template);

    result = open(name_template, O_RDWR | O_CREAT | O_TRUNC, 0600);

out:

    RETURN(result);
    return (result);
}
