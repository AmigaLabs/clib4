/*
 * $Id: stat_rmdir.c,v 1.9 2021-01-31 12:04:24 apalmate Exp $
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

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

int rmdir(const char *path_name)
{
#if defined(UNIX_PATH_SEMANTICS)
	struct name_translation_info path_name_nti;
#endif /* UNIX_PATH_SEMANTICS */
	BPTR dir_lock = ZERO;
	int result = ERROR;
	struct ExamineData *fib = NULL;
	LONG status;

	ENTER();

	SHOWSTRING(path_name);

	assert(path_name != NULL);

	if (__check_abort_enabled)
		__check_abort();

    if (path_name == NULL)
    {
        SHOWMSG("invalid path name parameter");

        __set_errno(EFAULT);
        goto out;
    }

#if defined(UNIX_PATH_SEMANTICS)
	if (__global_clib2->__unix_path_semantics)
	{
		if (path_name[0] == '\0')
		{
			SHOWMSG("no name given");

			__set_errno(ENOENT);
			goto out;
		}

		if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
			goto out;

		if (path_name_nti.is_root)
		{
			__set_errno(EACCES);
			goto out;
		}
	}
#endif /* UNIX_PATH_SEMANTICS */

	D(("trying to get a lock on '%s'", path_name));

	dir_lock = Lock((STRPTR)path_name, SHARED_LOCK);
	if (dir_lock == ZERO)
	{
		SHOWMSG("that didn't work");

		__set_errno(__translate_access_io_error_to_errno(IoErr()));
		goto out;
	}

	fib = ExamineObjectTags(EX_LockInput, dir_lock, TAG_DONE);
	if (fib == NULL)
	{
		SHOWMSG("couldn't examine it");

		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

	if (!EXD_IS_DIRECTORY(fib))
	{
		SHOWMSG("this is not a directory");

		__set_errno(ENOTDIR);
		goto out;
	}

	UnLock(dir_lock);

	dir_lock = ZERO;

	SHOWMSG("trying to delete it");

	status = DeleteFile((STRPTR)path_name);
	if (status == DOSFALSE)
	{
		SHOWMSG("that didn't work");

		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

	result = OK;

out:
	if (fib != NULL)
	{
		FreeDosObject(DOS_EXAMINEDATA, fib);
	}

	UnLock(dir_lock);

	RETURN(result);
	return (result);
}
