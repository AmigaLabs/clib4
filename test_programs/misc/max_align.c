#include <stdio.h>
#include <stddef.h>
#include <stdalign.h>
#include <stdlib.h>
#include <inttypes.h>

int main(void) {
    size_t sz = sizeof(max_align_t);
    printf("Size of max_align_t is %zu (%#zx)\n", sz, sz);

    size_t a = alignof(max_align_t);
    printf("Alignment of max_align_t is %zu (%#zx)\n", a, a);

    void *p = malloc(123);
    printf("The address obtained from malloc(123) is %#"
    PRIxPTR
    "\n",
            (uintptr_t) p);
    free(p);
}