#include <proto/exec.h>
#include <proto/dos.h>
#include <stdio.h>
#include <stdlib.h>

struct Clib4IFace *IClib4 = NULL;
struct ExecIFace *IExec = NULL;
struct DOSIFace *IDOS = NULL;
struct Library *DOSBase = NULL;
struct Library *SysBase = NULL;

int32 _start(char *args, int32 arglen, struct ExecBase *sysbase);

int32
_start(char *argstring, int32 arglen, struct ExecBase *sysbase) {
    SysBase = (struct Library *) sysbase;
    IExec = (struct ExecIFace *) sysbase->MainInterface;

    DOSBase = (struct Library *) OpenLibrary("dos.library", 53);
    if (!DOSBase) {
        return -1;
    }

    IDOS = (struct DOSIFace *) GetInterface(DOSBase, "main", 1, NULL);
    if (!IDOS) {
        CloseLibrary(DOSBase);
    }

    struct Library *Clib4Library = OpenLibrary("clib4.library", 0);
    if (Clib4Library) {
        IClib4 = (struct Clib4IFace *) GetInterface(Clib4Library, "main", 1, NULL);
        if (!IClib4) {
            Printf("Cannot get clib4 interface\n");
        }
    } else {
        Printf("Cannot open clib4.library\n");
    }

    void *mem = AllocVecTags(10, AVT_Type, MEMF_SHARED, TAG_DONE);
    if (mem) {
        Printf("AllocVecTags ok\n");
        FreeVec(mem);
    } else {
        Printf("AllocVecTags ko\n");
    }

    if (IClib4) {
        DropInterface((struct Interface *) IClib4);
        IClib4 = NULL;
    }
    if (Clib4Library) {
        CloseLibrary(Clib4Library);
        Clib4Library = NULL;
    }

    if (IDOS) {
        DropInterface((struct Interface *) IDOS);
        IDOS = NULL;
    }
    if (DOSBase) {
        CloseLibrary(DOSBase);
        DOSBase = NULL;
    }

    return 0;
}
