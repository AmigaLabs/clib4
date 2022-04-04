#include <stdio.h>
#include <alloca.h>

int main() {
    void *ptr = alloca(100);
    if (ptr)
        printf("Memory allocated with alloca()\n");
    else
        printf("Error allocating memory with alloca()\n");

    // Memory is freed at exit
    return 0;
}