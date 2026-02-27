#include "align.h"

void write8(uint8_t *ptr) {
    for (int i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < BUFFER_SIZE; z++) {
            ptr[z] = (i) % 0xff;
        }
}

void read8(uint8_t *ptr) {
    uint8_t w = 0;
    int i;

    for (i = 0; i < MAX_CHUNK; i++)
        for (int z = 0; z < BUFFER_SIZE; z++) {
            w = ptr[z];
        }

    // Just using w to disable Compiler error (Unused variable)
    i = w;
}

void default_allocation8(int shift, int align) {
    uint8_t *ptr;
    clock_t diff;

    ptr = alloc(shift, align);
    if (ptr != NULL) {
        diff = measure(&write8, ptr);
        print(false, 1, shift, align, diff);

        diff = measure(&read8, ptr);
        print(true, 1, shift, align, diff);

        free(ptr - shift);
    } else
        printf("default_allocation8 returns NULL: shift = %d - align = %d\n", shift, align);
}
