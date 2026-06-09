#undef __USE_INLINE__
#define __NOLIBBASE__
#define __NOGLOBALIFACE__

#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>

#include "interface.h"

/*
 * When built as a shared object (-DSHARED), we need our own IClib4 pointer
 * initialized via a constructor, since crtbegin.o is not linked into .so files.
 * For static linking (libc.a), IClib4 is already provided by crtbegin.o.
 */
#ifdef SHARED

struct Clib4IFace *IClib4 = NULL;

static struct Library *__clib4_so_base = NULL;

static void __init_clib4_so(void) {
    if (IClib4 != NULL)
        return;
    struct ExecBase *sysbase = *(struct ExecBase **)4;
    struct ExecIFace *iexec = (struct ExecIFace *)sysbase->MainInterface;
    __clib4_so_base = iexec->OpenLibrary("clib4.library", 1);
    if (__clib4_so_base) {
        IClib4 = (struct Clib4IFace *)iexec->GetInterface(__clib4_so_base, "main", 1, NULL);
        iexec->DropInterface((struct Interface *)IClib4);
    }
}

static void __exit_clib4_so(void) {
    if (__clib4_so_base) {
        struct ExecBase *sysbase = *(struct ExecBase **)4;
        struct ExecIFace *iexec = (struct ExecIFace *)sysbase->MainInterface;
        IClib4 = NULL;
        iexec->CloseLibrary(__clib4_so_base);
        __clib4_so_base = NULL;
    }
}

static void (*__init_clib4_so_ctor)(void) __attribute__((used, section(".ctors"))) = __init_clib4_so;
static void (*__exit_clib4_so_dtor)(void) __attribute__((used, section(".dtors"))) = __exit_clib4_so;
static void (*__init_clib4_so_arr)(void) __attribute__((used, section(".init_array"))) = __init_clib4_so;
static void (*__exit_clib4_so_fini)(void) __attribute__((used, section(".fini_array"))) = __exit_clib4_so;

#endif /* SHARED */

asm(".section  \".text\"             \n\
       .align 2                          \n\
       .globl __Clib4Call                \n\
       .type  __Clib4Call, @function     \n\
__Clib4Call:                          \n\
        lis   %r11, IClib4@ha            \n\
        lwz   %r0, IClib4@l(%r11)        \n\
        lwzx  %r11, %r12, %r0            \n\
        mtctr %r11                       \n\
        bctr                             \n\
       .size   __Clib4Call, .-__Clib4Call");

Clib4Call(__getClib4, 80);
//Clib4Call(__getGlobalClib4, 84);
Clib4Call(__translate_amiga_to_unix_path_name, 88);
Clib4Call(__translate_unix_to_amiga_path_name, 92);
//Clib4Call(__restore_path_name, 96);
Clib4Call(__print_termination_message, 100);
Clib4Call(__get_default_file, 104);
Clib4Call(unixPathsEnabled, 108);
