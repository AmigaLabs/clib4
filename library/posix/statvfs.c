/*
 * $Id: posix_statvfs.c,v 1.0 2022-03-28 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _POSIX_HEADERS_H
#include "posix_headers.h"
#endif /* _POSIX_HEADERS_H */

int
statvfs(const char *path, struct statvfs *buf) {
    struct name_translation_info path_name_nti;
    int result = -1;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(path);
    SHOWPOINTER(buf);

    if (__clib4->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&path, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno_r(__clib4, EINVAL);
            goto out;
        }
    }

    struct InfoData *info = AllocDosObject(DOS_INFODATA, 0);
    if (info != NULL) {
        // 3 is the number of tags passed to GetDiskInfoTags call
        if (GetDiskInfoTags(
                GDI_StringNameInput, path,
                GDI_VolumeRequired, TRUE,
                GDI_InfoData, info,
                TAG_END) == 3) {
            uint32_t maxlength = STATVFS_MAX_NAME;
            FileSystemAttrTags(
                    FSA_MaxFileNameLengthR, &maxlength,
                    FSA_StringNameInput, path,
                    TAG_END);

            if (info->id_VolumeNode == BZERO) {
                FreeDosObject(DOS_INFODATA, info);
                /* Device not present or not responding */
                __set_errno_r(__clib4, ENXIO);
                goto out;
            }

            __convert_info_to_statvfs(info, buf);

            result = 0;
        } else {
            __translate_io_error_to_errno(IoErr());
        }
        FreeDosObject(DOS_INFODATA, info);
    } else
        __set_errno_r(__clib4, ENOMEM);

out:
    RETURN(result);
    return (result);
}