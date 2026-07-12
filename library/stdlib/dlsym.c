/*
 * $Id: stdlib_dlsym.c,v 1.2 2010-08-21 11:37:03 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <dlfcn.h>
#include <libraries/elf.h>
#include <proto/elf.h>

void *
dlsym(void *handle, const char *symbol_name) {
    void *result = NULL;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWPOINTER(handle);
    SHOWSTRING(symbol_name);

    if (__clib4->__dl_root_handle != NULL) {
        struct ElfIFace *IElf = __clib4->IElf;
        APTR symbol_data = NULL;
        Elf32_Error error;
        APTR lookup_handle = handle;

        /*
         * dlopen(NULL) normally returns a libc.so DSO handle (see dlopen.c).
         * Only when no libc.so path could be opened does it fall back to
         * returning __dl_root_handle as a sentinel.  Detect that sentinel
         * here and map it to ELF32_RTLD_DEFAULT (global scope search).
         * Note: if a real libc.so handle was returned by dlopen(NULL), it
         * reaches DLSym below as a regular handle — no special casing needed.
         */
        if (handle == (void *)__clib4->__dl_root_handle)
            lookup_handle = ELF32_RTLD_DEFAULT;

        error = DLSym(__clib4->__dl_root_handle, lookup_handle, symbol_name, &symbol_data);

        /*
         * AmigaOS 4 ELF loader quirk: when a shared library is opened via
         * DLOpen() and its symbols are already present in the global ELF scope
         * (e.g. because clib4.library already provides them), DLSym() on the
         * library-specific handle returns ELF32_SYMBOL_NOT_FOUND even though
         * the symbol is available globally.  Fall back to a global-scope search
         * so that dlsym(dlopen("libc.so", ...), "malloc") works as expected.
         */
        if (error == ELF32_SYMBOL_NOT_FOUND && lookup_handle != ELF32_RTLD_DEFAULT) {
            SHOWMSG("dlsym: symbol not found in specific handle, falling back to global scope");
            error = DLSym(__clib4->__dl_root_handle, ELF32_RTLD_DEFAULT, symbol_name, &symbol_data);
        }

        if (error != ELF32_NO_ERROR) {
            __clib4->__elf_error_code = error;
            goto out;
        }
        result = symbol_data;
    } else {
        __set_errno(ENOSYS);
    }

out:
    RETURN(result);
    return (result);
}
