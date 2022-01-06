#include <unistd.h>
#include <stdio.h>

int main()
{
    int writeable;
    writeable = access("C:Rename", W_OK);
    if (writeable == -1)
        printf("Not writeable!\n");
    else
        printf("Writeable!\n");

    return 0;
}