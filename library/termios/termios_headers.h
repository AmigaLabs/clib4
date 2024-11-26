/*
 * $Id: termios_headers.h,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef	_TERMIOS_HEADERS_H
#define	_TERMIOS_HEADERS_H

/****************************************************************************/

#ifndef	_STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

#ifndef	_TERMIOS_H
#include <termios.h>
#endif /* _TERMIOS_H */

/****************************************************************************/

extern int64_t __termios_console_hook(struct _clib4 *__clib4, struct fd *,struct file_action_message *);
extern int64_t __termios_serial_hook(struct _clib4 *__clib4, struct fd *,struct file_action_message *);
extern struct termios *__get_termios(struct fd *);
extern int openserial(const char *device_name,int unit,int mode);

/****************************************************************************/

enum	/* Termios types. */
{
	TIOST_INVALID = 0,
	TIOST_CONSOLE,
	TIOST_SERIAL
};

enum
{
	TIOS_FLUSH_OUTPUT,
	TIOS_FLUSH_SYNC,
	TIOS_FLUSH_INPUT,
	TIOS_FLUSH_IO
};

/****************************************************************************/

#endif /* _TERMIOS_HEADERS_H */
