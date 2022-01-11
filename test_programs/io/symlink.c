/* This example works only under z/OS XL C, not z/OS XL C++  */
#define _POSIX1_SOURCE 2
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    char fn[] = "test.file";
    char sln[] = "test.symlink";
    int fd;

    if ((fd = creat(fn, S_IWUSR)) < 0)
        perror("creat() error");
    else
    {
        close(fd);
        puts("before symlink()");
        system("list test.*");
        if (symlink(fn, sln) != 0)
        {
            perror("symlink() error");
            unlink(fn);
        }
        else
        {
            puts("after symlink()");
            system("list test.*");
            unlink(fn);
            puts("after first unlink()");
            system("list test.*");
            unlink(sln);
        }
    }
    return 0;
}
