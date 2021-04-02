/*
 * $Id: unistd_fchown.c,v 1.16 2021-01-31 14:39:23 apalmate Exp $
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

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard. */

/****************************************************************************/

int fchown(int file_descriptor, uid_t owner, gid_t group)
{
	struct ExamineData *fib;
	BPTR parent_dir = ZERO;
	BPTR old_current_dir = ZERO;
	BOOL current_dir_changed = FALSE;
	int result = ERROR;
	struct fd *fd = NULL;
	LONG success;

	ENTER();

	SHOWVALUE(file_descriptor);
	SHOWVALUE(owner);
	SHOWVALUE(group);

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

	PROFILE_OFF();
	fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
	success = (fib != NULL && (parent_dir = __safe_parent_of_file_handle(fd->fd_File)) != ZERO);
	PROFILE_ON();

	if (NO success)
	{
		SHOWMSG("couldn't find parent directory");

		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

	old_current_dir = CurrentDir(parent_dir);
	current_dir_changed = TRUE;

	/* A value of -1 for either the owner or the group ID means
	   that what's currently being used should not be changed. */
	if (owner == (uid_t)-1)
		owner = fib->OwnerUID;

	if (group == (gid_t)-1)
		group = fib->OwnerGID;

	/* Check if the owner and group IDs are usable. This test
	   follows the comparison against -1 above just so that we
	   can be sure that we are not mistaking a -1 for a
	   large unsigned number. */
	if (owner > 65535 || group > 65535)
	{
		SHOWMSG("owner or group not OK");

		SHOWVALUE(owner);
		SHOWVALUE(group);

		__set_errno(EINVAL);
		goto out;
	}

	/* Did anything change at all? */
	if (group != fib->OwnerUID || owner != fib->OwnerUID)
	{
		PROFILE_OFF();

		success = SetOwnerInfoTags(OI_StringNameInput, fib->Name, OI_OwnerUID, (LONG)((((ULONG)owner) << 16) | (ULONG)group), TAG_DONE);

		PROFILE_ON();

		if (NO success)
		{
			SHOWMSG("couldn't change owner/group");

			__set_errno(__translate_io_error_to_errno(IoErr()));
			goto out;
		}
	}

	result = OK;

out:

	__fd_unlock(fd);

	PROFILE_OFF();

	UnLock(parent_dir);

	if (current_dir_changed)
		CurrentDir(old_current_dir);

	PROFILE_ON();

	__stdio_unlock();

	RETURN(result);
	return (result);
}