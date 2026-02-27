/** @file
 * Definitions for the Wireshark Memory Manager Misc Utilities
 * Copyright 2013, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __WMEM_MISCUTL_H__
#define __WMEM_MISCUTL_H__

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
 *    @defgroup wmem-strutl String Utilities
 *
 *    A collection of misc. utility functions for wmem.
 *
 *    @{
 */

/** Copies a block of memory.
 *
 * @param allocator The allocator object to use to allocate memory to copy into.
 * @param source The pointer to the memory block to copy.
 * @param size The amount of memory to copy.
 * @return The location of the memory copy or NULL if size is 0.
 */
extern
void *
wmem_memdup(wmem_allocator_t *allocator, const void *source, const size_t size) __attribute__((__alloc_size__(3)));

/** Generic GCompareFunc implementations to compare signed/unsigned integer
 */
extern
int
wmem_compare_int(const void *a, const void *b);

extern
int
wmem_compare_uint(const void *a, const void *b);

/**   @}
 *  @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WMEM_MISCUTL_H__ */

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
