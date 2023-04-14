/*
 * $Id: unistd_getpid.c,v 1.5 2021-02-07 19:24:11 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

pid_t
getpid(void)
{
	ENTER();

	pid_t result = (pid_t) GetPID(NULL, GPID_PROCESS);

	RETURN(result);
	return (result);
}
