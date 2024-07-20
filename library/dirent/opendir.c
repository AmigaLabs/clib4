/*
 * $Id: dirent_opendir.c,v 1.11 2021-01-31 12:04:22 clib4devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

static struct Node *
find_by_name(struct List *list, const char *name) {
    struct Node *result = NULL;
    struct Node *node;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ) {
        if (strcasecmp(node->ln_Name, name) == 0) {
            result = node;
            break;
        }
    }

    return (result);
}

DIR *
opendir(const char *path_name) {
    struct name_translation_info path_name_nti;
    struct DirectoryHandle *dh = NULL;
    DIR *result = NULL;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path_name);

    assert(path_name != NULL);

    __check_abort_f(__clib4);

    if (path_name == NULL) {
        SHOWMSG("invalid parameter");

        __set_errno(EFAULT);
        goto out;
    }

    dh = __malloc_r(__clib4, sizeof(*dh));
    if (dh == NULL) {
        SHOWMSG("memory allocation failed");
        goto out;
    }

    memset(dh, 0, sizeof(*dh));

    if (__clib4->__unix_path_semantics) {
        struct Node *node;

        NewList((struct List *) &dh->dh_VolumeList);

        if (path_name[0] == '\0') {
            SHOWMSG("no name given");

            __set_errno(ENOENT);
            goto out;
        }

        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        SHOWSTRING(path_name);

        if (path_name_nti.is_root) {
            struct DosList *dol;
            UBYTE *name;

            SHOWMSG("collecting volume names");

            dh->dh_ScanVolumeList = TRUE;

            dol = LockDosList(LDF_VOLUMES | LDF_READ);
            while ((dol = NextDosEntry(dol, LDF_VOLUMES | LDF_READ)) != NULL) {
                name = BADDR(dol->dol_Name);
                if (name != NULL && name[0] > 0) {
                    size_t len;

                    len = name[0];

                    node = __malloc_r(__clib4, sizeof(*node) + len + 2);
                    if (node == NULL) {
                        UnLockDosList(LDF_VOLUMES | LDF_READ);

                        __set_errno(ENOMEM);
                        goto out;
                    }

                    node->ln_Name = (char *) (node + 1);

                    memmove(node->ln_Name, &name[1], len);
                    node->ln_Name[len++] = ':';
                    node->ln_Name[len] = '\0';

                    /* Check if the name is already on the list. Mind you,
                        this is not the most sophisticated algorithm but then
                        the number of volumes should be small. */
                    if (find_by_name((struct List *) &dh->dh_VolumeList, node->ln_Name) != NULL) {
                        __free_r(__clib4, node);
                        continue;
                    }

                    D(("adding '%s'", node->ln_Name));

                    AddTail((struct List *) &dh->dh_VolumeList, node);
                }
            }

            UnLockDosList(LDF_VOLUMES | LDF_READ);

            /* Bail out if we cannot present anything. */
            if (IsMinListEmpty(&dh->dh_VolumeList)) {
                __set_errno(ENOMEM);
                goto out;
            }
        }
    }

    if (NOT dh->dh_ScanVolumeList) {
        SHOWMSG("we are supposed to scan a directory");
        SHOWSTRING(path_name);

        dh->dh_DirLock = Lock((STRPTR) path_name, SHARED_LOCK);
        if (dh->dh_DirLock == BZERO) {
            SHOWMSG("couldn't get a lock on it");

            __set_errno(__translate_access_io_error_to_errno(IoErr()));
            goto out;
        }

        dh->dh_FileInfo = ExamineObjectTags(EX_LockInput, dh->dh_DirLock, TAG_DONE);
        if (dh->dh_FileInfo == NULL) {
            SHOWMSG("couldn't examine it");

            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto out;
        }

        if (!EXD_IS_DIRECTORY(dh->dh_FileInfo)) {
            SHOWMSG("this isn't a directory");

            __set_errno(ENOTDIR);
            goto out;
        }

        dh->dh_Context = ObtainDirContextTags(EX_FileLockInput, dh->dh_DirLock,
                                              EX_DoCurrentDir, FALSE,
                                              EX_DataFields, EXF_ALL,
                                              TAG_END);
        if (dh->dh_Context == NULL) {
            SHOWMSG("couldn't examine it");

            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto out;
        }
        FreeDosObject(DOS_EXAMINEDATA, dh->dh_FileInfo);
    }

    SHOWMSG("OK, done");

    assert(__clib4->__directory_list.mlh_Head != NULL);

    __dirent_lock(__clib4);

    AddTail((struct List *) &__clib4->__directory_list, (struct Node *) dh);

    __dirent_unlock(__clib4);

    result = (DIR *) dh;
    dh = NULL;

out:

    if (dh != NULL) {
        SHOWMSG("ouch. cleaning up");
        FreeDosObject(DOS_EXAMINEDATA, dh->dh_FileInfo);

        if (__clib4->__unix_path_semantics) {
            struct Node *node;

            while ((node = RemHead((struct List *) &dh->dh_VolumeList)) != NULL)
                __free_r(__clib4, node);
        }

        UnLock(dh->dh_DirLock);

        __free_r(__clib4, dh);
    }

    RETURN(result);
    return (result);
}
