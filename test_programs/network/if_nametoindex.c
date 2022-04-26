#include <stdio.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    int index = 0;

    if (argc != 2)
    {
        printf("usage: if_nametoindex <interface-name>\n");
        return 0;
    }

    /* Get the interface index */
    if ((index = if_nametoindex(argv[1])) == 0)
        printf("if_nametoindex() failed to obtain interface index\n");
    else
        printf("if_nametoindex() found interface at index %d\n", index);
    return 0;
}