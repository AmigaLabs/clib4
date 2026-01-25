/*
 * Simple test for spawnvpe_fork - Direct pipe test
 *
 * This test uses existing AmigaOS commands to test pipe functionality
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define TEST_OK(msg) printf("+ %s\n", msg)
#define TEST_FAIL(msg) printf("x FAILED: %s\n", msg)

int main() {
    printf("\n=== Simple spawnvpe_fork Pipe Test ===\n\n");

    int pipefd[2];
    char buffer[1024];
    int status;

    /* Test 1: Use 'echo' to write to pipe */
    printf("Test 1: Child writes to pipe (using echo command)\n");

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* Spawn echo command that writes to stdout (which is our pipe) */
    const char *argv1[] = {"C:echo", "Hello from spawnvpe_fork!", NULL};

    printf("  Spawning: C:echo \"Hello from spawnvpe_fork!\"\n");
    printf("  Redirecting stdout to pipe...\n");

    pid_t pid1 = spawnvpe_fork("C:echo", argv1, NULL, NULL, -1, pipefd[1], -1);

    if (pid1 < 0) {
        TEST_FAIL("spawnvpe_fork failed");
        perror("  spawnvpe_fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    printf("  Child PID: %d\n", pid1);

    /* Close write end in parent */
    close(pipefd[1]);

    /* Read from pipe */
    printf("  Reading from pipe...\n");
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = '\0';
        printf("  Received %zd bytes: \"%s\"\n", n, buffer);

        if (strstr(buffer, "Hello from spawnvpe_fork!")) {
            TEST_OK("Data received correctly through pipe");
        } else {
            TEST_FAIL("Unexpected data");
            printf("    Expected: 'Hello from spawnvpe_fork!'\n");
            printf("    Got: '%s'\n", buffer);
        }
    } else {
        TEST_FAIL("No data received from pipe");
        printf("    read() returned: %zd\n", n);
        if (n < 0) perror("    read");
    }

    close(pipefd[0]);

    /* Wait for child */
    waitpid(pid1, &status, 0);
    printf("  Child exited with status: %d\n", WEXITSTATUS(status));

    /* Test 2: Verify pipe remains open after child exits */
    printf("\nTest 2: Parent writes to pipe after child exits\n");

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* Spawn a quick command */
    const char *argv2[] = {"C:echo", "quick", NULL};
    pid_t pid2 = spawnvpe_fork("C:echo", argv2, NULL, NULL, -1, pipefd[1], -1);

    if (pid2 < 0) {
        TEST_FAIL("spawnvpe_fork failed");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    /* Wait for child to finish */
    waitpid(pid2, &status, 0);
    printf("  Child finished\n");

    /* Now try to write to pipe from parent */
    const char *parent_msg = "Parent writes after child exit\n";
    ssize_t written = write(pipefd[1], parent_msg, strlen(parent_msg));

    if (written > 0) {
        TEST_OK("Parent can still write to pipe after child exit");
        printf("    Wrote %zd bytes\n", written);

        /* Read it back */
        close(pipefd[1]);
        n = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("    Read back: \"%s\"\n", buffer);
        }
    } else {
        TEST_FAIL("Could not write to pipe after child exit");
        printf("    This means file descriptor was closed by DOS!\n");
    }

    close(pipefd[0]);

    /* Test 3: Multiple children sharing same pipe */
    printf("\nTest 3: Multiple children writing to same pipe\n");

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* Spawn multiple children */
    const char *argv3a[] = {"C:echo", "Child1", NULL};
    const char *argv3b[] = {"C:echo", "Child2", NULL};
    const char *argv3c[] = {"C:echo", "Child3", NULL};

    pid_t pid3a = spawnvpe_fork("C:echo", argv3a, NULL, NULL, -1, pipefd[1], -1);
    pid_t pid3b = spawnvpe_fork("C:echo", argv3b, NULL, NULL, -1, pipefd[1], -1);
    pid_t pid3c = spawnvpe_fork("C:echo", argv3c, NULL, NULL, -1, pipefd[1], -1);

    if (pid3a < 0 || pid3b < 0 || pid3c < 0) {
        TEST_FAIL("Could not spawn all children");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    printf("  Spawned 3 children: %d, %d, %d\n", pid3a, pid3b, pid3c);

    /* Close write end in parent */
    close(pipefd[1]);

    /* Read all data */
    memset(buffer, 0, sizeof(buffer));
    int total = 0;
    while ((n = read(pipefd[0], buffer + total, sizeof(buffer) - total - 1)) > 0) {
        total += n;
        if (total >= sizeof(buffer) - 1) break;
    }

    close(pipefd[0]);

    /* Wait for all children */
    waitpid(pid3a, &status, 0);
    waitpid(pid3b, &status, 0);
    waitpid(pid3c, &status, 0);

    if (total > 0) {
        buffer[total] = '\0';
        printf("  Received %d bytes from all children:\n", total);
        printf("    %s\n", buffer);

        /* Check if we got data from all 3 children */
        int count = 0;
        if (strstr(buffer, "Child1")) count++;
        if (strstr(buffer, "Child2")) count++;
        if (strstr(buffer, "Child3")) count++;

        if (count == 3) {
            TEST_OK("All 3 children wrote to shared pipe");
        } else {
            TEST_FAIL("Did not receive data from all children");
            printf("    Received from %d/3 children\n", count);
        }
    } else {
        TEST_FAIL("No data received from children");
    }

    printf("\n=== Test Complete ===\n\n");

    return 0;
}

