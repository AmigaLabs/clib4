#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <stdlib.h>

int main() {
    int fd;
    char fn[] = "utime.file";
    struct utimbuf ubuf;

    if ((fd = creat(fn, S_IWUSR)) < 0)
        perror("creat() error");
    else {
        close(fd);
        puts("before utime()");
        system("LIST utime.file");
        ubuf.modtime = 0;
        time(&ubuf.actime);
        if (utime(fn, &ubuf) != 0)
            perror("utime() error");
        else {
            puts("after utime()");
            system("LIST utime.file");
        }
        unlink(fn);
    }
}