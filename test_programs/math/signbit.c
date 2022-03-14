/* signbit example */
#include <stdio.h>      /* printf */
#include <math.h>       /* signbit, sqrt */

int main() {
    printf("signbit(0.0)       : %d\n", signbit(0.0));
    printf("signbit(1.0/0.0)   : %d\n", signbit(1.0 / 0.0));
    printf("signbit(-1.0/0.0)  : %d\n", signbit(-1.0 / 0.0));
    printf("signbit(sqrt(-1.0)): %d\n", signbit(sqrt(-1.0)));
    return 0;
}