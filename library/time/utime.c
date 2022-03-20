/*
 * $Id: utime_utime.c,v 1.12 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UTIME_HEADERS_H
#include "utime_headers.h"
#endif /* _UTIME_HEADERS_H */

int utime(const char *path_name, const struct utimbuf *times)
{
#if defined(UNIX_PATH_SEMANTICS)
	struct name_translation_info path_name_nti;
#endif /* UNIX_PATH_SEMANTICS */
	struct DateStamp ds;
	int result = ERROR;
	LONG status;

	assert(path_name != NULL);

	if (__check_abort_enabled)
		__check_abort();

    if (path_name == NULL)
    {
        __set_errno(EFAULT);
        goto out;
    }

	/* If a modification time is provided, convert it into the local
	   DateStamp format, as used by the SetFileDate() function. */
	if (times != NULL)
	{
		if (CANNOT __convert_time_to_datestamp(times->modtime, &ds))
		{
			__set_errno(EINVAL);
			goto out;
		}
	}
	else
	{
		/* No special modification time provided; use the current
		   time instead. */
		DateStamp(&ds);
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

	status = SetFileDate((STRPTR)path_name, &ds);
	if (status == DOSFALSE)
	{
		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

	result = OK;

out:

	return (result);
}
