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

    /*
     * POSIX: dlopen(NULL, mode) returns a handle for the main program.
     *
     * On AmigaOS 4, libc/clib4 symbols live in a DSO (libc.so) that is
     * loaded with LOCAL scope, so ELF32_RTLD_DEFAULT (global scope) cannot
     * find them.  The practical fix is to open the libc DSO explicitly so
     * that DLSym() can look up symbols via the library-specific handle —
     * the same path that already works for an explicit dlopen("libc.so").
     *
     * If no standard libc path can be found we fall back to returning
     * __dl_root_handle as a sentinel; dlsym() and dlclose() recognise this
     * sentinel and handle it gracefully.
     */
    if (path_name == NULL) {
        if (__clib4->__dl_root_handle != NULL) {
            struct ElfIFace *IElf = __clib4->IElf;
            static const char * const self_paths[] = {
                "PROGDIR:SObjs/libc.so",
                NULL
            };
            for (int i = 0; self_paths[i] != NULL; i++) {
                result = DLOpen(__clib4->__dl_root_handle, self_paths[i], 0);
                if (result != NULL)
                    break;
            }
            if (result == NULL) {
                /* Last resort: return root handle as sentinel so callers at
                 * least get a non-NULL handle; dlsym() will try RTLD_DEFAULT. */
                result = (void *)__clib4->__dl_root_handle;
            }
        } else {
            __set_errno_r(__clib4, ENOSYS);
        }
        goto out;
    }

    if (path_name[0] == '\0') {
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
        if (result == NULL) {
            SHOWMSG("DLOpen returned NULL");
            __set_errno_r(__clib4, ENOENT);
        }
    } else {
        __set_errno_r(__clib4, ENOSYS);
    }

out:

    RETURN(result);
    return (result);
}
