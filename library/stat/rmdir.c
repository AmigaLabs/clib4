/*
 * $Id: stat_rmdir.c,v 1.9 2021-01-31 12:04:24 clib2devs Exp $
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
