/*
 * $Id: unistd_fchdir.c,v 1.0 2021-02-14 18:31:00 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
fchdir(int file_descriptor)
{
	int result = ERROR;
    (void) (file_descriptor);

	ENTER();

	if (__check_abort_enabled)
		__check_abort();

    __set_errno(ENOSYS);

	RETURN(result);
	return (result);
}
