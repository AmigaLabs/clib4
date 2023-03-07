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

    // schedule write
    // for valgrind mem leak output see comments from answer in
    //   https://stackoverflow.com/questions/4248720/aio-h-aio-read-and-write-memory-leak
    int ret = aio_write(&op);
    printf("aio_write 1: %d\n", ret);

    // wait until everything is done
    const struct aiocb *aiolist[1];
    aiolist[0] = &op;

    ret = aio_suspend(aiolist, 1, NULL);
    printf("aio_suspend: %d\n", ret);

    // report possible errors
    ret = aio_error(&op);
    printf("errno 1: %d\n", ret);

    fclose(file);

    return 0;
}