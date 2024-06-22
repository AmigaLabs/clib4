/*
* $Id: crtbegin.c,v 1.3 2022-03-09 21:07:25 clib4devs Exp $
  */
#undef __USE_INLINE__
#define __NOLIBBASE__
#define __NOGLOBALIFACE__

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include "shared_library/interface.h"

#include "c.lib_rev.h"

#ifndef _DEBUG_H
#include "debug/debug.h"
#endif /* _DEBUG_H */

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

register void *r13 __asm("r13");
extern void *_SDA_BASE_ __attribute__((force_no_baserel));

extern int main(int, char **);
int clib4_start(char *args, int32 arglen, struct Library *sysbase);
int _start(char *args, int32 arglen, struct Library *sysbase);

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
clib4_start(char *args, int32 arglen, struct Library *sysbase) {
    struct ExecIFace *iexec;
    struct Clib4IFace *iclib4;
    struct DOSIFace *idos;

    int rc = -1;
    void *old_r13 = r13;

    r13 = &_SDA_BASE_;
    SysBase = sysbase;

    iexec = (struct ExecIFace *) ((struct ExecBase *) SysBase)->MainInterface;
    iexec->Obtain();

    IExec = iexec;
    idos = (struct DOSIFace *) OpenLibraryInterface(iexec, "dos.library", MIN_OS_VERSION);
    if (idos) {
        IDOS = idos;
        IUtility = (struct UtilityIFace *) OpenLibraryInterface(iexec, "utility.library", MIN_OS_VERSION);
        if (IUtility != NULL) {
            UtilityBase = IUtility->Data.LibBase;
            iclib4 = (struct Clib4IFace *) OpenLibraryInterface(iexec, "clib4.library", 1);
            if (iclib4 != NULL) {
                struct Library *clib4base = ((struct Interface *) iclib4)->Data.LibBase;
                if (clib4base->lib_Version >= VERSION && clib4base->lib_Revision >= REVISION) {
                    IClib4 = iclib4;

                    rc = iclib4->library_start(args, arglen, main, __CTOR_LIST__, __DTOR_LIST__);
                }
                else {
                    idos->Printf("This program requires clib4.library %d\n", VERS);
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

    r13 = old_r13;

    return rc;
}

int
_start(STRPTR argstring, int32 arglen, struct Library *sysbase) {
    return clib4_start(argstring, arglen, sysbase);
}