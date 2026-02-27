#include <stdio.h>

extern int __get_default_file(int file_descriptor, long *file_ptr);

typedef long BPTR;

int main() {
    BPTR default_file;
    int error = 0;

    error = __get_default_file(0, &default_file);
    if (error != 0)
        printf("Error getting default STDIN BPTR file\n");
    else
        printf("Default STDIN BPTR file at %p\n", default_file);
}