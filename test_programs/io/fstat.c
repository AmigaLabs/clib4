#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

int main() {
    char fn[] = "temp.file";
    struct stat info;
    int file_descriptor;

    if ((file_descriptor = creat(fn, S_IWUSR)) < 0)
        perror("creat() error");
    else {
        if (fstat(file_descriptor, &info) != 0)
            perror("fstat() error");
        else {
            puts("fstat() returned:");
            printf("  inode:   %d\n", (int) info.st_ino);
            printf(" dev id:   %d\n", (int) info.st_dev);
            printf("   mode:   %08x\n", info.st_mode);
            printf("  links:   %d\n", info.st_nlink);
            printf("    uid:   %d\n", (int) info.st_uid);
            printf("    gid:   %d\n", (int) info.st_gid);
            printf("   name:   %s\n", (int) info.st_name); //clib2 extension
        }
        close(file_descriptor);
        unlink(fn);
    }

    return 0;
}