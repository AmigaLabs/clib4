#include "align.h"

void write64(uint8_t *ptr2) {
    uint64_t *ptr = (uint64_t *) ptr2;
    int buf = BUFFER_SIZE / 8;

    for (int i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < buf; z++) {
            ptr[z] = i % 0xffff;
        }
}

void read64(uint8_t *ptr2) {
    uint64_t *ptr = (uint64_t *) ptr2;
    int buf = BUFFER_SIZE / 8;
    uint64_t w = 0;
    int i;

    for (i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < buf; z++) {
            w = ptr[z];
        }

    // Just using w to disable Compiler error (Unused variable)
    i = w;
}

void default_allocation64(int shift, int align) {
    uint8_t *ptr;
    clock_t diff;

    // Allocating a bit extra to try unaligned
    ptr = alloc(shift, align);

    diff = measure(&write64, ptr);

    print(false, 8, shift, align, diff);

    diff = measure(&read64, ptr);
    print(true, 8, shift, align, diff);

    free(ptr - shift);
}
