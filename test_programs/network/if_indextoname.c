#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>

int main(int argc, char **argv)
{
    char name[IF_NAMESIZE] = {0};

    if (argc != 2)
    {
        printf("usage: if_indextoname <interface-index>\n");
        return 0;
    }

    printf("interface name = %s\n", if_indextoname(atoi(argv[1]), name));
    return 0;
}