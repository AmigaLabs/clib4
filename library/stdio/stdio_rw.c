/*
 * $Id: stdio_rw.c,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Bridge functions connecting FILE function pointers (_read/_write/_seek/_close)
 * to the fd layer's fd_Action hooks.
 *
 * These are the defaults set on every FILE opened via fopen()/fdopen()/freopen().
 * The cookie is a pointer to the struct iob itself.
 *
 * Flow: FILE._read(cookie, ...) → __sread() → fd→fd_Action(read) → AmigaDOS Read()
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/****************************************************************************/

/*
 * __sread — Read from the underlying fd.
 *
 * cookie points to the struct iob.
 * Returns bytes read, 0 for EOF, or -1 on error (sets errno).
 */
ssize_t
__sread(void *cookie, char *buf, int n) {
    struct iob *fp = (struct iob *) cookie;
    struct _clib4 *__clib4 = __CLIB4;
    struct fd *fd;
    struct file_action_message fam;
    int64_t result;

    ENTER();

    /* Inline fd lookup — avoid __get_file_descriptor which takes a global lock */
    if (__builtin_expect(fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd, 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    fd = __clib4->__fd[fp->iob_Descriptor];
    if (__builtin_expect(fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE), 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    /* Resolve alias */
    if (__builtin_expect(fd->fd_Original != NULL, 0))
        fd = fd->fd_Original;

    fam.fam_Action = file_action_read;
    fam.fam_Data = buf;
    fam.fam_Size = n;
    fam.fam_Error = 0;

    assert(fd->fd_Action != NULL);
    result = (*fd->fd_Action)(__clib4, fd, &fam);

    if (__builtin_expect(result == EOF, 0)) {
        if (fam.fam_Error != 0)
            __set_errno(fam.fam_Error);
        RETURN(-1);
        return -1;
    }

    /* Update cached file position if tracking is active */
    if (FLAG_IS_SET(fp->iob_Flags, IOBF_OFF))
        fp->_offset += result;

    RETURN(result);
    return (ssize_t) result;
}

/****************************************************************************/

/*
 * __swrite — Write to the underlying fd.
 *
 * cookie points to the struct iob.
 * Returns bytes written, or -1 on error.
 */
ssize_t
__swrite(void *cookie, const char *buf, int n) {
    struct iob *fp = (struct iob *) cookie;
    struct _clib4 *__clib4 = __CLIB4;
    struct fd *fd;
    struct file_action_message fam;
    int64_t result;

    ENTER();

    /* Inline fd lookup — avoid __get_file_descriptor which takes a global lock */
    if (__builtin_expect(fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd, 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    fd = __clib4->__fd[fp->iob_Descriptor];
    if (__builtin_expect(fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE), 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    /* Resolve alias */
    if (__builtin_expect(fd->fd_Original != NULL, 0))
        fd = fd->fd_Original;

    /* If append mode, seek to end before writing */
    if (__builtin_expect(FLAG_IS_SET(fp->iob_Flags, IOBF_APP), 0)) {
        struct file_action_message seek_fam;
        seek_fam.fam_Action = file_action_seek;
        seek_fam.fam_Offset = 0;
        seek_fam.fam_Mode = SEEK_END;
        (*fd->fd_Action)(__clib4, fd, &seek_fam);
    }

    fam.fam_Action = file_action_write;
    fam.fam_Data = (char *) buf;
    fam.fam_Size = n;
    fam.fam_Error = 0;

    assert(fd->fd_Action != NULL);
    result = (*fd->fd_Action)(__clib4, fd, &fam);

    if (__builtin_expect(result == EOF, 0)) {
        if (fam.fam_Error != 0)
            __set_errno(fam.fam_Error);
        RETURN(-1);
        return -1;
    }

    /* Update cached file position if tracking is active */
    if (FLAG_IS_SET(fp->iob_Flags, IOBF_OFF))
        fp->_offset += result;

    RETURN(result);
    return (ssize_t) result;
}

/****************************************************************************/

/*
 * __sseek — Seek the underlying fd.
 *
 * cookie points to the struct iob.
 * Returns the new position, or -1 on error.
 */
fpos_t
__sseek(void *cookie, fpos_t offset, int whence) {
    struct iob *fp = (struct iob *) cookie;
    struct _clib4 *__clib4 = __CLIB4;
    struct fd *fd;
    struct file_action_message fam;
    int64_t result;

    ENTER();

    /* Inline fd lookup */
    if (__builtin_expect(fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd, 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return (fpos_t) -1;
    }

    fd = __clib4->__fd[fp->iob_Descriptor];
    if (__builtin_expect(fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE), 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return (fpos_t) -1;
    }

    if (__builtin_expect(fd->fd_Original != NULL, 0))
        fd = fd->fd_Original;

    fam.fam_Action = file_action_seek;
    fam.fam_Offset = offset;
    fam.fam_Mode = whence;
    fam.fam_Error = 0;

    assert(fd->fd_Action != NULL);
    result = (*fd->fd_Action)(__clib4, fd, &fam);

    if (result == EOF) {
        __set_errno(fam.fam_Error != 0 ? fam.fam_Error : EIO);
        RETURN(-1);
        return (fpos_t) -1;
    }

    /* Update cached offset */
    fp->_offset = result;
    SET_FLAG(fp->iob_Flags, IOBF_OFF);

    RETURN(result);
    return (fpos_t) result;
}

/****************************************************************************/

/*
 * __sclose — Close the underlying fd.
 *
 * cookie points to the struct iob.
 * Returns 0 on success, -1 on error.
 */
int
__sclose(void *cookie) {
    struct iob *fp = (struct iob *) cookie;
    struct _clib4 *__clib4 = __CLIB4;
    struct fd *fd;
    struct file_action_message fam;
    int64_t result;

    ENTER();

    memset(&fam, 0, sizeof(fam));

    /* Inline fd lookup */
    if (__builtin_expect(fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd, 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    fd = __clib4->__fd[fp->iob_Descriptor];
    if (__builtin_expect(fd == NULL || FLAG_IS_CLEAR(fd->fd_Flags, FDF_IN_USE), 0)) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    /* Do NOT resolve fd_Original here. For close, we must affect THIS
     * specific fd slot — not the original in an alias chain. Following
     * fd_Original would close the WRONG fd and corrupt the fd table.
     * (Consistent with __iob_hook_entry which uses
     *  __get_file_descriptor_dont_resolve for close operations.) */

    fam.fam_Action = file_action_close;

    assert(fd->fd_Action != NULL);
    result = (*fd->fd_Action)(__clib4, fd, &fam);

    if (result == EOF) {
        if (fam.fam_Error != 0)
            __set_errno(fam.fam_Error);
        RETURN(-1);
        return -1;
    }

    RETURN(0);
    return 0;
}
