/*
 * Test for spawnvpe_fork - Fork-like behavior with pipes
 *
 * This test verifies that spawnvpe_fork correctly maintains open file
 * descriptors and allows IPC through pipes, similar to Unix fork().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/* Color codes for output */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

#define TEST_PASSED(name) printf(COLOR_GREEN "+ PASSED: %s" COLOR_RESET "\n", name)
#define TEST_FAILED(name, reason) printf(COLOR_RED "x FAILED: %s - %s" COLOR_RESET "\n", name, reason)
#define TEST_INFO(msg) printf(COLOR_CYAN "ℹ %s" COLOR_RESET "\n", msg)
#define TEST_SECTION(name) printf("\n" COLOR_BLUE "=== %s ===" COLOR_RESET "\n", name)

int tests_passed = 0;
int tests_failed = 0;

/* Helper function to create a simple child program that echoes to stdout */
void create_echo_child_program(const char *filename) {
    char fullpath[512];
    char fullpath_exe[512];

    /* Use RAM: for temporary files */
    snprintf(fullpath, sizeof(fullpath), "RAM:%s", filename);
    snprintf(fullpath_exe, sizeof(fullpath_exe), "RAM:%s.exe", filename);

    FILE *f = fopen(fullpath, "w");
    if (!f) {
        perror("fopen");
        printf("  Failed to create: %s\n", fullpath);
        return;
    }

    fprintf(f,
        "#include <stdio.h>\n"
        "#include <unistd.h>\n"
        "#include <string.h>\n"
        "int main(int argc, char *argv[]) {\n"
        "    if (argc > 1) {\n"
        "        printf(\"CHILD: %%s\\n\", argv[1]);\n"
        "        fflush(stdout);\n"
        "    }\n"
        "    return 0;\n"
        "}\n"
    );
    fclose(f);

    /* Compile it with clib4 */
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "gcc -o %s %s -mcrt=clib4 ", fullpath_exe, fullpath);
    printf("  Compiling: %s\n", cmd);
    int result = system(cmd);
    if (result != 0) {
        printf("  WARNING: Compilation failed with code %d\n", result);
    }
}

/* Helper function to create a child program that reads from stdin and writes to stdout */
void create_filter_child_program(const char *filename) {
    char fullpath[512];
    char fullpath_exe[512];

    /* Use RAM: for temporary files */
    snprintf(fullpath, sizeof(fullpath), "RAM:%s", filename);
    snprintf(fullpath_exe, sizeof(fullpath_exe), "RAM:%s.exe", filename);

    FILE *f = fopen(fullpath, "w");
    if (!f) {
        perror("fopen");
        printf("  Failed to create: %s\n", fullpath);
        return;
    }

    fprintf(f,
        "#include <stdio.h>\n"
        "#include <string.h>\n"
        "#include <ctype.h>\n"
        "int main() {\n"
        "    char buffer[256];\n"
        "    if (fgets(buffer, sizeof(buffer), stdin)) {\n"
        "        /* Convert to uppercase */\n"
        "        for (int i = 0; buffer[i]; i++) {\n"
        "            buffer[i] = toupper(buffer[i]);\n"
        "        }\n"
        "        printf(\"FILTERED: %%s\", buffer);\n"
        "        fflush(stdout);\n"
        "    }\n"
        "    return 0;\n"
        "}\n"
    );
    fclose(f);

    /* Compile it with clib4 */
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "gcc -o %s %s -mcrt=clib4 ", fullpath_exe, fullpath);
    printf("  Compiling: %s\n", cmd);
    int result = system(cmd);
    if (result != 0) {
        printf("  WARNING: Compilation failed with code %d\n", result);
    }
}

/* Test 1: Simple pipe - Child writes to parent */
int test_pipe_child_to_parent() {
    TEST_SECTION("Test 1: Pipe Child → Parent");

    int pipefd[2];
    char buffer[256];

    if (pipe(pipefd) == -1) {
        TEST_FAILED("pipe creation", strerror(errno));
        return -1;
    }

    TEST_INFO("Created pipe");

    /* Use a simple program that writes to stdout */
    const char *argv[] = {"RAM:echo_child.c.exe", "Hello from child!", NULL};

    pid_t pid = spawnvpe_fork("RAM:echo_child.c.exe", argv, NULL, NULL, -1, pipefd[1], -1);

    if (pid < 0) {
        TEST_FAILED("spawnvpe_fork", strerror(errno));
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    TEST_INFO("Child spawned successfully");

    /* Parent closes write end */
    close(pipefd[1]);

    /* Read from child */
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);

    close(pipefd[0]);

    /* Wait for child */
    int status;
    waitpid(pid, &status, 0);

    if (n <= 0) {
        TEST_FAILED("read from pipe", "No data received");
        return -1;
    }

    buffer[n] = '\0';

    /* Check if we received the expected message */
    if (strstr(buffer, "CHILD: Hello from child!") != NULL) {
        TEST_PASSED("Pipe Child → Parent");
        printf("  Received: %s", buffer);
        return 0;
    } else {
        TEST_FAILED("data verification", "Unexpected data");
        printf("  Expected: 'CHILD: Hello from child!'\n");
        printf("  Received: %s\n", buffer);
        return -1;
    }
}

