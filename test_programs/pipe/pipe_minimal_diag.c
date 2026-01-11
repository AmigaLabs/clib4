/*
 * Minimal diagnostic test - like pipe_simple_rw but with hex dumps
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define TEST_STRING "HELLO"

int main(void) {
    int pipefd[2];
    char write_buf[20];
    char read_buf[20];
    
    printf("=== MINIMAL DIAGNOSTIC TEST ===\n\n");
    
    /* Initialize buffers */
    memset(write_buf, 'Z', sizeof(write_buf));
    memcpy(write_buf, TEST_STRING, strlen(TEST_STRING));
    write_buf[strlen(TEST_STRING)] = '\0';
    
    memset(read_buf, 'A', sizeof(read_buf));
    read_buf[sizeof(read_buf) - 1] = '\0';
    
    /* Create pipe */
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    
    printf("Pipe: read=%d, write=%d\n", pipefd[0], pipefd[1]);
    
    /* Write */
    printf("Writing '%s'...\n", TEST_STRING);
    ssize_t nwritten = write(pipefd[1], write_buf, strlen(TEST_STRING));
    printf("write() = %ld\n", (long)nwritten);
    
    /* Read immediately */
    printf("Reading...\n");
    ssize_t nread = read(pipefd[0], read_buf, strlen(TEST_STRING));
    printf("read() = %ld\n", (long)nread);
    
    read_buf[nread] = '\0';
    
    /* Show results */
    printf("\nResult: '%s'\n", read_buf);
    printf("Expected: '%s'\n", TEST_STRING);
    
    printf("\nHex comparison:\n");
    for (size_t i = 0; i < strlen(TEST_STRING); i++) {
        printf("  [%zu] got=0x%02X expected=0x%02X %s\n",
               i, (unsigned char)read_buf[i], (unsigned char)TEST_STRING[i],
               (read_buf[i] == TEST_STRING[i]) ? "✓" : "✗");
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    if (strcmp(read_buf, TEST_STRING) == 0) {
        printf("\n✓ SUCCESS!\n");
        return 0;
    } else {
        printf("\n✗ FAILURE!\n");
        return 1;
    }
}
