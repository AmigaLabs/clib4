/*
 * $Id: termios_openserial.c,v 1.2 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef	_TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

/****************************************************************************/

/*
 * This is a clib2/Amiga specific function to obtain a file descriptor
 * for a serial port. This is intended as an easy porting path for Unix
 * code (which might e.g. open "/dev/ttyS0") and the same concept might
 * be applied to other device types as well.
 *
 * TODO: Implement it. And a serial_fd_hook. And, well, everything else too....
 */

int
openserial(const char *device_name,int unit,int mode)
{
    (void) (device_name);
    (void) (unit);
    (void) (mode);

	__set_errno(EIO);

	return(ERROR);
}
