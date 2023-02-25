/*
 * $Id: dirent_closedir.c,v 1.15 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/* Directories being scanned whose locks need to be freed when shutting down. */
struct MinList NOCOMMON __directory_list;
static struct SignalSemaphore *dirent_lock;

void __dirent_lock(void) {
    if (dirent_lock != NULL)
        ObtainSemaphore(dirent_lock);
}

void __dirent_unlock(void) {
    if (dirent_lock != NULL)
        ReleaseSemaphore(dirent_lock);
}

CLIB_CONSTRUCTOR(dirent_init) {
    BOOL success = FALSE;

    ENTER();

    NewList((struct List *)&__directory_list);

    dirent_lock = __create_semaphore();
    if (dirent_lock == NULL)
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

CLIB_DESTRUCTOR(dirent_exit) {
    ENTER();

    if (__directory_list.mlh_Head != NULL)
    {
        while (NOT IsMinListEmpty(&__directory_list))
        closedir((DIR *) __directory_list.mlh_Head);
    }

    __delete_semaphore(dirent_lock);
    dirent_lock = NULL;

    LEAVE();
}

int
closedir(DIR *directory_pointer) {
    struct DirectoryHandle *dh;
    int result = ERROR;

    ENTER();

    SHOWPOINTER(directory_pointer);

    __check_abort();

    __dirent_lock();

    if (directory_pointer == NULL) {
        __set_errno(EBADF);
        goto out;
    }

#ifndef NDEBUG
    {
        BOOL directory_pointer_is_valid = FALSE;

        for (dh = (struct DirectoryHandle *) __directory_list.mlh_Head;
             dh->dh_MinNode.mln_Succ != NULL;
             dh = (struct DirectoryHandle *) dh->dh_MinNode.mln_Succ) {
            if (dh == (struct DirectoryHandle *) directory_pointer) {
                directory_pointer_is_valid = TRUE;
                break;
            }
        }

        if (NOT directory_pointer_is_valid) {
            __set_errno(EBADF);
            goto out;
        }
    }
#endif /* NDEBUG */

    dh = (struct DirectoryHandle *) directory_pointer;

    Remove((struct Node *) dh);

    if (__unix_path_semantics) {
        struct Node *node;

        while ((node = RemHead((struct List *) &dh->dh_VolumeList)) != NULL)
            free(node);
    }

    if (dh->dh_Context != NULL) {
        ReleaseDirContext(dh->dh_Context);
        dh->dh_Context = NULL;
    }

    UnLock(dh->dh_DirLock);

    /* If we have an associated fd file close it */
    if (dh->dh_Fd > 0) {
        close(dh->dh_Fd);
    }

    free(dh);

    result = OK;

out:

    __dirent_unlock();

    RETURN(result);
    return (result);
}
