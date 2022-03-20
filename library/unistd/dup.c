/*
 * $Id: unistd_dup.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
dup(int file_descriptor)
{
	return(dup2(file_descriptor,-1));
}
