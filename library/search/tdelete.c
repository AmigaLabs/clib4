/*
 * $Id: search_tdelete.c,v 1.0 2021-02-21 19:38:14 clib4devs Exp $
*/

#define _SEARCH_PRIVATE
#include <search.h>
#include <string.h>
#include <stdlib.h>

/* delete node with given key */
void *
tdelete(const void *vkey, void **vrootp, int (*compar)(const void *, const void *))
{
    node_t **rootp = (node_t **)vrootp;
    node_t *p, *q, *r;
    int cmp;

    if (rootp == NULL || (p = *rootp) == NULL)
        return NULL;

    while ((cmp = (*compar)(vkey, (*rootp)->key)) != 0)
    {
        p = *rootp;
        rootp = (cmp < 0) ? &(*rootp)->llink : /* follow llink branch */
                    &(*rootp)->rlink;          /* follow rlink branch */
        if (*rootp == NULL)
            return NULL; /* key not found */
    }
    r = (*rootp)->rlink;               /* D1: */
    if ((q = (*rootp)->llink) == NULL) /* Left NULL? */
        q = r;
    else if (r != NULL)
    { /* Right link is NULL? */
        if (r->llink == NULL)
        { /* D2: Find successor */
            r->llink = q;
            q = r;
        }
        else
        { /* D3: Find NULL link */
            for (q = r->llink; q->llink != NULL; q = r->llink)
                r = q;
            r->llink = q->rlink;
            q->llink = (*rootp)->llink;
            q->rlink = (*rootp)->rlink;
        }
    }
    free(*rootp); /* D4: Free node */
    *rootp = q;   /* link parent to new node */
    return p;
}
