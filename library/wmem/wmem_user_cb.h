/** @file
 * Definitions for the Wireshark Memory Manager User Callbacks
 * Copyright 2012, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __WMEM_USER_CB_H__
#define __WMEM_USER_CB_H__

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup wmem
 *  @{
 *    @defgroup wmem-user-cb User Callbacks
 *
 *    User callbacks.
 *
 *    @{
 */

/** The events that can trigger a callback. */
typedef enum _wmem_cb_event_t {
    WMEM_CB_FREE_EVENT,    /**< wmem_free_all() */
    WMEM_CB_DESTROY_EVENT  /**< wmem_destroy_allocator() */
} wmem_cb_event_t;

/** Function signature for registered user callbacks.
 *
 * allocator The allocator that triggered this callback.
 * event     The event type that triggered this callback.
 * user_data Whatever user_data was originally passed to the call to
 *                  wmem_register_callback().
 * @return          false to unregister the callback, true otherwise.
 */
typedef bool (*wmem_user_cb_t) (wmem_allocator_t*, wmem_cb_event_t, void*);

/** Register a callback function with the given allocator pool.
 *
 * @param allocator The allocator with which to register the callback.
 * @param callback  The function to be called as the callback.
 * @param user_data An arbitrary data pointer that is passed to the callback as
 *                  a way to specify extra parameters or store extra data. Note
 *                  that this pointer is not freed when a callback is finished,
 *                  you have to do that yourself in the callback, or just
 *                  allocate it in the appropriate wmem pool.
 * @return          ID of this callback that can be passed back to
 *                  wmem_unregister_callback().
 */
extern
unsigned
wmem_register_callback(wmem_allocator_t *allocator, wmem_user_cb_t callback,
        void *user_data);

/** Unregister the callback function with the given ID.
 *
 * @param allocator The allocator from which to unregister the callback.
 * @param id        The callback id as returned from wmem_register_callback().
 */
extern
void
wmem_unregister_callback(wmem_allocator_t *allocator, unsigned id);

/**   @}
 *  @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WMEM_USER_CB_H__ */

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
