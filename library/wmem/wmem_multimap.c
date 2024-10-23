/* wmem_multimap.c
 * Wireshark Memory Manager Hash Multimap
 * Copyright 2021, John Thacker <johnthacker@gmail.com>
 * Copyright 2014, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include "wmem_core.h"
#include "wmem_list.h"
#include "wmem_map.h"
#include "wmem_multimap.h"
#include "wmem_tree.h"
#include "wmem_user_cb.h"

struct _wmem_multimap_t {

    wmem_map_t *map;

    unsigned metadata_scope_cb_id;
    unsigned data_scope_cb_id;

    wmem_allocator_t *metadata_allocator;
    wmem_allocator_t *data_allocator;
};

wmem_multimap_t *
wmem_multimap_new(wmem_allocator_t *allocator,
                  GHashFunc hash_func, GEqualFunc eql_func) {
    wmem_multimap_t *multimap;

    multimap = wmem_new(allocator, wmem_multimap_t);

    multimap->map = wmem_map_new(allocator, hash_func, eql_func);
    multimap->metadata_allocator = allocator;
    multimap->data_allocator = allocator;

    return multimap;
}

static bool
wmem_multimap_reset_cb(wmem_allocator_t *allocator, wmem_cb_event_t event, void *user_data) {
    (void) (allocator);

    wmem_multimap_t *multimap = (wmem_multimap_t *) user_data;

    if (event == WMEM_CB_DESTROY_EVENT) {
        wmem_unregister_callback(multimap->metadata_allocator, multimap->metadata_scope_cb_id);
        wmem_free(multimap->metadata_allocator, multimap);
    }

    return true;
}

static bool
wmem_multimap_destroy_cb(wmem_allocator_t *allocator, wmem_cb_event_t event, void *user_data) {
    (void) (allocator);
    (void) (event);

    wmem_multimap_t *multimap = (wmem_multimap_t *) user_data;

    wmem_unregister_callback(multimap->data_allocator, multimap->data_scope_cb_id);

    return false;
}

wmem_multimap_t *
wmem_multimap_new_autoreset(wmem_allocator_t *metadata_scope, wmem_allocator_t *data_scope,
                            GHashFunc hash_func, GEqualFunc eql_func) {
    wmem_multimap_t *multimap;

    multimap = wmem_new(metadata_scope, wmem_multimap_t);

    multimap->map = wmem_map_new_autoreset(metadata_scope, data_scope, hash_func, eql_func);
    multimap->metadata_allocator = metadata_scope;
    multimap->data_allocator = data_scope;

    multimap->metadata_scope_cb_id = wmem_register_callback(metadata_scope, wmem_multimap_destroy_cb, multimap);
    multimap->data_scope_cb_id = wmem_register_callback(data_scope, wmem_multimap_reset_cb, multimap);

    return multimap;
}

wmem_list_t *
wmem_multimap_get_keys(wmem_allocator_t *list_allocator, wmem_multimap_t *map) {
    return wmem_map_get_keys(list_allocator, map->map);
}

static void
count_nodes(void *key, void *value, void *user_data) {
    (void) (key);

    unsigned *count = (unsigned *) user_data;
    (*count) += wmem_tree_count(value);
}

unsigned
wmem_multimap_size(wmem_multimap_t *map) {
    unsigned count = 0;

    wmem_map_foreach(map->map, count_nodes, &count);
    return count;
}

unsigned
wmem_multimap_count(wmem_multimap_t *map, const void *key) {
    wmem_tree_t *tree;

    if ((tree = wmem_map_lookup(map->map, key)) == NULL) {
        return 0;
    }
    return wmem_tree_count(tree);
}

bool
wmem_multimap_insert32(wmem_multimap_t *map, const void *key, uint32_t frame_num, void *value) {
    wmem_tree_t *tree;
    bool ret = true;

    if ((tree = wmem_map_lookup(map->map, key)) == NULL) {
        tree = wmem_tree_new(map->data_allocator);
        wmem_map_insert(map->map, key, tree);
        ret = false;
    }
    wmem_tree_insert32(tree, frame_num, value);

    return ret;
}

void *
wmem_multimap_lookup32(wmem_multimap_t *map, const void *key, uint32_t frame_num) {
    wmem_tree_t *tree;

    if ((tree = wmem_map_lookup(map->map, key)) == NULL) {
        return NULL;
    }
    return wmem_tree_lookup32(tree, frame_num);
}

void *
wmem_multimap_lookup32_le(wmem_multimap_t *map, const void *key, uint32_t frame_num) {
    wmem_tree_t *tree;

    if ((tree = wmem_map_lookup(map->map, key)) == NULL) {
        return NULL;
    }
    return wmem_tree_lookup32_le(tree, frame_num);
}

void *
wmem_multimap_remove32(wmem_multimap_t *map, const void *key, const uint32_t frame_num) {
    wmem_tree_t *tree;

    if ((tree = wmem_map_lookup(map->map, key)) == NULL) {
        return NULL;
    }
    return wmem_tree_remove32(tree, frame_num);
}

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
