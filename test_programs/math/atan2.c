#include <math.h>
#include <stdio.h>
#include <errno.h>

int main(void) {
    double a, b, c, d, e;

    c = 0.45;
    d = 0.23;

    a = atan(c);
    b = atan2(c, d);
    e = atan2(0, 0);

    printf("atan( %lf ) = %lf\n", c, a);
    printf("atan2( %lf, %lf ) = %lf\n", c, d, b);
    printf("atan2( %lf, %lf ) = %lf\n", 0.0, 0.0, e);
    printf("errno = %d\n", errno);
    return 0;
}