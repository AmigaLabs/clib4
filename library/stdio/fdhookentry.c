/*
 * $Id: stdio_fdhookentry.c,v 1.37 2022-08-11 17:12:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <strings.h>
#include <limits.h>

int64_t __fd_hook_entry(struct _clib2 *__clib2, struct fd *fd, struct file_action_message *fam) {
    struct ExamineData *exd = NULL;
    BOOL fib_is_valid = FALSE;
    struct FileHandle *fh;
    int64_t current_position = 0;
    int64_t new_position = 0;
    int64_t file_size = 0;
    int new_mode;
    char *buffer = NULL;
    int64_t result = EOF;
    BOOL is_aliased;
    BPTR file;

    ENTER();

    assert(fam != NULL && fd != NULL);
    assert(__is_valid_fd(__clib2, fd));

    /* Careful: file_action_close has to monkey with the file descriptor
                table and therefore needs to obtain the stdio lock before
                it locks this particular descriptor entry. */
    if (fam->fam_Action == file_action_close)
        __stdio_lock(__clib2);

    __fd_lock(fd);

    file = __resolve_fd_file(fd);
    if (file == ZERO) {
        SHOWMSG("file is closed");

        fam->fam_Error = EBADF;
        goto out;
    }

    switch (fam->fam_Action) {
        case file_action_read:

            SHOWMSG("file_action_read");

            assert(fam->fam_Data != NULL);
            assert(fam->fam_Size > 0);

            if (!FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY) && !FLAG_IS_SET(fd->fd_Flags, FDF_PATH_ONLY)) {
                D(("read %ld bytes from position %ld to 0x%08lx", fam->fam_Size, GetFilePosition(file), fam->fam_Data));

                if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIN_READ_MARK)) {
                    CLEAR_FLAG(fd->fd_Flags, FDF_STDIN_READ_MARK);
                    fam->fam_Error = EAGAIN;
                    result = EOF;
                    goto out;
                }

                result = (int64_t) Read(file, fam->fam_Data, fam->fam_Size);
                if (result == EOF) {
                    D(("read failed ioerr=%ld\n", IoErr()));

                    fam->fam_Error = __translate_io_error_to_errno(IoErr());
                    goto out;
                }

                fd->fd_Position += result;
                /* If we are reading char by char from STDIN used like a SOCKET and
                 * we get a CR we need to mark it as EOF for the next read call
                 */

                if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIN_AS_SOCKET) && result > 0) {
                    if (fam->fam_Data[result - 1] == 13 || fam->fam_Data[result - 1] == 10) {
                        SET_FLAG(fd->fd_Flags, FDF_STDIN_READ_MARK);
                    }
                }
            } else {
                fam->fam_Error = EBADF;
            }

            break;

        case file_action_write:

            SHOWMSG("file_action_write");

            assert(fam->fam_Data != NULL);
            assert(fam->fam_Size > 0);

            if (!FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY) && !FLAG_IS_SET(fd->fd_Flags, FDF_PATH_ONLY)) {
                if (FLAG_IS_SET(fd->fd_Flags, FDF_APPEND)) {
                    SHOWMSG("appending data");

                    /* Make sure that if we get a value of -1 out of Seek() to check whether this was an error or a numeric overflow. */
                    int64_t position = ChangeFilePosition(file, 0, OFFSET_END);
                    if (position != CHANGE_FILE_ERROR) {
                        if (FLAG_IS_SET(fd->fd_Flags, FDF_CACHE_POSITION)) {
                            fd->fd_Position = GetFilePosition(file);
                            if (fd->fd_Position == GETPOSITION_ERROR) {
                                D(("seek to end of file failed; ioerr=%ld", IoErr()));

                                fam->fam_Error = __translate_io_error_to_errno(IoErr());
                                goto out;
                            }
                        }
                    }
                    else {
                        D(("seek to end of file failed; ioerr=%ld", IoErr()));

                        fam->fam_Error = __translate_io_error_to_errno(IoErr());
                        goto out;
                    }
                }

                D(("write %ld bytes to position %ld from 0x%08lx", fam->fam_Size, GetFilePosition(file), fam->fam_Data));

                result = Write(file, fam->fam_Data, fam->fam_Size);
                if (result == -1) {
                    D(("write failed ioerr=%ld", IoErr()));

                    fam->fam_Error = __translate_io_error_to_errno(IoErr());
                    goto out;
                }

                fd->fd_Position += (int64_t) result;
            } else
                fam->fam_Error = EBADF;

            break;

        case file_action_close:

            SHOWMSG("file_action_close");
            /* The following is almost guaranteed not to fail. */
            result = OK;

            if (!FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY) && !FLAG_IS_SET(fd->fd_Flags, FDF_PATH_ONLY)) {
                /* If this is an alias, just remove it. */
                is_aliased = __fd_is_aliased(fd);
                if (is_aliased) {
                    __remove_fd_alias(fd);
                } else if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_STDIO)) {
                    /* Should we reset this file into line buffered mode? */
                    if (FLAG_IS_SET(fd->fd_Flags, FDF_NON_BLOCKING) && FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE)) {
                        /* Set canonical mode. */
                        if (fam->fam_DOSMode == DOSTRUE) {
                            SetMode(fd->fd_File, DOSFALSE);
                            fam->fam_DOSMode = DOSFALSE;
                        }
                    }

                    /* Are we allowed to close this file? */
                    if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_NO_CLOSE)) {
                        BOOL name_and_path_valid = FALSE;
                        struct ExamineData *fib = NULL;
                        BPTR parent_dir;

                        /* Call a cleanup function, such as the one which releases locked records. */
                        if (fd->fd_Cleanup != NULL)
                            (*fd->fd_Cleanup)(fd);

                        parent_dir = __safe_parent_of_file_handle(fd->fd_File);
                        if (parent_dir == ZERO) {
                            SHOWMSG("couldn't find parent directory");

                            __set_errno(__translate_io_error_to_errno(IoErr()));
                            goto out;
                        }

                        fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
                        if (fib != NULL) {
                            name_and_path_valid = TRUE;
                        }

                        if (CANNOT Close(fd->fd_File))
                        {
                            fam->fam_Error = __translate_io_error_to_errno(IoErr());

                            result = EOF;
                        }

                        if (fd->fd_File)
                            fd->fd_File = ZERO;

                        if (__clib2->__unix_path_semantics) {
                            DECLARE_UTILITYBASE();

                            assert(UtilityBase != NULL);

                            /* Now that we have closed this file, know where it is and what its
                                 * name would be, check if we tried to unlink it earlier. If we did,
                                 * we'll try to finish the job here and now.
                                 */
                            if (name_and_path_valid) {
                                struct UnlinkNode *node;
                                struct UnlinkNode *uln_next;
                                struct UnlinkNode *uln;
                                BOOL file_deleted = FALSE;

                                assert(__clib2->__unlink_list.mlh_Head != NULL);

                                /* Check all files to be unlinked when this program exits. */
                                for (uln = (struct UnlinkNode *) __clib2->__unlink_list.mlh_Head;
                                     (uln_next = (struct UnlinkNode *) uln->uln_MinNode.mln_Succ) != NULL;
                                     uln = uln_next) {
                                    node = NULL;

                                    /* If the file name matches, check if the path matches, too. */
                                    if (Stricmp(FilePart(uln->uln_Name), fib->Name) == SAME) {
                                        BPTR old_dir;
                                        BPTR node_lock;
                                        BPTR path_lock = ZERO;

                                        /* Try to get a lock on the file first, then move on to
                                             * the directory it is stored in.
                                             */
                                        old_dir = CurrentDir(uln->uln_Lock);

                                        node_lock = Lock(uln->uln_Name, SHARED_LOCK);
                                        if (node_lock != ZERO) {
                                            path_lock = ParentDir(node_lock);

                                            UnLock(node_lock);
                                        }

                                        CurrentDir(old_dir);

                                        /* If we found the file's parent directory, check if it matches
                                             * the parent directory of the file we just closed.
                                             */
                                        if (path_lock != ZERO) {
                                            if (SameLock(path_lock, parent_dir) == LOCK_SAME)
                                                node = uln;

                                            UnLock(path_lock);
                                        }
                                    }

                                    /* If we found that this file was set up for deletion,
                                         * delete it here and now.
                                         */
                                    if (node != NULL) {
                                        if (NOT file_deleted) {
                                            BPTR old_dir;
                                            old_dir = CurrentDir(parent_dir);

                                            if (DeleteFile(fib->Name)) {
                                                file_deleted = TRUE;
                                                name_and_path_valid = FALSE;
                                            }

                                            CurrentDir(old_dir);
                                        }

                                        if (file_deleted) {
                                            Remove((struct Node *) node);
                                            free(node);
                                        }
                                    }
                                }
                            }
                        }

                        if (FLAG_IS_SET(fd->fd_Flags, FDF_CREATED) && name_and_path_valid) {
                            BPTR old_dir;
                            old_dir = CurrentDir(parent_dir);
                            SetProtection(fib->Name, 0);
                            CurrentDir(old_dir);
                        }

                        if (fib != NULL)
                            FreeDosObject(DOS_EXAMINEDATA, fib);

                        UnLock(parent_dir);
                    }
                } else {
                    // Clear FDF_STDIN_AS_SOCKET just in case it was used as socket */
                    if (FLAG_IS_SET(fd->fd_Flags, FDF_STDIN_AS_SOCKET)) {
                        CLEAR_FLAG(fd->fd_Flags, FDF_STDIN_AS_SOCKET);
                    }
                }
            } else {
                is_aliased = FALSE;

                if (fd->fd_DefaultFile != ZERO) {
                    UnLock(fd->fd_DefaultFile);
                    fd->fd_DefaultFile = ZERO;
                }
            }

            __fd_unlock(fd);

            /* Free the lock semaphore now. */
            if (NOT is_aliased)
                __delete_semaphore(fd->fd_Lock);

            /* And that's the last for this file descriptor. */
            memset(fd, 0, sizeof(*fd));
            fd = NULL;

            break;

        case file_action_seek:
            SHOWMSG("file_action_seek");
            if (!FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY) && !FLAG_IS_SET(fd->fd_Flags, FDF_PATH_ONLY)) {
                /* Reset error to OK */
                fam->fam_Error = OK;

                if (fam->fam_Mode == SEEK_CUR)
                    new_mode = OFFSET_CURRENT;
                else if (fam->fam_Mode == SEEK_SET)
                    new_mode = OFFSET_BEGINNING;
                else
                    new_mode = OFFSET_END;

                if (FLAG_IS_SET(fd->fd_Flags, FDF_CACHE_POSITION)) {
                    current_position = fd->fd_Position;
                } else {
                    int64_t position = GetFilePosition(file);
                    if (position == GETPOSITION_ERROR || IoErr() != OK) {
                        fam->fam_Error = EBADF;
                        goto out;
                    }

                    current_position = position;
                }
                new_position = current_position;

                switch (new_mode) {
                    case OFFSET_CURRENT:
                        new_position += fam->fam_Offset;
                        break;

                    case OFFSET_BEGINNING:
                        new_position = fam->fam_Offset;
                        break;

                    case OFFSET_END:
                        file_size = GetFileSize(file);
                        if (file_size != GETPOSITION_ERROR) {
                            new_position = file_size + fam->fam_Offset;
                            fib_is_valid = TRUE;
                        }

                        break;
                }
                /* if new_position is < 0. Force it to 0 */
                if (new_position < 0) {
                    ChangeFilePosition(file, 0, OFFSET_BEGINNING);
                    fd->fd_Position = new_position = 0;
                } else if (new_position != current_position) {
                    int64_t position = ChangeFilePosition(file, fam->fam_Offset, new_mode);

                    /* Same as above: verify that what we got out of
                           Seek() is really an error and not a valid
                           file position. */
                    if (position == CHANGE_FILE_ERROR) {
                        fam->fam_Error = __translate_io_error_to_errno(IoErr());

                        if (__clib2->__unix_path_semantics) {
                            /* Check if this operation failed because the file is shorter than
                                   the new file position. First, we need to find out if the file
                                   is really shorter than required. If not, then it must have
                                   been a different error. */
                            exd = ExamineObjectTags(EX_FileHandleInput, file, TAG_DONE);
                            if ((NOT fib_is_valid && exd == NULL) || (exd == NULL) ||
                                                                     (new_position <= (int64_t) exd->FileSize))
                            goto out;

                            /* Don't extend if the file is opened read-only */
                            if (FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE)) {
                                fam->fam_Error = EPERM;
                                goto out;
                            }

                            /* Now try to make that file larger. */
                            if (__grow_file_size(fd, new_position - (int64_t) exd->FileSize) < 0) {
                                fam->fam_Error = __translate_io_error_to_errno(IoErr());
                                FreeDosObject(DOS_EXAMINEDATA, exd);
                                goto out;
                            }
                            fam->fam_Error = OK;
                            FreeDosObject(DOS_EXAMINEDATA, exd);
                        } else {
                            goto out;
                        }
                    } else {
                        new_position = GetFilePosition(file);
                        if (new_position == GETPOSITION_ERROR) {
                            fam->fam_Error = __translate_io_error_to_errno(IoErr());
                        }
                    }

                    fd->fd_Position = new_position;
                }

                result = new_position;
            } else
                fam->fam_Error = EBADF;

            break;

        case file_action_set_blocking:

            SHOWMSG("file_action_set_blocking");

            if (!FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY) && !FLAG_IS_SET(fd->fd_Flags, FDF_PATH_ONLY)) {
                if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE)) {
                    LONG mode;

                    SHOWMSG("changing the mode");

                    if (fam->fam_Arg != 0)
                        mode = DOSFALSE; /* buffered mode */
                    else
                        mode = DOSTRUE; /* single character mode */

                    if (CANNOT SetMode(file, mode)) {
                        fam->fam_Error = __translate_io_error_to_errno(IoErr());
                        goto out;
                    }

                    result = OK;
                } else {
                    SHOWMSG("can't do anything here");

                    fam->fam_Error = EBADF;
                }
            } else
                fam->fam_Error = EBADF;

            break;

        case file_action_examine:

            SHOWMSG("file_action_examine");

            fh = BADDR(file);

            /* Special treatment for "NIL:", for which we make some stuff up. */
            if (fh->fh_Type == NULL) {
                /* Make up some stuff for this stream. */
                memset(fam->fam_FileInfo, 0, sizeof(*fam->fam_FileInfo));

                DateStamp(&fam->fam_FileInfo->Date);

                // TODO - Check this on OS4 with NIL:
                fam->fam_FileInfo->Type = ST_NIL;
            } else {
                BPTR lock_type = FLAG_IS_SET(fd->fd_Flags, FDF_IS_DIRECTORY) || FLAG_IS_SET(fd->fd_Flags, FDF_PATH_ONLY)
                                 ? EX_LockInput
                                 : EX_FileHandleInput;
                fam->fam_FileInfo = ExamineObjectTags(lock_type, file, TAG_DONE);
                if (fam->fam_FileInfo == NULL) {
                    LONG error;

                    /* So that didn't work. Did the file system simply fail to
                        respond to the request or is something more sinister
                        at work? */
                    error = IoErr();
                    if (error != ERROR_ACTION_NOT_KNOWN) {
                        SHOWMSG("couldn't examine the file");

                        fam->fam_Error = __translate_io_error_to_errno(error);
                        goto out;
                    }

                    /* OK, let's have another look at this file. Could it be a console stream? */
                    if (NOT IsInteractive(file)) {
                        SHOWMSG("whatever it is, we don't know");

                        fam->fam_Error = ENOSYS;
                        goto out;
                    }

                    /* Create an empty examineData struct */
                    struct ExamineData *examineData = malloc(sizeof(struct ExamineData));
                    fam->fam_FileInfo = examineData;

                    /* Make up some stuff for this stream. */
                    memset(fam->fam_FileInfo, 0, sizeof(*fam->fam_FileInfo));

                    DateStamp(&fam->fam_FileInfo->Date);

                    // TODO - Check this on OS4 with CON:
                    fam->fam_FileInfo->Type = ST_CONSOLE;
                }
            }
            fam->fam_FileSystem = fh->fh_Type;
            result = OK;
            break;

        default:
            SHOWVALUE(fam->fam_Action);

            fam->fam_Error = EBADF;
            break;
    }

out:
    __fd_unlock(fd);

    if (fam->fam_Action == file_action_close)
        __stdio_unlock(__clib2);

    if (buffer != NULL)
        free(buffer);

    SHOWVALUE(result);

    __check_abort();

    RETURN(result);
    return (result);
}
