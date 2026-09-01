/*
 * $Id: socket_init_exit.c,v 1.26 2006-04-05 08:39:45 clib4devs Exp $
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

struct Library *__SocketBase;
struct SocketIFace *__ISocket;

/* Call-back hook for use with SBTC_ERROR_HOOK */
struct _ErrorHookMsg {
    ULONG ehm_Size;     /* Size of this data structure; this must be >= 12 */
    ULONG ehm_Action;   /* See below for a list of definitions */
    LONG ehm_Code;      /* The error code to use */
};

/* Which action the hook is to perform */
#define EHMA_Set_errno   1 /* Set the local 'errno' to what is found in ehm_Code */
#define EHMA_Set_h_errno 2 /* Set the local 'h_errno' to what is found in ehm_Code */

/****************************************************************************/

/* This hook function is called whenever either the errno or h_errno
   variable is to be changed by the bsdsocket.library code. It is invoked
   on the context of the caller, which means that the Process which called
   the library will also be the one will eventually call the hook function.
   You can key off this in your own __set_errno() or __set_h_errno()
   functions, setting a Process-specific set of variables. */
static LONG ASM
error_hook_function(
        REG(a0, struct Hook *unused_hook),
        REG(a2, APTR unused_reserved),
        REG(a1, struct _ErrorHookMsg *ehm)
) {
    (void) (unused_hook);
    (void) (unused_reserved);

    if (ehm != NULL && ehm->ehm_Size >= 12) {
        if (ehm->ehm_Action == EHMA_Set_errno)
            __set_errno(ehm->ehm_Code);
        else if (ehm->ehm_Action == EHMA_Set_h_errno)
            __set_h_errno(ehm->ehm_Code);
    }

    return (0);
}

STATIC struct Hook error_hook = {
    {NULL, NULL},
    (HOOKFUNC) error_hook_function,
    (HOOKFUNC) NULL,
    NULL
};

void socket_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    /* Disable ^C checking. */
    if (__clib4->__SocketBase != NULL) {
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
    __close_all_files(__clib4);
    if (__clib4->__ISocket != NULL) {
        DropInterface((struct Interface *) __clib4->__ISocket);
        __clib4->__ISocket = NULL;
    }
    if (__clib4->__SocketBase != NULL) {
        CloseLibrary(__clib4->__SocketBase);
        __clib4->__SocketBase = NULL;
    }

    LEAVE();
}

/****************************************************************************/

/* Open bsdsocket.library and wire it up for this process.  Returns TRUE when
 * __SocketBase/__ISocket are usable afterwards.
 *
 * Callers must serialise concurrent invocations; socket_init() runs before the
 * process has any other thread and __ensure_socket_library() takes socket_lock.
 */
