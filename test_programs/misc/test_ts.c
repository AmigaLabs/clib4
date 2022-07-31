#include <stdio.h>

int main() {
#ifdef __THREAD_SAFE
    printf("Thread safe clib2\n");
#else
    printf("This isn't thread safe clib2\n");
#endif
    return 0;
}