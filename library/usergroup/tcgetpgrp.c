/*
 * $Id: usergroup_tcgetpgrp.c,v 1.0 2022-02-16 18:17:00 clib4devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

/* On AmigaOS4 we don't have terminal foreground process group
 * So return getpgrp value
 */
pid_t
tcgetpgrp(int fildes) {
    pid_t result;
    (void) (fildes);

    ENTER();

    result = __getpgrp();

    __check_abort();

    RETURN(result);
    return (result);
}
