#include "align.h"

uint8_t *alloc(int shift, int aligned) {
    void *ptr;

    if (aligned == 0) {
        // Allocating a bit extra to try unaligned
        ptr = (uint8_t *) malloc((BUFFER_SIZE + shift) * sizeof(uint8_t)) + shift;
    } else {
        ptr = (uint8_t *) aligned_alloc(aligned, (BUFFER_SIZE + shift) * sizeof(uint8_t)) + shift;
    }

    return ptr;
}
