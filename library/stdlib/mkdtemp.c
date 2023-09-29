/*
 * $Id: stdlib_mkdtemp.c,v 1.5 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/stat.h>

char *
mkdtemp(char *name_template) {
    char *result = NULL;

    ENTER();

    SHOWSTRING(name_template);

    assert(name_template != NULL);

    if (name_template == NULL) {
        SHOWMSG("invalid name template");

        __set_errno(EFAULT);
        goto out;
    }

    if (mktemp(name_template) == NULL) {
        SHOWMSG("could not pick a temp name");
        goto out;
    }

    if (mkdir(name_template, 0700) == -1) {
        SHOWMSG("could not create directory");
        goto out;
    }

    result = name_template;

out:

    RETURN(result);
    return (result);
}
