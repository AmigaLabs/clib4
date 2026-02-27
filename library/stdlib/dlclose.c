/*
 * $Id: stdlib_dlclose.c,v 1.2 2010-08-21 11:37:03 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <dlfcn.h>
#include <libraries/elf.h>
#include <proto/elf.h>

int
dlclose(void *handle) {
    int result = -1;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWPOINTER(handle);

    if (__clib4->__dl_root_handle != NULL) {
        struct ElfIFace *IElf = __clib4->IElf;
        Elf32_Error error;

        error = DLClose(__clib4->__dl_root_handle, handle);
        if (error != ELF32_NO_ERROR) {
            __clib4->__elf_error_code = error;
            goto out;
        }
    } else {
        __set_errno_r(__clib4, ENOSYS);
    }

    result = 0;

out:
    RETURN(result);
    return (result);
}
