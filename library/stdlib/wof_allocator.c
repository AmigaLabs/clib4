/* Wheel-of-Fortune Memory Allocator
 * Copyright 2013, Evan Huus <eapache@gmail.com>
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <features.h>

#include "wof_allocator.h"

/* MASTER/RECYCLER HELPERS */

/* Cycles the recycler. See the design notes in the readme for more details. */
static void
wof_cycle_recycler(wof_allocator_t *allocator) {
    wof_chunk_hdr_t *chunk;
    wof_free_hdr_t *free_chunk;

    ObtainSemaphore(allocator->semaphore);

    chunk = allocator->recycler_head;

    if (chunk == NULL) {
        ReleaseSemaphore(allocator->semaphore);
        return;
    }

    free_chunk = WOF_GET_FREE(chunk);

    if (free_chunk->next->len < chunk->len) {
        /* Hold the current head fixed during rotation. */
        WOF_GET_FREE(free_chunk->next)->prev = free_chunk->prev;
        WOF_GET_FREE(free_chunk->prev)->next = free_chunk->next;

        free_chunk->prev = free_chunk->next;
        free_chunk->next = WOF_GET_FREE(free_chunk->next)->next;

        WOF_GET_FREE(free_chunk->next)->prev = chunk;
        WOF_GET_FREE(free_chunk->prev)->next = chunk;
    } else {
        /* Just rotate everything. */
        allocator->recycler_head = free_chunk->next;
    }
    ReleaseSemaphore(allocator->semaphore);
}

/* Adds a chunk from the recycler. */
static void
wof_add_to_recycler(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk) {
    wof_free_hdr_t *free_chunk;

    if (WOF_CHUNK_DATA_LEN(chunk) < WOF_FREE_HEADER_SIZE) {
        return;
    }

    free_chunk = WOF_GET_FREE(chunk);

    if (!allocator->recycler_head) {
        /* First one */
        free_chunk->next = chunk;
        free_chunk->prev = chunk;
        allocator->recycler_head = chunk;
    } else {
        free_chunk->next = allocator->recycler_head;
        free_chunk->prev = WOF_GET_FREE(allocator->recycler_head)->prev;

        WOF_GET_FREE(free_chunk->next)->prev = chunk;
        WOF_GET_FREE(free_chunk->prev)->next = chunk;

        if (chunk->len > allocator->recycler_head->len) {
            allocator->recycler_head = chunk;
        }
    }
}

/* Removes a chunk from the recycler. */
static void
wof_remove_from_recycler(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk) {
    wof_free_hdr_t *free_chunk;

    free_chunk = WOF_GET_FREE(chunk);

    if (free_chunk->prev == chunk && free_chunk->next == chunk) {
        /* Only one item in recycler, just empty it. */
        allocator->recycler_head = NULL;
    } else {
        /* Two or more items, usual doubly-linked-list removal. It's circular
         * so we don't need to worry about null-checking anything, which is
         * nice. */
        WOF_GET_FREE(free_chunk->prev)->next = free_chunk->next;
        WOF_GET_FREE(free_chunk->next)->prev = free_chunk->prev;
        if (allocator->recycler_head == chunk) {
            allocator->recycler_head = free_chunk->next;
        }
    }
}

/* Pushes a chunk onto the master stack. */
static void
wof_push_master(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk) {
    wof_free_hdr_t *free_chunk;

    free_chunk = WOF_GET_FREE(chunk);
    free_chunk->prev = NULL;
    free_chunk->next = allocator->master_head;
    if (free_chunk->next) {
        WOF_GET_FREE(free_chunk->next)->prev = chunk;
    }
    allocator->master_head = chunk;
}

/* Removes the top chunk from the master stack. */
static void
wof_pop_master(wof_allocator_t *allocator) {
    wof_chunk_hdr_t *chunk;
    wof_free_hdr_t *free_chunk;

    chunk = allocator->master_head;

    free_chunk = WOF_GET_FREE(chunk);

    allocator->master_head = free_chunk->next;
    if (free_chunk->next) {
        WOF_GET_FREE(free_chunk->next)->prev = NULL;
    }
}

/* CHUNK HELPERS */

/* Takes a free chunk and checks the chunks to its immediate right and left in
 * the block. If they are also free, the contigous free chunks are merged into
 * a single free chunk. The resulting chunk ends up in either the master list or
 * the recycler, depending on where the merged chunks were originally.
 */
