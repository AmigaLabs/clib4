#include "animal.h"
#include <stdio.h>
#include <dlfcn.h>

void print_name(const char* type)
{
    FILE *n = fopen("T:test.txt", "w+");
    fprintf(n, "Called\n");
    fprintf(n, "Tama is a %s.\n", type);
    fclose(n);

    void *handle;
    void (*func_print_name)(const char*);

    handle = dlopen("libdog.so", RTLD_LAZY);
    if (handle != NULL) {
        *(void**)(&func_print_name) = dlsym(handle, "print_name");
        func_print_name("Peppe\n");
        dlclose(handle);
    }
}

