#define _LARGEFILE64_SOURCE //Must be at the very beginning of the source file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

off_t filesize(FILE *fp)
{
    long int save_pos;
    long size_of_file;

    /* Save the current position. */
    save_pos = ftell(fp);
    /* Jump to the end of the file. */
    fseek(fp, 0L, SEEK_END);

    /* Get the end position. */
    size_of_file = ftell(fp);

    /* Jump back to the original position. */
    fseek(fp, save_pos, SEEK_SET);

    return (size_of_file);
}

_off64_t filesize64(FILE *fp)
{
    _off64_t save_pos;
    _off64_t size_of_file;

    /* Save the current position. */
    save_pos = ftello64(fp);
    /* Jump to the end of the file. */
    fseeko64(fp, 0L, SEEK_END);

    /* Get the end position. */
    size_of_file = ftello64(fp);

    /* Jump back to the original position. */
    fseeko64(fp, save_pos, SEEK_SET);

    return (size_of_file);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: fopen LARGEFILE\n");
        return -1;
    }

    FILE *fd = fopen(argv[1], "r");
    if (fd != NULL)
    {
        off_t size1 = filesize(fd);
        _off64_t size2 = filesize64(fd);
        printf("Size1 = %ld\n", size1);
        printf("Size2 = %lld\n", size2);
        fclose(fd);
    }
    return 0;
}