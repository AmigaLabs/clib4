/*
 * $Id: stdio_stdio_ext.c,v 1.0 2022-08-26 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void _flushlbf(void) {
    fflush(0);
}

int __fsetlocking(FILE *f, int type) {
    // TODO - Add this around flockfile or something else
    (void) f;
    (void) type;

    return 0;
}

int __fwriting(FILE *f) {
    struct _clib4 *__clib4 = __CLIB4;
    struct iob *file = (struct iob *) f;
    struct fd *filedes = __get_file_descriptor(__clib4, file->iob_Descriptor);

    return FLAG_IS_SET(filedes->fd_Flags, FDF_WRITE) && FLAG_IS_CLEAR(filedes->fd_Flags, FDF_READ);
}

int __freading(FILE *f) {
    struct _clib4 *__clib4 = __CLIB4;
    struct iob *file = (struct iob *) f;
    struct fd *filedes = __get_file_descriptor(__clib4, file->iob_Descriptor);

    return FLAG_IS_SET(filedes->fd_Flags, FDF_READ) && FLAG_IS_CLEAR(filedes->fd_Flags, FDF_WRITE);
}

int __freadable(FILE *f) {
    struct _clib4 *__clib4 = __CLIB4;
    struct iob *file = (struct iob *) f;
    struct fd *filedes = __get_file_descriptor(__clib4, file->iob_Descriptor);

    return FLAG_IS_SET(filedes->fd_Flags, FDF_READ);
}

int __fwritable(FILE *f) {
    struct _clib4 *__clib4 = __CLIB4;
    struct iob *file = (struct iob *) f;
    struct fd *filedes = __get_file_descriptor(__clib4, file->iob_Descriptor);

    return FLAG_IS_SET(filedes->fd_Flags, FDF_WRITE);
}

int __flbf(FILE *f) {
    struct iob *file = (struct iob *) f;
    return FLAG_IS_SET(file->iob_Flags, IOBF_BUFFER_MODE_LINE);
}

size_t __fbufsize(FILE *f) {
    return f->size;
}

size_t __fpending(FILE *f) {
    return f->num_write_bytes;
}

int __fpurge(FILE *f) {
    f->position = f->num_read_bytes = f->num_write_bytes = 0;
    return 0;
}

int fpurge(FILE *f) {
    return __fpurge(f);
}

void __fseterr(FILE *f) {
    f->_flags |= __SERR;
}

size_t __freadahead(FILE *f) {
    return f->num_read_bytes ? f->num_read_bytes - f->position : 0;
}

void __freadptrinc(FILE *f, size_t inc) {
    f->position += inc;
}
