#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>
#include <debug.h>

extern struct _clib2 *__clib2;
extern struct _global_clib2 *__global_clib2;

struct _clib2 *
__getClib2(void) {
    ENTER();
    struct _clib2 *r = NULL;
    struct Task *t = FindTask(NULL);

    if (NT_PROCESS == t->tc_Node.ln_Type) {
        r = (struct _clib2 *) ((struct Process *) t)->pr_CLibData;
    }

    SHOWPOINTER(r);

    if (!r) {
        r = __clib2;
    }

    SHOWPOINTER(r);

    RETURN(r);
    return r;
}

struct _global_clib2 *
__getGlobalClib2(void) {
    ENTER();

    SHOWPOINTER(__global_clib2);

    RETURN(__global_clib2);
    return __global_clib2;
}