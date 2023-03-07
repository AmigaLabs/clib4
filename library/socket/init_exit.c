/*
 * $Id: socket_init_exit.c,v 1.26 2006-04-05 08:39:45 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/****************************************************************************/

struct Library *NOCOMMON __SocketBase;
struct SocketIFace *NOCOMMON __ISocket;

int NOCOMMON h_errno;

/* Call-back hook for use with SBTC_ERROR_HOOK */
struct _ErrorHookMsg
{
	ULONG ehm_Size;	  /* Size of this data structure; this
						   must be >= 12 */
	ULONG ehm_Action; /* See below for a list of definitions */

	LONG ehm_Code; /* The error code to use */
};

/* Which action the hook is to perform */
#define EHMA_Set_errno 1   /* Set the local 'errno' to what is \
							  found in ehm_Code */
#define EHMA_Set_h_errno 2 /* Set the local 'h_errno' to what is \
							  found in ehm_Code */

/****************************************************************************/

BOOL NOCOMMON __can_share_socket_library_base;
BOOL NOCOMMON __thread_safe_errno_h_errno;

/****************************************************************************/

/* This hook function is called whenever either the errno or h_errno
   variable is to be changed by the bsdsocket.library code. It is invoked
   on the context of the caller, which means that the Process which called
   the library will also be the one will eventually call the hook function.
   You can key off this in your own __set_errno() or __set_h_errno()
   functions, setting a Process-specific set of variables. */
STATIC LONG ASM
	error_hook_function(
		REG(a0, struct Hook *unused_hook),
		REG(a2, APTR unused_reserved),
		REG(a1, struct _ErrorHookMsg *ehm))
{
    (void) (unused_hook);
    (void) (unused_reserved);

	if (ehm != NULL && ehm->ehm_Size >= 12)
	{
		if (ehm->ehm_Action == EHMA_Set_errno)
			__set_errno(ehm->ehm_Code);
		else if (ehm->ehm_Action == EHMA_Set_h_errno)
			__set_h_errno(ehm->ehm_Code);
	}

	return (0);
}

STATIC struct Hook error_hook =
	{
		{NULL, NULL},
		(HOOKFUNC)error_hook_function,
		(HOOKFUNC)NULL,
		NULL};

SOCKET_DESTRUCTOR(socket_exit)
{
	ENTER();

	/* Disable ^C checking. */
	if (__SocketBase != NULL)
	{
		struct TagItem tags[2];

		tags[0].ti_Tag = SBTM_SETVAL(SBTC_BREAKMASK);
		tags[0].ti_Data = 0;
		tags[1].ti_Tag = TAG_END;

		__SocketBaseTagList(tags);
	}

	/*
	 * Careful: if this function is ever invoked, it must make sure that
	 *          the socket file descriptors are invalidated. If that
	 *          does not happen, the stdio cleanup function will
	 *          crash (with bells on).
	 */
	__close_all_files();
	if (__ISocket != NULL)
	{
		DropInterface((struct Interface *)__ISocket);
		__ISocket = NULL;
	}
	if (__SocketBase != NULL)
	{
		CloseLibrary(__SocketBase);
		__SocketBase = NULL;
	}

	LEAVE();
}

/****************************************************************************/

SOCKET_CONSTRUCTOR(socket_init)
{
	struct TagItem tags[5];
	BOOL success = FALSE;
	LONG status;

	ENTER();

	/* bsdsocket.library V3 is sufficient for all the tasks we
	   may have to perform. */
	__SocketBase = OpenLibrary("bsdsocket.library", 3);
	if (__SocketBase != NULL)
	{
		__ISocket = (struct SocketIFace *)GetInterface(__SocketBase, "main", 1, 0);
		if (__ISocket == NULL)
		{
			CloseLibrary(__SocketBase);
			__SocketBase = NULL;
		}
	}

	if (__SocketBase == NULL)
	{
		SHOWMSG("bsdsocket.library V3 didn't open");

		__show_error("\"bsdsocket.library\" V3 could not be opened.");
		goto out;
	}

	/* Wire the library's errno variable to our local errno. */
	tags[0].ti_Tag = SBTM_SETVAL(SBTC_ERRNOLONGPTR);
	tags[0].ti_Data = (ULONG)&errno;

	/* Also enable ^C checking if desired. */
	tags[1].ti_Tag = SBTM_SETVAL(SBTC_BREAKMASK);

	if (__check_abort_enabled)
		tags[1].ti_Data = __break_signal_mask;
	else
		tags[1].ti_Data = 0;

	tags[2].ti_Tag = SBTM_SETVAL(SBTC_LOGTAGPTR);
	tags[2].ti_Data = (ULONG)__program_name;

	/* Wire the library's h_errno variable to our local h_errno. */
	tags[3].ti_Tag = SBTM_SETVAL(SBTC_HERRNOLONGPTR);
	tags[3].ti_Data = (ULONG)&h_errno;

	tags[4].ti_Tag = TAG_END;

	status = __SocketBaseTagList(tags);
	if (status != 0)
	{
		SHOWMSG("couldn't initialize the library");

		__show_error("\"bsdsocket.library\" could not be initialized.");
		goto out;
	}

/* In the thread-safe library we try to enable two features which so
	   far only the Roadshow TCP/IP stack supports: allow more than one
	   Process to use the same bsdsocket.library base and to propagate
	   changes to the errno and h_errno variable through a call-back
	   hook. If either of these features are supported can be checked
	   by looking at the global __can_share_socket_library_base and
	   __thread_safe_errno_h_errno variables. */
	if (__SocketBase->lib_Version >= 4)
	{
		tags[0].ti_Tag = SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES);
		tags[0].ti_Data = TRUE;

		tags[1].ti_Tag = TAG_END;

		if (__SocketBaseTagList(tags) == 0)
			__can_share_socket_library_base = TRUE;

		if (__can_share_socket_library_base)
		{
			tags[0].ti_Tag = SBTM_SETVAL(SBTC_ERROR_HOOK);
			tags[0].ti_Data = (ULONG)&error_hook;

			tags[1].ti_Tag = TAG_END;

			if (__SocketBaseTagList(tags) == 0)
				__thread_safe_errno_h_errno = TRUE;
		}
	}

	/* Check if this program was launched as a server by the Internet
	   superserver. */
	if (CANNOT __obtain_daemon_message())
		goto out;

	success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}
