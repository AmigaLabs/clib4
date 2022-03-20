/*
 * $Id: fcntl_lseek.c,v 1.11 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

off_t lseek(int file_descriptor, off_t offset, int mode)
{
	struct file_action_message fam;
	off_t result = CHANGE_FILE_ERROR;
	struct fd *fd = NULL;
	off_t position;

	ENTER();

	SHOWVALUE(file_descriptor);
	SHOWVALUE(offset);
	SHOWVALUE(mode);

	assert(file_descriptor >= 0 && file_descriptor < __num_fd);
	assert(__fd[file_descriptor] != NULL);
	assert(FLAG_IS_SET(__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

	if (__check_abort_enabled)
		__check_abort();

	__stdio_lock();

	fd = __get_file_descriptor(file_descriptor);
	if (fd == NULL)
	{
		__set_errno(EBADF);
		goto out;
	}

	__fd_lock(fd);

	if (mode < SEEK_SET || mode > SEEK_END)
	{
		SHOWMSG("seek mode is invalid");

		__set_errno(EINVAL);
		goto out;
	}

	fam.fam_Action = file_action_seek;
	fam.fam_Offset = offset;
	fam.fam_Mode = mode;

	assert(fd->fd_Action != NULL);

	/* Note that a return value of 0 (= CHANGE_FILE_ERROR) may be a
	   valid file position in files larger than 2 GBytes. Just
	   to be sure, we therefore also check the secondary error
	   to verify that what could be a file position is really
	   an error indication. */
	position = (*fd->fd_Action)(fd, &fam);
	if (position == CHANGE_FILE_ERROR && fam.fam_Error != OK)
	{
		__set_errno(fam.fam_Error);
		goto out;
	}

	/* If this is a valid file position, clear 'errno' so that
	   it cannot be mistaken for an error. */
	if (position < 0)
		__set_errno(OK);

	result = position;

out:

	__fd_unlock(fd);

	__stdio_unlock();

	RETURN(result);
	return (result);
}
