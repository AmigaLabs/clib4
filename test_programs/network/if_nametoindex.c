#include <stdio.h>
#include <net/if.h>

int main()
{
    int index = 0;
    /* Get the interface index */
    if ((index = if_nametoindex("eth0")) == 0)
        printf("if_nametoindex() failed to obtain interface index\n");
    else
        printf("if_nametoindex() found interface at index %d\n", index);
    return 0;
}