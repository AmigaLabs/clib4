#include <stdio.h>
#include <complex.h>

int main(void)
{
    double complex z = CMPLX(0.0, -0.0);
    printf("z = %.1f%+.1fi\n", creal(z), cimag(z));

    double complex z1 = 1 + 2*I;
    z1 = 1/z1;
    printf("1/(1.0+2.0i) = %.1f%+.1fi\n", creal(z1), cimag(z1));

    return 0;
}