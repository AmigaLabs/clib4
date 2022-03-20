/*
 * $Id: termios_cfsetispeed.c,v 1.2 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef	_TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

/****************************************************************************/

int
cfsetispeed(struct termios *tios,speed_t ispeed)
{
	int result = ERROR;

	ENTER();

	SHOWPOINTER(tios);
	SHOWVALUE(ispeed);

	if(tios == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	switch(ispeed)
	{
		case B0:
		case B50:
		case B75:
		case B110:
		case B134:
		case B150:
		case B200:
		case B300:
		case B600:
		case B1200:
		case B1800:
		case B2400:
		case B4800:
		case B9600:
		case B19200:
		case B31250:
		case B38400:
		case B57600:
		case B115200:
		case B230400:
		case B460800:
		case B576000:
		case B1152000:

			tios->c_ispeed = ispeed;
			break;

		default:

			__set_errno(EINVAL);
			goto out;
	}

	result = OK;

 out:

	RETURN(result);
	return(result);
}
