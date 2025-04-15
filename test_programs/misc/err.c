#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <fcntl.h>

int main() {
    void *p;
    const char *file_name = "fakefile.txt";
    int fd;

    if ((fd = open(file_name, O_RDONLY, 0)) == -1)
        warn("%s", file_name);

    if ((fd = open(file_name, O_RDONLY, 0)) == -1)
        warnx("%s", file_name);

    if ((fd = open(file_name, O_RDONLY, 0)) == -1)
        errx(EX_NOINPUT, "%s", file_name);

    return 0;
}