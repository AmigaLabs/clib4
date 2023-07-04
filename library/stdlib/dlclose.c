/*
 * $Id: stdlib_dlclose.c,v 1.2 2010-08-21 11:37:03 clib2devs Exp $
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
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();
    SHOWPOINTER(handle);

    if (__clib2->__dl_elf_handle != NULL) {
        struct ElfIFace *IElf = __clib2->IElf;
        Elf32_Error error;

        error = DLClose(__clib2->__dl_elf_handle, handle);
        if (error != ELF32_NO_ERROR) {
            __clib2->__elf_error_code = error;
            goto out;
        }
    } else {
        __set_errno(ENOSYS);
    }

    result = 0;

out:
    RETURN(result);
    return (result);
}
