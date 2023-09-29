/*
 * $Id: usergroup_tcgetpgrp.c,v 1.0 2022-02-16 18:17:00 clib4devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/* On AmigaOS4 we don't have terminal foreground process group
 * So use setgid
 */

int tcsetpgrp(int filedes, pid_t pgrp) {
    (void) (filedes);

    ENTER();

    int result = setgid((gid_t) pgrp);

    RETURN(result);
    return (result);
}