/* https://www.thegeekstuff.com/2012/08/gprof-tutorial/
 * compile with:
 * ppc-amigaos -mcrt=clib2 -pg test_gprof.c -o test_gprof -lprofile
*/
#include <stdio.h>

void new_func1(void) {
    printf("\n Inside new_func1()\n");
    int i = 0;

    for (; i < 0xffffffee; i++);

    return;
}

void func1(void) {
    printf("\n Inside func1 \n");
    int i = 0;

    for (; i < 0xffffffff; i++);
    new_func1();

    return;
}

static void func2(void) {
    printf("\n Inside func2 \n");
    int i = 0;

    for (; i < 0xffffffaa; i++);
    return;
}

int main(void) {
    printf("\n Inside main()\n");
    int i = 0;

    for (; i < 0xffffff; i++);
    func1();
    func2();

    return 0;
}