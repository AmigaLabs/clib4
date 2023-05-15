#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>
#include <debug.h>

struct _clib2 *
__getClib2(void) {
    ENTER();

    struct _clib2 *r = NULL;
    struct Task *t = FindTask(NULL);
    SHOWPOINTER(t);

    if (NT_PROCESS == t->tc_Node.ln_Type) {
        r = (struct _clib2 *) ((struct Process *) t)->pr_CLibData;
        SHOWPOINTER(r);
    }

    RETURN(r);
    return r;
}
