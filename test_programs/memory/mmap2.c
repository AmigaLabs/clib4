#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int id;
    char message[32];
} Record;

int main() {
    const char *filepath = "data.bin";
    size_t size = sizeof(Record);

    // 1. Open the file (Read/Write, create if it doesn't exist)
    int fd = open(filepath, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // 2. Resize the file to hold our struct
    if (ftruncate(fd, size) == -1) {
        perror("Error ftruncate");
        close(fd);
        return 1;
    }

    // 3. Map the file into memory
    // PROT_READ | PROT_WRITE: we can read and write to this memory
    // MAP_SHARED: changes are shared with other processes and reflected in the file
    Record *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error mmap");
        close(fd);
        return 1;
    }

    // 4. Write data directly to memory (as if it were a normal pointer)
    ptr->id = 42;
    strncpy(ptr->message, "Hello from mmap!", 32);
    printf("Data written to memory: %d, %s\n", ptr->id, ptr->message);

    // 5. SYNCHRONIZATION (msync)
    // MS_SYNC forces the kernel to flush the data to disk BEFORE continuing
    printf("Syncing to disk...\n");
    if (msync(ptr, size, MS_SYNC) == -1) {
        perror("Error msync");
    } else {
        printf("Data successfully persisted to disk.\n");
    }

    // 6. Cleanup
    munmap(ptr, size);
    close(fd);

    return 0;
}