#include "align.h"

void write16(uint8_t *ptr2) {
    uint16_t *ptr = (uint16_t *) ptr2;
    int buf = BUFFER_SIZE / 2;

    for (int i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < buf; z++) {
            ptr[z] = i % 0xffff;
        }
}

void read16(uint8_t *ptr2) {
    uint16_t *ptr = (uint16_t *) ptr2;
    int buf = BUFFER_SIZE / 2;
    uint16_t w = 0;
    int i;

    for (i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < buf; z++) {
            w = ptr[z];
        }

    // Just using w to disable Compiler error (Unused variable)
    i = w;
}

void default_allocation16(int shift, int align) {
    uint8_t *ptr;
    clock_t diff;

    ptr = alloc(shift, align);

    diff = measure(&write16, ptr);
    print(false, 2, shift, align, diff);

    diff = measure(&read16, ptr);
    print(true, 2, shift, align, diff);

    free(ptr - shift);
}
