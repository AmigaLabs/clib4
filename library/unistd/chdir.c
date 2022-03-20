/*
 * $Id: unistd_chdir.c,v 1.9 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/* ZZZ chdir() must be reentrant according to POSIX.1 */
int chdir(const char *path_name)
{
#if defined(UNIX_PATH_SEMANTICS)
	struct name_translation_info path_name_nti;
#endif /* UNIX_PATH_SEMANTICS */
	BPTR dir_lock = ZERO;
	struct ExamineData *status = NULL;
	int result = ERROR;

	ENTER();

	SHOWSTRING(path_name);

	assert(path_name != NULL);

	if (__check_abort_enabled)
		__check_abort();

    if (path_name == NULL)
    {
        SHOWMSG("invalid path name");

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

		/* The pseudo root directory is a very special case indeed. We
			* just accept it and don't pretend to have obtained a lock
			* on anything.
			*/
		if (path_name_nti.is_root)
		{
			SHOWMSG("this is the / directory");

			__restore_path_name(&path_name, &path_name_nti);

			/* ZZZ this must not fail */
			__set_current_path(path_name);

			result = OK;

			goto out;
		}
	}
#endif /* UNIX_PATH_SEMANTICS */

	D(("trying to get a lock on '%s'", path_name));

	dir_lock = Lock((STRPTR)path_name, SHARED_LOCK);
	if (dir_lock == ZERO)
	{
		__set_errno(__translate_access_io_error_to_errno(IoErr()));
		goto out;
	}

	status = ExamineObjectTags(EX_LockInput, dir_lock, TAG_DONE);
	if (status == NULL)
	{
		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

	if (!EXD_IS_DIRECTORY(status))
	{
		SHOWMSG("this is not a directory");

		__set_errno(ENOTDIR);
		goto out;
	}

	if (__current_directory_changed)
	{
		BPTR old_dir;

		old_dir = CurrentDir(dir_lock);

		if (__unlock_current_directory)
			UnLock(old_dir);
	}
	else
	{
		__original_current_directory = CurrentDir(dir_lock);

		__current_directory_changed = TRUE;
	}

	__unlock_current_directory = TRUE;

	dir_lock = ZERO;

#if defined(UNIX_PATH_SEMANTICS)
	if (__global_clib2->__unix_path_semantics)
		__restore_path_name(&path_name, &path_name_nti);

	/* ZZZ this must not fail */
	__set_current_path(path_name);
#endif /* UNIX_PATH_SEMANTICS */

	result = OK;

out:

	if (status != NULL) {
		FreeDosObject(DOS_EXAMINEDATA, status);
	}
	
	UnLock(dir_lock);

	RETURN(result);
	return (result);
}
