/*
 * $Id: search_twalk.c,v 1.0 2021-02-21 19:38:14 clib2devs Exp $
*/

#define _SEARCH_PRIVATE
#include <search.h>
#include <string.h>
#include <stdlib.h>

static void
trecurse(const node_t *root, void (*action)(const void *, VISIT, int), int level)
{

    if (root->llink == NULL && root->rlink == NULL)
        (*action)(root, leaf, level);
    else
    {
        (*action)(root, preorder, level);
        if (root->llink != NULL)
            trecurse(root->llink, action, level + 1);
        (*action)(root, postorder, level);
        if (root->rlink != NULL)
            trecurse(root->rlink, action, level + 1);
        (*action)(root, endorder, level);
    }
}

/* Walk the nodes of a tree */
void 
twalk(const void *vroot, void (*action)(const void *, VISIT, int))
{
    if (vroot != NULL && action != NULL)
        trecurse(vroot, action, 0);
}