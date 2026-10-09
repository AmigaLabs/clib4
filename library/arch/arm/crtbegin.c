/*
 * crtbegin.c - the program start for ARM. It does what the PowerPC crtbegin
 * does: it opens dos, utility and clib4.library, takes the "main" interface of
 * clib4.library and hands the program to its library_start(). The libc calls in
 * the program go through the stubs in libc.a, which jump through IClib4.
 *
 * The PowerPC crtbegin binds r13 and r2 as globals. On AAPCS those are sp and a
 * caller-saved register, so this file reserves no register. It does not register
 * exception frames either: __register_frame_info is a weak undefined symbol, and
 * the ARM loader refuses weak references that nothing defines. ARM unwinding uses
 * the .ARM.exidx tables, which need no registration.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#undef __USE_INLINE__
#define __NOLIBBASE__
#define __NOGLOBALIFACE__

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#include <stdint.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include <workbench/startup.h>

#include "shared_library/interface.h"

#include "c.lib_rev.h"

#ifndef _DEBUG_H
#include "debug/debug.h"
#endif /* _DEBUG_H */

/*
 * Constructor and destructor lists, in the same sections as the PowerPC build.
 * The first entry is the count sentinel; crtend.o terminates the lists, and
 * library_start() runs them.
 */
static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors"))) = { (void *)~0 };
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors"))) = { (void *)~0 };

const struct Library *SysBase = NULL;
const struct ExecIFace *IExec = NULL;

const struct Library *DOSBase = NULL;
const struct DOSIFace *IDOS = NULL;

const struct Library *UtilityBase = NULL;
const struct UtilityIFace *IUtility = NULL;

struct Library *ElfBase = NULL;
struct ElfIFace *IElf = NULL;

const struct Clib4IFace *IClib4 = NULL;

extern int main(int, char **, char **);
int clib4_start(char *args, const int32 arglen, struct Library *sysbase);
int _start(char *argstring, int32 arglen, struct Library *sysbase);

static struct Interface *OpenLibraryInterface(struct ExecIFace *iexec, const char *name, int version) {
    struct Library *library;
    struct Interface *interface;

    library = iexec->OpenLibrary(name, version);
    if (library == NULL)
        return NULL;

    interface = iexec->GetInterface(library, "main", 1, NULL);
    if (interface == NULL) {
        iexec->CloseLibrary(library);
        return NULL;
    }

    return interface;
}

static void CloseLibraryInterface(struct ExecIFace *iexec, struct Interface *interface) {
    if (interface != NULL) {
        struct Library *library = interface->Data.LibBase;

        iexec->DropInterface(interface);
        interface = NULL;
        if (library != NULL) {
            iexec->CloseLibrary(library);
            library = NULL;
        }
    }
}

int
clib4_start(char *args, const int32 arglen, struct Library *sysbase) {
    struct ExecIFace *iexec;
    struct Clib4IFace *iclib4 = NULL;
    struct DOSIFace *idos;

    int rc = -1;

    struct Process *me;
    struct WBStartup *sms = NULL;

    SysBase = sysbase;

    iexec = (struct ExecIFace *) ((struct ExecBase *) SysBase)->MainInterface;
    iexec->Obtain();

    /* Pick up the Workbench startup message, if available. */
    me = (struct Process *) iexec->FindTask(NULL);
    if (!me->pr_CLI) {
        struct MsgPort *mp = &me->pr_MsgPort;
        iexec->WaitPort(mp);
        sms = (struct WBStartup *) iexec->GetMsg(mp);
    }

    IExec = iexec;
    idos = (struct DOSIFace *) OpenLibraryInterface(iexec, "dos.library", MIN_OS_VERSION);
    if (idos) {
        IDOS = idos;
        IUtility = (struct UtilityIFace *) OpenLibraryInterface(iexec, "utility.library", MIN_OS_VERSION);
        if (IUtility != NULL) {
            UtilityBase = IUtility->Data.LibBase;
            iclib4 = (struct Clib4IFace *) OpenLibraryInterface(iexec, "clib4.library", 1);
            if (iclib4 != NULL) {
                const struct Library *clib4base = ((struct Interface *) iclib4)->Data.LibBase;
                if (clib4base->lib_Version > VERSION || (clib4base->lib_Version == VERSION && clib4base->lib_Revision >= REVISION)) {
                    IClib4 = iclib4;

                    rc = iclib4->library_start(args, arglen, main, __CTOR_LIST__, __DTOR_LIST__, sms);
                }
                else {
                    idos->Printf("This program requires clib4.library version %ld.%ld\n", VERSION, REVISION);
                }
            } else {
                idos->Printf("Cannot open %s\n", VERS);
            }
        }
        else {
            idos->Printf("Cannot open utility.library version %ld!\n", MIN_OS_VERSION);
        }
    }
    else {
        iexec->Alert(AT_Recovery | AG_OpenLib | AO_DOSLib);
    }

    CloseLibraryInterface(iexec, (struct Interface *) iclib4);
    CloseLibraryInterface(iexec, (struct Interface *) IUtility);
    CloseLibraryInterface(iexec, (struct Interface *) idos);

    iexec->Release();

    return rc;
}

int
_start(STRPTR argstring, int32 arglen, struct Library *sysbase) {
    return clib4_start(argstring, arglen, sysbase);
}