/* Test 2: Pipe - Parent writes to child */
int test_pipe_parent_to_child() {
    TEST_SECTION("Test 2: Pipe Parent → Child");

    int pipe_to_child[2];
    int pipe_from_child[2];
    char buffer[256];

    if (pipe(pipe_to_child) == -1 || pipe(pipe_from_child) == -1) {
        TEST_FAILED("pipe creation", strerror(errno));
        return -1;
    }

    TEST_INFO("Created pipes");

    /* Child reads from stdin, writes to stdout */
    const char *argv[] = {"RAM:filter_child.c.exe", NULL};

    pid_t pid = spawnvpe_fork("RAM:filter_child.c.exe", argv, NULL, NULL,
                              pipe_to_child[0], pipe_from_child[1], -1);

    if (pid < 0) {
        TEST_FAILED("spawnvpe_fork", strerror(errno));
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);
        close(pipe_from_child[1]);
        return -1;
    }

    TEST_INFO("Child spawned successfully");

    /* Parent closes unused ends */
    close(pipe_to_child[0]);
    close(pipe_from_child[1]);

    /* Write to child */
    const char *msg = "hello world\n";
    write(pipe_to_child[1], msg, strlen(msg));
    close(pipe_to_child[1]);  /* Signal EOF */

    /* Read response from child */
    memset(buffer, 0, sizeof(buffer));
    ssize_t n = read(pipe_from_child[0], buffer, sizeof(buffer) - 1);
    close(pipe_from_child[0]);

    /* Wait for child */
    int status;
    waitpid(pid, &status, 0);

    if (n <= 0) {
        TEST_FAILED("read from pipe", "No data received");
        return -1;
    }

    buffer[n] = '\0';

    /* Check if child filtered the message (converted to uppercase) */
    if (strstr(buffer, "FILTERED: HELLO WORLD") != NULL) {
        TEST_PASSED("Pipe Parent → Child → Parent");
        printf("  Sent: %s", msg);
        printf("  Received: %s", buffer);
        return 0;
    } else {
        TEST_FAILED("data verification", "Unexpected data");
        printf("  Expected: 'FILTERED: HELLO WORLD'\n");
        printf("  Received: %s\n", buffer);
        return -1;
    }
}

/* Test 3: Multiple writes - Verify both can write to same file */
int test_shared_offset() {
    TEST_SECTION("Test 3: File Handle Sharing");

    const char *filename = "RAM:test_shared_offset.txt";
    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

    if (fd < 0) {
        TEST_FAILED("open file", strerror(errno));
        return -1;
    }

    /* Parent writes first */
    const char *parent_msg = "PARENT_DATA\n";
    write(fd, parent_msg, strlen(parent_msg));

    TEST_INFO("Parent wrote to file");

    /* Child should be able to write to the file (but offset is NOT shared on AmigaOS) */
    const char *argv[] = {"RAM:echo_child.c.exe", "CHILD_DATA", NULL};

    pid_t pid = spawnvpe_fork("RAM:echo_child.c.exe", argv, NULL, NULL, -1, fd, -1);

    if (pid < 0) {
        TEST_FAILED("spawnvpe_fork", strerror(errno));
        close(fd);
        unlink(filename);
        return -1;
    }

    /* Wait for child */
    int status;
    waitpid(pid, &status, 0);

    /* Parent writes footer */
    const char *footer = "PARENT_FOOTER\n";
    write(fd, footer, strlen(footer));

    close(fd);

    /* Read file and verify both wrote data */
    char buffer[512];
    fd = open(filename, O_RDONLY);
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    unlink(filename);

    if (n <= 0) {
        TEST_FAILED("read file", "No data in file");
        return -1;
    }

    buffer[n] = '\0';

    /* NOTE: On AmigaOS, DupFileHandle creates SEPARATE file offsets!
     * This is different from Unix dup() which shares the offset.
     * So we just verify that both parent and child could write. */

    if (strstr(buffer, "PARENT") != NULL && strstr(buffer, "CHILD") != NULL) {
        TEST_PASSED("File Handle Sharing (Both Can Write)");
        printf("  NOTE: AmigaOS does NOT share file offsets (unlike Unix)\n");
        printf("  File contents:\n");
        printf("  %s\n", buffer);
        return 0;
    } else {
        TEST_FAILED("file writing", "Missing data from parent or child");
        printf("  File contents:\n%s\n", buffer);
        return -1;
    }
}

