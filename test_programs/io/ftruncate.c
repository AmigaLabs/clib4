#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define string_len 1000

int main()
{
    char *mega_string;
    int file_descriptor;
    int ret;
    char fn[] = "write.file";
    struct stat st;

    if ((mega_string = (char *)malloc(string_len)) == NULL)
        perror("malloc() error");
    else if ((file_descriptor = creat(fn, S_IWUSR)) < 0)
        perror("creat() error");
    else
    {
        memset(mega_string, '0', string_len);
        if ((ret = write(file_descriptor, mega_string, string_len)) == -1)
            perror("write() error");
        else
        {
            printf("write() wrote %d bytes\n", ret);
            fstat(file_descriptor, &st);
            printf("the file has %ld bytes\n", (long)st.st_size);
            if (ftruncate(file_descriptor, 1) != 0)
                perror("ftruncate() error");
            else
            {
                fstat(file_descriptor, &st);
                printf("the file has %ld bytes\n", (long)st.st_size);
            }
        }
        close(file_descriptor);
        unlink(fn);
    }
    free(mega_string);

    return 0;
}
