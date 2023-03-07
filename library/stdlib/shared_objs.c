/*
 * $Id: stdlib_shared_objs.c,v 1.1 2010-08-21 11:37:03 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

static BOOL open_elf_library(void);
static VOID close_elf_library(void);

/* These are used to initialize the shared objects linked to this binary,
   and for the dlopen(), dlclose() and dlsym() functions. */
struct Library *__ElfBase;
struct ElfIFace *__IElf;
static Elf32_Handle elf_handle;

static VOID close_elf_library(void) {
    if (__IElf != NULL) {
        DropInterface((struct Interface *) __IElf);
        __IElf = NULL;
    }

    if (__ElfBase != NULL) {
        CloseLibrary(__ElfBase);
        __ElfBase = NULL;
    }
}

static BOOL open_elf_library(void) {
    BOOL success = FALSE;

    /* We need elf.library V52.2 or higher. */
    __ElfBase = OpenLibrary("elf.library", 0);
    if (__ElfBase == NULL || (__ElfBase->lib_Version < 52) ||
        (__ElfBase->lib_Version == 52 && __ElfBase->lib_Revision < 2))
        goto out;

    __IElf = (struct ElfIFace *) GetInterface(__ElfBase, "main", 1, NULL);
    if (__IElf == NULL)
        goto out;

    success = TRUE;

out:

    return (success);
}

void shared_obj_exit(void) {
    struct ElfIFace *IElf = __IElf;

    /* If we got what we wanted, trigger the destructors, etc. in the shared objects linked to this binary. */
    if (elf_handle != NULL) {
        InitSHLibs(elf_handle, FALSE);
        elf_handle = NULL;
    };

    close_elf_library();
}

void shared_obj_init(void) {
    if (open_elf_library()) {
        struct ElfIFace *IElf = __IElf;

        BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
        if (segment_list != ZERO) {
            int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &elf_handle, TAG_DONE);
            if (ret == 1) {
                if (elf_handle != NULL) {
                    /* Trigger the constructors, etc. in the shared objects linked to this binary. */
                    InitSHLibs(elf_handle, TRUE);
                }
            }
        }
    }
}