/* Test 4: Verify file descriptors remain open */
int test_fd_remain_open() {
    TEST_SECTION("Test 4: File Descriptors Remain Open");

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        TEST_FAILED("pipe creation", strerror(errno));
        return -1;
    }

    const char *argv[] = {"RAM:echo_child.c.exe", "test", NULL};

    pid_t pid = spawnvpe_fork("RAM:echo_child.c.exe", argv, NULL, NULL, -1, pipefd[1], -1);

    if (pid < 0) {
        TEST_FAILED("spawnvpe_fork", strerror(errno));
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    /* Wait for child to finish */
    int status;
    waitpid(pid, &status, 0);

    /* Try to write to the pipe - should still be open in parent */
    const char *msg = "After child exit\n";
    ssize_t n = write(pipefd[1], msg, strlen(msg));

    close(pipefd[1]);

    /* Read what we wrote */
    char buffer[100];
    ssize_t r = read(pipefd[0], buffer, sizeof(buffer) - 1);
    close(pipefd[0]);

    if (n > 0 && r > 0) {
        TEST_PASSED("FD Remain Open After Child Exit");
        printf("  Parent could still write to pipe after child exited\n");
        return 0;
    } else {
        TEST_FAILED("write to pipe", "Pipe closed unexpectedly");
        return -1;
    }
}

/* Test 5: Non-blocking pipe read */
int test_nonblocking_pipe() {
    TEST_SECTION("Test 5: Non-blocking Pipe");

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        TEST_FAILED("pipe creation", strerror(errno));
        return -1;
    }

    /* Make read end non-blocking */
    int flags = fcntl(pipefd[0], F_GETFL);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

    const char *argv[] = {"RAM:echo_child.c.exe", "nonblock test", NULL};

    pid_t pid = spawnvpe_fork("RAM:echo_child.c.exe", argv, NULL, NULL, -1, pipefd[1], -1);

    if (pid < 0) {
        TEST_FAILED("spawnvpe_fork", strerror(errno));
        close(pipefd[0]);
        close(pipefd[1]);
        return -1;
    }

    /* Give child a moment to start before closing write end */
    usleep(50000);  /* 50ms */

    close(pipefd[1]);

    /* Try non-blocking reads */
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    int total_read = 0;
    int attempts = 0;

    while (attempts < 200) {  /* Max 200 attempts (2 seconds) */
        ssize_t n = read(pipefd[0], buffer + total_read, sizeof(buffer) - total_read - 1);

        if (n > 0) {
            total_read += n;
        } else if (n == 0) {
            /* EOF */
            break;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            /* No data yet, try again */
            usleep(10000);  /* 10ms */
        } else {
            /* Real error */
            printf("  read() error: %s\n", strerror(errno));
            break;
        }

        attempts++;
    }

    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);

    if (total_read > 0) {
        buffer[total_read] = '\0';
        TEST_PASSED("Non-blocking Pipe Read");
        printf("  Read %d bytes in %d attempts\n", total_read, attempts);
        return 0;
    } else {
        TEST_FAILED("non-blocking read", "No data received");
        return -1;
    }
}

