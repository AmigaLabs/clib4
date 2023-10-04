/*
 * $Id: string_strerror_r.c,v 1.2 2006-09-21 09:24:20 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

static const char *error_table[ENOTSUP - EPERM + 1] =
        {
                "Operation not permitted",
                "No such file or directory",
                "No such process",
                "Interrupted system call",
                "Input/output error",
                "Device not configured",
                "Argument list too long",
                "Exec format error",
                "Bad file descriptor",
                "No child processes",
                "Resource deadlock avoided",
                "Cannot allocate memory",
                "Permission denied",
                "Bad address",
                "Block device required",
                "Device busy",
                "File exists",
                "Cross-device link",
                "Operation not supported by device",
                "Not a directory",
                "Is a directory",
                "Invalid argument",
                "Too many open files in system",
                "Too many open files",
                "Inappropriate ioctl for device",
                "Text file busy",
                "File too large",
                "No space left on device",
                "Illegal seek",
                "Read-only file system",
                "Too many links",
                "Broken pipe",
                "Numerical argument out of domain",
                "Numerical result out of range",
                "Resource temporarily unavailable",
                "Operation now in progress",
                "Operation already in progress",
                "Socket operation on non-socket",
                "Destination address required",
                "Message too long",
                "Protocol wrong type for socket",
                "Protocol not available",
                "Protocol not supported",
                "Socket type not supported",
                "Operation not supported on socket",
                "Protocol family not supported",
                "Address family not supported by protocol family",
                "Address already in use",
                "Can't assign requested address",
                "Network is down",
                "Network is unreachable",
                "Network dropped connection on reset",
                "Software caused connection abort",
                "Connection reset by peer",
                "No buffer space available",
                "Socket is already connected",
                "Socket is not connected",
                "Can't send after socket shutdown",
                "Too many references: can't splice",
                "Connection timed out",
                "Connection refused",
                "Too many levels of symbolic links",
                "File name too long",
                "Host is down",
                "No route to host",
                "Directory not empty",
                "Too many processes",
                "Too many users",
                "Disc quota exceeded",
                "Stale NFS file handle",
                "Too many levels of remote in path",
                "RPC struct is bad",
                "RPC version wrong",
                "RPC program not available",
                "Program version wrong",
                "Bad procedure for program",
                "No locks available",
                "Function not implemented",
                "Inappropriate file type or format",
                "Authentication error",
                "Need authenticator",
                "Identifier removed",
                "No message of the desired type.",
                "Value too large to be stored in data type.",
                "Encoding error detected",
                "Protocol error",
                "Operation canceled",
                "Bad message",
                "Not supported"
        };

int
strerror_r(int number, char *buffer, size_t buffer_size) {
    int result = -1;
    const char *str;
    size_t len;

    if (number < EPERM || number > ENOTSUP) {
        __set_errno(EINVAL);
        goto out;
    }

    str = error_table[number - EPERM];

    len = strlen(str);
    if (len >= buffer_size) {
        __set_errno(ERANGE);
        goto out;
    }

    strcpy(buffer, str);

    result = 0;

out:

    return (result);
}
