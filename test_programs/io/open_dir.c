#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    const char *dir_path = "T:"; // Use the current directory
    int fd;
    struct stat sb;

    // Open the directory
    printf("Attempting to open directory: '%s'\n", dir_path);
    fd = open(dir_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("Directory opened successfully with file descriptor: %d\n", fd);

    // Get file status using fstat() on the file descriptor
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("fstat() called successfully on the file descriptor.\n");

    // Print some information from the stat buffer
    printf("File type:                ");
    switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:  printf("directory\n");        break;
        case S_IFREG:  printf("regular file\n");     break;
        case S_IFLNK:  printf("symbolic link\n");    break;
        default:       printf("unknown?\n");         break;
    }
    printf("Number of hard links:     %ld\n", (long) sb.st_nlink);
    printf("File size:                %lld bytes\n", (long long) sb.st_size);

    // Close the file descriptor
    close(fd);
    printf("File descriptor closed.\n");

    return 0;
}
