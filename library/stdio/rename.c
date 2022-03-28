/*
 * $Id: stdio_rename.c,v 1.10 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

int rename(const char *oldname, const char *newname)
{
	struct name_translation_info old_nti;
	struct name_translation_info new_nti;
	int result = ERROR;
	LONG status;

	ENTER();

	SHOWSTRING(oldname);
	SHOWSTRING(newname);

	assert(oldname != NULL && newname != NULL);

	if (__check_abort_enabled)
		__check_abort();

    if (oldname == NULL || newname == NULL)
    {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

	if (__unix_path_semantics)
	{
		if (oldname[0] == '\0' || newname[0] == '\0')
		{
			SHOWMSG("no name given");

			__set_errno(ENOENT);
			goto out;
		}

		if (__translate_unix_to_amiga_path_name(&oldname, &old_nti) != 0)
			goto out;

		if (__translate_unix_to_amiga_path_name(&newname, &new_nti) != 0)
			goto out;

		if (old_nti.is_root || new_nti.is_root)
		{
			__set_errno(EACCES);
			goto out;
		}
	}

	D(("renaming '%s' to '%s'", oldname, newname));

	status = Rename((STRPTR)oldname, (STRPTR)newname);

	if (status == DOSFALSE)
	{
		SHOWMSG("that didn't work");

		if (__unix_path_semantics)
		{
			LONG error;

			error = IoErr();
			if (error != ERROR_OBJECT_EXISTS)
			{
				SHOWMSG("that was some other error");

				__set_errno(__translate_io_error_to_errno(error));
				goto out;
			}

			SHOWMSG("deleting the target file and renaming the source file");

			/* ZZZ there should be a safer solution for this */

			if (CANNOT DeleteFile((STRPTR)newname))
			{
				SHOWMSG("couldn't delete the file");

				__set_errno(__translate_io_error_to_errno(error));
				goto out;
			}

			if (CANNOT Rename((STRPTR)oldname, (STRPTR)newname))
			{
				SHOWMSG("that didn't work");

				__set_errno(__translate_access_io_error_to_errno(IoErr()));
				goto out;
			}
		}
		else
		{
			__set_errno(__translate_io_error_to_errno(IoErr()));
			goto out;
		}
	}

	result = OK;

out:

	RETURN(result);
	return (result);
}
