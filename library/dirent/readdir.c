/*
 * $Id: dirent_readdir.c,v 1.11 2021-01-18 10:32:15 clib2devs Exp $
*/

#ifndef _DIRENT_HEADERS_H
#include "dirent_headers.h"
#endif /* _DIRENT_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

struct dirent *
readdir(DIR *directory_pointer) {
    struct dirent *result = NULL;
    struct DirectoryHandle *dh;
    BPTR parent_directory = ZERO;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    SHOWPOINTER(directory_pointer);

    __check_abort();

    if (directory_pointer == NULL) {
        SHOWMSG("ouch. invalid parameter");

        __set_errno(EBADF);
        goto out;
    }

    dh = (struct DirectoryHandle *) directory_pointer;

    if (__clib2->__unix_path_semantics && dh->dh_ScanVolumeList) {
        SHOWMSG("we are scanning the volume list");

        if (dh->dh_Position == 0) {
            SHOWMSG("returning the .");

            dh->dh_Position++;

            dh->dh_DirectoryEntry.d_ino = 0;
            strcpy(dh->dh_DirectoryEntry.d_name, ".");
            dh->dh_DirectoryEntry.d_reclen = sizeof(struct dirent);
            dh->dh_DirectoryEntry.d_type = DT_DIR;

            result = &dh->dh_DirectoryEntry;
        } else {
            struct ExamineData *fib = NULL;
            D_S(struct bcpl_name, bcpl_name);
            char *name = (char *) bcpl_name->name;
            BPTR dir_lock;

            assert((((ULONG) name) & 3) == 0);

            if (dh->dh_VolumeNode == NULL && NOT IsMinListEmpty(&dh->dh_VolumeList))
            dh->dh_VolumeNode = (struct Node *) dh->dh_VolumeList.mlh_Head;

            strcpy(name, "\1:"); /* BSTR for ":" */

            while (result == NULL && dh->dh_VolumeNode != NULL && dh->dh_VolumeNode->ln_Succ != NULL) {
                if (IsFileSystem(dh->dh_VolumeNode->ln_Name)) {
                    struct DevProc *dvp;

                    dvp = GetDeviceProc(dh->dh_VolumeNode->ln_Name, NULL);
                    if (dvp != NULL) {
                        dir_lock = DoPkt(dvp->dvp_Port, ACTION_LOCATE_OBJECT, ZERO, MKBADDR(name), SHARED_LOCK, 0, 0);
                        if (dir_lock != ZERO) {
                            fib = ExamineObjectTags(EX_LockInput, dir_lock, TAG_DONE);
                            if (fib) {
                                assert(sizeof(dh->dh_DirectoryEntry.d_name) >= sizeof(fib->Name));
                                dh->dh_DirectoryEntry.d_ino = fib->ObjectID;
                                dh->dh_DirectoryEntry.d_reclen = sizeof(struct dirent);
                                dh->dh_DirectoryEntry.d_namlen = strlcpy(dh->dh_DirectoryEntry.d_name, fib->Name,
                                                                         NAME_MAX + 1);
                                dh->dh_DirectoryEntry.d_type = DT_LABEL;

                                result = &dh->dh_DirectoryEntry;

                                FreeDosObject(DOS_EXAMINEDATA, fib);
                            }
                            UnLock(dir_lock);
                        }

                        FreeDeviceProc(dvp);
                    }
                }

                dh->dh_VolumeNode = dh->dh_VolumeNode->ln_Succ;
            }
        }
    }

    if (NOT dh->dh_ScanVolumeList) {
        if (__clib2->__unix_path_semantics) {
            if (dh->dh_Position == 0) {
                SHOWMSG("returning .");

                dh->dh_Position++;

                dh->dh_DirectoryEntry.d_ino = dh->dh_FileInfo->ObjectID;
                strcpy(dh->dh_DirectoryEntry.d_name, ".");
                dh->dh_DirectoryEntry.d_reclen = sizeof(struct dirent);
                dh->dh_DirectoryEntry.d_namlen = 2;
                dh->dh_DirectoryEntry.d_type = DT_DIR;

                result = &dh->dh_DirectoryEntry;
            } else if (dh->dh_Position == 1) {
                ino_t ino = 0;

                dh->dh_Position++;

                parent_directory = ParentDir(dh->dh_DirLock);
                if (parent_directory != ZERO) {
                    struct ExamineData *fib = ExamineObjectTags(EX_LockInput, parent_directory, TAG_DONE);
                    if (fib == NULL) {
                        __set_errno(__translate_io_error_to_errno(IoErr()));
                        goto out;
                    }
                    ino = fib->ObjectID;

                    FreeDosObject(DOS_EXAMINEDATA, fib);
                }

                SHOWMSG("returning ..");

                dh->dh_DirectoryEntry.d_ino = ino;
                dh->dh_DirectoryEntry.d_reclen = sizeof(struct dirent);
                dh->dh_DirectoryEntry.d_namlen = 3;
                dh->dh_DirectoryEntry.d_type = DT_DIR;

                strcpy(dh->dh_DirectoryEntry.d_name, "..");

                result = &dh->dh_DirectoryEntry;
            }
        }

        if (result == NULL) {
            if (dh->dh_Context) {
                dh->dh_FileInfo = ExamineDir(dh->dh_Context);
                if (dh->dh_FileInfo != NULL) {
                    dh->dh_DirectoryEntry.d_ino = dh->dh_FileInfo->ObjectID;
                    dh->dh_DirectoryEntry.d_namlen = strlcpy(dh->dh_DirectoryEntry.d_name, dh->dh_FileInfo->Name,
                                                             NAME_MAX + 1);
                    dh->dh_DirectoryEntry.d_reclen = sizeof(struct dirent);
                    if (EXD_IS_SOFTLINK(dh->dh_FileInfo))
                        dh->dh_DirectoryEntry.d_type = DT_LNK;
                    else if (EXD_IS_FILE(dh->dh_FileInfo))
                        dh->dh_DirectoryEntry.d_type = DT_REG;
                    else if (EXD_IS_PIPE(dh->dh_FileInfo))
                        dh->dh_DirectoryEntry.d_type = DT_FIFO;
                    else if (EXD_IS_SOCKET(dh->dh_FileInfo))
                        dh->dh_DirectoryEntry.d_type = DT_SOCK;
                    else if (EXD_IS_DIRECTORY(dh->dh_FileInfo)) {
                        dh->dh_DirectoryEntry.d_type = DT_DIR;
                    } else {
                        dh->dh_DirectoryEntry.d_type = DT_UNKNOWN;
                    }

                    result = &dh->dh_DirectoryEntry;
                    /* Don't free dh_FileInfo. ReleaseDirContext will do all the work */
                } else {
                    if (IoErr() != ERROR_NO_MORE_ENTRIES) {
                        SHOWMSG("error scanning directory");

                        __set_errno(__translate_io_error_to_errno(IoErr()));
                        goto out;
                    }

                    SHOWMSG("that was the end of the line");
                }
            } else {
                __set_errno(__translate_io_error_to_errno(IoErr()));
                goto out;
            }
        }
    }

    if (result != NULL)
        D(("next entry to return is '%s'", result->d_name));
    else
        SHOWMSG("end of directory reached");

out:

    UnLock(parent_directory);

    RETURN(result);
    return (result);
}
