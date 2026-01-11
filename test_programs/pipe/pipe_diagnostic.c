/*
 * Diagnostic test for pipe read/write issue
 * This test checks if data written to a pipe is correctly read back
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define TEST_STRING "HELLO"
#define BUFFER_SIZE 20

int main(void) {
    int pipefd[2];
    char write_buf[BUFFER_SIZE];
    char read_buf[BUFFER_SIZE];
    ssize_t nwritten, nread;
    
    printf("=== PIPE DIAGNOSTIC TEST ===\n\n");
    
    /* Initialize buffers with known patterns */
    memset(write_buf, 'Z', sizeof(write_buf));
    memcpy(write_buf, TEST_STRING, strlen(TEST_STRING));
    write_buf[strlen(TEST_STRING)] = '\0';
    
    memset(read_buf, 'A', sizeof(read_buf));
    read_buf[sizeof(read_buf) - 1] = '\0';
    
    printf("Write buffer before write:\n");
    printf("  Content: '%s'\n", write_buf);
    printf("  Hex dump: ");
    for (int i = 0; i < 10; i++) {
        printf("%02X ", (unsigned char)write_buf[i]);
    }
    printf("\n");
    printf("  Address: %p\n\n", write_buf);
    
    printf("Read buffer before read (filled with 'A'):\n");
    printf("  Content: '%s'\n", read_buf);
    printf("  Hex dump: ");
    for (int i = 0; i < 10; i++) {
        printf("%02X ", (unsigned char)read_buf[i]);
    }
    printf("\n");
    printf("  Address: %p\n\n", read_buf);
    
    /* Create pipe */
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    
    printf("Pipe created successfully:\n");
    printf("  Read FD:  %d\n", pipefd[0]);
    printf("  Write FD: %d\n\n", pipefd[1]);
    
    /* Write test string */
    printf("Writing %zu bytes: '%s'\n", strlen(TEST_STRING), TEST_STRING);
    printf("  From address: %p\n", write_buf);
    printf("  To FD: %d\n", pipefd[1]);
    
    nwritten = write(pipefd[1], write_buf, strlen(TEST_STRING));
    
    printf("  write() returned: %ld\n", (long)nwritten);
    if (nwritten < 0) {
        printf("  errno: %d (%s)\n", errno, strerror(errno));
        return 1;
    }
    printf("\n");
    
    /* Small delay to ensure data is in pipe */
    usleep(10000);
    
    /* Verify write buffer wasn't corrupted */
    printf("Write buffer after write:\n");
    printf("  Content: '%s'\n", write_buf);
    printf("  Hex dump: ");
    for (int i = 0; i < 10; i++) {
        printf("%02X ", (unsigned char)write_buf[i]);
    }
    printf("\n\n");
    
    /* Read back */
    printf("Reading %zu bytes:\n", strlen(TEST_STRING));
    printf("  Into address: %p\n", read_buf);
    printf("  From FD: %d\n", pipefd[0]);
    
    /* Show read buffer BEFORE read */
    printf("  Buffer BEFORE read: ");
    for (int i = 0; i < 10; i++) {
        printf("%02X ", (unsigned char)read_buf[i]);
    }
    printf("\n");
    
    nread = read(pipefd[0], read_buf, strlen(TEST_STRING));
    
    printf("  read() returned: %ld\n", (long)nread);
    if (nread < 0) {
        printf("  errno: %d (%s)\n", errno, strerror(errno));
        return 1;
    }
    
    /* Show read buffer AFTER read */
    printf("  Buffer AFTER read:  ");
    for (int i = 0; i < 10; i++) {
        printf("%02X ", (unsigned char)read_buf[i]);
    }
    printf("\n");
    
    read_buf[nread] = '\0';
    
    printf("\nRead buffer contents:\n");
    printf("  As string: '%s'\n", read_buf);
    printf("  Expected:  '%s'\n", TEST_STRING);
    printf("  Length: %zu (expected %zu)\n\n", strlen(read_buf), strlen(TEST_STRING));
    
    /* Byte-by-byte comparison */
    printf("Byte-by-byte comparison:\n");
    int mismatches = 0;
    for (size_t i = 0; i < strlen(TEST_STRING); i++) {
        char got = read_buf[i];
        char expected = TEST_STRING[i];
        printf("  [%zu] got=0x%02X ('%c'), expected=0x%02X ('%c') %s\n",
               i, (unsigned char)got, got, 
               (unsigned char)expected, expected,
               (got == expected) ? "✓" : "✗ MISMATCH");
        if (got != expected) mismatches++;
    }
    
    /* Close pipe */
    close(pipefd[0]);
    close(pipefd[1]);
    
    printf("\n");
    if (mismatches == 0) {
        printf("✓ SUCCESS: All bytes match!\n");
        return 0;
    } else {
        printf("✗ FAILURE: %d byte(s) mismatch!\n", mismatches);
        return 1;
    }
}
