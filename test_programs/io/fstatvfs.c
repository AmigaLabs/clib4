#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    // Check if a file path is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open the file and get the file descriptor
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Create a statvfs structure
    struct statvfs svfs;

    // Use fstatvfs() to get file system information
    if (fstatvfs(fd, &svfs) == -1) {
        perror("Error getting file system info");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Close the file
    close(fd);

    // Print the file system information
    printf("File system information for %s:\n", argv[1]);
    printf("Block size (f_bsize): %ld bytes\n", svfs.f_bsize);
    printf("Fragment size (f_frsize): %ld bytes\n", svfs.f_frsize);
    printf("Total number of blocks (f_blocks): %lld\n", svfs.f_blocks);
    printf("Free blocks available to unprivileged user (f_bfree): %lld\n", svfs.f_bfree);
    printf("Free blocks for privileged user (f_bavail): %lld\n", svfs.f_bavail);
    printf("Total number of inodes (f_files): %lld\n", svfs.f_files);
    printf("Free inodes (f_ffree): %lld\n", svfs.f_ffree);
    printf("Available inodes (f_favail): %lld\n", svfs.f_favail);
    printf("File system ID (f_fsid): %ld\n", svfs.f_fsid);
    printf("Mount flags (f_flag): %ld\n", svfs.f_flag);
    printf("Maximum filename length (f_namemax): %ld\n", svfs.f_namemax);
    printf("Type Name (f_fstypename): %s\n", svfs.f_fstypename);
    printf("Mount Name (f_mntonname): %s\n", svfs.f_mntonname);

    return 0;
}
