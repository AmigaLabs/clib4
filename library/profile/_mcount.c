/*
 * $Id: profile__mcount.c,v 1.0 2022-08-06 10:36:26 clib2devs Exp $
*/

#include <exec/exec.h>
#include <proto/exec.h>
#include <stddef.h>
#include <stdio.h>

#include "gmon.h"

void __mcount(uint32 frompc, uint32 selfpc);

void
__mcount(uint32 frompc, uint32 selfpc) {
    register ARCINDEX *frompcindex;
    register struct tostruct *top, *prevtop;
    register struct gmonparam *p;
    register ARCINDEX toindex;
    int i;

    p = &_gmonparam;

    if (p->state != kGmonProfOn)
        return;

    p->state = kGmonProfBusy;

    /*
     * Check if the PC is inside our text segment.
     * Really should be...
     */
    dprintf("p->textsize %x\n", p->textsize);
    dprintf("p->lowpc %x\n", p->lowpc);
    dprintf("frompc1 %x\n", frompc);
    dprintf("selfpc1 %x\n", selfpc);
    frompc -= p->lowpc;
    selfpc -= p->lowpc;
    dprintf("frompc2 %x\n", frompc);
    dprintf("selfpc2 %x\n", selfpc);
    if (frompc > p->textsize)
        goto done;

    /* The following test used to be
		if (p->log_hashfraction >= 0)
	   But we can simplify this if we assume the profiling data
	   is always initialized by the functions in gmon.c.  But
	   then it is possible to avoid a runtime check and use the
	   same `if' as in gmon.c.  So keep these tests in sync.
    */
    if ((HASHFRACTION & (HASHFRACTION - 1)) == 0) {
        /* avoid integer divide if possible: */
        i = frompc >> p->log_hashfraction;
    } else {
        i = frompc / (p->hashfraction * sizeof(*p->froms));
    }

    frompcindex = &p->froms[i];
    toindex = *frompcindex;
    if (toindex == 0) {
        /*
         *	first time traversing this arc
         */
        toindex = ++p->tos[0].link;
        if (toindex >= p->tolimit)
            /* halt further profiling */
            goto overflow;

        *frompcindex = toindex;
        top = &p->tos[toindex];
        top->selfpc = selfpc;
        top->count = 1;
        top->link = 0;
        goto done;
    }
    top = &p->tos[toindex];
    printf("p->lowpc = %p - top->selfpc = %x - selfpc = %x\n", p->lowpc, top->selfpc, selfpc);
    if (top->selfpc == selfpc) {
        /* arc at front of chain; usual case. */
        top->count++;
        goto done;
    }
    /*
     * have to go looking down chain for it.
     * top points to what we are looking at,
     * prevtop points to previous top.
     * we know it is not at the head of the chain.
     */
    for (;; ) {
        if (top->link == 0) {
            /*
             * top is end of the chain and none of the chain
             * had top->selfpc == selfpc.
             * so we allocate a new tostruct
             * and link it to the head of the chain.
             */
            toindex = ++p->tos[0].link;
            if (toindex >= p->tolimit)
                goto overflow;

            top = &p->tos[toindex];
            top->selfpc = selfpc;
            top->count = 1;
            top->link = *frompcindex;
            *frompcindex = toindex;
            goto done;
        }
        /*
         * otherwise, check the next arc on the chain.
         */
        prevtop = top;
        top = &p->tos[top->link];
        if (top->selfpc == selfpc) {
            /*
             * there it is.
             * increment its count
             * move it to the head of the chain.
             */
            top->count++;
            toindex = prevtop->link;
            prevtop->link = top->link;
            top->link = *frompcindex;
            *frompcindex = toindex;
            goto done;
        }
    }

done:
    p->state = kGmonProfOn;
    return;

overflow:
    p->state = kGmonProfError;
    return;
}
