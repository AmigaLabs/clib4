#include <sys/types.h> /* UNIX types      POSIX */
#include <sys/uio.h>   /* BSD  I/O        BSD   */
#include <stdio.h>     /* I/O lib         C89   */
#include <errno.h>     /* error stf       POSIX */
#include <stdlib.h>    /* Standard Lib    C89   */
#include <unistd.h>    /* UNIX std stf    POSIX */
#include <fcntl.h>     /* UNIX file ctrl  UNIX  */
#include <sys/mman.h>  /* mmap()          POSIX */
#include <sys/stat.h>  /* UNIX stat       POSIX */

int main(int argc, char *argv[])
{
    int FD;
    char *fileName = "T:mmapFile.tmp";
    char *fileData;

    /* ------------------------------------------------------------------------------------------------------------------------------ */
    /* First we must make a file with some data in it...*/

    /* Open our test file (wack it if it already exists). */
    if ((FD = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
    {
        printf("ERROR: Could not open file (for write only).\n");
        exit(1);
    } /* end if */

    /* Note we write the \0 too. */
    if (write(FD, "0123456789abcdef", 17) < 0)
    {
        printf("ERROR: Could not write to file.\n");
        exit(1);
    } /* end if */

    /* Close the test file. */
    if (close(FD) < 0)
    {
        printf("ERROR: Could not close the file\n");
        exit(1);
    } /* end if */

    /* ------------------------------------------------------------------------------------------------------------------------------ */
    /* Now we have a test file to play with (/tmp/mmapFile.tmp).  Let's
     access it with mmap(). */

    /* First we have to open the file! */
    if ((FD = open(fileName, O_RDWR)) < 0)
    {
        printf("ERROR(%d): Could not open file (for read/write).\n", errno);
        exit(1);
    } /* end if */

    /* Now we must map the thing into our memory space.  A size is required, so use stat() first to figure out how long a file is if
     you don't already know.  Note: It is VERY important that the file length NOT change while it is mmap'ed.*/
    fileData = (char *)mmap(NULL,                   // Almost always not used
                            17,                     // Length of the mapped space
                            PROT_READ | PROT_WRITE, // Access type
                            MAP_SHARED,             // Write changes to device (see: MAP_PRIVATE)
                            FD,                     // FD of the file to map
                            0);                     // Offset into file.

    /* Checking for an error with mmap is strange.  Instead of returning NULL, which any God fearing, pointer returning function
     should do upon error, it returns MAP_FAILED.  On many platforms, this is 0 or -1, but on some it is an address that is simply
     invalid for the current program... */
    if (fileData == MAP_FAILED)
    {
        /* Some of the following are only listed for completeness as they can not happen in this case... */
        switch (errno)
        {
        case EACCES:
            printf("mmap failed: The FD was not open for read, or for write with (PROT_WRITE or MAP_SHARED)\n");
            break;
        case EAGAIN:
            printf("mmap failed: The mapping could not be locked in memory\n");
            break;
        case EBADF:
            printf("mmap failed: The FD not a valid open file descriptor.\n");
            break;
        case EINVAL:
            printf("mmap failed: The value of len is zero, addr is not valid, bad combination of args\n");
            break;
        case EMFILE:
            printf("mmap failed: The too many regions mapped already\n");
            break;
        case ENODEV:
            printf("mmap failed: The FD file type is not supported by mmap().\n");
            break;
        case ENOMEM:
            printf("mmap failed: Not enough memory\n");
            break;
        case ENOTSUP:
            printf("mmap failed: Options not supported on this platform\n");
            break;
        case ENXIO:
            printf("mmap failed: Range [off,off+len) are invalid for the FD, MAP_FIXED & invalid addresses, or FD not accessible\n");
            break;
        case EOVERFLOW:
            printf("mmap failed: File is too big!\n");
            break;
        default:
            printf("mmap failed: Duno why! (errno: %d)\n", errno);
            break;
        } /* end switch */
        exit(1);
    } /* end if */

    /* ------------------------------------------------------------------------------------------------------------------------------ */
    /* Now we can use the fileData pointer just like any other pointer! */

    /* We put the \0 in the file, so we can use the file content as a string! */
    printf("File content: '%s'\n", fileData);

    /* fileData is just a pointer.. */
    printf("The 4th char of the file is: %c\n", fileData[3]);

    /* ------------------------------------------------------------------------------------------------------------------------------ */
    /* We are done with the map, so we can unmap it and close the file now. */

    /* Unmap the file now. */
    if (munmap(fileData, 17) < 0)
    {
        switch (errno)
        {
        case EINVAL:
            printf("munmap failed: The address range [addr,addr+len) is invalid.\n"
                   "               munmap failed: The len argument is 0.\n"
                   "               munmap failed: The addr argument is not a multiple of page size.\n");
            break;
        default:
            printf("munmap failed: Duno why!  (errno %d).\n", errno);
            break;
        } /* end switch */
        exit(1);
    } /* end if */

    /* Close the test file. */
    if (close(FD) < 0)
    {
        printf("ERROR: Could not close the file\n");
        exit(1);
    } /* end if */

    return 0;
} /* end func main */
