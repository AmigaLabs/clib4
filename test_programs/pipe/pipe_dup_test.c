/*
 * Test to verify if DupFileHandle() works correctly with PIPE:
 * This test checks if duplicating a pipe file handle allows both
 * handles to read/write from the same pipe buffer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <proto/dos.h>

#define TEST_DATA "Hello from pipe!"
#define TEST_SIZE (sizeof(TEST_DATA) - 1)

int main(void) {
    printf("=== Pipe DupFileHandle Test ===\n\n");
    
    /* Create a pipe */
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    
    printf("Created pipe: read=%d, write=%d\n", pipefd[0], pipefd[1]);
    
    /* Get the AmigaOS BPTR for the write end */
    struct fd *fd_write = __get_file_descriptor(__CLIB4, pipefd[1]);
    if (!fd_write) {
        fprintf(stderr, "Failed to get fd structure for write end\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    
    BPTR original_fh = (BPTR)__resolve_fd_file(fd_write);
    printf("Original write BPTR: 0x%lx\n", (unsigned long)original_fh);
    
    /* Duplicate the write handle */
    BPTR dup_fh = DupFileHandle(original_fh);
    if (!dup_fh) {
        fprintf(stderr, "DupFileHandle failed!\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    printf("Duplicated write BPTR: 0x%lx\n\n", (unsigned long)dup_fh);
    
    /* Write using the duplicated handle */
    printf("Writing '%s' using duplicated handle...\n", TEST_DATA);
    LONG written = Write(dup_fh, TEST_DATA, TEST_SIZE);
    if (written != TEST_SIZE) {
        fprintf(stderr, "Write failed: written=%ld, expected=%d\n", written, TEST_SIZE);
        Close(dup_fh);
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    printf("Successfully wrote %ld bytes\n\n", written);
    
    /* Close the duplicated handle */
    Close(dup_fh);
    
    /* Now try to read from the read end */
    char buffer[256] = {0};
    printf("Reading from read end (fd=%d)...\n", pipefd[0]);
    ssize_t nread = read(pipefd[0], buffer, sizeof(buffer));
    if (nread < 0) {
        perror("read");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
    
    printf("Read %ld bytes: '%.*s'\n", (long)nread, (int)nread, buffer);
    
    /* Verify data */
    if (nread == TEST_SIZE && memcmp(buffer, TEST_DATA, TEST_SIZE) == 0) {
        printf("\n✓ SUCCESS: Data written via DupFileHandle was read correctly!\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return 0;
    } else {
        printf("\n✗ FAIL: Data mismatch!\n");
        printf("  Expected: '%s' (%d bytes)\n", TEST_DATA, TEST_SIZE);
        printf("  Got:      '%.*s' (%ld bytes)\n", (int)nread, buffer, (long)nread);
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }
}
