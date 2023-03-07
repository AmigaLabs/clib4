#include <stdio.h>
#include <string.h>
#include <aio.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFSIZE 1024

int main(void) {
    int fd, ret;
    struct aiocb my_aiocb;

    fd = open("file.txt", O_RDONLY);
    if (fd < 0)
        perror("open");

    bzero((char *) &my_aiocb, sizeof(struct aiocb));

    my_aiocb.aio_buf = calloc(1, BUFSIZE + 1);
    if (!my_aiocb.aio_buf)
        perror("malloc");

    my_aiocb.aio_fildes = fd;
    my_aiocb.aio_nbytes = BUFSIZE;
    my_aiocb.aio_offset = 0;

    ret = aio_read(&my_aiocb);
    if (ret < 0)
        perror("aio_read");

    while (aio_error(&my_aiocb) == EINPROGRESS)
        continue;

    if ((ret = aio_return(&my_aiocb))) {
        printf("ret:%d,read:%s\n", ret, my_aiocb.aio_buf);
    } else {
        printf("read 0\n");
    }


    close(fd);
    return 0;
}