/*
 * $Id: unistd_ftruncate.c,v 1.15 2006-11-16 14:39:23 obarthel Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int ftruncate(int file_descriptor, off_t length)
{
	struct ExamineData *fib = NULL;
	int result = ERROR;
	struct fd *fd = NULL;
	off_t current_file_size;
	off_t initial_position = 0;
	BOOL initial_position_valid = FALSE;

	ENTER();

	SHOWVALUE(file_descriptor);
	SHOWVALUE(length);

	assert(file_descriptor >= 0 && file_descriptor < __num_fd);
	assert(__fd[file_descriptor] != NULL);
	assert(FLAG_IS_SET(__fd[file_descriptor]->fd_Flags, FDF_IN_USE));

	if (__check_abort_enabled)
		__check_abort();

	PROFILE_OFF();

	__stdio_lock();

	fd = __get_file_descriptor(file_descriptor);
	if (fd == NULL)
	{
		__set_errno(EBADF);
		goto out;
	}

	__fd_lock(fd);

	if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_SOCKET))
	{
		__set_errno(EINVAL);
		goto out;
	}

	if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIO))
	{
		__set_errno(EBADF);
		goto out;
	}

	if (length < 0)
	{
		SHOWMSG("invalid length");

		__set_errno(EINVAL);
		goto out;
	}

	assert(FLAG_IS_SET(fd->fd_Flags, FDF_IN_USE));

	if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE))
	{
		SHOWMSG("file descriptor is not write-enabled");

		__set_errno(EINVAL);
		goto out;
	}

	/* Figure out how large the file is right now. */
	fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
	if (fib == NULL)
	{
		SHOWMSG("couldn't examine file");

		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

	current_file_size = (off_t)fib->FileSize;
	initial_position = (off_t)GetFilePosition(fd->fd_File);

	if (ChangeFileSize(fd->fd_File, length, OFFSET_BEGINNING) == CHANGE_FILE_ERROR || IoErr() != OK)
	{
		D(("could not reduce file to size %ld", length));

		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

	result = OK;

out:

	/* ftruncate() may change the size of the file, but it may
	   not change the current file position. */
	ChangeFilePosition(fd->fd_File, initial_position, OFFSET_BEGINNING);

	if (fib != NULL)
		FreeDosObject(DOS_EXAMINEDATA, fib);
		
	__fd_unlock(fd);

	__stdio_unlock();

	PROFILE_ON();

	RETURN(result);
	return (result);
}
