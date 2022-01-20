// C program to read nth byte of a file and
// copy it to another file using lseek
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
  
void func(char arr[], int n)
{
    // Open the file for READ only.
    int f_write = open("start.txt", O_RDONLY);
    if (f_write == -1)
    {  
	printf("start.txt not found\n");
        return;
    }   

    // Open the file for WRITE and READ only.
    int f_read = open("end.txt", O_WRONLY);
    if (f_read == -1)
    {  
        printf("cannot open end.txt for write\n");
        return;
    }        

    int count = 0;
    while (read(f_write, arr, 1))
    {
        // to write the 1st byte of the input file in
        // the output file
        if (count < n)
        {
            // SEEK_CUR specifies that
            // the offset provided is relative to the
            // current file position
            lseek (f_write, n, SEEK_CUR);
            write (f_read, arr, 1);
            count = n;
        }
  
        // After the nth byte (now taking the alternate
        // nth byte)
        else
        {
            count = (2*n);
            lseek(f_write, count, SEEK_CUR);
            write(f_read, arr, 1);
        }
    }
    close(f_write);
    close(f_read);
}
  
// Driver code
int main()
{
    char arr[100];
    int n;
    n = 5;
  
    // Calling for the function
    func(arr, n);
    return 0;
}
