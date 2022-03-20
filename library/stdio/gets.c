/*
 * $Id: stdio_gets.c,v 1.6 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
gets(char *s)
{
	char *result = s;
	int c;

	ENTER();

	SHOWPOINTER(s);

	assert(s != NULL && stdin != NULL);

	if (__check_abort_enabled)
		__check_abort();

	flockfile(stdin);

    if (s == NULL || stdin == NULL)
    {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);

        result = NULL;
        goto out;
    }

	/* Take care of the checks and data structure changes that
	 * need to be handled only once for this stream.
	 */
	if (__fgetc_check(stdin) < 0)
	{
		result = NULL;
		goto out;
	}

	/* So that we can tell error and 'end of file' conditions apart. */
	clearerr(stdin);

	while (TRUE)
	{
		c = __getc(stdin);
		if (c == EOF)
		{
			if (ferror(stdin))
			{
				/* Just to be on the safe side. */
				(*s) = '\0';

				result = NULL;
				goto out;
			}

			/* Make sure that we return NULL if we really
			   didn't read anything at all */
			if (s == result)
				result = NULL;

			break;
		}

		if (c == '\n')
			break;

		(*s++) = c;
	}

	(*s) = '\0';

	SHOWSTRING(result);

out:

	funlockfile(stdin);

	RETURN(result);
	return (result);
}
