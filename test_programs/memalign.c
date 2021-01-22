#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

int main()
{
    #define m (1024 * 4)
    #define a 1024
    char *ptr1 = malloc(m);
    char *ptr2 = memalign(a, m);
    char *ptr3 = valloc(m);

    if (ptr1) printf("malloc   - %d byte aligned = %s\n", a, (((size_t)ptr1) % a) ? "no" : "yes");
    if (ptr2) printf("memalign - %d byte aligned = %s\n", a, (((size_t)ptr2) % a) ? "no" : "yes");
    if (ptr2) printf("valloc   - %d byte aligned = %s\n", sysconf(_SC_PAGESIZE), (((size_t)ptr3) % sysconf(_SC_PAGESIZE)) ? "no" : "yes");

    if (ptr1) free(ptr1);
    if (ptr2) free(ptr2);
    if (ptr3) free(ptr3);

    return 0;
}