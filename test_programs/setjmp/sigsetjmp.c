#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

sigjmp_buf mark;

void p(void);
void recover(void);

int main(void) {
    if (sigsetjmp(mark, 1) != 0) {
        printf("siglongjmp() has been called\n");
        recover();
        exit(1);
    }
    printf("sigsetjmp() has been called\n");
    p();
}

void p(void) {
    int error = 0;
    error = 9;
    if (error != 0)
        siglongjmp(mark, -1);
}

void recover(void) {

}