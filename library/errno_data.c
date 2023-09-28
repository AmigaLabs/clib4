/*
 * $Id: errno_data.c,v 1.4 2023-04-27 12:04:22 clib4devs Exp $
*/

#ifndef _ERRNO_H
#include <errno.h>
#endif /* _ERRNO_H */

#include <dos.h>

int *
__errno () {
    return &__CLIB4->_errno;
}

int *
__h_errno () {
    return &__CLIB4->_h_errno;
}