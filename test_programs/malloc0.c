#include <stdlib.h>
#include <stdio.h>

int main(void)
{
        void *p = malloc(0);
        void *q = malloc(0);
        void *r = malloc(0);
        if (!p || !q || !r)
                printf("malloc(0) returned NULL\n");
        if (p == q || p == r || q == r)
                printf("malloc(0) returned non-unique pointers: %p, %p, %p\n", p, q, r);
        if (q) free(q);
        if (p) free(p);
        if (r) free(r);
        return 0;
}