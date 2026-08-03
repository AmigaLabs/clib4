/*
 * Test for the clib4 malloc_dump() extension.
 *
 * malloc_dump() walks the internal structures of the per-process wmem
 * allocator (OS blocks, chunks, live allocations) and prints them on the
 * serial port via DebugPrintF(), together with summary totals. Capture
 * the output with a serial terminal or a DebugPrintF redirector such as
 * Sashimi.
 *
 * The default allocator is the 'block' one; set the env variable
 * CLIB4_MEMORY_ALLOCATOR to 1 (simple), 3 (strict) or 4 (block_fast)
 * before running to inspect the other allocators:
 *
 *   setenv CLIB4_MEMORY_ALLOCATOR 3
 *   malloc_dump_test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int
main(void) {
    void *small, *medium, *aligned, *jumbo;

    printf("malloc_dump() test - the dump goes to the serial port\n\n");

    printf("1) dump right after startup (stdio buffers etc.):\n");
    malloc_dump();

    small = malloc(100);
    medium = calloc(1, 64 * 1024);
    aligned = memalign(256, 10000);
    jumbo = malloc(10 * 1024 * 1024); /* jumbo chunk in the block allocator */

    if (small == NULL || medium == NULL || aligned == NULL || jumbo == NULL) {
        printf("FAIL: allocation failed\n");
        return EXIT_FAILURE;
    }

    /* Touch the memory so nothing is optimized away */
    memset(small, 0xAA, 100);
    memset(medium, 0xBB, 64 * 1024);
    memset(aligned, 0xCC, 10000);
    memset(jumbo, 0xDD, 10 * 1024 * 1024);

    printf("2) dump with 4 extra live allocations (100 B, 64 KB, 10 KB aligned, 10 MB jumbo):\n");
    malloc_dump();

    free(medium);
    free(jumbo);

    printf("3) dump after freeing the 64 KB and the 10 MB jumbo:\n");
    malloc_dump();

    free(small);
    free(aligned);

    printf("4) final dump, everything freed again:\n");
    malloc_dump();

    printf("\ndone - compare the dumps on the serial output\n");

    return EXIT_SUCCESS;
}
