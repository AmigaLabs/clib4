/*
 * $Id: socket_setlogmask.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <syslog.h>

int
setlogmask(int maskpri) {
    ENTER();

    SHOWVALUE(maskpri);
    struct _clib2 *__clib2 = __CLIB2;

    int ret = __clib2->syslog_mask;
    if (maskpri)
        __clib2->syslog_mask = maskpri;

    RETURN(ret);
    return ret;
}