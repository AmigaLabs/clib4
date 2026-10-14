/*
* $Id: stdlib_getexecfullname.c,v 1.0 2026-10-14 13:28:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

const char *
getexecfullname(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    char current_full_name[MAXPATHLEN] = {0};
    snprintf(current_full_name, sizeof(current_full_name), "%s/%s", __clib4->__current_path_name, __clib4->__progname);

    RETURN((const char *) current_full_name);
    return strdup((const char *) current_full_name);
}
