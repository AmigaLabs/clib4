/*
 * $Id: nltypes_catopen.c,v 1.0 2023-04-22 07:38:10 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <nl_types.h>

/* TODO - Not implemented */
nl_catd
catopen(const char *name, int flag) {
    __set_errno(ENOSYS);
    return (nl_catd) -1;
}