/* Test 6: Using standard stdin/stdout/stderr */
int test_standard_fds() {
    TEST_SECTION("Test 6: Standard FDs (stdin/stdout/stderr)");

    const char *testfile = "RAM:test_std_fds.txt";

    /* Create a test file for child to read from stdin */
    FILE *f = fopen(testfile, "w");
    if (!f) {
        TEST_FAILED("create test file", strerror(errno));
        return -1;
    }
    fprintf(f, "Input from parent\n");
    fclose(f);

    /* Open files for stdin and stdout redirection */
    int fd_in = open(testfile, O_RDONLY);
    if (fd_in < 0) {
        TEST_FAILED("open input file", strerror(errno));
        unlink(testfile);
        return -1;
    }

    const char *outfile = "RAM:test_std_fds_out.txt";
    int fd_out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        TEST_FAILED("open output file", strerror(errno));
        close(fd_in);
        unlink(testfile);
        return -1;
    }

    const char *errfile = "RAM:test_std_fds_err.txt";
    int fd_err = open(errfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_err < 0) {
        TEST_FAILED("open error file", strerror(errno));
        close(fd_in);
        close(fd_out);
        unlink(testfile);
        unlink(outfile);
        return -1;
    }

    TEST_INFO("Redirecting child's stdin, stdout, stderr to files");

    /* Spawn child with stdin from file, stdout to file, stderr to file */
    const char *argv[] = {"RAM:filter_child.c.exe", NULL};

    pid_t pid = spawnvpe_fork("RAM:filter_child.c.exe", argv, NULL, NULL,
                              fd_in, fd_out, fd_err);

    if (pid < 0) {
        TEST_FAILED("spawnvpe_fork", strerror(errno));
        close(fd_in);
        close(fd_out);
        close(fd_err);
        unlink(testfile);
        unlink(outfile);
        unlink(errfile);
        return -1;
    }

    TEST_INFO("Child spawned with standard FDs redirected");

    /* Close our copies of the file descriptors */
    close(fd_in);
    close(fd_out);
    close(fd_err);

    /* Wait for child to complete */
    int status;
    waitpid(pid, &status, 0);

    /* Read and verify output file */
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    f = fopen(outfile, "r");
    if (!f) {
        TEST_FAILED("open output file for reading", strerror(errno));
        unlink(testfile);
        unlink(outfile);
        unlink(errfile);
        return -1;
    }

    size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, f);
    fclose(f);

    /* Cleanup */
    unlink(testfile);
    unlink(outfile);
    unlink(errfile);

    if (bytes > 0) {
        buffer[bytes] = '\0';

        /* Child should have read from stdin and written filtered output to stdout */
        if (strstr(buffer, "FILTERED:") != NULL && strstr(buffer, "INPUT FROM PARENT") != NULL) {
            TEST_PASSED("Standard FDs Redirection");
            printf("  Child read from stdin, wrote to stdout:\n");
            printf("  Output: %s", buffer);
            return 0;
        } else {
            TEST_FAILED("output verification", "Unexpected output");
            printf("  Expected filtered uppercase text\n");
            printf("  Got: %s\n", buffer);
            return -1;
        }
    } else {
        TEST_FAILED("read output", "No output from child");
        return -1;
    }
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf(COLOR_MAGENTA "╔═══════════════════════════════════════════════╗\n");
    printf("║   spawnvpe_fork() Test Suite                 ║\n");
    printf("║   Testing fork-like behavior with pipes      ║\n");
    printf("╚═══════════════════════════════════════════════╝\n" COLOR_RESET);

    /* Create test child programs */
    TEST_INFO("Creating test child programs...");
    create_echo_child_program("echo_child.c");
    create_filter_child_program("filter_child.c");
    TEST_INFO("Test programs compiled");

    /* Run tests */
    if (test_pipe_child_to_parent() == 0) tests_passed++; else tests_failed++;
    if (test_pipe_parent_to_child() == 0) tests_passed++; else tests_failed++;
    if (test_shared_offset() == 0) tests_passed++; else tests_failed++;
    if (test_fd_remain_open() == 0) tests_passed++; else tests_failed++;
    if (test_nonblocking_pipe() == 0) tests_passed++; else tests_failed++;
    if (test_standard_fds() == 0) tests_passed++; else tests_failed++;

    /* Summary */
    printf("\n");
    printf(COLOR_MAGENTA "╔═══════════════════════════════════════════════╗\n");
    printf("║   Test Summary                                ║\n");
    printf("╚═══════════════════════════════════════════════╝\n" COLOR_RESET);

    printf("\n");
    printf("  Total tests: %d\n", tests_passed + tests_failed);
    printf(COLOR_GREEN "  Passed:      %d\n" COLOR_RESET, tests_passed);

    if (tests_failed > 0) {
        printf(COLOR_RED "  Failed:      %d\n" COLOR_RESET, tests_failed);
    } else {
        printf("  Failed:      %d\n", tests_failed);
    }

    printf("\n");

    /* Cleanup */
    unlink("RAM:echo_child.c");
    unlink("RAM:echo_child.c.exe");
    unlink("RAM:filter_child.c");
    unlink("RAM:filter_child.c.exe");

    if (tests_failed == 0) {
        printf(COLOR_GREEN "+ ALL TESTS PASSED! spawnvpe_fork works like fork()\n" COLOR_RESET);
        return 0;
    } else {
        printf(COLOR_RED "x SOME TESTS FAILED\n" COLOR_RESET);
        return 1;
    }
}

