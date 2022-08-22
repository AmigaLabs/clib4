#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
{
    const int iter = 1000000;
    int     i;
    pctrval v;

    v = rdtsc();
    for (i = 0; i < iter; i++)
        arc4random();
    v = rdtsc() - v;
    v /= iter;

    printf("%qd cycles\n", v);
}