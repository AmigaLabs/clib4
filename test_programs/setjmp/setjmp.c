#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

void jmpfunction(jmp_buf env_buf) {
    longjmp(env_buf, 1);
}

int main() {
    int val;
    jmp_buf env_buffer;

    /* save calling environment for longjmp */
    val = setjmp(env_buffer);
    printf("val = %d\n", val);
    if (val != 0) {
        printf("Returned from a longjmp() with value = %d\n", val);
        exit(0);
    }

    printf("Jump function call\n");
    jmpfunction(env_buffer);

    printf("exit now\n");
    return (0);
}
