#include <stdio.h>

int main() {
#ifdef __THREAD_SAFE
    printf("Thread safe clib4\n");
#else
    printf("This isn't thread safe clib4\n");
#endif
    return 0;
}