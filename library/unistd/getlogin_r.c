/*
 * $Id: unistd_getlogin_r.c,v 1.0 2022-03-22 09:51:53 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int getlogin_r(char *name, size_t size) {
    char *logname = getlogin();
    if (!logname) return ENXIO;
    if (strlen(logname) >= size) return ERANGE;
    strcpy(name, logname);
    return 0;
}