/*
 * $Id: errno_data.c,v 1.4 2023-04-27 12:04:22 clib2devs Exp $
*/

#ifndef _ERRNO_H
#include <errno.h>
#endif /* _ERRNO_H */

#include <dos.h>

int *
__errno () {
    return &__CLIB2->_errno;
}