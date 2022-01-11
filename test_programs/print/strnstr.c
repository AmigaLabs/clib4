#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const char *largestring = "Foo Bar Baz";
    const char *smallstring = "Bar";
    char *ptr;

    ptr = strnstr(largestring, smallstring, 4);
    printf("ptr = %s\n");
    return 0;
}