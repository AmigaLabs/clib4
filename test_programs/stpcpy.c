#include <string.h>
#include <stdio.h>

int main(void)
{
    char buffer[20] = {0};
    char *to = buffer;

    to = stpcpy(to, "foo");
    to = stpcpy(to, "bar");
    printf("%s\n", buffer);

    return 0;
}