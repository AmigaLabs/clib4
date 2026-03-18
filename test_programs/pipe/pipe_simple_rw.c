/*
 * Simplest possible pipe test - write and read in same process
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    int pipefd[2];
    char write_data[] = "HELLO";
    char read_data[20];
    
    printf("=== SIMPLE PIPE READ/WRITE TEST ===\n\n");
    
    /* Initialize read buffer */
    memset(read_data, 'A', sizeof(read_data));
    read_data[sizeof(read_data) - 1] = '\0';
    
    /* Create pipe */
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    
    printf("Pipe created: read_fd=%d, write_fd=%d\n\n", pipefd[0], pipefd[1]);
    
    /* Write */
    printf("Writing '%s' (%zu bytes)...\n", write_data, strlen(write_data));
    ssize_t nwritten = write(pipefd[1], write_data, strlen(write_data));
    printf("write() returned: %ld\n", (long)nwritten);
    if (nwritten < 0) {
        perror("write");
        return 1;
    }
    printf("\n");
    
    /* Read */
    printf("Reading %zu bytes...\n", strlen(write_data));
    printf("Buffer before read: '%s'\n", read_data);
    
    ssize_t nread = read(pipefd[0], read_data, strlen(write_data));
    printf("read() returned: %ld\n", (long)nread);
    if (nread < 0) {
        perror("read");
        return 1;
    }
    
    read_data[nread] = '\0';
    printf("Buffer after read: '%s'\n", read_data);
    printf("Expected: '%s'\n\n", write_data);
    
    /* Compare */
    if (strcmp(read_data, write_data) == 0) {
        printf("+ SUCCESS!\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return 0;
    } else {
        printf("x FAILURE: data mismatch!\n");
        printf("  Got:      '%s'\n", read_data);
        printf("  Expected: '%s'\n", write_data);
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
}
