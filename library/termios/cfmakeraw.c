/*
 * $Id: termios_cfmakeraw.c,v 1.2 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef	_TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

/****************************************************************************/

int
cfmakeraw(struct termios *tios)
{
	int result = ERROR;

	ENTER();

	SHOWPOINTER(tios);

	if(tios == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	tios->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	tios->c_oflag &= ~OPOST;
	tios->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	tios->c_cflag &= ~(CSIZE|PARENB);
	tios->c_cflag |= CS8;

	result = OK;

 out:

	return(result);
}
