#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define SHM_SIZE 4096

int main() {
    int fd = shm_open("/my_shared_mem", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        printf("shm_open failed!\n");
        return 1;
    }

    // Set the size of the shared memory object
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // Map the shared memory object into the process's address space
    void *shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Write something into shared memory
    const char *message = "Hello from shared memory!";
    memcpy(shm_ptr, message, strlen(message) + 1);

    printf("Message written to shared memory: %s\n", (char *) shm_ptr);

    // Unmap and close the shared memory
    munmap(shm_ptr, SHM_SIZE);

    sleep(20);  // Keep alive for Process 2 to read
    shm_unlink("/my_shared_mem");
    return 0;
}
