/** @file
 *
 * Definitions for the Wireshark Memory Manager User Callback Internals
 * Copyright 2012, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __WMEM_USER_CB_INT_H__
#define __WMEM_USER_CB_INT_H__

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include "wmem_user_cb.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void wmem_call_callbacks(wmem_allocator_t *allocator, wmem_cb_event_t event) __attribute__ ((visibility ("hidden"))) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WMEM_USER_CB_INT_H__ */

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
