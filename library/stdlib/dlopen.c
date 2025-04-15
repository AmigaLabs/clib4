/*
 * $Id: stdlib_dlopen.c,v 1.2 2010-08-21 11:37:03 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <dlfcn.h>
#include <libraries/elf.h>
#include <proto/elf.h>

void *
dlopen(const char *path_name, int mode) {
    void *result = NULL;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWSTRING(path_name);
    SHOWVALUE(mode);

    if (path_name == NULL || path_name[0] == '\0') {
        __set_errno_r(__clib4, ENOENT);
        goto out;
    }

    struct name_translation_info path_name_nti;
    if (__clib4->__unix_path_semantics) {
        if (__translate_unix_to_amiga_path_name(&path_name, &path_name_nti) != 0)
            goto out;

        if (path_name_nti.is_root) {
            __set_errno_r(__clib4, EACCES);
            goto out;
        }
    }
    SHOWPOINTER(__clib4->__dl_root_handle);

    if (__clib4->__dl_root_handle != NULL) {
        struct ElfIFace *IElf = __clib4->IElf;
        SHOWPOINTER(IElf);
        uint32 flags = 0;

        if (mode & RTLD_LOCAL)
            flags = ELF32_RTLD_LOCAL;

        if (mode & RTLD_GLOBAL)
            flags = ELF32_RTLD_GLOBAL;

        result = DLOpen(__clib4->__dl_root_handle, path_name, flags);
    } else {
        __set_errno_r(__clib4, ENOSYS);
    }

out:

    RETURN(result);
    return (result);
}
