/*
 * $Id: socket_ioctl.c,v 1.14 2006-11-16 10:41:15 clib4devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _STDARG_H
#include <stdarg.h>
#endif /* _STDARG_H */

#include <sys/ioctl.h>
#include <termios.h>

#define BUFFER_SIZE 30

static BOOL readSize(uint32 *rows, uint32 *columns) {
    BPTR fh = Open("CONSOLE:", MODE_OLDFILE);
    BOOL success = FALSE;
    if (fh) {
        uint32 width = 0, height = 0;
        char r;
        char buffer[BUFFER_SIZE + 1] = {0};

        SetMode(fh, 1); // RAW mode
        if (Write(fh, "\x9b q", 3) == 3) {
            LONG actual = 0;
            LONG ret = Read(fh, &r, 1);
            while (r != 'r' && ret != 0 && actual < BUFFER_SIZE) {
                buffer[actual] = r;
                actual += ret;
                ret = Read(fh, &r, 1);
            }
            if (actual >= 0 && actual < BUFFER_SIZE) {
                buffer[actual] = '\0';
                if (sscanf(buffer, "\x9b"
                                   "1;1;%ld;%ld r", &height, &width) == 2) {
                    success = TRUE;
                }
            }
        }
        SetMode(fh, 0); // Normal mode
        Close(fh);

        if (success) {
            *rows = height;
            *columns = width;
        }
    }

    return success;
}

static BOOL writeSize(uint32 rows, uint32 columns) {
    BPTR fh = Open("CONSOLE:", MODE_OLDFILE);
    BOOL success = FALSE;

    if (fh) {
        char buffer[51] = {0};

        SetMode(fh, 1); // RAW mode
        snprintf(buffer, 50, "\x9b%dt\x9b%du", rows, columns);
        int bufferLen = strlen(buffer);
        int bytesWritten = Write(fh, buffer, bufferLen);
        if (bytesWritten == bufferLen) {
            success = TRUE;
        }
        SetMode(fh, 0); // Normal mode
        Close(fh);
    }

    return success;
}

int
ioctl(int sockfd, int request, ... /* char *arg */) {
    va_list arg;
    char *param;
    struct fd *fd = NULL;
    int result = ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(sockfd);
    SHOWVALUE(request);

    __set_errno(0);
    DECLARE_SOCKETBASE();

    if (request != TIOCGWINSZ && request != TIOCSWINSZ) {
        if (FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IS_SOCKET)) {
            assert(sockfd >= 0 && sockfd < __clib4->__num_fd);
            assert(__clib4->__fd[sockfd] != NULL);
            assert(FLAG_IS_SET(__clib4->__fd[sockfd]->fd_Flags, FDF_IN_USE));

            fd = __get_file_descriptor_socket(sockfd);
            if (fd == NULL)
                goto out;

            __fd_lock(fd);

            va_start(arg, request);
            param = va_arg(arg, char *);
            va_end(arg);

            SHOWPOINTER(param);

            result = __IoctlSocket(fd->fd_Socket, request, param);
            if (result == 0) {
                const int *option = (const int *) param;

                if (request == (int) FIONBIO) {
                    if ((*option) != 0)
                        SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
                    else
                        CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
                } else if (request == (int) FIOASYNC) {
                    if ((*option) != 0)
                        SET_FLAG(fd->fd_Flags, FDF_ASYNC_IO);
                    else
                        CLEAR_FLAG(fd->fd_Flags, FDF_ASYNC_IO);
                }
            }

            __fd_unlock(fd);
        }
        else {
            if (request != FIONBIO) {
                fd = __get_file_descriptor(sockfd);
                if (fd == NULL)
                    goto out;

                __fd_lock(fd);

                va_start(arg, request);
                param = va_arg(arg, char *);
                va_end(arg);

                SHOWPOINTER(param);

                const int *option = (const int *) param;
                if ((*option) != 0)
                    SET_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);
                else
                    CLEAR_FLAG(fd->fd_Flags, FDF_NON_BLOCKING);

                __fd_unlock(fd);

                result = OK;
            }
        }
    } else if (request == TIOCGWINSZ) {
        struct winsize *size;
        // Get them from console device
        va_start(arg, request);
        size = va_arg(arg, struct winsize *);
        va_end(arg);

        if (size == NULL) {
            __set_errno(EFAULT);
            goto out;
        }

        uint32 cols, rows;
        BOOL success = readSize(&rows, &cols);
        if (!success) {
            size->ws_row = 80;
            size->ws_col = 23;
        } else {
            size->ws_row = (short) rows;
            size->ws_col = (short) cols;
        }
    } else if (request == TIOCSWINSZ) {
        struct winsize *size;
        va_start(arg, request);
        size = va_arg(arg, struct winsize *);
        va_end(arg);

        if (size == NULL) {
            __set_errno(EFAULT);
            goto out;
        }

        /* Write size will clear and refresh only the window.
         * Console device don't support resizing via CSI
         */
#if 0
        writeSize(size->ws_row, size->ws_col);
#endif
    }

out:

    __check_abort();

    RETURN(result);
    return (result);
}
