/*
 * $Id: termios_console_fdhookentry.c,v 1.7 2021-01-31 13:12:59 clib4devs Exp $
 *
 * Hook for termios emulation on a console. This can probably be cleaned up a bit
 * by removing things which will (should) never happen on a console.
 */

#ifndef _TERMIOS_HEADERS_H
#include "termios_headers.h"
#endif /* _TERMIOS_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <strings.h>
#include <limits.h>

/*
 * Emulate canonical no-echo mode with a simple line-editor in raw mode.
 */
static int64_t
LineEditor(BPTR file, char *buf, const int buflen, struct termios *tios) {
    int64_t pos = 0, len = 0;
    unsigned char z;
    int do_edit = 1;
    int shift_mode = 0;
    int esc_mode = 0;

    SetMode(file, DOSTRUE); /* Set raw mode. */

    while (do_edit && len < buflen) {
        /* 5 seconds. */
        if (WaitForChar(file, 5000000) != DOSFALSE) {
            if (Read(file, &z, 1) == ERROR) {
                len = -1;
                break;
            }

            if (z == tios->c_cc[VQUIT])
                break;

            switch (z) {
                case '\n': /* NL */
                case '\r': /* CR */

                    do_edit = 0;

                    buf[len++] = '\n';
                    continue;

                case 27: /* ESC */
                    esc_mode = 1;
                    continue;

                case 155: /* CSI */
                    shift_mode = 1;
                    continue;

                case '\b': /* Backspace */

                    if (pos > 0) {
                        memmove(&buf[pos - 1], &buf[pos], len - pos);
                        pos--;
                        len--;
                    }

                    continue;

                case 127: /* Delete */

                    if (pos < len) {
                        memmove(&buf[pos], &buf[pos + 1], len - pos - 1);
                        len--;
                    }

                    continue;
            }


            if (shift_mode || esc_mode) {
                shift_mode = 0;
                esc_mode = 0;

                switch (z) {
                    case 'C': /* Right arrowkey */

                        if (pos < len)
                            pos++;

                        continue;

                    case 'D': /* Left arrowkey */

                        if (pos > 0)
                            pos--;

                        continue;

                    case 'A': /* Up arrowkey */

                        if (pos < len)
                            pos++;

                        continue;

                    case 'B': /* Down arrowkey */

                        if (pos > 0)
                            pos--;

                        continue;

                }
            }

            if (pos != len)
                memmove(&buf[pos + 1], &buf[pos], len - pos);

            buf[pos] = z;
            pos++;
            len++;
        }
    }

    if (len >= 0 && len < buflen) /* Does not hurt to null-terminate if we can. */
        buf[len] = '\0';

    SetMode(file, DOSFALSE); /* Restore mode */

    return (len); /* Number of characters read. */
}

