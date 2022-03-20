/*
 * $Id: search_tfind.c,v 1.0 2021-02-21 19:38:14 clib2devs Exp $
*/

#define _SEARCH_PRIVATE
#include <search.h>
#include <string.h>

/* find a node, or return 0 */
void *
tfind(const void *vkey, void **vrootp, int (*compar)(const void *, const void *))
{
    node_t **rootp = (node_t **)vrootp;

    if (rootp == NULL)
        return NULL;

    while (*rootp != NULL)
    { /* T1: */
        int r;

        if ((r = (*compar)(vkey, (*rootp)->key)) == 0) /* T2: */
            return *rootp;                             /* key found */
        rootp = (r < 0) ? &(*rootp)->llink :           /* T3: follow left branch */
                    &(*rootp)->rlink;                  /* T4: follow right branch */
    }
    return NULL;
}