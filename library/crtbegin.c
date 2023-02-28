/*
* $Id: crtbegin.c,v 1.3 2022-03-09 21:07:25 clib2devs Exp $
  */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/elf.h>
#include <libraries/elf.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

/*
 * Dummy constructor and destructor array. The linker script will put these at the
 * very beginning of section ".ctors" and ".dtors". crtend.o contains a similar entry
 * with a NULL pointer entry and is put at the end of the sections. This way, the init
 * code can find the global constructor/destructor pointers.
 *
 * WARNING:
 * This hack does not work correctly with GCC 5 and higher. The optimizer
 * will see a one element array and act appropriately. The current workaround
 * is to use -fno-aggressive-loop-optimizations when compiling this file.
 *
 * NOTE:  In order to be able to support SVR4 shared libraries, we arrange
 * to have one set of symbols { __CTOR_LIST__, __DTOR_LIST__, __CTOR_END__,
 * __DTOR_END__ } per root executable and also one set of these symbols
 * per shared library.  So in any given whole process image, we may have
 * multiple definitions of each of these symbols.  In order to prevent
 * these definitions from conflicting with one another, and in order to
 * ensure that the proper lists are used for the initialization/finalization
 * of each individual shared library (respectively), we give these symbols
 * only internal (i.e. `static') linkage, and we also make it a point to
 * refer to only the __CTOR_END__ symbol in sh/crtend.o and the __DTOR_LIST__
 * symbol in sh/crtbegin.o, where they are defined.  */

static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors")));
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors")));

BOOL open_libraries(void);
void close_libraries(void);

/* These are used to initialize the shared objects linked to this binary,
   and for the dlopen(), dlclose() and dlsym() functions. */
struct Library *__ElfBase;
struct ElfIFace *__IElf;
static Elf32_Handle elf_handle;

void _init(void) {
    int i = 0;

    while (__CTOR_LIST__[i + 1]) {
        i++;
    }
    SHOWVALUE(i);
    while (i > 0) {
        SHOWMSG("Calling ctor");
        SHOWVALUE(i);
        __CTOR_LIST__[i--]();
    }
}

void _fini(void) {
    int i = 1;

    while (__DTOR_LIST__[i]) {
        SHOWMSG("Calling dtor");
        SHOWVALUE(i);
        __DTOR_LIST__[i++]();
    }
}

void close_libraries(void) {
    if (__IUtility != NULL) {
        DropInterface((struct Interface *) __IUtility);
        __IUtility = NULL;
    }

    if (__UtilityBase != NULL) {
        CloseLibrary(__UtilityBase);
        __UtilityBase = NULL;
    }

    if (__IElf != NULL) {
        DropInterface((struct Interface *) __IElf);
        __IElf = NULL;
    }

    if (__ElfBase != NULL) {
        CloseLibrary(__ElfBase);
        __ElfBase = NULL;
    }

    if (IDOS != NULL) {
        DropInterface((struct Interface *) IDOS);
        IDOS = NULL;
    }

    if (DOSBase != NULL) {
        CloseLibrary(DOSBase);
        DOSBase = NULL;
    }
}

BOOL open_libraries(void) {
    /* Open the minimum required libraries. */
    BOOL success = FALSE;

    /* Open the minimum required libraries. */
    DOSBase = (struct Library *) OpenLibrary("dos.library", MIN_OS_VERSION);
    if (DOSBase == NULL) {
        SHOWMSG("Cannot get DOSBase!");
        goto out;
    }

    /* Obtain the interfaces for these libraries. */
    IDOS = (struct DOSIFace *) GetInterface(DOSBase, "main", 1, NULL);
    if (IDOS == NULL) {
        SHOWMSG("Cannot get IDOS!");
        goto out;
    }

    /* We need elf.library V52.2 or higher. */
    __ElfBase = OpenLibrary("elf.library", 0);
    if (__ElfBase == NULL || (__ElfBase->lib_Version < 52) || (__ElfBase->lib_Version == 52 && __ElfBase->lib_Revision < 2)) {
        SHOWMSG("Cannot get __ElfBase >=52.2!");
        goto out;
    }

    __IElf = (struct ElfIFace *) GetInterface(__ElfBase, "main", 1, NULL);
    if (__IElf == NULL) {
        SHOWMSG("Cannot get __IElf!");
        goto out;
    }

    __UtilityBase = OpenLibrary("utility.library", MIN_OS_VERSION);
    if (__UtilityBase == NULL)
        goto out;

    __IUtility = (struct UtilityIFace *) GetInterface(__UtilityBase, "main", 1, 0);
    if (__IUtility == NULL)
        goto out;

    success = TRUE;

out:

    return (success);
}

void shared_obj_exit(void) {
    struct ElfIFace *IElf = __IElf;
    ENTER();

    /* If we got what we wanted, trigger the destructors, etc. in the shared objects linked to this binary. */
    if (elf_handle != NULL) {
        SHOWMSG("Invoking shared object destructors");
        InitSHLibs(elf_handle, FALSE);
        elf_handle = NULL;
    };

    close_libraries();
    LEAVE();
}

void shared_obj_init(void) {
    ENTER();

    if (open_libraries()) {
        struct ElfIFace *IElf = __IElf;

        BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
        if (segment_list != ZERO) {
            int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &elf_handle, TAG_DONE);
            if (ret == 1) {
                if (elf_handle != NULL) {
                    /* Trigger the constructors, etc. in the shared objects linked to this binary. */
                    InitSHLibs(elf_handle, TRUE);
                }
                else {
                    SHOWMSG("elf_handle == NULL!");
                }
            }
            else {
                SHOWMSG("GetSegListInfoTags fail!");
            }
        }
        else {
            SHOWMSG("GetProcSegList return ZERO!");
        }
    }
    else {
        SHOWMSG("Cannot open libraries!");
        const char *error_message;

        /* If available, use the error message provided by the client. */
        error_message = __minimum_os_lib_error;

        if (error_message == NULL)
            error_message = "This program requires AmigaOS 4.0 (52.2) or higher.";

        __show_error(error_message);
    }
    LEAVE();
}

int _start(STRPTR argstring, int32 arglen, struct ExecBase *sysbase) {
    SysBase = *(struct Library **) 4;
    IExec = (struct ExecIFace *) ((struct ExecBase *) SysBase)->MainInterface;

    /* The shared objects need to be set up before any local constructors are invoked. */
    shared_obj_init();

    int r = _main();

    /* The shared objects need to be cleaned up after all local destructors have been invoked. */
    shared_obj_exit();

    return r;
}