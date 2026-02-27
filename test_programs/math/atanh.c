#include <math.h>
#include <stdio.h>

int main(void) {
    double pi = 3.1415926535;
    double x, y;

    long double lpi = 3.1415926535;
    long double lx, ly;

    x = tanh(pi / 4);
    y = atanh(x);

    lx = tanhl(pi / 4);
    ly = atanhl(x);

    printf("tanh( %f ) = %f\n", pi / 4, x);
    printf("atanh( %f ) = %f\n", x, y);

    printf("tanhl( %Lf ) = %Lf\n", lpi / 4, lx);
    printf("atanhl( %Lf ) = %Lf\n", lx, ly);

}