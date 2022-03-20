/*
 * $Id: usergroup_init_exit.c,v 1.12 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _USERGROUP_HEADERS_H
#include "usergroup_headers.h"
#endif /* _USERGROUP_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/****************************************************************************/

/* Pointer to errno, length == sizeof(long) */
#define UGT_ERRNOLPTR 0x80000004

/****************************************************************************/

struct Library *NOCOMMON __UserGroupBase;
struct UserGroupIFace *NOCOMMON __IUserGroup;

/****************************************************************************/

CLIB_DESTRUCTOR(usergroup_exit)
{
	ENTER();

	if (__IUserGroup != NULL)
	{
		DropInterface((struct Interface *)__IUserGroup);
		__IUserGroup = NULL;
	}

	if (__UserGroupBase != NULL)
	{
		CloseLibrary(__UserGroupBase);
		__UserGroupBase = NULL;
	}

	LEAVE();
}

/****************************************************************************/

CLIB_CONSTRUCTOR(usergroup_init)
{
	struct TagItem tags[2];
	BOOL success = FALSE;

	ENTER();

	__UserGroupBase = OpenLibrary("usergroup.library", 0);

	if (__UserGroupBase != NULL)
	{
		__IUserGroup = (struct UserGroupIFace *)GetInterface(__UserGroupBase, "main", 1, 0);
		if (__IUserGroup == NULL)
		{
			CloseLibrary(__UserGroupBase);
			__UserGroupBase = NULL;
		}
	}

	if (__UserGroupBase == NULL)
	{
		SHOWMSG("usergroup.library did not open");

		__show_error("\"usergroup.library\" could not be opened.");
		goto out;
	}

	/* Wire the library's errno variable to our local errno. */
	tags[0].ti_Tag = UGT_ERRNOLPTR;
	tags[0].ti_Data = (ULONG)&errno;

	tags[1].ti_Tag = TAG_END;

	if (__ug_SetupContextTagList(__program_name, tags) != 0)
	{
		SHOWMSG("could not initialize usergroup.library");

		__show_error("\"usergroup.library\" could not be initialized.");
		goto out;
	}

	success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}
