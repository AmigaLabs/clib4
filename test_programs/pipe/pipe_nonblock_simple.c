/*
 * Simple test for non-blocking pipe operations
 * Tests basic scenarios with O_NONBLOCK flag
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#define TEST_DATA_SIZE 4096 * 100

/* Set non-blocking mode */
static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        perror("fcntl F_GETFL");
        return -1;
    }
    
    printf("Current flags for fd %d: 0x%x\n", fd, flags);
    
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
        perror("fcntl F_SETFL");
        return -1;
    }
    
    /* Verify the flags were set */
    int new_flags = fcntl(fd, F_GETFL);
    printf("New flags for fd %d: 0x%x (O_NONBLOCK = %s)\n", 
           fd, new_flags, (new_flags & O_NONBLOCK) ? "SET" : "NOT SET");
    
    return 0;
}

/* Child process that echoes data */
int child_main(void) {
    char buffer[1024];
    ssize_t nread;
    
    fprintf(stderr, "[CHILD] Using stdin/stdout for pipes\n");
    
    fprintf(stderr, "[CHILD] Setting stdin to non-blocking\n");
    if (set_nonblocking(STDIN_FILENO) < 0) {
        return 1;
    }
    
    fprintf(stderr, "[CHILD] Setting stdout to non-blocking\n");
    if (set_nonblocking(STDOUT_FILENO) < 0) {
        return 1;
    }
    
    /* Send ready signal to parent */
    char ready_signal = 'R';
    if (write(STDOUT_FILENO, &ready_signal, 1) != 1) {
        fprintf(stderr, "[CHILD] Failed to send ready signal\n");
        return 1;
    }
    fprintf(stderr, "[CHILD] Sent ready signal\n");
    fflush(stderr);

    fprintf(stderr, "[CHILD] Starting echo loop\n");
    fflush(stderr);
    
    int iterations = 0;
    int eagain_read = 0;
    int eagain_write = 0;
    
    while (1) {
        iterations++;
        
        nread = read(STDIN_FILENO, buffer, sizeof(buffer));
        
        if (nread > 0) {
            fprintf(stderr, "[CHILD] Read %ld bytes\n", (long)nread);
            fflush(stderr);
            
            ssize_t written = 0;
            while (written < nread) {
                ssize_t n = write(STDOUT_FILENO, buffer + written, nread - written);
                if (n > 0) {
                    written += n;
                } else if (n < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        eagain_write++;
                        usleep(5000);
                        continue;
                    } else {
                        perror("[CHILD] write");
                        return 1;
                    }
                }
            }
            fprintf(stderr, "[CHILD] Wrote %ld bytes\n", (long)written);
            fflush(stderr);
        } else if (nread == 0) {
            fprintf(stderr, "[CHILD] EOF detected, exiting\n");
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                eagain_read++;
                usleep(5000);
            } else {
                perror("[CHILD] read");
                return 1;
            }
        }
        
        if (iterations > 100000) {
            fprintf(stderr, "[CHILD] Too many iterations, breaking\n");
            break;
        }
    }
    
    fprintf(stderr, "[CHILD] Stats: iterations=%d, EAGAIN_read=%d, EAGAIN_write=%d\n",
           iterations, eagain_read, eagain_write);
    fflush(stderr);
    
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--child") == 0) {
        return child_main();
    }
    
    printf("=== Simple Non-Blocking Pipe Test ===\n\n");
    
    /* Create pipes */
    int pipe_to_child[2], pipe_from_child[2];
    
    if (pipe(pipe_to_child) < 0) {
        perror("pipe(to_child)");
        return 1;
    }
    
    if (pipe(pipe_from_child) < 0) {
        perror("pipe(from_child)");
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        return 1;
    }
    
    printf("Pipes created:\n");
    printf("  to_child: read=%d, write=%d\n", pipe_to_child[0], pipe_to_child[1]);
    printf("  from_child: read=%d, write=%d\n\n", pipe_from_child[0], pipe_from_child[1]);
    
    /* Spawn child with pipe redirection */
    char *child_argv[] = { argv[0], "--child", NULL };
    char *child_env[] = { NULL };
    
    printf("Spawning child process with pipes: stdin=%d, stdout=%d\n",
           pipe_to_child[0], pipe_from_child[1]);
    pid_t pid = spawnvpe(argv[0], (const char **)child_argv, child_env, NULL,
                         pipe_to_child[0], pipe_from_child[1], -1);
    
    if (pid < 0) {
        perror("spawnvpe");
        return 1;
    }
    
    printf("Child spawned with PID %d\n\n", pid);
    
    /* Close unused ends */
    close(pipe_to_child[0]);
    close(pipe_from_child[1]);
    
    /* Set parent's ends to non-blocking */
    printf("Setting parent's pipe ends to non-blocking...\n");
    if (set_nonblocking(pipe_to_child[1]) < 0) {
        goto error;
    }
    if (set_nonblocking(pipe_from_child[0]) < 0) {
        goto error;
    }
    
    /* Wait for child ready signal */
    printf("Waiting for child ready signal...\n");
    char ready_signal;
    int ready_received = 0;
    int wait_attempts = 0;
    while (!ready_received && wait_attempts < 100) {
        ssize_t n = read(pipe_from_child[0], &ready_signal, 1);
        if (n == 1 && ready_signal == 'R') {
            ready_received = 1;
            printf("Child is ready!\n\n");
        } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("Error waiting for ready signal");
            goto error;
        } else {
            usleep(100000);
            wait_attempts++;
        }
    }
    
    if (!ready_received) {
        fprintf(stderr, "Child did not send ready signal\n");
        goto error;
    }
    
    printf("\n=== Test 1: Small write/read ===\n");
    const char *test_msg = "Hello from parent!";
    ssize_t written = write(pipe_to_child[1], test_msg, strlen(test_msg));
    if (written < 0) {
        perror("write");
        goto error;
    }
    printf("Wrote %ld bytes: '%s'\n", (long)written, test_msg);
    
    /* Give child time to process */
    usleep(50000); /* 50ms */
    
    char recv_buf[256];
    ssize_t nread = read(pipe_from_child[0], recv_buf, sizeof(recv_buf) - 1);
    if (nread > 0) {
        recv_buf[nread] = '\0';
        printf("Read %ld bytes: '%s'\n", (long)nread, recv_buf);
        if (strcmp(recv_buf, test_msg) == 0) {
            printf("+ Test 1 PASSED\n");
        } else {
            printf("x Test 1 FAILED: data mismatch\n");
        }
    } else if (nread < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        printf("x Test 1: No data available (EAGAIN)\n");
    } else {
        perror("read");
        goto error;
    }
    
    printf("\n=== Test 2: Large data transfer ===\n");
    char *large_buffer = malloc(TEST_DATA_SIZE);
    char *recv_buffer = malloc(TEST_DATA_SIZE);
    
    if (!large_buffer || !recv_buffer) {
        fprintf(stderr, "malloc failed\n");
        goto error;
    }
    
    /* Fill with pattern */
    for (int i = 0; i < TEST_DATA_SIZE; i++) {
        large_buffer[i] = (char)(i & 0xFF);
    }
    
    /* Write in chunks and read back */
    ssize_t total_written = 0;
    ssize_t total_read = 0;
    int eagain_count = 0;
    
    while (total_written < TEST_DATA_SIZE || total_read < TEST_DATA_SIZE) {
        /* Write chunk */
        if (total_written < TEST_DATA_SIZE) {
            ssize_t n = write(pipe_to_child[1], large_buffer + total_written, 
                            TEST_DATA_SIZE - total_written);
            if (n > 0) {
                total_written += n;
                if (total_written % 1024 == 0 || total_written == TEST_DATA_SIZE) {
                    printf("Written: %ld/%d bytes\n", (long)total_written, TEST_DATA_SIZE);
                }
            } else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                eagain_count++;
            } else if (n < 0) {
                perror("write");
                goto error;
            }
        }
        
        /* Read chunk */
        if (total_read < TEST_DATA_SIZE) {
            ssize_t n = read(pipe_from_child[0], recv_buffer + total_read,
                           TEST_DATA_SIZE - total_read);
            if (n > 0) {
                total_read += n;
                if (total_read % 1024 == 0 || total_read == TEST_DATA_SIZE) {
                    printf("Read: %ld/%d bytes\n", (long)total_read, TEST_DATA_SIZE);
                }
            } else if (n < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
                perror("read");
                goto error;
            }
        }
        
        usleep(1000); /* 1ms */
    }
    
    printf("Transfer complete: %ld bytes written, %ld bytes read\n",
           (long)total_written, (long)total_read);
    printf("EAGAIN encountered %d times\n", eagain_count);
    
    /* Verify data */
    int errors = 0;
    for (int i = 0; i < TEST_DATA_SIZE; i++) {
        if (recv_buffer[i] != large_buffer[i]) {
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("+ Test 2 PASSED: data integrity verified\n");
    } else {
        printf("x Test 2 FAILED: %d byte mismatches\n", errors);
    }
    
    free(large_buffer);
    free(recv_buffer);
    
    /* Close and wait */
    printf("\nClosing pipes and waiting for child...\n");
    close(pipe_to_child[1]);
    close(pipe_from_child[0]);
    
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("\n+ All tests completed successfully\n");
        return 0;
    } else {
        printf("\nx Child exited with error\n");
        return 1;
    }
    
error:
    /* Close pipes to signal EOF to child */
    close(pipe_to_child[1]);
    close(pipe_from_child[0]);
    
    /* Wait for child with timeout */
    printf("Waiting for child to exit after error...\n");
    int wait_count = 0;
    while (wait_count < 50) {  /* Max 5 seconds */
        int child_status;
        pid_t result = waitpid(pid, &child_status, WNOHANG);
        if (result == pid) {
            break;
        } else if (result < 0) {
            perror("waitpid");
            break;
        }
        //usleep(100000);  /* 100ms */
        wait_count++;
    }
    
    return 1;
}
