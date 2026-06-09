/*
 * Test program to verify that libc.so can be loaded and malloc/free work correctly
 * Tests that IClib4 is properly initialized in shared libraries before constructors run
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

int main(void) {
    void *handle;
    void *(*dl_malloc)(size_t);
    void (*dl_free)(void *);
    void *ptr1, *ptr2;
    
    printf("Opening libc.so...\n");
#ifdef CLIB4    
    handle = dlopen("PROGDIR:libc.so", RTLD_LAZY | RTLD_GLOBAL);
#else
    handle = dlopen("SOBJS:libc.so", RTLD_LAZY);
#endif    
    if (!handle) {
        fprintf(stderr, "Error opening libc.so: %s\n", dlerror());
        return EXIT_FAILURE;
    }
    printf("libc.so opened successfully\n");
    
    /* Get malloc symbol */
    *(void **)(&dl_malloc) = dlsym(handle, "malloc");
    if (!dl_malloc) {
        fprintf(stderr, "Error getting malloc symbol: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }
    printf("malloc symbol found\n");
    
    /* Get free symbol */
    *(void **)(&dl_free) = dlsym(handle, "free");
    if (!dl_free) {
        fprintf(stderr, "Error getting free symbol: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }
    printf("free symbol found\n");
    
    /* Test malloc/free */
    printf("\nTesting malloc/free from libc.so:\n");
    
    printf("Allocating 100 bytes...\n");
    ptr1 = dl_malloc(100);
    if (!ptr1) {
        fprintf(stderr, "Error: malloc returned NULL\n");
        dlclose(handle);
        return EXIT_FAILURE;
    }
    printf("malloc(100) = %p\n", ptr1);
    
    printf("Allocating 1024 bytes...\n");
    ptr2 = dl_malloc(1024);
    if (!ptr2) {
        fprintf(stderr, "Error: malloc returned NULL\n");
        dl_free(ptr1);
        dlclose(handle);
        return EXIT_FAILURE;
    }
    printf("malloc(1024) = %p\n", ptr2);
    
    /* Fill with some data */
    memset(ptr1, 0xAA, 100);
    memset(ptr2, 0xBB, 1024);
    printf("Memory filled with test data\n");
    
    /* Free the memory */
    printf("Freeing first allocation...\n");
    dl_free(ptr1);
    
    printf("Freeing second allocation...\n");
    dl_free(ptr2);
    
    printf("\nAll malloc/free tests passed!\n");
    
    /* Close the library */
    printf("Closing libc.so...\n");
    dlclose(handle);
    
    printf("\nTest completed successfully!\n");
    return EXIT_SUCCESS;
}
