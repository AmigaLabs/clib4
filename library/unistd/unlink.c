/*
 * $Id: unistd_unlink.c,v 1.11 2006-09-27 09:40:06 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
unlink(const char *path_name) {
    DECLARE_UTILITYBASE();
    struct name_translation_info path_name_nti;
    BPTR current_dir = BZERO;
    int result = ERROR;
    LONG status;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path_name);

    assert(path_name != NULL);

    __check_abort_f(__clib4);

    if (path_name == NULL) {
        SHOWMSG("invalid path name");

        __set_errno(EFAULT);
        goto out;
    }

    if (__clib4->__unix_path_semantics) {
        if (path_name[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno(ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno(EACCES);
            goto out;
        }
    }

    D(("trying to delete '%s'", path_name));

    status = Delete((STRPTR) path_name);
    if (status == DOSFALSE) {
        if (__clib4->__unix_path_semantics) {
            struct UnlinkNode *uln = NULL;
            struct UnlinkNode *node;
            BOOL found = FALSE;

            assert(UtilityBase != NULL);

            if (NOT __clib4->__unlink_retries || IoErr() != ERROR_OBJECT_IN_USE) {
                __set_errno(__translate_access_io_error_to_errno(IoErr()));
                goto out;
            }

            SHOWMSG("will try to delete that file later");

            /* ZZZ we should be locking the path's parent directory
               and then just remember what the last part of the path
               pointed to. */
            current_dir = Lock("", SHARED_LOCK);
            if (current_dir == BZERO) {
                __set_errno(__translate_io_error_to_errno(IoErr()));
                goto out;
            }

            ObtainSemaphore(&__clib4->__unlink_semaphore);

            assert(__clib4->__unlink_list.mlh_Head != NULL);

            for (node = (struct UnlinkNode *) __clib4->__unlink_list.mlh_Head;
                 node->uln_MinNode.mln_Succ != NULL;
                 node = (struct UnlinkNode *) node->uln_MinNode.mln_Succ) {
                if (Stricmp(node->uln_Name, path_name) == SAME && SameLock(node->uln_Lock, current_dir) == LOCK_SAME) {
                    found = TRUE;
                    break;
                }
            }

            if (NOT found) {
                uln = __malloc_r(__clib4, sizeof(*uln) + strlen(path_name) + 1);
                if (uln != NULL) {
                    uln->uln_Lock = current_dir;
                    uln->uln_Name = (char *) (uln + 1);

                    strcpy(uln->uln_Name, path_name);
                    AddTail((struct List *) &__clib4->__unlink_list, (struct Node *) uln);

                    current_dir = BZERO;
                }
            }

            ReleaseSemaphore(&__clib4->__unlink_semaphore);

            if (NOT found && uln == NULL)
            {
                __set_errno(ENOMEM);
                goto out;
            }
        } else {
            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto out;
        }
    }

    result = OK;

out:

    UnLock(current_dir);

    RETURN(result);
    return (result);
}
