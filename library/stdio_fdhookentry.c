/*
 * $Id: stdio_fdhookentry.c,v 1.36 2021-01-31 17:12:23 apalmate Exp $
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

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/****************************************************************************/

#include <strings.h>
#include <limits.h>

/****************************************************************************/

int64_t __fd_hook_entry(
	struct fd *fd,
	struct file_action_message *fam)
{
	struct ExamineData *exd = NULL;
	BOOL fib_is_valid = FALSE;
	struct FileHandle *fh;
	int64_t current_position;
	int64_t new_position;
	int new_mode;
	char *buffer = NULL;
	int64_t result = EOF;
	BOOL is_aliased;
	BPTR file;

	ENTER();

	assert(fam != NULL && fd != NULL);
	assert(__is_valid_fd(fd));

	/* Careful: file_action_close has to monkey with the file descriptor
	            table and therefore needs to obtain the stdio lock before
	            it locks this particular descriptor entry. */
	if (fam->fam_Action == file_action_close)
		__stdio_lock();

	__fd_lock(fd);

	file = __resolve_fd_file(fd);
	if (file == ZERO)
	{
		SHOWMSG("file is closed");

		fam->fam_Error = EBADF;
		goto out;
	}

	switch (fam->fam_Action)
	{
	case file_action_read:

		SHOWMSG("file_action_read");

		assert(fam->fam_Data != NULL);
		assert(fam->fam_Size > 0);

		D(("read %ld bytes from position %ld to 0x%08lx", fam->fam_Size, Seek(file, 0, OFFSET_CURRENT), fam->fam_Data));

		PROFILE_OFF();

		result = Read(file, fam->fam_Data, fam->fam_Size);

		PROFILE_ON();

		if (result == -1)
		{
			D(("read failed ioerr=%ld", IoErr()));

			fam->fam_Error = __translate_io_error_to_errno(IoErr());
			goto out;
		}

		fd->fd_Position += (int64_t)result;

		break;

	case file_action_write:

		SHOWMSG("file_action_write");

		assert(fam->fam_Data != NULL);
		assert(fam->fam_Size > 0);

		if (FLAG_IS_SET(fd->fd_Flags, FDF_APPEND))
		{
			int64_t position;

			SHOWMSG("appending data");

			PROFILE_OFF();

			/* Make sure that if we get a value of -1 out of Seek()
				   to check whether this was an error or a numeric
				   overflow. */
			position = ChangeFilePosition(file, 0, OFFSET_END);
			if (position != 0)
				fd->fd_Position = GetFilePosition(file);

			PROFILE_ON();

			if (position == GETPOSITION_ERROR)
			{
				D(("seek to end of file failed; ioerr=%ld", IoErr()));

				fam->fam_Error = __translate_io_error_to_errno(IoErr());
				goto out;
			}
		}

		D(("write %ld bytes to position %ld from 0x%08lx", fam->fam_Size, GetFilePosition(file), fam->fam_Data));

		PROFILE_OFF();

		result = Write(file, fam->fam_Data, fam->fam_Size);

		PROFILE_ON();

		if (result == -1)
		{
			D(("write failed ioerr=%ld", IoErr()));

			fam->fam_Error = __translate_io_error_to_errno(IoErr());
			goto out;
		}

		fd->fd_Position += (int64_t)result;

		break;

	case file_action_close:

		SHOWMSG("file_action_close");

		/* The following is almost guaranteed not to fail. */
		result = OK;

		/* If this is an alias, just remove it. */
		is_aliased = __fd_is_aliased(fd);
		if (is_aliased)
		{
			__remove_fd_alias(fd);
		}
		else if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_STDIO))
		{
			/* Should we reset this file into line buffered mode? */
			if (FLAG_IS_SET(fd->fd_Flags, FDF_NON_BLOCKING) && FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE))
				SetMode(fd->fd_File, DOSFALSE);

			/* Are we allowed to close this file? */
			if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_NO_CLOSE))
			{
				BOOL name_and_path_valid = FALSE;
				struct ExamineData *fib = NULL;
				BPTR parent_dir;

				/* Call a cleanup function, such as the one which releases locked records. */
				if (fd->fd_Cleanup != NULL)
					(*fd->fd_Cleanup)(fd);

				PROFILE_OFF();

				parent_dir = __safe_parent_of_file_handle(fd->fd_File);
				if (parent_dir != ZERO)
				{
					fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
					if (fib != NULL)
						name_and_path_valid = TRUE;
				}

				if (CANNOT Close(fd->fd_File))
				{
					fam->fam_Error = __translate_io_error_to_errno(IoErr());

					result = EOF;
				}

				PROFILE_ON();

				fd->fd_File = ZERO;

#if defined(UNIX_PATH_SEMANTICS)
				{
					DECLARE_UTILITYBASE();

					assert(UtilityBase != NULL);

					/* Now that we have closed this file, know where it is and what its
						 * name would be, check if we tried to unlink it earlier. If we did,
						 * we'll try to finish the job here and now.
						 */
					if (name_and_path_valid)
					{
						struct UnlinkNode *node;
						struct UnlinkNode *uln_next;
						struct UnlinkNode *uln;
						BOOL file_deleted = FALSE;

						assert(__unlink_list.mlh_Head != NULL);

						/* Check all files to be unlinked when this program exits. */
						for (uln = (struct UnlinkNode *)__unlink_list.mlh_Head;
							 (uln_next = (struct UnlinkNode *)uln->uln_MinNode.mln_Succ) != NULL;
							 uln = uln_next)
						{
							node = NULL;

							/* If the file name matches, check if the path matches, too. */
							if (Stricmp(FilePart(uln->uln_Name), fib->Name) == SAME)
							{
								BPTR old_dir;
								BPTR node_lock;
								BPTR path_lock = ZERO;

								PROFILE_OFF();

								/* Try to get a lock on the file first, then move on to
									 * the directory it is stored in.
									 */
								old_dir = CurrentDir(uln->uln_Lock);

								node_lock = Lock(uln->uln_Name, SHARED_LOCK);
								if (node_lock != ZERO)
								{
									path_lock = ParentDir(node_lock);

									UnLock(node_lock);
								}

								CurrentDir(old_dir);

								/* If we found the file's parent directory, check if it matches
									 * the parent directory of the file we just closed.
									 */
								if (path_lock != ZERO)
								{
									if (SameLock(path_lock, parent_dir) == LOCK_SAME)
										node = uln;

									UnLock(path_lock);
								}

								PROFILE_ON();
							}

							/* If we found that this file was set up for deletion,
								 * delete it here and now.
								 */
							if (node != NULL)
							{
								if (NOT file_deleted)
								{
									BPTR old_dir;

									PROFILE_OFF();

									old_dir = CurrentDir(parent_dir);

									if (DeleteFile(fib->Name))
									{
										file_deleted = TRUE;
										name_and_path_valid = FALSE;
									}

									CurrentDir(old_dir);

									PROFILE_ON();
								}

								if (file_deleted)
								{
									Remove((struct Node *)node);
									free(node);
								}
							}
						}
					}
				}
#endif /* UNIX_PATH_SEMANTICS */

				if (FLAG_IS_SET(fd->fd_Flags, FDF_CREATED) && name_and_path_valid)
				{
					BPTR old_dir;

					PROFILE_OFF();

					old_dir = CurrentDir(parent_dir);

					SetProtection(fib->Name, 0);

					CurrentDir(old_dir);

					PROFILE_ON();
				}

				PROFILE_OFF();
				if (fib != NULL)
					FreeDosObject(DOS_EXAMINEDATA, fib);

				UnLock(parent_dir);
				PROFILE_ON();
			}
		}

		__fd_unlock(fd);

