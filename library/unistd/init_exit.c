/*
 * $Id: unistd_init_exit.c,v 1.14 2006-09-27 09:40:06 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

CLIB_CONSTRUCTOR(unistd_init) {
    ENTER();

    NewList((struct List *)&__CLIB2->__unlink_list);
    InitSemaphore(&__CLIB2->__unlink_semaphore);

    LEAVE();

    CONSTRUCTOR_SUCCEED();
}

CLIB_DESTRUCTOR(unistd_exit) {
    ENTER();

    if (__CLIB2->__unlink_list.mlh_Head != NULL && NOT IsMinListEmpty(&__CLIB2->__unlink_list)) {
        struct UnlinkNode *uln;
        BPTR old_dir;

        /* Close all the files that still might be open. */
        __close_all_files();

        while ((uln = (struct UnlinkNode *) RemHead((struct List *) &__CLIB2->__unlink_list))) {
            D(("deleting '%s'", uln->uln_Name));

            old_dir = CurrentDir(uln->uln_Lock);
            DeleteFile(uln->uln_Name);
            CurrentDir(old_dir);

            UnLock(uln->uln_Lock);
        }
    }

    LEAVE();
}