int64_t
__termios_console_hook(struct _clib4 *__clib4, struct fd *fd, struct file_action_message *fam) {
    const unsigned char CR = '\r', NL = '\n';
    struct FileHandle *fh;
    char *buffer = NULL;
    int64_t result = EOF;
    int actual_out;
    BOOL is_aliased;
    BPTR file;
    struct termios *tios;

    ENTER();

    assert(fam != NULL && fd != NULL);
    assert(__is_valid_fd(__clib4, fd));
    assert(FLAG_IS_SET(fd->fd_Flags, FDF_TERMIOS));
    assert(fd->fd_Aux != NULL);

    tios = (struct termios *) fd->fd_Aux;

    /* Careful: file_action_close has to monkey with the file descriptor
     * table and therefore needs to obtain the stdio lock before
     * it locks this particular descriptor entry. */
    if (fam->fam_Action == file_action_close)
        __stdio_lock(__clib4);

    __fd_lock(fd);

    file = __resolve_fd_file(fd);
    if (file == BZERO) {
        SHOWMSG("file is closed");

        fam->fam_Error = EBADF;
        goto out;
    }

    switch (fam->fam_Action) {
        case file_action_read:
            SHOWMSG("file_action_read");

            if (FLAG_IS_CLEAR(tios->c_cflag, CREAD)) {
                SHOWMSG("Reading is not enabled for this console descriptor.");
                fam->fam_Error = EIO;
                goto out;
            }
            assert(fam->fam_Data != NULL);
            assert(fam->fam_Size > 0);

            /* Attempt to fake everything needed in non-canonical mode. */
            if (FLAG_IS_SET(tios->c_lflag, ICANON)) {
                /* Canonical read = same as usual. Unless... */
                if (FLAG_IS_CLEAR(tios->c_lflag, ECHO)) {
                    /* No-echo mode needs to be emulated. */
                    result = LineEditor(file, fam->fam_Data, fam->fam_Size, tios);
                } else {
                    result = Read(file, fam->fam_Data, fam->fam_Size);
                }
            } else if (fam->fam_Size > 0) {
                /* Non-canonical reads have timeouts and a minimum number of characters to read. */
                int i = 0;
                result = 0;

                if (tios->c_cc[VMIN] > 0 && FLAG_IS_CLEAR(fd->fd_Flags, FDF_NON_BLOCKING)) {
                    /* Reading the first character is not affected by the timeout unless VMIN==0. */
                    i = Read(file, fam->fam_Data, 1);
                    if (i == ERROR) {
                        fam->fam_Error = EIO;
                        goto out;
                    }

                    result = i;

                    while ((result < tios->c_cc[VMIN]) && (result < fam->fam_Size)) {
                        if (tios->c_cc[VTIME] > 0) {
                            if (WaitForChar(file, 100000 * tios->c_cc[VTIME]) == DOSFALSE) {
                                break; /* No more characters available within alloted time. */
                            }
                        }

                        i = Read(file, &fam->fam_Data[result], 1);
                        if (i <= 0) {
                            break; /* Break out of this while loop only. */
                        }

                        result += i;
                    }
                } else {
                    if (WaitForChar(file, 100000 * tios->c_cc[VTIME])) {
                        result = Read(file, fam->fam_Data, fam->fam_Size);
                    }
                }
            } else {
                result = 0; /* Reading zero characters will always succeed. */
            }

            if (result == ERROR) {
                D(("read failed ioerr=%ld", IoErr()));

                fam->fam_Error = __translate_io_error_to_errno(IoErr());
                goto out;
            }

            if (result > 0) {
                /* Input processing enabled. */
                if (tios->c_iflag != 0) {
                    int64_t i, n;
                    int64_t num_bytes = result;
                    unsigned char byte_in;

                    /* XXX The input substitution could possibly be moved to the console handler with an input-map. (?) */
                    for (i = n = 0; i < num_bytes; i++) {
                        byte_in = fam->fam_Data[i];

                        if (FLAG_IS_SET(tios->c_iflag, ISTRIP)) {
                            /* Strip 8:th bit. Done before any other processing. */
                            byte_in &= 0x7f;
                        }

                        if (FLAG_IS_SET(tios->c_iflag, IGNCR) && byte_in == CR) {
                            /* Remove CR */
                            result--;
                            continue;
                        }

                        if (FLAG_IS_SET(tios->c_iflag, ICRNL) && byte_in == CR) /* Map CR->NL */
                            byte_in = NL;

                        if (FLAG_IS_SET(tios->c_iflag, INLCR) && byte_in == NL) /* Map NL->CR */
                            byte_in = CR;

                        if (FLAG_IS_SET(tios->c_lflag, ISIG)) {
                            //Printf("byte_in = %ld\n", byte_in);
                            switch (byte_in) {
                                case 3: //INTR
                                    raise(SIGINT);
                                    break;
                                case 28: //QUIT
                                    raise(SIGQUIT);
                                    break;
                                default:
                                    // no other signals can be raised
                                    break;
                            }
                        }
                        fam->fam_Data[n++] = byte_in;
                    }
                }

                if (FLAG_IS_SET(tios->c_lflag, ECHO) && FLAG_IS_CLEAR(tios->c_lflag, ICANON) &&
                    FLAG_IS_SET(fd->fd_Flags, FDF_WRITE)) {
                    if (Write(file, fam->fam_Data, result) == ERROR) {
                        /* "Silently" disable echoing. */
                        SHOWMSG("Echo failed and has been disabled.");
                        CLEAR_FLAG(tios->c_lflag, ECHO);
                    }
                }
            }

            if (FLAG_IS_SET(fd->fd_Flags, FDF_CACHE_POSITION))
                fd->fd_Position += (ULONG) result;

            break;

        case file_action_write:

            SHOWMSG("file_action_write");

            assert(fam->fam_Data != NULL);
            assert(fam->fam_Size > 0);

            if (FLAG_IS_SET(tios->c_oflag, OPOST)) /* Output processing enabled. */
            {
                unsigned char byte_out;
                int i, n;

                buffer = __malloc_r(__clib4, 2 * fam->fam_Size);
                if (buffer == NULL) {
                    fam->fam_Error = ENOMEM;
                    goto out;
                }

                for (i = n = 0; i < fam->fam_Size; i++) {
                    byte_out = fam->fam_Data[i];

                    if (FLAG_IS_SET(tios->c_oflag, ONLRET) && byte_out == CR)
                        continue;

                    if (FLAG_IS_SET(tios->c_oflag, OCRNL) && byte_out == CR)
                        byte_out = NL;

                    if (FLAG_IS_SET(tios->c_oflag, ONOCR) && byte_out == CR)
                        byte_out = NL;

                    if (FLAG_IS_SET(tios->c_oflag, ONLCR) && byte_out == NL) {
                        buffer[n++] = CR;
                        byte_out = NL;
                    }

                    buffer[n++] = byte_out;
                }

                actual_out = n;
            } else {
                buffer = fam->fam_Data;
                actual_out = fam->fam_Size;
            }

            /* Note. When output processing is enabled, write() can return _more_ than the data length. */
            if (actual_out > 0) {
                result = Write(file, buffer, actual_out);
            } else {
                result = 0;
            }

            if (buffer == fam->fam_Data)
                buffer = NULL; /* Must do this to avoid freeing the user data. */

            if (result == ERROR) {
                D(("write failed ioerr=%ld", IoErr()));

                fam->fam_Error = __translate_io_error_to_errno(IoErr());
                goto out;
            }

            if (FLAG_IS_SET(fd->fd_Flags, FDF_CACHE_POSITION))
                fd->fd_Position += (ULONG) result;

            break;

        case file_action_close:

            SHOWMSG("file_action_close");

            /* The following is almost guaranteed not to fail. */
            result = OK;

            /* If this is an alias, just remove it. */
            is_aliased = __fd_is_aliased(fd);
            if (is_aliased) {
                __remove_fd_alias(__clib4, fd);
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
                    /* Call a cleanup function, such as the one which releases locked records. */
                    if (fd->fd_Cleanup != NULL)
                        (*fd->fd_Cleanup)(__clib4, fd);

                    if (CANNOT Close(fd->fd_File))
                    {
                        fam->fam_Error = __translate_io_error_to_errno(IoErr());

                        result = EOF;
                    }

                    fd->fd_File = BZERO;
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

            fam->fam_Error = EINVAL;
            goto out;

        case file_action_set_blocking:

            SHOWMSG("file_action_set_blocking");

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

                /* Update tios to reflect state change. */
                if (mode == DOSTRUE)
                    CLEAR_FLAG(tios->c_lflag, ICANON);
                else
                    SET_FLAG(tios->c_lflag, ICANON);

                result = OK;
            } else {
                SHOWMSG("can't do anything here");

                fam->fam_Error = EBADF;
            }
            break;

        case file_action_examine:

            SHOWMSG("file_action_examine");

            fh = BADDR(file);

            /* Special treatment for "NIL:", for which we make some stuff up. */
            if (fh->fh_MsgPort == NULL) {
                /* Make up some stuff for this stream. */
                memset(fam->fam_FileInfo, 0, sizeof(*fam->fam_FileInfo));

                DateStamp(&fam->fam_FileInfo->Date);

                fam->fam_FileInfo->Type = ST_NIL;
            } else {
                fam->fam_FileInfo = ExamineObjectTags(EX_FileHandleInput, file, TAG_DONE);
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

                    /* OK, let's have another look at this file. Could it be a
                        console stream? */
                    if (NOT IsInteractive(file)) {
                        SHOWMSG("whatever it is, we don't know");

                        fam->fam_Error = ENOSYS;
                        goto out;
                    }

                    /* Make up some stuff for this stream. */
                    memset(fam->fam_FileInfo, 0, sizeof(*fam->fam_FileInfo));

                    DateStamp(&fam->fam_FileInfo->Date);

                    fam->fam_FileInfo->Type = ST_CONSOLE;
                }
            }

            fam->fam_FileSystem = fh->fh_MsgPort;

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
        __stdio_unlock(__clib4);

    if (buffer != NULL)
        __free_r(__clib4, buffer);

    SHOWVALUE(result);

    RETURN(result);
    return (result);
}
