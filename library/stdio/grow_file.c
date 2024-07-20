/*
 * $Id: stdio_grow_file.c,v 1.10 2024-07-06 17:35:41 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/* Change file position to the end of a file, then add a certain number of 0 bytes. Note that
   this function will change the current file position! */
int64_t
__grow_file_size(struct _clib4 *__clib4, struct fd *fd, int num_bytes) {
    unsigned char *aligned_buffer;
    unsigned char *buffer;
    struct FileHandle *fh;
    D_S(struct InfoData, id);
    LONG block_size;
    int bytes_written;
    int buffer_size;
    int size;
    _off64_t seek_position;
    _off64_t current_position;
    int alignment_skip;
    int result = ERROR;

    assert(fd != NULL);

    D(("we have to grow the file by %ld bytes", num_bytes));

    block_size = 0;

    assert(FLAG_IS_CLEAR(fd->fd_Flags, FDF_STDIO));

    fh = BADDR(fd->fd_File);
    if (fh != NULL && fh->fh_MsgPort != NULL && DoPkt(fh->fh_MsgPort, ACTION_HANDLER_INFO, MKBADDR(id), 0, 0, 0, 0))
        block_size = id->id_BytesPerBlock;

    if (block_size < 512)
        block_size = 512;

    /* We have to fill up the file with zero bytes.
     * That data comes from a local buffer. How
     * large can we make it?
     */
    buffer_size = 8 * block_size;
    if (buffer_size > num_bytes)
        buffer_size = num_bytes;

    /* Allocate a little more memory than required to allow for
     * the buffer to be aligned to a cache line boundary.
     */
    buffer = __malloc_r(__clib4, (size_t) buffer_size + (__clib4->__cache_line_size - 1));
    if (buffer == NULL) {
        SHOWMSG("not enough memory for write buffer");

        SetIoErr(ERROR_NO_FREE_STORE);
        goto out;
    }

    /* Align the buffer to a cache line boundary. */
    aligned_buffer = (unsigned char *) (((ULONG)(buffer + (__clib4->__cache_line_size - 1))) &
                                        ~(__clib4->__cache_line_size - 1));

    memset(aligned_buffer, 0, (size_t) buffer_size);

    seek_position = ChangeFilePosition(fd->fd_File, 0, OFFSET_END);
    if (seek_position == CHANGE_FILE_ERROR) {
        SHOWMSG("could not move to the end of the file");
        goto out;
    }

    seek_position = GetFilePosition(fd->fd_File);
    current_position = (_off64_t) seek_position;

    /* Try to make the first write access align the file position
     * to a block offset. Subsequent writes will then access the
     * file at positions that are multiples of the block size.
     */
    if (num_bytes > block_size && (current_position % block_size) != 0)
        alignment_skip = block_size - (current_position % block_size);
    else
        alignment_skip = 0;

    while (num_bytes > 0) {
        __check_abort_f(__clib4);

        size = buffer_size;
        if (size > num_bytes)
            size = num_bytes;

        /* If possible, even out the block offset. */
        if (alignment_skip > 0 && size > alignment_skip)
            size = alignment_skip;

        alignment_skip = 0;

        bytes_written = Write(fd->fd_File, aligned_buffer, size);
        if (bytes_written != size)
            goto out;

        num_bytes -= size;
    }

    SHOWMSG("all done.");

    result = OK;

out:

    if (buffer != NULL)
        __free_r(__clib4, buffer);

    return (result);
}