static void
wof_merge_free(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk) {
    wof_chunk_hdr_t *tmp;
    wof_chunk_hdr_t *left_free = NULL;
    wof_chunk_hdr_t *right_free = NULL;

    /* Check the chunk to our right. If it is free, merge it into our current
     * chunk. If it is big enough to hold a free-header, save it for later (we
     * need to know about the left chunk before we decide what goes where). */
    tmp = WOF_CHUNK_NEXT(chunk);
    if (tmp && !tmp->used) {
        if (WOF_CHUNK_DATA_LEN(tmp) >= WOF_FREE_HEADER_SIZE) {
            right_free = tmp;
        }
        chunk->len += tmp->len;
        chunk->last = tmp->last;
    }

    /* Check the chunk to our left. If it is free, merge our current chunk into
     * it (thus chunk = tmp). As before, save it if it has enough space to
     * hold a free-header. */
    tmp = WOF_CHUNK_PREV(chunk);
    if (tmp && !tmp->used) {
        if (WOF_CHUNK_DATA_LEN(tmp) >= WOF_FREE_HEADER_SIZE) {
            left_free = tmp;
        }
        tmp->len += chunk->len;
        tmp->last = chunk->last;
        chunk = tmp;
    }

    /* The length of our chunk may have changed. If we have a chunk following,
     * update its 'prev' count. */
    if (!chunk->last) {
        WOF_CHUNK_NEXT(chunk)->prev = chunk->len;
    }

    /* Now that the chunk headers are merged and consistent, we need to figure
     * out what goes where in which free list. */
    if (right_free && right_free == allocator->master_head) {
        /* If we merged right, and that chunk was the head of the master list,
         * then we leave the resulting chunk at the head of the master list. */
        wof_free_hdr_t *moved;
        if (left_free) {
            wof_remove_from_recycler(allocator, left_free);
        }
        moved = WOF_GET_FREE(chunk);
        moved->prev = NULL;
        moved->next = WOF_GET_FREE(right_free)->next;
        allocator->master_head = chunk;
        if (moved->next) {
            WOF_GET_FREE(moved->next)->prev = chunk;
        }
    } else {
        /* Otherwise, we remove the right-merged chunk (if there was one) from
         * the recycler. Then, if we merged left we have nothing to do, since
         * that recycler entry is still valid. If not, we add the chunk. */
        if (right_free) {
            wof_remove_from_recycler(allocator, right_free);
        }
        if (!left_free) {
            wof_add_to_recycler(allocator, chunk);
        }
    }
}

/* Takes an unused chunk and a size, and splits it into two chunks if possible.
 * The first chunk (at the same address as the input chunk) is guaranteed to
 * hold at least `size` bytes of data, and to not be in either the master or
 * recycler lists.
 *
 * The second chunk gets whatever data is left over. It is marked unused and
 * replaces the input chunk in whichever list it originally inhabited. */
static void
wof_split_free_chunk(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk,  const size_t size) {
    wof_chunk_hdr_t *extra;
    wof_free_hdr_t *old_blk, *new_blk;
    size_t aligned_size, available;
    BOOL last;

    aligned_size = WOF_ALIGN_SIZE(size) + WOF_CHUNK_HEADER_SIZE;

    if (WOF_CHUNK_DATA_LEN(chunk) < aligned_size + WOF_FREE_HEADER_SIZE) {
        /* If the available space is not enough to store all of
         * (hdr + requested size + alignment padding + hdr + free-header) then
         * just remove the current chunk from the free list and return, since we
         * can't usefully split it. */
        if (chunk == allocator->master_head) {
            wof_pop_master(allocator);
        } else if (WOF_CHUNK_DATA_LEN(chunk) >= WOF_FREE_HEADER_SIZE) {
            wof_remove_from_recycler(allocator, chunk);
        }
        return;
    }

    /* preserve a few values from chunk that we'll need to manipulate */
    last = chunk->last;
    available = chunk->len - aligned_size;

    /* set new values for chunk */
    chunk->len = (int) aligned_size;
    chunk->last = FALSE;

    /* with chunk's values set, we can use the standard macro to calculate
     * the location and size of the new free chunk */
    extra = WOF_CHUNK_NEXT(chunk);

    /* Now we move the free chunk's address without changing its location
     * in whichever list it is in.
     *
     * Note that the new chunk header 'extra' may overlap the old free header,
     * so we have to copy the free header before we write anything to extra.
     */
    old_blk = WOF_GET_FREE(chunk);
    new_blk = WOF_GET_FREE(extra);

    if (allocator->master_head == chunk) {
        new_blk->prev = old_blk->prev;
        new_blk->next = old_blk->next;

        if (old_blk->next) {
            WOF_GET_FREE(old_blk->next)->prev = extra;
        }

        allocator->master_head = extra;
    } else {
        if (old_blk->prev == chunk) {
            new_blk->prev = extra;
            new_blk->next = extra;
        } else {
            new_blk->prev = old_blk->prev;
            new_blk->next = old_blk->next;

            WOF_GET_FREE(old_blk->prev)->next = extra;
            WOF_GET_FREE(old_blk->next)->prev = extra;
        }

        if (allocator->recycler_head == chunk) {
            allocator->recycler_head = extra;
        }
    }

    /* Now that we've copied over the free-list stuff (which may have overlapped
     * with our new chunk header) we can safely write our new chunk header. */
    extra->len = (int) available;
    extra->last = last;
    extra->prev = chunk->len;
    extra->used = FALSE;
    extra->jumbo = FALSE;

    /* Correctly update the following chunk's back-pointer */
    if (!last) {
        WOF_CHUNK_NEXT(extra)->prev = extra->len;
    }
}

