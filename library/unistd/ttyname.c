/*
 * $Id: unistd_ttyname.c,v 1.5 2006-09-25 14:05:31 clib2devs Exp $
*/

#ifndef	_UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

/*
 * Just a quick kludge.
 */

char *
ttyname(int file_descriptor)
{
	static char tty_file_name[_POSIX_TTY_NAME_MAX];
	char * result = NULL;

	ENTER();

	if(ttyname_r(file_descriptor,tty_file_name,sizeof(tty_file_name)) != 0)
		goto out;

	result = tty_file_name;

 out:

	RETURN(result);
	return(result);
}
