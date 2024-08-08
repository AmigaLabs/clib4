#include "align.h"

time_t measure(void (*f)(uint8_t *), uint8_t *buf) {
    clock_t begin = clock();
    f(buf);
    clock_t end = clock();

    return end - begin;
}