/* Takes a used chunk and a size, and splits it into two chunks if possible.
 * The first chunk can hold at least `size` bytes of data, while the second gets
 * whatever's left over. The second is marked as unused and is added to the
 * recycler. */
static void
wof_split_used_chunk(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk, const size_t size) {
    wof_chunk_hdr_t *extra;
    size_t aligned_size, available;
    BOOL last;

    aligned_size = WOF_ALIGN_SIZE(size) + WOF_CHUNK_HEADER_SIZE;

    if (aligned_size > WOF_CHUNK_DATA_LEN(chunk)) {
        /* in this case we don't have enough space to really split it, so
         * it's basically a no-op */
        return;
    }
    /* otherwise, we have room to split it, though the remaining free chunk
     * may still not be usefully large */

    /* preserve a few values from chunk that we'll need to manipulate */
    last = chunk->last;
    available = chunk->len - aligned_size;

    /* set new values for chunk */
    chunk->len = (int) aligned_size;
    chunk->last = FALSE;

    /* with chunk's values set, we can use the standard macro to calculate
     * the location and size of the new free chunk */
    extra = WOF_CHUNK_NEXT(chunk);

    /* set the new values for the chunk */
    extra->len = (int) available;
    extra->last = last;
    extra->prev = chunk->len;
    extra->used = FALSE;
    extra->jumbo = FALSE;

    /* Correctly update the following chunk's back-pointer */
    if (!last) {
        WOF_CHUNK_NEXT(extra)->prev = extra->len;
    }

    /* Merge it to its right if possible (it can't be merged left, obviously).
     * This also adds it to the recycler. */
    wof_merge_free(allocator, extra);
}

/* BLOCK HELPERS */

/* Add a block to the allocator's embedded doubly-linked list of OS-level blocks
 * that it owns. */
static void
wof_add_to_block_list(wof_allocator_t *allocator,  wof_block_hdr_t *block) {
    block->prev = NULL;
    block->next = allocator->block_list;
    if (block->next) {
        block->next->prev = block;
    }
    allocator->block_list = block;
}

/* Remove a block from the allocator's embedded doubly-linked list of OS-level
 * blocks that it owns. */
static void
wof_remove_from_block_list(wof_allocator_t *allocator, wof_block_hdr_t *block) {
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        allocator->block_list = block->next;
    }

    if (block->next) {
        block->next->prev = block->prev;
    }
}

/* Initializes a single unused chunk at the beginning of the block, and
 * adds that chunk to the free list. */
static void
wof_init_block(wof_allocator_t *allocator,
               wof_block_hdr_t *block) {
    wof_chunk_hdr_t *chunk;

    /* a new block contains one chunk, right at the beginning */
    chunk = WOF_BLOCK_TO_CHUNK(block);

    chunk->used = FALSE;
    chunk->jumbo = FALSE;
    chunk->last = TRUE;
    chunk->prev = 0;
    chunk->len = WOF_BLOCK_SIZE - WOF_BLOCK_HEADER_SIZE;

    /* now push that chunk onto the master list */
    wof_push_master(allocator, chunk);
}

