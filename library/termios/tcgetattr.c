/*
 * $Id: termios_tcgetattr.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef	_TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

/****************************************************************************/

static const cc_t def_console_cc[NCCS]=
{
	3,		/* VINTR	= ETX */
	28,		/* VQUIT	= FS  */
	127,	/* VERASE 	= DEL */
	24,		/* VKILL	= DC4 */
	4,		/* VEOF		= EOT */
	1,		/* VMIN		= Minimum number of characters in a non-canonical read. */
	0,		/* VEOL		= NUL */
	0,		/* VTIME	= Timout in deciseconds for a non-canonical read. */
	17,		/* VSTART	= DC1 */
	19,		/* VSTOP	= DC3 */
	26,		/* VSUSP	= SUB */
	23		/* VWERASE	= ETB */
};

/******************************************************************/

static struct termios *
get_console_termios(struct fd *fd)
{
	struct termios *tios;

	/* Create a new, fresh termios. TODO: Actually query some values,
	   e.g. ICANON and ECHO should not be set for RAW consoles. */
	tios = malloc(sizeof(*tios));
	if(tios == NULL)
	{
		__set_errno(ENOMEM);
		goto out;
	}

	memset(tios,0,sizeof(*tios));

	/* Set up the initial termios state. */
	tios->c_iflag = 0;
	tios->c_oflag = 0;
	tios->c_cflag = CS8|CLOCAL;

	if(FLAG_IS_SET(fd->fd_Flags,FDF_READ))
		SET_FLAG(tios->c_cflag,CREAD);

	tios->c_lflag = ISIG|ICANON|ECHO;

	memcpy(tios->c_cc,def_console_cc,NCCS);

	tios->c_ispeed = B9600;	/* A default as good as any? */
	tios->c_ospeed = B9600;

	tios->type = TIOST_CONSOLE;
	tios->flags = 0;

	/* Change the hook. */
	fd->fd_Action = __termios_console_hook;
	fd->fd_Aux = tios;

	SET_FLAG(fd->fd_Flags,FDF_TERMIOS);

 out:

	return(tios);
}

/******************************************************************/

struct termios *
__get_termios(struct fd *fd)
{
	struct termios *tios = NULL;

	if(FLAG_IS_SET(fd->fd_Flags,FDF_IS_SOCKET)) /* Network socket. Remote terminal? */
	{
		__set_errno(ENODEV);
		goto out;
	}

	if(FLAG_IS_CLEAR(fd->fd_Flags,FDF_IS_INTERACTIVE))
	{
		__set_errno(ENOTTY);
		goto out;
	}

	if(FLAG_IS_SET(fd->fd_Flags,FDF_TERMIOS))
		tios = fd->fd_Aux;
	else
		tios = get_console_termios(fd);

 out:

	return(tios);
}

/******************************************************************/

int
tcgetattr(int file_descriptor,struct termios *user_tios)
{
	int result = ERROR;
	struct fd *fd = NULL;
	struct termios *tios;

	__stdio_lock();

	if(user_tios == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	fd = __get_file_descriptor(file_descriptor);
	if(fd == NULL)
	{
		__set_errno(EBADF);
		goto out;
	}

	__fd_lock(fd);

	if(FLAG_IS_SET(fd->fd_Flags,FDF_TERMIOS))
	{
		assert(fd->fd_Aux != NULL);

		memcpy(user_tios,fd->fd_Aux,sizeof(struct termios));
	}
	else
	{
		tios = __get_termios(fd);
		if(tios == NULL)
			goto out;

		memcpy(user_tios,tios,sizeof(struct termios));
	}

	result = OK;

 out:

	__fd_unlock(fd);

	__stdio_unlock();

	return(result);
}
