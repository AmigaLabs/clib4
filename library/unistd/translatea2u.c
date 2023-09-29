/*
 * $Id: unistd_translatea2u.c,v 1.7 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

/****************************************************************************/

int __translate_amiga_to_unix_path_name(char const **name_ptr, struct name_translation_info *nti)
{
	const int max_unix_len = sizeof(nti->substitute) - 1;
	char local_replacement[sizeof(nti->substitute)];
	BOOL have_double_slash;
	int len, replace_len;
	int result = ERROR;
	int i;
	char *replace;
	char *name;

	assert(name_ptr != NULL && (*name_ptr) != NULL && nti != NULL);

	replace = nti->substitute;

	name = (char *)(*name_ptr);

	/* Check if the name would become too long to handle. */
	len = strlen(name);
	if (len >= (int)sizeof(nti->substitute))
	{
		D(("path name '%s' is too long (%ld characters total; maximum is %ld)!", name, len, sizeof(nti->substitute) - 1));

		__set_errno(ENAMETOOLONG);
		goto out;
	}

	/* Reduce any '//' embedded in the name if possible. */
	have_double_slash = FALSE;

	for (i = 0; i < len - 1; i++)
	{
		if (name[i] == '/' && name[i + 1] == '/')
		{
			have_double_slash = TRUE;
			break;
		}
	}

	if (have_double_slash)
	{
		/* We will need to make a copy of the name. Make sure that it fits. */
		if (len > max_unix_len)
		{
			__set_errno(ENAMETOOLONG);
			goto out;
		}

		strcpy(local_replacement, name);
		name = local_replacement;

		len = __strip_double_slash(name, len);
	}

	/* The empty string corresponds to the current
	 * directory, which is the ".".
	 */
	if (len == 0)
	{
		strcpy(replace, ".");
	}
	else
	{
		char *volume_name;
		int volume_name_len;

		/* Figure out if the path includes a volume, device or assignment name.
		   If so, strip it from the path, to be translated and added later. */
		volume_name = NULL;
		volume_name_len = 0;

		for (i = 0; i < len; i++)
		{
			if (name[i] == ':')
			{
				volume_name = name;
				volume_name_len = i;

				name += volume_name_len + 1;
				len -= volume_name_len + 1;

				break;
			}
		}

		/* Replace all "/" characters in the name which don't just act as
		 * separators with "..". Otherwise, copy the name string to the
		 * replacement buffer.
		 */
		replace_len = 0;

		while (name[0] == '/')
		{
			if (replace_len + 3 > max_unix_len)
			{
				__set_errno(ENAMETOOLONG);
				goto out;
			}

			memmove(&replace[replace_len], "../", 3);
			replace_len += 3;

			name++;
			len--;

			if (len == 0)
			{
				/* Strip the trailing '/'. */
				replace[--replace_len] = '\0';
			}
		}

		for (i = 0; i < len; i++)
		{
			if (name[i] == '/' && (name[i + 1] == '/'))
			{
				if (replace_len + 3 > max_unix_len)
				{
					__set_errno(ENAMETOOLONG);
					goto out;
				}

				memmove(&replace[replace_len], "/..", 3);
				replace_len += 3;
			}
			else
			{
				if (replace_len + 1 > max_unix_len)
				{
					__set_errno(ENAMETOOLONG);
					goto out;
				}

				replace[replace_len++] = name[i];
			}
		}

		assert(replace_len <= max_unix_len);

		replace[replace_len] = '\0';

		/* If necessary, add and translate the volume name found above. */
		if (volume_name != NULL)
		{
			/* Check if the complete path will fit. This assumes the worst
			   case scenario. */
			if (1 + volume_name_len + 1 + replace_len > max_unix_len)
			{
				__set_errno(ENAMETOOLONG);
				goto out;
			}

			if (volume_name_len > 0)
			{
				if (replace_len > 0)
				{
					/* Add the volume name in front of the path. */
					memmove(&replace[1 + volume_name_len + 1], replace, (size_t)(replace_len + 1));
					replace[0] = replace[1 + volume_name_len] = '/';
					memmove(&replace[1], volume_name, (size_t)volume_name_len);
				}
				else
				{
					/* There is only the volume name. */
					replace[0] = '/';
					memmove(&replace[1], volume_name, (size_t)volume_name_len);
					replace[volume_name_len + 1] = '\0';
				}
			}
			else
			{
				int device_name_len = 0;

				/* We need to expand ':'. If the current directory name is
				   known and refers to an absolute path, we borrow the
				   absolute path name component. */
				char device_name[256] = {0};
				if (DevNameFromLock(GetCurrentDir(), device_name, sizeof(device_name), DN_DEVICEONLY))
				{
					device_name_len = strlen(device_name);
					if (device_name_len > 0)
						device_name_len--;

					/* If we found the absolute path name portion, use it. */
					if (device_name_len > 0)
					{
						if (1 + device_name_len + 1 + replace_len > max_unix_len)
						{
							__set_errno(ENAMETOOLONG);
							goto out;
						}

						if (replace_len > 0)
						{
							memmove(&replace[1 + device_name_len + 1], replace, (size_t)(replace_len + 1));
							replace[0] = replace[1 + device_name_len] = '/';
							memmove(&replace[1], device_name, (size_t)device_name_len);
						}
						else
						{
							replace[0] = '/';
							memmove(&replace[1], device_name, (size_t)device_name_len);
							replace[device_name_len + 1] = '\0';
						}
					}
				}

				if (device_name_len == 0)
				{
					/* The ':foo' path gets translated into '/foo'. */
					memmove(&replace[1], replace, (size_t)(replace_len + 1));
					replace[0] = '/';
				}
			}
		}
	}

	name = replace;

	assert(strlen(name) < (size_t)max_unix_len);

	nti->original_name = (char *)(*name_ptr);
	(*name_ptr) = name;

	D(("translated '%s' to '%s'", nti->original_name, name));

	result = OK;

out:

	return (result);
}
