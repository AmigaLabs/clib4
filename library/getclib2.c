#include <proto/exec.h>
#include <dos/dosextens.h>
#include <dos.h>
#include <proto/dos.h>
#include <debug.h>

#include "shared_library/clib2.h"
#include "map.h"

struct _clib2 *
__getClib2(void) {
    struct _clib2 *r = NULL;

    struct Task *t = FindTask(NULL);
    if (NT_PROCESS == t->tc_Node.ln_Type) {
        uint32 value;
        /* Get _clib2 address stored into process pr_UID field */
        GetOwnerInfoTags(OI_ProcessInput, 0, OI_OwnerUID, &value, TAG_END);
        r = (struct _clib2 *) value;
    }

    if (!r) {
        struct Clib2Resource *res = (APTR) OpenResource(RESOURCE_NAME);
        if (res) {
            return res->fallbackClib;
        }
    }
    return r;
}
