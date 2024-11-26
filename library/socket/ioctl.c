/*
 * $Id: socket_ioctl.c,v 1.17 2024-07-04 10:41:15 clib4devs Exp $
*/

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _STDARG_H
#include <stdarg.h>
#endif /* _STDARG_H */

#include <proto/intuition.h>

#include <sys/ioctl.h>
#include <termios.h>

#define BUFFER_SIZE 30

int
getCurrentCliActionMode(void) {
    int consoleMode = ID_CON;  // Return normal mode on error
    struct Task *task = FindTask(NULL);
    if (task->tc_Node.ln_Type == NT_PROCESS) {
        struct MsgPort *port = ((struct Process *) task)->pr_ConsolePort;
        if (port != NULL) {
            struct ConsoleWindowData *conData = ObtainConsoleDataTags(OCD_MsgPortInput, port, TAG_END);
            if (conData != NULL) {
                consoleMode = conData->ConsoleType;
                ReleaseConsoleData(conData);
            }
        }
    }

    return consoleMode;
}

struct Window *
getCurrentCliWindow(void) {
    struct Window *cliWindow = NULL;
    struct Task *task = FindTask(NULL);
    if (task->tc_Node.ln_Type == NT_PROCESS) {
        struct MsgPort *port = ((struct Process *) task)->pr_ConsolePort;
        if (port != NULL) {
            struct ConsoleWindowData *conData = ObtainConsoleDataTags(OCD_MsgPortInput, port, TAG_END);
            if (conData != NULL) {
                cliWindow = conData->ConsoleWindow;
                ReleaseConsoleData(conData);
            }
        }
    }

    return cliWindow;
}

static void readSize(uint32 *rows, uint32 *columns, uint32 *xpixel, uint32 *ypixel) {
    BPTR fh = Output();
    if (fh) {
        uint32 _cols = 80, _rows = 23, _xpixel = 0, _ypixel = 0;
        char r;
        char buffer[BUFFER_SIZE + 1] = {0};
        int oldMode = getCurrentCliActionMode();

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
                                   "1;1;%ld;%ld r", &_rows, &_cols) == 2) {
                    struct Window *w = getCurrentCliWindow();
                    if (w != NULL) {
                        _xpixel = w->Width - w->BorderLeft - w->BorderRight;
                        _ypixel = w->Height - w->BorderTop - w->BorderBottom;
                    }
                }
            }
        }
        SetMode(fh, oldMode);

        *rows = _rows;
        *columns = _cols;
        *xpixel = _xpixel;
        *ypixel = _ypixel;
    }
}

static BOOL writeSize(uint32 rows, uint32 columns) {
    BPTR fh = Output();
    BOOL success = FALSE;

    if (fh) {
        char buffer[51] = {0};
        int oldMode = getCurrentCliActionMode();

        SetMode(fh, 1); // RAW mode
        snprintf(buffer, 50, "\x9b%dt\x9b%du", columns + 1, rows + 1);
        int bufferLen = strlen(buffer);
        int bytesWritten = Write(fh, buffer, bufferLen);
        if (bytesWritten == bufferLen) {
            struct Window *cliWindow = getCurrentCliWindow();
            if (cliWindow != NULL) {
                struct IntuitionIFace *IIntuition = NULL;
                struct Library *IntuitionBase;

                IntuitionBase = OpenLibrary("intuition.library", 53);
                if (IntuitionBase != NULL) {
                    IIntuition = (struct IntuitionIFace *) GetInterface(IntuitionBase, "main", 1, 0);
                    if (IIntuition == NULL) {
                        CloseLibrary(IntuitionBase);
                        IntuitionBase = NULL;
                    } else {
                        /* This is an hack to refresh the current window.
                         * Notice that not the main AmigaShell window will be resized but only the active tab
                         * This means that all other active tabs will use the same rows and cols
                         * Check tty-size.c example
                         */
                        SetWindowAttrs(cliWindow, WA_Width, cliWindow->Width + 1, WA_Height, cliWindow->Height + 1, TAG_DONE);
                        SetWindowAttrs(cliWindow, WA_Width, cliWindow->Width - 1, WA_Height, cliWindow->Height - 1, TAG_DONE);
                    }
                }
                if (IIntuition != NULL) {
                    DropInterface((struct Interface *) IIntuition);
                    IIntuition = NULL;
                }
                if (IntuitionBase != NULL) {
                    CloseLibrary(IntuitionBase);
                    IntuitionBase = NULL;
                }
            }
            success = TRUE;
        }
        SetMode(fh, oldMode);
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

            fd = __get_file_descriptor_socket(__clib4, sockfd);
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
        } else {
            if (request != FIONBIO) {
                fd = __get_file_descriptor(__clib4, sockfd);
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
        size = va_arg(arg,
        struct winsize *);
        va_end(arg);

        if (size == NULL) {
            __set_errno(EFAULT);
            goto out;
        }

        uint32 cols = 80, rows = 23, xpixel = 0, ypixel = 0;
        readSize(&rows, &cols, &xpixel, &ypixel);
        size->ws_row = (short) rows;
        size->ws_col = (short) cols;
        size->ws_xpixel = (short) xpixel;
        size->ws_ypixel = (short) ypixel;
    } else if (request == TIOCSWINSZ) {
        struct winsize *size;
        va_start(arg, request);
        size = va_arg(arg,
        struct winsize *);
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

    __check_abort_f(__clib4);

    RETURN(result);
    return (result);
}
