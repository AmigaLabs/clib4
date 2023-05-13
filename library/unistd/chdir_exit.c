/*
 * $Id: unistd_chdir_exit.c,v 1.7 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

CLIB_DESTRUCTOR(__chdir_exit) {
    ENTER();
    struct _clib2 *__clib2 = __CLIB2;

    if (__clib2->__current_directory_changed) {
        BPTR old_dir;

        old_dir = CurrentDir(__clib2->__original_current_directory);
        __clib2->__original_current_directory = ZERO;

        if (__clib2->__unlock_current_directory) {
            UnLock(old_dir);

            __clib2->__unlock_current_directory = FALSE;
        }

        __clib2->__current_directory_changed = FALSE;
    }

    LEAVE();
}