#if defined(__THREAD_SAFE)
		{
			/* Free the lock semaphore now. */
			if (NOT is_aliased)
				__delete_semaphore(fd->fd_Lock);
		}
#endif /* __THREAD_SAFE */

		/* And that's the last for this file descriptor. */
		memset(fd, 0, sizeof(*fd));
		fd = NULL;

		break;

	case file_action_seek:
		SHOWMSG("file_action_seek");
		/* Reset error to OK */
		fam->fam_Error = OK;

		if (fam->fam_Mode == SEEK_CUR)
			new_mode = OFFSET_CURRENT;
		else if (fam->fam_Mode == SEEK_SET)
			new_mode = OFFSET_BEGINNING;
		else
			new_mode = OFFSET_END;

		if (FLAG_IS_SET(fd->fd_Flags, FDF_CACHE_POSITION))
		{
			current_position = fd->fd_Position;
		}
		else
		{
			int64_t position;

			PROFILE_OFF();
			position = GetFilePosition(file);
			PROFILE_ON();

			if (position == GETPOSITION_ERROR || IoErr() != OK)
			{
				fam->fam_Error = EBADF;
				goto out;
			}

			current_position = position;
		}

		new_position = current_position;

		switch (new_mode)
		{
		case OFFSET_CURRENT:
			new_position += fam->fam_Offset;
			break;

		case OFFSET_BEGINNING:
			new_position = fam->fam_Offset;
			break;

		case OFFSET_END:
			new_position = GetFileSize(file);
			if (new_position != GETPOSITION_ERROR)
			{
				if (fam->fam_Offset < 0)
					new_position += fam->fam_Offset;

				fib_is_valid = TRUE;
			}

			break;
		}
		/* if new_position is < 0. Force it to 0 */
		if (new_position < 0) {
			ChangeFilePosition(file, 0, OFFSET_BEGINNING);
			fd->fd_Position = new_position = 0;
		}
		else if (new_position != current_position)
		{
			int64_t position;

			PROFILE_OFF();
			position = ChangeFilePosition(file, fam->fam_Offset, new_mode);
			PROFILE_ON();

			/* Same as above: verify that what we got out of
				   Seek() is really an error and not a valid
				   file position. */
			if (position == CHANGE_FILE_ERROR)
			{
				fam->fam_Error = __translate_io_error_to_errno(IoErr());

#if defined(UNIX_PATH_SEMANTICS)
				{
					/* Check if this operation failed because the file is shorter than
						   the new file position. First, we need to find out if the file
						   is really shorter than required. If not, then it must have
						   been a different error. */
					exd = ExamineObjectTags(EX_FileHandleInput, file, TAG_DONE);
					if ((NOT fib_is_valid && exd == NULL) || (exd == NULL) || (new_position <= (int64_t)exd->FileSize))
						goto out;

					/* Now try to make that file larger. */
					if (__grow_file_size(fd, new_position - (int64_t)exd->FileSize) < 0)
					{
						fam->fam_Error = __translate_io_error_to_errno(IoErr());
						FreeDosObject(DOS_EXAMINEDATA, exd);
						goto out;
					}
					fam->fam_Error = OK;
					FreeDosObject(DOS_EXAMINEDATA, exd);
				}
#else
				{
					goto out;
				}
#endif /* UNIX_PATH_SEMANTICS */
			}
			else {
				new_position = GetFilePosition(file);
				if (new_position == GETPOSITION_ERROR) {
					fam->fam_Error = __translate_io_error_to_errno(IoErr());
				}
			}

			fd->fd_Position = new_position;
		}

		result = new_position;
		break;

	case file_action_set_blocking:

		SHOWMSG("file_action_set_blocking");

		PROFILE_OFF();

		if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE))
		{
			LONG mode;

			SHOWMSG("changing the mode");

			if (fam->fam_Arg != 0)
				mode = DOSFALSE; /* buffered mode */
			else
				mode = DOSTRUE; /* single character mode */

			if (CANNOT SetMode(file, mode))
			{
				fam->fam_Error = __translate_io_error_to_errno(IoErr());
				goto out;
			}

			result = OK;
		}
		else
		{
			SHOWMSG("can't do anything here");

			fam->fam_Error = EBADF;
		}

		PROFILE_ON();

		break;

	case file_action_examine:

		SHOWMSG("file_action_examine");

		fh = BADDR(file);

		/* Special treatment for "NIL:", for which we make some stuff up. */
		if (fh->fh_Type == NULL)
		{
			/* Make up some stuff for this stream. */
			memset(fam->fam_FileInfo, 0, sizeof(*fam->fam_FileInfo));

			DateStamp(&fam->fam_FileInfo->Date);

			// TODO - Check this on OS4 with NIL:
			fam->fam_FileInfo->Type = ST_NIL;
		}
		else
		{
			fam->fam_FileInfo = ExamineObjectTags(EX_FileHandleInput, file, TAG_DONE);
			if (fam->fam_FileInfo == NULL) {
				LONG error;

				/* So that didn't work. Did the file system simply fail to
					respond to the request or is something more sinister
					at work? */
				error = IoErr();
				if (error != ERROR_ACTION_NOT_KNOWN)
				{
					SHOWMSG("couldn't examine the file");

					fam->fam_Error = __translate_io_error_to_errno(error);
					goto out;
				}

				/* OK, let's have another look at this file. Could it be a
					console stream? */
				if (NOT IsInteractive(file))
				{
					SHOWMSG("whatever it is, we don't know");

					fam->fam_Error = ENOSYS;
					goto out;
				}

				/* Create an empty examineData struct */
				struct ExamineData *examineData = malloc(sizeof (struct ExamineData));
				fam->fam_FileInfo = examineData;
				/* Make up some stuff for this stream. */
				memset(fam->fam_FileInfo, 0, sizeof(*fam->fam_FileInfo));

				DateStamp(&fam->fam_FileInfo->Date);

				// TODO - Check this on OS4 with CON:
				fam->fam_FileInfo->Type = ST_CONSOLE;
			}
		}

		fam->fam_FileSystem = fh->fh_Type;

		result = OK;

		break;

	default:

		SHOWVALUE(fam->fam_Action);

		fam->fam_Error = EBADF;
		break;
	}

out:
	__fd_unlock(fd);

	if (fam->fam_Action == file_action_close)
		__stdio_unlock();

	if (buffer != NULL)
		free(buffer);

	SHOWVALUE(result);

	RETURN(result);
	return (result);
}
