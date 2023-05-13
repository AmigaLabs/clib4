#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>
#include <debug.h>

extern struct _clib2 *__fallback_clib2;

struct _clib2 *
__getClib2(void) {
    ENTER();

    struct _clib2 *r = NULL;
    struct Task *t = FindTask(NULL);

    if (NT_PROCESS == t->tc_Node.ln_Type) {
        r = (struct _clib2 *) ((struct Process *) t)->pr_CLibData;
        SHOWPOINTER(r);
    }

    if (!r) {
        r = __fallback_clib2;
        SHOWPOINTER(r);
    }
    RETURN(r);
    return r;
}
