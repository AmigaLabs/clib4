/* copysign example */
#include <stdio.h>      /* printf */
#include <math.h>       /* copysign */

int main() {
    printf("copysign ( 10.0,-1.0) = %f\n", copysign(10.0, -1.0));
    printf("copysign (-10.0,-1.0) = %f\n", copysign(-10.0, -1.0));
    printf("copysign (-10.0, 1.0) = %f\n", copysign(-10.0, 1.0));

    printf("copysignl ( 10.0,-1.0) = %Lf\n", copysignl(10.0, -1.0));
    printf("copysignl (-10.0,-1.0) = %Lf\n", copysignl(-10.0, -1.0));
    printf("copysignl (-10.0, 1.0) = %Lf\n", copysignl(-10.0, 1.0));

    return 0;
}