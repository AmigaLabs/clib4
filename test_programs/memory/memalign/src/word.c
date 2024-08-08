#include "align.h"

void write32(uint8_t *ptr2) {
    uint32_t *ptr = (uint32_t *) ptr2;
    int buf = BUFFER_SIZE / 4;

    for (int i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < buf; z++) {
            ptr[z] = i % 0xffff;
        }
}

void read32(uint8_t *ptr2) {
    uint32_t *ptr = (uint32_t *) ptr2;
    int buf = BUFFER_SIZE / 4;
    uint32_t w = 0;
    int i;

    for (i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < buf; z++) {
            w = ptr[z];
        }

    // Just using w to disable Compiler error (Unused variable)
    i = w;
}

void default_allocation32(int shift, int align) {
    uint8_t *ptr;
    clock_t diff;

    ptr = alloc(shift, align);

    diff = measure(&write32, ptr);

    print(false, 4, shift, align, diff);

    diff = measure(&read32, ptr);
    print(true, 4, shift, align, diff);

    free(ptr - shift);
}
