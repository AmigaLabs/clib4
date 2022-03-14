#include <stdio.h>
#include <math.h>
#include <errno.h>

int main()
{
    printf("acosh(0.5) = %f\n", acosh(0.5f));
    printf("errno: %d\n", errno);

    printf("acosf(0.5) = %f\n", acosh(0.5f));
    printf("errno: %d\n", errno);

    printf("asinf(-1.5) = %f\n", asinf(-1.5f));
    printf("errno: %d\n", errno);

    printf("asinf(1.0) = %f\n", asinf(1.0f));
    printf("errno: %d\n", errno);

    return 0;
}