/* Creates a new block, and initializes it. */
static void
wof_new_block(wof_allocator_t *allocator) {
    wof_block_hdr_t *block;

    /* allocate the new block and add it to the block list */
    block = (wof_block_hdr_t *) AllocVecTags(WOF_BLOCK_SIZE, AVT_Type, MEMF_PRIVATE, TAG_DONE);
    if (block == NULL) {
        return;
    }

    wof_add_to_block_list(allocator, block);

    /* initialize it */
    wof_init_block(allocator, block);
}

/* JUMBO ALLOCATIONS */

/* Allocates special 'jumbo' blocks for sizes that won't fit normally. */
static void *
wof_alloc_jumbo(wof_allocator_t *allocator, const size_t size) {
    wof_block_hdr_t *block;
    wof_chunk_hdr_t *chunk;

    /* allocate a new block of exactly the right size */
    block = (wof_block_hdr_t *) AllocVecTags(size
                                             + WOF_BLOCK_HEADER_SIZE
                                             + WOF_CHUNK_HEADER_SIZE,
                                             AVT_Type, MEMF_PRIVATE,
                                             TAG_DONE);
    if (block == NULL) {
        return NULL;
    }

    /* add it to the block list */
    wof_add_to_block_list(allocator, block);

    /* the new block contains a single jumbo chunk */
    chunk = WOF_BLOCK_TO_CHUNK(block);
    chunk->last = TRUE;
    chunk->used = TRUE;
    chunk->jumbo = TRUE;
    chunk->len = 0;
    chunk->prev = 0;

    /* and return the data pointer */
    return WOF_CHUNK_TO_DATA(chunk);
}

/* Frees special 'jumbo' blocks of sizes that won't fit normally. */
static void
wof_free_jumbo(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk) {
    wof_block_hdr_t *block;

    block = WOF_CHUNK_TO_BLOCK(chunk);

    if(!block) {
        return;
    }

    wof_remove_from_block_list(allocator, block);

    FreeVec(block);
    block = NULL;
}

/* Reallocs special 'jumbo' blocks of sizes that won't fit normally. */
static void *
wof_realloc_jumbo(wof_allocator_t *allocator, wof_chunk_hdr_t *chunk, const size_t size) {
    void *result;
    wof_block_hdr_t *block;
    wof_block_hdr_t *newptr;
    size_t old_size;

    old_size = WOF_CHUNK_DATA_LEN(chunk);
    block = WOF_CHUNK_TO_BLOCK(chunk);

    /* If we have an invalid block return NULL */
    if (block == NULL || !old_size || !size) {
        return NULL;
    }

    /* If we have the same size return the old chuck */
    if (size == old_size) {
        return chunk;
    }

    if (old_size > size) {
        if (old_size/2 >= size) {
            /* at least half of the memory can be freed: allocate new, copy and free old. */
            /* Allocate new block of memory */
            newptr = (wof_block_hdr_t *) AllocVecTags(size
                                                      + WOF_BLOCK_HEADER_SIZE
                                                      + WOF_CHUNK_HEADER_SIZE,
                                                      AVT_Type, MEMF_PRIVATE,
                                                      TAG_DONE);

            if (newptr == NULL) {
                return NULL;
            }
            /* Copy old block to new one */
            memcpy(newptr, block, size);

            /* Free old block */
            FreeVec(block);
            block = NULL;

            if (newptr->next) {
                newptr->next->prev = newptr;
            }

            if (newptr->prev) {
                newptr->prev->next = newptr;
            } else {
                allocator->block_list = newptr;
            }
            result = WOF_CHUNK_TO_DATA(WOF_BLOCK_TO_CHUNK(newptr));
        }
        else {
            result = chunk;
        }
    }
    else {
        /* Allocate new block of memory */
        newptr = (wof_block_hdr_t *) AllocVecTags(size
                                                  + WOF_BLOCK_HEADER_SIZE
                                                  + WOF_CHUNK_HEADER_SIZE,
                                                  AVT_Type, MEMF_PRIVATE,
                                                  TAG_DONE);

        if (newptr == NULL) {
            return NULL;
        }
        /* Copy old block to new one */
        memcpy(newptr, block, old_size);

        /* Free old block */
        FreeVec(block);
        block = NULL;

        if (newptr->next) {
            newptr->next->prev = newptr;
        }

        if (newptr->prev) {
            newptr->prev->next = newptr;
        } else {
            allocator->block_list = newptr;
        }
        result = WOF_CHUNK_TO_DATA(WOF_BLOCK_TO_CHUNK(newptr));
    }
    return result;
}

