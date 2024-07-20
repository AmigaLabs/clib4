/*
 * $Id: posix_fstatvfs.c,v 1.1 2024-07-04 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _POSIX_HEADERS_H
#include "posix_headers.h"
#endif /* _POSIX_HEADERS_H */

int
fstatvfs(int fd, struct statvfs *buf) {
    BPTR file;
    int result = -1;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWVALUE(fd);
    SHOWPOINTER(buf);

    struct fd *fildes = __get_file_descriptor(__clib4, fd);
    if (fildes == NULL) {
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    file = ParentOfFH(fildes->fd_File);
    if (file == BZERO) {
        __set_errno_r(__clib4, EINVAL);
        goto out;
    }

    struct InfoData *info = AllocDosObject(DOS_INFODATA, 0);
    // 3 is the number of tags passed to GetDiskInfoTags call
    if (GetDiskInfoTags(
            GDI_FileHandleInput, file,
            GDI_VolumeRequired, TRUE,
            GDI_InfoData, info,
            TAG_END) == 3) {
        uint32_t maxlength = STATVFS_MAX_NAME;

        FileSystemAttrTags(
                FSA_MaxFileNameLengthR, &maxlength,
                FSA_FileHandleInput, file,
                TAG_END);

        if (info->id_VolumeNode == BZERO) {
            FreeDosObject(DOS_INFODATA, info);
            /* Device not present or not responding */
            __set_errno_r(__clib4, ENXIO);
            goto out;
        }

        __convert_info_to_statvfs(info, buf);

        FreeDosObject(DOS_INFODATA, info);

        result = 0;
    } else {
        __translate_io_error_to_errno(IoErr());
    }

out:
    RETURN(result);
    return (result);
}