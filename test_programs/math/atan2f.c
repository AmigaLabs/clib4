#include <stdio.h>
#include <math.h>

int main(void) {
    // normal usage: the signs of the two arguments determine the quadrant
    // atan2(1,1) = +pi/4, Quad I
    printf("(+1,+1) cartesian is (%f,%f) polar\n", hypot(1, 1), atan2(1, 1));
    // atan2(1, -1) = +3pi/4, Quad II
    printf("(+1,-1) cartesian is (%f,%f) polar\n", hypot(1, -1), atan2(1, -1));
    // atan2(-1,-1) = -3pi/4, Quad III
    printf("(-1,-1) cartesian is (%f,%f) polar\n", hypot(-1, -1), atan2(-1, -1));
    // atan2(-1,-1) = -pi/4, Quad IV
    printf("(-1,+1) cartesian is (%f,%f) polar\n", hypot(-1, 1), atan2(-1, 1));

    // special values
    printf("atan2(0, 0) = %f atan2(0, -0)=%f\n", atan2(0, 0), atan2(0, -0.0));
    printf("atan2(7, 0) = %f atan2(7, -0)=%f\n", atan2(7, 0), atan2(7, -0.0));

    printf("\n\nNow using atan2l\n\n");

    printf("(+1,+1) cartesian is (%f,%f) polar\n", hypot(1, 1), atan2l(1, 1));
    // atan2(1, -1) = +3pi/4, Quad II
    printf("(+1,-1) cartesian is (%f,%f) polar\n", hypot(1, -1), atan2l(1, -1));
    // atan2(-1,-1) = -3pi/4, Quad III
    printf("(-1,-1) cartesian is (%f,%f) polar\n", hypot(-1, -1), atan2l(-1, -1));
    // atan2(-1,-1) = -pi/4, Quad IV
    printf("(-1,+1) cartesian is (%f,%f) polar\n", hypot(-1, 1), atan2l(-1, 1));

    // special values
    printf("atan2l(0, 0) = %f atan2l(0, -0)=%f\n", atan2l(0, 0), atan2l(0, -0.0));
    printf("atan2l(7, 0) = %f atan2l(7, -0)=%f\n", atan2l(7, 0), atan2l(7, -0.0));
    return 0;
}