/* API */

__BEGIN_DECLS

void *
wof_alloc(wof_allocator_t *allocator, const size_t size) {
    wof_chunk_hdr_t *chunk;

    if (size == 0) {
        return NULL;
    } else if (size > WOF_BLOCK_MAX_ALLOC_SIZE) {
        return wof_alloc_jumbo(allocator, size);
    }

    if (allocator->recycler_head &&
        WOF_CHUNK_DATA_LEN(allocator->recycler_head) >= size) {

        /* If we can serve it from the recycler, do so. */
        chunk = allocator->recycler_head;
    } else {
        if (allocator->master_head &&
            WOF_CHUNK_DATA_LEN(allocator->master_head) < size) {

            /* Recycle the head of the master list if necessary. */
            chunk = allocator->master_head;
            wof_pop_master(allocator);
            wof_add_to_recycler(allocator, chunk);
        }

        if (!allocator->master_head) {
            /* Allocate a new block if necessary. */
            wof_new_block(allocator);
        }

        chunk = allocator->master_head;
    }

    if (!chunk) {
        /* We don't have enough, and the OS wouldn't give us more. */
        return NULL;
    }

    /* Split our chunk into two to preserve any trailing free space */
    wof_split_free_chunk(allocator, chunk, size);

    /* Now cycle the recycler */
    wof_cycle_recycler(allocator);

    /* mark it as used */
    chunk->used = TRUE;

    /* and return the user's pointer */
    return WOF_CHUNK_TO_DATA(chunk);
}

void
wof_free(wof_allocator_t *allocator, void *ptr) {
    wof_chunk_hdr_t *chunk;

    if (ptr == NULL) {
        return;
    }

    chunk = WOF_DATA_TO_CHUNK(ptr);
    if (chunk->jumbo) {
        wof_free_jumbo(allocator, chunk);
        return;
    }

    /* mark it as unused */
    chunk->used = FALSE;

    /* merge it with any other free chunks adjacent to it, so that contiguous
     * free space doesn't get fragmented */
    wof_merge_free(allocator, chunk);
    /* Now cycle the recycler */
    wof_cycle_recycler(allocator);
}

void *
wof_realloc(wof_allocator_t *allocator, void *ptr, const size_t size) {
    wof_chunk_hdr_t *chunk;

    if (ptr == NULL) {
        return wof_alloc(allocator, size);
    }

    if (size == 0) {
        wof_free(allocator, ptr);
        return NULL;
    }

    chunk = WOF_DATA_TO_CHUNK(ptr);

    if (chunk->jumbo) {
        return wof_realloc_jumbo(allocator, chunk, size);
    }

    if (size > WOF_CHUNK_DATA_LEN(chunk)) {
        /* grow */
        wof_chunk_hdr_t *tmp;

        tmp = WOF_CHUNK_NEXT(chunk);

        if (tmp && (!tmp->used) &&
            (size < WOF_CHUNK_DATA_LEN(chunk) + tmp->len)) {
            /* the next chunk is free and has enough extra, so just grab
             * from that */
            size_t split_size;

            /* we ask for the next chunk to be split, but we don't end up
             * using the split chunk header (it just gets merged into this one),
             * so we want the split to be of (size - curdatalen - header_size).
             * However, this can underflow by header_size, so we do a quick
             * check here and floor the value to 0. */
            split_size = size - WOF_CHUNK_DATA_LEN(chunk);

            if (split_size < WOF_CHUNK_HEADER_SIZE) {
                split_size = 0;
            } else {
                split_size -= WOF_CHUNK_HEADER_SIZE;
            }

            wof_split_free_chunk(allocator, tmp, split_size);

            /* Now do a 'quickie' merge between the current block and the left-
             * hand side of the split. Simply calling wof_merge_free
             * might confuse things, since we may temporarily have two blocks
             * to our right that are both free (and it isn't guaranteed to
             * handle that case). Update our 'next' count and last flag, and
             * our (new) successor's 'prev' count */
            chunk->len += tmp->len;
            chunk->last = tmp->last;
            tmp = WOF_CHUNK_NEXT(chunk);
            if (tmp) {
                tmp->prev = chunk->len;
            }

            /* Now cycle the recycler */
            wof_cycle_recycler(allocator);

            /* And return the same old pointer */
            return ptr;
        } else {
            /* no room to grow, need to alloc, copy, free */
            void *newptr;

            newptr = wof_alloc(allocator, size);
            if (newptr == NULL) {
                return NULL;
            }
            memcpy(newptr, ptr, WOF_CHUNK_DATA_LEN(chunk));
            wof_free(allocator, ptr);

            /* No need to cycle the recycler, alloc and free both did that
             * already */

            return newptr;
        }
    } else if (size < WOF_CHUNK_DATA_LEN(chunk)) {
        /* shrink */
        wof_split_used_chunk(allocator, chunk, size);

        /* Now cycle the recycler */
        wof_cycle_recycler(allocator);

        return ptr;
    }

    /* no-op */
    return ptr;
}

