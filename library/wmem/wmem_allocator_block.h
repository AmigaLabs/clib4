/** @file
 *
 * Definitions for the Wireshark Memory Manager Large-Block Allocator
 * Copyright 2012, Evan Huus <eapache@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef __WMEM_ALLOCATOR_BLOCK_H__
#define __WMEM_ALLOCATOR_BLOCK_H__

#include "wmem_core.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
wmem_block_allocator_init(wmem_allocator_t *allocator);

/* How many OS blocks the table of a block allocator holds. */
#define WMEM_OS_BLOCKS  256
#define WMEM_OS_NOTABLE (-1)    /* no block allocator, or none to read */
#define WMEM_OS_LOST    (-2)    /* the table does not name them all */

/* The OS blocks of a block allocator, read with no lock: for a program nobody of runs. */
int
wmem_block_os_blocks(const wmem_allocator_t *allocator, void **addr, size_t *size, int max);

/* What wmem_block_live_ranges tells, for each OS block: the block first, then what of it may be in use. */
#define WMEM_LIVE_BLOCK 0   /* an OS block of the allocator */
#define WMEM_LIVE_RANGE 1   /* used chunks in a row, their headers included */
#define WMEM_LIVE_WHOLE 2   /* all of the block: a jumbo one, or one whose chunks do not add up */

typedef void (*wmem_live_fn)(void *data, int kind, void *addr, size_t size);

/* The OS blocks of a block allocator and the ranges in them that may be in use, read with no lock and
 * no write: for a program nobody of runs. Returns the number of blocks, or WMEM_OS_NOTABLE or WMEM_OS_LOST. */
int
wmem_block_live_ranges(const wmem_allocator_t *allocator, wmem_live_fn say, void *data);

/* Exposed only for testing purposes */
void
wmem_block_verify(wmem_allocator_t *allocator);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WMEM_ALLOCATOR_BLOCK_H__ */

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
