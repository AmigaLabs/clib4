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

    if (fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    fd = __get_file_descriptor(__clib4, fp->iob_Descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    memset(&fam, 0, sizeof(fam));
    fam.fam_Action = file_action_read;
    fam.fam_Data = buf;
    fam.fam_Size = n;

    assert(fd->fd_Action != NULL);
    result = (*fd->fd_Action)(__clib4, fd, &fam);

    if (result == EOF) {
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

    if (fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    fd = __get_file_descriptor(__clib4, fp->iob_Descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    /* If append mode, seek to end before writing */
    if (FLAG_IS_SET(fp->iob_Flags, IOBF_APP)) {
        struct file_action_message seek_fam;
        memset(&seek_fam, 0, sizeof(seek_fam));
        seek_fam.fam_Action = file_action_seek;
        seek_fam.fam_Offset = 0;
        seek_fam.fam_Mode = SEEK_END;
        (*fd->fd_Action)(__clib4, fd, &seek_fam);
    }

    memset(&fam, 0, sizeof(fam));
    fam.fam_Action = file_action_write;
    fam.fam_Data = (char *) buf;
    fam.fam_Size = n;

    assert(fd->fd_Action != NULL);
    result = (*fd->fd_Action)(__clib4, fd, &fam);

    if (result == EOF) {
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

    if (fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd) {
        __set_errno(EBADF);
        RETURN(-1);
        return (fpos_t) -1;
    }

    fd = __get_file_descriptor(__clib4, fp->iob_Descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        RETURN(-1);
        return (fpos_t) -1;
    }

    memset(&fam, 0, sizeof(fam));
    fam.fam_Action = file_action_seek;
    fam.fam_Offset = offset;
    fam.fam_Mode = whence;

    assert(fd->fd_Action != NULL);
    result = (*fd->fd_Action)(__clib4, fd, &fam);

    if (result == EOF) {
        if (fam.fam_Error != 0)
            __set_errno(fam.fam_Error);
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

    if (fp->iob_Descriptor < 0 || fp->iob_Descriptor >= __clib4->__num_fd) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    fd = __get_file_descriptor(__clib4, fp->iob_Descriptor);
    if (fd == NULL) {
        __set_errno(EBADF);
        RETURN(-1);
        return -1;
    }

    memset(&fam, 0, sizeof(fam));
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