void
wof_free_all(wof_allocator_t *allocator) {
    wof_block_hdr_t *cur;
    wof_chunk_hdr_t *chunk;

    /* the existing free lists are entirely irrelevant */
    allocator->master_head = NULL;
    allocator->recycler_head = NULL;

    /* iterate through the blocks, reinitializing each one */
    cur = allocator->block_list;

    while (cur) {
        chunk = WOF_BLOCK_TO_CHUNK(cur);
        if (chunk->jumbo) {
            wof_remove_from_block_list(allocator, cur);
            cur = cur->next;

            wof_block_hdr_t *block = WOF_CHUNK_TO_BLOCK(chunk);
            FreeVec(block);
            block = NULL;
        } else {
            wof_init_block(allocator, cur);
            cur = cur->next;
        }
    }
}

void
wof_gc(wof_allocator_t *allocator) {
    wof_block_hdr_t *cur, *next;
    wof_chunk_hdr_t *chunk;
    wof_free_hdr_t *free_chunk;

    /* Walk through the blocks, adding used blocks to the new list and
     * completely destroying unused blocks. */
    cur = allocator->block_list;
    allocator->block_list = NULL;

    while (cur) {
        chunk = WOF_BLOCK_TO_CHUNK(cur);
        next = cur->next;

        if (!chunk->jumbo && !chunk->used && chunk->last) {
            /* If the first chunk is also the last, and is unused, then
             * the block as a whole is entirely unused, so return it to
             * the OS and remove it from whatever lists it is in. */
            free_chunk = WOF_GET_FREE(chunk);
            if (free_chunk->next) {
                WOF_GET_FREE(free_chunk->next)->prev = free_chunk->prev;
            }
            if (free_chunk->prev) {
                WOF_GET_FREE(free_chunk->prev)->next = free_chunk->next;
            }
            if (allocator->recycler_head == chunk) {
                if (free_chunk->next == chunk) {
                    allocator->recycler_head = NULL;
                } else {
                    allocator->recycler_head = free_chunk->next;
                }
            } else if (allocator->master_head == chunk) {
                allocator->master_head = free_chunk->next;
            }
            FreeVec(cur);
            cur = NULL;
        } else {
            /* part of this block is used, so add it to the new block list */
            wof_add_to_block_list(allocator, cur);
        }

        cur = next;
    }
}

void
wof_allocator_destroy(wof_allocator_t *allocator) {

    __delete_semaphore(allocator->semaphore);

    /* The combination of free_all and gc returns all our memory to the OS
     * except for the struct itself */
    wof_free_all(allocator);
    wof_gc(allocator);

    /* then just free the struct */
    FreeVec(allocator);
    allocator = NULL;
}

wof_allocator_t *
wof_allocator_new(void) {
    ENTER();

    wof_allocator_t *allocator = (wof_allocator_t *) AllocVecTags(sizeof(wof_allocator_t), MEMF_PRIVATE, AVT_ClearWithValue, 0, TAG_DONE);
    if (allocator == NULL) {
        SHOWMSG("Unable to create wof_allocator");
        RETURN(NULL);
        return NULL;
    }

    allocator->block_list = NULL;
    allocator->master_head = NULL;
    allocator->recycler_head = NULL;
    allocator->semaphore = __create_semaphore();

    RETURN(allocator);
    return allocator;
}

__END_DECLS
