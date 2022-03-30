/*
 * $Id: unistd_readlink.c,v 1.9 2006-11-13 09:25:28 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STAT_HEADERS_H
#include "stat_headers.h"
#endif /* _STAT_HEADERS_H */

int readlink(const char *path_name, char *buffer, int buffer_size)
{
	struct name_translation_info path_name_nti;
	struct name_translation_info buffer_nti;
	BPTR lock = ZERO;
	int result = ERROR;
	int target_length = -1;

	ENTER();

	SHOWSTRING(path_name);
	SHOWPOINTER(buffer);
	SHOWVALUE(buffer_size);

	assert(path_name != NULL && buffer != NULL);

	if (__check_abort_enabled)
		__check_abort();

    if (path_name == NULL || buffer == NULL)
    {
        SHOWSTRING("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

	if (__unix_path_semantics)
	{
		if (path_name[0] == '\0')
		{
			SHOWMSG("no name given");

			__set_errno(ENOENT);
			goto out;
		}

		if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
			goto out;
	}

	D(("trying to get a lock on '%s'", path_name));

	lock = __lock((STRPTR)path_name, SHARED_LOCK, &target_length, buffer, (size_t)buffer_size);
	if (lock != ZERO)
	{
		__set_errno(EINVAL);
		goto out;
	}
	else if (target_length <= 0) /* No a soft-link. */
	{
		__set_errno(__translate_io_error_to_errno(IoErr()));
		goto out;
	}

    if (__unix_path_semantics)
    {
        if (__translate_amiga_to_unix_path_name((char const **)&buffer, &buffer_nti) != 0)
            goto out;

        __restore_path_name((char const **)&buffer, &buffer_nti);

        strcpy(buffer, buffer_nti.substitute);
    }

	result = strlen(buffer);

	SHOWSTRING(buffer);

out:

	UnLock(lock);

	RETURN(result);
	return (result);
}
