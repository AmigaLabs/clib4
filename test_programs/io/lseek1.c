// C program to read nth byte of a file and
// copy it to another file using lseek
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
  
int main()
{
    int f_read = open("start.txt", O_RDONLY);
    if (f_read == -1)
    {
        printf("start.txt not found\n");
        return 0;
    }
    off_t curPos = lseek(f_read, 0, SEEK_CUR);
    off_t lengthTemp = lseek(f_read, 0, SEEK_END);
    lseek(f_read, curPos, SEEK_SET);
    close(f_read);

    int64_t length = (int64_t)lengthTemp;
    printf("File Length = %lld\n", lengthTemp);
    return 0;
}
