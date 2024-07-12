/*
 * $Id: search_tsearch.c,v 1.0 2021-02-21 19:38:14 clib4devs Exp $
*/

#define _SEARCH_PRIVATE

#include <search.h>
#include <string.h>
#include <stdlib.h>

/* find or insert datum into search tree */
void *
tsearch(const void *vkey, void **vrootp, int (*compar)(const void *, const void *)) {
    node_t *q;
    node_t **rootp = (node_t **) vrootp;

    if (rootp == NULL)
        return NULL;

    while (*rootp != NULL) { /* Knuth's T1: */
        int r;

        if ((r = (*compar)(vkey, (*rootp)->key)) == 0) /* T2: */
            return *rootp;                             /* we found it! */

        rootp = (r < 0) ? &(*rootp)->llink : /* T3: follow left branch */
                &(*rootp)->rlink;        /* T4: follow right branch */
    }

    q = malloc(sizeof(node_t)); /* T5: key not found */
    if (q != 0) {               /* make new node */
        *rootp = q; /* link new node to old */
        /* LINTED const castaway ok */
        q->key = (void *) vkey; /* initialize new node */
        q->llink = q->rlink = NULL;
    }
    return q;
}
