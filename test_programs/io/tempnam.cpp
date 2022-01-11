#include <stdio.h>

char *tname;

int main()
{
    if ((tname = tempnam("T:test", "std")) != NULL)
        printf("temp file 1: %s\n", tname);
    else
        printf("can't make temp file 1\n");

    if ((tname = tempnam("T:test", NULL)) != NULL)
        printf("temp file 2: %s\n", tname);
    else
        printf("can't make temp file 2\n");
}