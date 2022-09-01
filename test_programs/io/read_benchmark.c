#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>

int READ_BUFFER = 2048;
#define FILE_PATH ("test.txt")

int testRead(char *file_path);
int testFread(char *file_path);
int testFreadWithBuffer(char *file_path);

int testRead(char *file_path) {
    clock_t begin = clock();
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        printf("can't read test file %s\n", FILE_PATH);
        return errno;
    }

    char buffer[READ_BUFFER];
    int length = sizeof(buffer);
    int read_count, i = 1;
    while ((read_count = read(fd, buffer, length)) != 0) {
        //printBuffer(buffer, read_count);
        i++;
    }
    printf("Read Cycles = %d\n", i);

    close(fd);
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;

    printf("%s\t%lf\n", __func__, time_spent);
}


int
testFread(char *file_path) {
    clock_t begin = clock();

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("can't read test file %s\n", FILE_PATH);
        return errno;
    }

    char buffer[READ_BUFFER];
    int length = sizeof(buffer);
    int read_count, i = 1;
    while ((read_count = fread(buffer, 1, length, file)) != 0) {
        // printBuffer(buffer, read_count);
        i++;
    }
    printf("Read Cycles = %d\n", i);
    fclose(file);

    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("%s\t%lf\n", __func__, time_spent);
}

int
testFreadWithBuffer(char *file_path) {
    clock_t begin = clock();
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("can't read test file %s\n", FILE_PATH);
        return errno;
    }

    char buffer[READ_BUFFER];
    int length = sizeof(buffer);
    setvbuf(file, NULL, _IOFBF, READ_BUFFER * 4);    // large buffer
    int read_count, i = 1;
    while ((read_count = fread(buffer, 1, length, file)) != 0) {
        // printBuffer(buffer, read_count);
        i++;
    }
    printf("Read Cycles = %d\n", i);
    fclose(file);

    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("%s\t%lf\n", __func__, time_spent);
}


void
printBuffer(char *buffer, int count) {
    int length = sizeof(buffer);
    if (count == length) {
        printf("%s", buffer);
    } else {
        for (int i = 0; i < count; i++)
            printf("%c", buffer[i]);
    }
}

int
main(int argc, char **argv) {
    char *filename = FILE_PATH;
    if (argc > 1)
        filename = argv[1];
    if (argc > 2) {
        READ_BUFFER = atoi(argv[2]);
        if (READ_BUFFER <= 0)
            READ_BUFFER = 2048;
    }
    printf("Using READ BUFFER %d\n", 2048);

    testRead(filename);
    testFread(filename);
    testFreadWithBuffer(filename);

    return 0;
}