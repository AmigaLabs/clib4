/*
 * $Id: socket_setlogmask.c,v 1.0 2022-01-22 18:27:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <syslog.h>

int syslog_mask = 0xff;

int
setlogmask(int maskpri) {
    ENTER();

    SHOWVALUE(maskpri);

    int ret = syslog_mask;
    if (maskpri)
        syslog_mask = maskpri;

    RETURN(ret);
    return ret;
}