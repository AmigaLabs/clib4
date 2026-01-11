/** @file
 *
 * Definitions for the Wireshark Memory Manager Allocator
 * Copyright 2012, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __WMEM_ALLOCATOR_H__
#define __WMEM_ALLOCATOR_H__

#include <features.h>

#include "wmem_core.h"

#include <string.h>

__BEGIN_DECLS

struct _wmem_user_cb_container_t;

/* See section "4. Internal Design" of doc/README.wmem for details
 * on this structure */
struct _wmem_allocator_t {
    /* Consumer functions */
    void *(*walloc)(void *private_data, const size_t size, int32_t alignment);
    void  (*wfree)(void *private_data, void *ptr);
    void *(*wrealloc)(void *private_data, void *ptr, const size_t size, int32_t alignment);

    /* Producer/Manager functions */
    void  (*free_all)(void *private_data);
    void  (*gc)(void *private_data);
    void  (*cleanup)(void *private_data);

    /* Callback List */
    struct _wmem_user_cb_container_t *callbacks;

    /* Implementation details */
    void                        *private_data;
    enum _wmem_allocator_type_t  type;
    bool                         in_scope;
};

__END_DECLS

#endif /* __WMEM_ALLOCATOR_H__ */

