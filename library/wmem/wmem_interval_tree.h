/** @file
 * Definitions for the Wireshark Memory Manager Red-Black Tree
 * Based on the red-black tree implementation in epan/emem.*
 * Copyright 2015, Matthieu coudron <matthieu.coudron@lip6.fr>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef __WMEM_INTERVAL_TREE_H__
#define __WMEM_INTERVAL_TREE_H__

#include "wmem_core.h"
#include "wmem_tree.h"
#include "wmem_list.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup wmem
 *  @{
 *    @defgroup wmem-interval-tree Interval Tree
 *
 * http://www.geeksforgeeks.org/interval-tree/
 * The idea is to augment a self-balancing Binary Search Tree (BST) like Red Black Tree, AVL Tree, etc ...
 * to maintain a set of intervals so that all operations can be done in O(Logn) time.
 *    @{
 * Following wikipedia's convention this is an augmented tree rather then an interval tree
 * http://www.wikiwand.com/en/Interval_tree
 */

struct _wmem_tree_t;
typedef struct _wmem_tree_t wmem_itree_t;

struct _wmem_range_t {
    uint64_t low;        /* low is used as the key in the binary tree */
    uint64_t high;       /* Max value of the range */
    uint64_t max_edge;   /* max value among subtrees */
};

extern
wmem_itree_t *
wmem_itree_new(wmem_allocator_t *allocator)
 __attribute__((__malloc__));


/** Returns true if the tree is empty (has no nodes). */
extern
bool
wmem_itree_is_empty(wmem_itree_t *tree);


/** Inserts a range low-high indexed by "low" in O(log(n)).
 * As in wmem_tree, if a key "low" already exists, it will be overwritten with the new data
 *
 */
extern
void
wmem_itree_insert(wmem_itree_t *tree, const uint64_t low, const uint64_t high, void *data);


/*
 * Save results in a wmem_list with the scope passed as a parameter.
 * wmem_list_t is always allocated even if there is no result
 */
extern
wmem_list_t *
wmem_itree_find_intervals(wmem_itree_t *tree, wmem_allocator_t *allocator, uint64_t low, uint64_t high);


/**
 * Print ranges along the tree
 */
void
wmem_print_itree(wmem_itree_t *tree);

/**   @}
 *  @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WMEM_INTERVAL_TREE_H__ */

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
