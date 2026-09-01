/*
 * $Id: usergroup_init_exit.c,v 1.12 2006-01-08 12:04:27 clib4devs Exp $
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

void usergroup_exit(void) {
	ENTER();
    struct _clib4 *__clib4 = __CLIB4;

	if (__clib4->__IUserGroup != NULL) {
		DropInterface((struct Interface *)__clib4->__IUserGroup);
        __clib4->__IUserGroup = NULL;
	}

	if (__clib4->__UserGroupBase != NULL) {
		CloseLibrary(__clib4->__UserGroupBase);
        __clib4->__UserGroupBase = NULL;
	}

	LEAVE();
}

/****************************************************************************/

/* Open usergroup.library and set up this process's context. Returns TRUE when
 * __UserGroupBase/__IUserGroup are usable afterwards.
 *
 * Callers must serialise concurrent invocations; usergroup_init() runs before
 * the process has any other thread and __ensure_usergroup_library() takes
 * usergroup_lock.
 */
static BOOL
open_usergroup_library(struct _clib4 *__clib4) {
	struct TagItem tags[2];
	BOOL success = FALSE;

	ENTER();

	if (__clib4->__IUserGroup != NULL) {
		/* Somebody opened it already. */
		success = TRUE;
		goto out;
	}

    __clib4->__UserGroupBase = OpenLibrary("usergroup.library", 0);

	if (__clib4->__UserGroupBase != NULL) {
        __clib4->__IUserGroup = (struct UserGroupIFace *)GetInterface(__clib4->__UserGroupBase, "main", 1, 0);
		if (__clib4->__IUserGroup == NULL) {
			CloseLibrary(__clib4->__UserGroupBase);
            __clib4->__UserGroupBase = NULL;
		}
	}

	if (__clib4->__UserGroupBase == NULL) {
		SHOWMSG("usergroup.library did not open");
		goto out;
	}

	/* Wire the library's errno variable to our local errno. */
	tags[0].ti_Tag = UGT_ERRNOLPTR;
	tags[0].ti_Data = (ULONG)&errno;

	tags[1].ti_Tag = TAG_END;

	if (__ug_SetupContextTagList(__clib4->__progname, tags) != 0)
	{
		SHOWMSG("could not initialize usergroup.library");

		DropInterface((struct Interface *)__clib4->__IUserGroup);
		__clib4->__IUserGroup = NULL;
		CloseLibrary(__clib4->__UserGroupBase);
		__clib4->__UserGroupBase = NULL;
		goto out;
	}

	success = TRUE;

out:

	SHOWVALUE(success);
	RETURN(success);
	return success;
}

/****************************************************************************/

/* Make sure usergroup.library is available before one of its functions is used.
 *
 * usergroup.library ships with the TCP/IP stack and, depending on the version,
 * needs bsdsocket.library itself: a program which runs before the
 * startup-sequence may well be unable to open it. clib4 used to open it in its
 * constructor and killed the whole process when that failed, so a program that
 * never looks at a user or a group could not even start that early.
 *
 * Like __ensure_socket_library(), this opens the library on demand and retries
 * on every call, so a program started before the stack came up still gets the
 * real user/group database as soon as the library shows up.
 *
 * Returns TRUE when usergroup.library can be used, FALSE otherwise. errno is
 * left alone: the callers decide what a missing library means for them.
 */
BOOL
__ensure_usergroup_library(struct _clib4 *__clib4) {
	BOOL result;

	if (__clib4 == NULL) {
		return FALSE;
	}

	/* Fast path: already open. */
	if (__clib4->__IUserGroup != NULL) {
		return TRUE;
	}

	if (__clib4->usergroup_lock != NULL)
		ObtainSemaphore(__clib4->usergroup_lock);

	result = open_usergroup_library(__clib4);

	if (__clib4->usergroup_lock != NULL)
		ReleaseSemaphore(__clib4->usergroup_lock);

	return result;
}

/****************************************************************************/

void usergroup_init(void) {
    struct _clib4 *__clib4 = __CLIB4;

	ENTER();

	/* Not being able to open usergroup.library is not a fatal condition:
	 * see __ensure_usergroup_library() above. */
	(void) open_usergroup_library(__clib4);

	LEAVE();

	CONSTRUCTOR_SUCCEED();
}
