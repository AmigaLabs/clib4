/*
 * $Id: amiga_newlist.c,v 1.4 2006-09-25 15:12:47 clib4devs Exp $
*/

#include <exec/lists.h>
#include <clib/alib_protos.h>

#include "debug.h"

VOID NewList(struct List *lh);

VOID
NewList(struct List *lh) {
    ENTER();

    SHOWPOINTER(lh);

    assert(lh != NULL);

    lh->lh_Head = (struct Node *) &lh->lh_Tail;
    lh->lh_Tail = NULL;
    lh->lh_TailPred = (struct Node *) &lh->lh_Head;

    LEAVE();
}
