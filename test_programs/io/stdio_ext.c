/* CELEBF89

   This example writes and reads data to a file while querying the
   stream for information about data in the I/O buffer.

*/

#include <stdio.h>
#include <stdio_ext.h>
#include <string.h>

void main() {
    FILE *f;
    char filename[FILENAME_MAX] = "T:myfile.dat";
    char data[128] = "There are 34 bytes in this buffer\n";
    int datalen = strlen(data);
    size_t n = 0;

    f = fopen(filename, "wb+");
    if (f == NULL) {
        perror("fopen() failed\n");
        return;
    }

    if (__fwritable(f)) printf("Writing is allowed on the open stream\n");
    if (__freadable(f)) printf("Reading is allowed on the open stream\n");

    n = fputs(data, f);
    if (n == EOF) {
        perror("fputs() failed\n");
        return;
    }

    n = __fpending(f);
    printf("There are %d bytes in the buffer pending to be written\n", n);

    if (__fwriting(f)) printf("The last operation on the stream was a write\n");

    rewind(f);

    n = fgetc(f);

    n = __freadahead(f);
    printf("There are %d bytes remaining to be read from the buffer\n", n);

    if (__freading(f)) printf("The last operation on the stream was a read\n");

    return;
}