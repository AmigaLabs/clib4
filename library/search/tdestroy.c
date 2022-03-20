/*
 * $Id: search_tdestroy.c,v 1.0 2021-02-21 19:38:14 clib2devs Exp $
*/

#define _SEARCH_PRIVATE
#include <search.h>
#include <string.h>
#include <stdlib.h>

/* Walk the nodes of a tree */
static void
trecurse(node_t *root, void (*free_action)(void *))
{
    if (root->llink != NULL)
        trecurse(root->llink, free_action);
    if (root->rlink != NULL)
        trecurse(root->rlink, free_action);

    (*free_action)((void *)root->key);
    free(root);
}

void 
tdestroy(void *vrootp, void (*freefct)(void *))
{
    node_t *root = (node_t *)vrootp;

    if (root != NULL)
        trecurse(root, freefct);
}