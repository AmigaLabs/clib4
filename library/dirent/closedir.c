/*
 * $Id: dirent_closedir.c,v 2.0 2023-05-13 12:04:22 clib2devs Exp $
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

void __dirent_lock(struct _clib2 *__clib2) {
    if (__clib2->dirent_lock != NULL)
        ObtainSemaphore(__clib2->dirent_lock);
}

void __dirent_unlock(struct _clib2 *__clib2) {
    if (__clib2->dirent_lock != NULL)
        ReleaseSemaphore(__clib2->dirent_lock);
}

CLIB_CONSTRUCTOR(dirent_init) {
    BOOL success = FALSE;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    NewList((struct List *)&__clib2->__directory_list);

    __clib2->dirent_lock = __create_semaphore();
    if (__clib2->dirent_lock == NULL)
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
    struct _clib2 *__clib2 = __CLIB2;

    if (__clib2->__directory_list.mlh_Head != NULL) {
        while (NOT IsMinListEmpty(&__clib2->__directory_list))
        closedir((DIR *) __clib2->__directory_list.mlh_Head);
    }

    __delete_semaphore(__clib2->dirent_lock);
    __clib2->dirent_lock = NULL;

    LEAVE();
}

int
closedir(DIR *directory_pointer) {
    struct DirectoryHandle *dh;
    int result = ERROR;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWPOINTER(directory_pointer);

    __check_abort();

    __dirent_lock(__clib2);

    if (directory_pointer == NULL) {
        __set_errno(EBADF);
        goto out;
    }

#ifndef NDEBUG
    {
        BOOL directory_pointer_is_valid = FALSE;

        for (dh = (struct DirectoryHandle *) __clib2->__directory_list.mlh_Head;
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

    if (__clib2->__unix_path_semantics) {
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

    __dirent_unlock(__clib2);

    RETURN(result);
    return result;
}