static BOOL
open_socket_library(struct _clib4 *__clib4) {
    struct TagItem tags[5];
    BOOL success = FALSE;
    LONG status;

    ENTER();

    if (__clib4->__ISocket != NULL) {
        /* Somebody opened it already. */
        success = TRUE;
        goto out;
    }

    /* bsdsocket.library V3 is sufficient for all the tasks we may have to perform. */
    __clib4->__SocketBase = OpenLibrary("bsdsocket.library", 3);
    if (__clib4->__SocketBase != NULL) {
        __clib4->__ISocket = (struct SocketIFace *) GetInterface(__clib4->__SocketBase, "main", 1, 0);
        if (__clib4->__ISocket == NULL) {
            CloseLibrary(__clib4->__SocketBase);
            __clib4->__SocketBase = NULL;
        }
    }

    if (__clib4->__SocketBase == NULL) {
        SHOWMSG("bsdsocket.library V3 didn't open");
        goto out;
    }

    /* Wire the library's errno variable to our local errno. */
    tags[0].ti_Tag = SBTM_SETVAL(SBTC_ERRNOLONGPTR);
    tags[0].ti_Data = (ULONG)&__clib4->_errno;

    /* Also enable ^C checking if desired. */
    tags[1].ti_Tag = SBTM_SETVAL(SBTC_BREAKMASK);
    tags[1].ti_Data = __clib4->__break_signal_mask;

    tags[2].ti_Tag = SBTM_SETVAL(SBTC_LOGTAGPTR);
    tags[2].ti_Data = (ULONG)__clib4->__progname;

    /* Wire the library's h_errno variable to our local h_errno. */
    tags[3].ti_Tag = SBTM_SETVAL(SBTC_HERRNOLONGPTR);
    tags[3].ti_Data = (ULONG)&__clib4->_h_errno;

    tags[4].ti_Tag = TAG_END;

    status = __SocketBaseTagList(tags);
    if (status != 0) {
        SHOWMSG("couldn't initialize the library");

        DropInterface((struct Interface *) __clib4->__ISocket);
        __clib4->__ISocket = NULL;
        CloseLibrary(__clib4->__SocketBase);
        __clib4->__SocketBase = NULL;
        goto out;
    }

/* In the thread-safe library we try to enable two features which so
   far only the Roadshow TCP/IP stack supports: allow more than one
   Process to use the same bsdsocket.library base and to propagate
   changes to the errno and h_errno variable through a call-back
   hook. If either of these features are supported can be checked
   by looking at the global __can_share_socket_library_base and
   __thread_safe_errno_h_errno variables. */
    if (__clib4->__SocketBase->lib_Version >= 4) {
        tags[0].ti_Tag = SBTM_SETVAL(SBTC_CAN_SHARE_LIBRARY_BASES);
        tags[0].ti_Data = TRUE;

        tags[1].ti_Tag = TAG_END;

        if (__SocketBaseTagList(tags) == 0)
            __clib4->__can_share_socket_library_base = TRUE;

        if (__clib4->__can_share_socket_library_base) {
            tags[0].ti_Tag = SBTM_SETVAL(SBTC_ERROR_HOOK);
            tags[0].ti_Data = (ULONG) &error_hook;

            tags[1].ti_Tag = TAG_END;

            if (__SocketBaseTagList(tags) == 0)
                __clib4->__thread_safe_errno_h_errno = TRUE;
        }
    }

    success = TRUE;

out:

    SHOWVALUE(success);
    RETURN(success);
    return success;
}

/****************************************************************************/

/* Make sure bsdsocket.library is available before a socket API is used.
 *
 * bsdsocket.library is not a disk library: it is created in memory by the
 * TCP/IP stack, which on AmigaOS is started from S:Network-Startup, i.e. from
 * the startup-sequence.  Anything that runs *before* the startup-sequence
 * therefore cannot open it yet.  newlib copes with that because it only opens
 * the library the first time a socket function is called; clib4 used to open
 * it in its constructor and killed the whole process when that failed, so a
 * clib4 program that never touches a socket could not even start that early.
 *
 * This function restores the newlib behaviour: the library is opened on demand
 * and the attempt is repeated on every call, so a program which was started
 * before the TCP/IP stack came up will still get working sockets as soon as
 * the stack is there.
 *
 * Returns TRUE when sockets can be used, FALSE (with errno set to ENETDOWN)
 * when the TCP/IP stack is still unavailable.
 */
BOOL
__ensure_socket_library(struct _clib4 *__clib4) {
    BOOL result;

    if (__clib4 == NULL) {
        return FALSE;
    }

    /* Fast path: already open. */
    if (__clib4->__ISocket != NULL) {
        return TRUE;
    }

    if (__clib4->socket_lock != NULL)
        ObtainSemaphore(__clib4->socket_lock);

    result = open_socket_library(__clib4);

    if (__clib4->socket_lock != NULL)
        ReleaseSemaphore(__clib4->socket_lock);

    if (!result) {
        /* No TCP/IP stack (yet). */
        __set_errno_r(__clib4, ENETDOWN);
        __set_h_errno_r(__clib4, TRY_AGAIN);
    }

    return result;
}

/****************************************************************************/

void socket_init(void) {
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    /* Not being able to open bsdsocket.library is not a fatal condition:
     * see __ensure_socket_library() above.  Programs which never use a socket
     * must be able to run before the TCP/IP stack has been started, and those
     * which do use one will open the library on first use. */
    (void) open_socket_library(__clib4);

    LEAVE();

    CONSTRUCTOR_SUCCEED();
}
