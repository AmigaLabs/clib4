/*
 * Change the ownership of a list of files
 * to the current user/group
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int i;
    int ecode = 0;

    for (i = 1; i < argc; i++)
    {
        if (chown(argv[i], getuid(), getgid()) == -1)
        {
            perror(argv[i]);
            ecode++;
        }
    }
    exit(ecode);
}
