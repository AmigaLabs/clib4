/*
 * $Id: unistd_getlogin.c,v 1.1 2023-04-13 09:51:53 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

char *getlogin(void) {
    static char username[LOGIN_NAME_MAX];
    return getlogin_r(username, sizeof(username)) == 0 ? username : NULL;
}