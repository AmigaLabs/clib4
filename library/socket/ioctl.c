/*
 * $Id: socket_ioctl.c,v 1.14 2006-11-16 10:41:15 clib2devs Exp $
*/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _STDARG_H
#include <stdarg.h>
#endif /* _STDARG_H */

#include <sys/ioctl.h>
#include <termios.h>

static BOOL readSize(uint32 *rows, uint32 *columns) {
    BPTR fh = Open("CONSOLE:", MODE_OLDFILE);
    BOOL success = FALSE;
    if (fh) {
        uint32 width = 0, height = 0;
        char r[2] = {0};
        char buffer[25 + 1] = {0};

        SetMode(fh, 1); // RAW mode
        if (Write(fh, "\x9b q", 3) == 3) {
            LONG actual = 0;
            LONG ret = Read(fh, r, 1);
            while (r[0] != 'r' && ret != 0) {
                buffer[actual] = r[0];
                actual += ret;
                ret = Read(fh, r, 1);
            }
            if (actual >= 0) {
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

    ENTER();

    SHOWVALUE(sockfd);
    SHOWVALUE(request);

    __set_errno(0);

    if (request != TIOCGWINSZ && request != TIOCSWINSZ) {
        assert(__SocketBase != NULL);

        assert(sockfd >= 0 && sockfd < __num_fd);
        assert(__fd[sockfd] != NULL);
        assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IN_USE));
        assert(FLAG_IS_SET(__fd[sockfd]->fd_Flags, FDF_IS_SOCKET));

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
        writeSize(size->ws_row, size->ws_col);
    }

out:

    __check_abort();

    RETURN(result);
    return (result);
}
