#include <aio.h>
#include <stdio.h>

char CONTENT[] = "asdf;";
const int LENGTH = 5;

struct aiocb createIoRequest(int fd, off_t offset, volatile void *content, size_t length) {
    // create and initialize the aiocb structure.
    // If we don't init to 0, we have undefined behavior.
    // E.g. through sigevent op.aio_sigevent there could be
    //      a callback function being set, that the program
    //      tries to call - which will then fail.
    struct aiocb ret = {0};
    {
        ret.aio_fildes = fd;
        ret.aio_offset = offset;
        ret.aio_buf = content;
        ret.aio_nbytes = length;
    }
    return ret;
}


int main() {
    FILE *file = fopen("RAM:outfile.txt", "w");
    int fd = fileno(file);

    struct aiocb op = createIoRequest(fd, 0, CONTENT, LENGTH);
    struct aiocb op2 = createIoRequest(fd, LENGTH, CONTENT, LENGTH);

    // schedule write
    // for valgrind mem leak output see comments from answer in
    //   https://stackoverflow.com/questions/4248720/aio-h-aio-read-and-write-memory-leak
    int ret = aio_write(&op);
    printf("aio_write 1: %d\n", ret);
    ret = aio_write(&op2);
    printf("aio_write 2: %d\n", ret);

    // wait until everything is done

    const int OPs = 2;
    const struct aiocb *aiolist[OPs];
    aiolist[0] = &op;
    aiolist[1] = &op2;

    ret = aio_suspend(aiolist, OPs, NULL);
    printf("aio_suspend: %d\n", ret);

    // report possible errors
    ret = aio_error(&op);
    printf("errno 1: %d\n", ret);
    ret = aio_error(&op2);
    printf("errno 2: %d\n", ret);
    // error codes can be checked in <errno.h>

    fclose(file);

    return 0;
}