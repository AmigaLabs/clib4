/*
 * $Id: unistd_getlogin.c,v 1.0 2022-03-22 09:51:53 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char *getlogin(void) {
    return getenv("LOGNAME");
}