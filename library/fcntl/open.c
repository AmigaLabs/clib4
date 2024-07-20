/*
 * $Id: fcntl_open.c,v 1.19 2021-01-31 12:04:22 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
open(const char *path_name, int open_flag, ... /* mode_t mode */) {
    DECLARE_UTILITYBASE();
    struct name_translation_info path_name_nti;
    struct ExamineData *fib = NULL;
    struct SignalSemaphore *fd_lock;
    LONG is_file_system = FALSE;
    LONG open_mode;
    BPTR lock = BZERO, dir_lock = BZERO;
    BPTR handle = BZERO;
    BOOL create_new_file = FALSE;
    LONG is_interactive;
    int fd_slot_number;
    struct fd *fd;
    int access_mode;
    int result = ERROR;
    int i;
    BOOL is_directory = FALSE;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path_name);
    SHOWVALUE(open_flag);

    assert(path_name != NULL);
    assert(UtilityBase != NULL);

    __check_abort_f(__clib4);

    __stdio_lock(__clib4);

    if (path_name == NULL) {
        SHOWMSG("path name is invalid");

        __set_errno(EFAULT);
        goto out;
    }

    access_mode = (open_flag & 3);
    if (access_mode < O_RDONLY && access_mode > O_RDWR) {
        SHOWMSG("access mode is invalid");

        __set_errno(EINVAL);
        goto out;
    }

    if (FLAG_IS_SET(open_flag, O_DIRECTORY)) {
        lock = Lock((STRPTR) path_name, SHARED_LOCK);
        if (lock != BZERO) {
            fib = ExamineObjectTags(EX_LockInput, lock, TAG_DONE);
            if (fib == NULL) {
                SHOWMSG("could not examine the object");

                __set_errno(__translate_io_error_to_errno(IoErr()));
                goto out;
            }

            /* We can open only directories. */
            if (!EXD_IS_DIRECTORY(fib)) {
                SHOWMSG("we need a directory");

                __set_errno(ENOTDIR);
                goto out;
            }

            FreeDosObject(DOS_EXAMINEDATA, fib);
        }
    }

    fd_slot_number = __find_vacant_fd_entry(__clib4);
    if (fd_slot_number < 0) {
        if (__grow_fd_table(__clib4, 0) < 0) {
            SHOWMSG("couldn't find a vacant file descriptor, and couldn't allocate one either");
            goto out;
        }

        fd_slot_number = __find_vacant_fd_entry(__clib4);
        assert(fd_slot_number >= 0);
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

    if (Strnicmp(path_name, "PIPE:", 5) == SAME && FLAG_IS_SET(open_flag, O_CREAT)) {
        open_mode = MODE_NEWFILE;
    } else if (Strnicmp(path_name, "NIL:", 4) != SAME && (
            FLAG_IS_SET(open_flag, O_CREAT) ||
            FLAG_IS_SET(open_flag, O_WRONLY) ||
            FLAG_IS_SET(open_flag, O_RDWR)
    )) {
        if (FLAG_IS_SET(open_flag, O_EXCL)) {
            LONG error;

            SHOWMSG("checking if the file to create already exists");

            lock = Lock((STRPTR) path_name, SHARED_LOCK);
            if (lock != BZERO) {
                SHOWMSG("the file already exists");

                __set_errno(EEXIST);
                goto out;
            }

            error = IoErr();

            if (error == ERROR_OBJECT_WRONG_TYPE) {
                SHOWMSG("there's something not a directory on the path");

                __set_errno(ENOTDIR);
                goto out;
            } else if (error != ERROR_OBJECT_NOT_FOUND && error != ERROR_ACTION_NOT_KNOWN) {
                SHOWMSG("error accessing the object");

                __set_errno(__translate_io_error_to_errno(IoErr()));
                goto out;
            }

            SHOWMSG("the object does not already exist");
        }

        open_mode = MODE_READWRITE;

        if (FLAG_IS_SET(open_flag, O_TRUNC)) {
            SHOWMSG("checking if the file to create already exists");

            lock = Lock((STRPTR) path_name, SHARED_LOCK);
            if (lock != BZERO) {
                SHOWMSG("File already exists");
                fib = ExamineObjectTags(EX_LockInput, lock, TAG_DONE);
                if (fib == NULL) {
                    SHOWMSG("could not examine the object");

                    __set_errno(__translate_io_error_to_errno(IoErr()));
                    goto out;
                }

                /* We can open only files, but never directories. */
                if (EXD_IS_DIRECTORY(fib)) {
                    SHOWMSG("can't open a directory");

                    __set_errno(EISDIR);
                    goto out;
                }

                if (FLAG_IS_SET(fib->Protection, EXDF_NO_WRITE) ||
                    FLAG_IS_SET(fib->Protection, EXDF_NO_DELETE)) {
                    SHOWMSG("this object is not write enabled");

                    __set_errno(EACCES);
                    goto out;
                }

                open_mode = MODE_NEWFILE;

                UnLock(lock);
                lock = BZERO;
            } else {
                LONG error;

                error = IoErr();

                if (error == ERROR_OBJECT_WRONG_TYPE) {
                    SHOWMSG("there's something not a directory on the path");

                    __set_errno(ENOTDIR);
                    goto out;
                } else if (error != ERROR_OBJECT_NOT_FOUND && error != ERROR_ACTION_NOT_KNOWN) {
                    SHOWMSG("error accessing the object");

                    __set_errno(__translate_io_error_to_errno(IoErr()));
                    goto out;
                }
            }
        }

        create_new_file = TRUE;
    }
    else {
        dir_lock = Lock((STRPTR) path_name, SHARED_LOCK);
        if (dir_lock != BZERO) {
            fib = ExamineObjectTags(EX_LockInput, dir_lock, TAG_DONE);
            if (fib == NULL) {
                SHOWMSG("could not examine the object");

                __set_errno(__translate_io_error_to_errno(IoErr()));
                goto out;
            }

            if (EXD_IS_DIRECTORY(fib)) {
                is_directory = TRUE;

                goto directory;
            }

            UnLock(dir_lock);
            dir_lock = BZERO;
        }
        open_mode = MODE_OLDFILE;
    }

    SHOWSTRING(path_name);

    if (!FLAG_IS_SET(open_flag, O_PATH)) {
        handle = Open((STRPTR) path_name, open_mode);
        if (handle == BZERO) {
            LONG io_err = IoErr();

            D(("the file '%s' didn't open in mode %ld", path_name, open_mode));
            __set_errno(__translate_access_io_error_to_errno(io_err));

            /* Check if ended up trying to open a directory as if it were a plain file. */
            if (io_err == ERROR_OBJECT_WRONG_TYPE) {
                lock = Lock((STRPTR) path_name, SHARED_LOCK);
                if (lock != BZERO) {
                    fib = ExamineObjectTags(EX_LockInput, lock, TAG_DONE);
                    if (fib != NULL && !EXD_IS_DIRECTORY(fib)) {
                        __set_errno(EISDIR);
                    }
                }
            }

            goto out;
        }
    }

directory:

    fd_lock = __create_semaphore();
    if (fd_lock == NULL) {
        __set_errno(ENOMEM);
        goto out;
    }

    fd = __clib4->__fd[fd_slot_number];

    if (is_directory || FLAG_IS_SET(open_flag, O_PATH))
        __initialize_fd(fd, __fd_hook_entry, dir_lock, 0, fd_lock); // TODO - Create a new dir hook
    else
        __initialize_fd(fd, __fd_hook_entry, handle, 0, fd_lock);

    fd->fd_Aux = (char *) path_name;

    /* If O_PATH is set only stat* functions can be used */
    if (FLAG_IS_SET(open_flag, O_PATH))
        SET_FLAG(fd->fd_Flags, FDF_PATH_ONLY);

    if (is_directory) {
        /* Set FD flag as Directory */
        SET_FLAG(fd->fd_Flags, FDF_IS_DIRECTORY);
    }
    /* Don't execute file stuff if the path is a directory */
    else {
        /* Figure out if this stream is attached to a console. */
        is_interactive = IsInteractive(handle);
        if (is_interactive) {
            SET_FLAG(fd->fd_Flags, FDF_IS_INTERACTIVE);

            if (FLAG_IS_SET(open_flag, O_NONBLOCK)) {
                SHOWMSG("enabling non-blocking mode");

                if (SetMode(handle, DOSTRUE)) /* single character mode */
                    SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
            }
        } else {
            size_t len;

            len = 0;

            for (i = 0; path_name[i] != '\0'; i++) {
                if (path_name[i] == ':') {
                    len = i + 1;
                    break;
                }
            }

            if (len > 0) {
                char *path_name_copy;

                path_name_copy = __malloc_r(__clib4, len + 1);
                if (path_name_copy != NULL) {
                    memmove(path_name_copy, path_name, len);
                    path_name_copy[len] = '\0';

                    is_file_system = IsFileSystem(path_name_copy);
                    __free_r(__clib4, path_name_copy);
                }
            } else {
                is_file_system = IsFileSystem("");
            }

            if (is_file_system) {
                /* We opened the file in exclusive access mode. Switch it back
                   into shared access mode so that its contents can be read
                   while it's still open. */
                if (open_mode == MODE_NEWFILE)
                    ChangeMode(CHANGE_FH, handle, SHARED_LOCK);

                /* We should be able to seek in this file. */
                SET_FLAG(fd->fd_Flags, FDF_CACHE_POSITION);
            }
        }

        if (FLAG_IS_SET(open_flag, O_APPEND)) {
            SHOWMSG("appending; seeking to end of file");

            ChangeFilePosition(handle, 0, OFFSET_END);

            SET_FLAG(fd->fd_Flags, FDF_APPEND);
        }

        switch (access_mode) {
            case O_RDONLY:

                SET_FLAG(fd->fd_Flags, FDF_READ);
                break;

            case O_WRONLY:

                SET_FLAG(fd->fd_Flags, FDF_WRITE);
                break;

            case O_RDWR:

                SET_FLAG(fd->fd_Flags, FDF_READ);
                SET_FLAG(fd->fd_Flags, FDF_WRITE);
                break;
        }

        if (create_new_file && is_file_system)
            SET_FLAG(fd->fd_Flags, FDF_CREATED);
    }

    SET_FLAG(fd->fd_Flags, FDF_IN_USE);

    result = fd_slot_number;
    handle = BZERO;

    assert(result != ERROR);

out:
    if (handle != BZERO)
        Close(handle);

    FreeDosObject(DOS_EXAMINEDATA, fib);
    if (lock != BZERO)
        UnLock(lock);

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
