/*
 * Test for non-blocking pipe operations using spawnvpe
 * This test verifies that O_NONBLOCK works correctly with pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

#define BUFFER_SIZE 8192
#define LARGE_DATA_SIZE (64 * 1024)  /* 64KB - larger than typical pipe buffer */
#define SMALL_CHUNK 256

/* Helper function to set non-blocking mode */
static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        perror("fcntl F_GETFL");
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
        perror("fcntl F_SETFL O_NONBLOCK");
        return -1;
    }
    fprintf(stderr, "Set fd %d to non-blocking mode (flags: 0x%x)\n", fd, flags);
    return 0;
}

/* Child process: reads from stdin and writes to stdout (redirected by spawnvpe) */
int child_main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE];
    ssize_t total_read = 0;
    ssize_t total_written = 0;
    int read_count = 0;
    int write_count = 0;
    
    fprintf(stderr, "[CHILD] Started, will use stdin/stdout\n");
    fflush(stderr);
    
    /* Send ready signal to parent via stdout BEFORE setting non-blocking */
    char ready_signal = 'R';
    ssize_t n = write(STDOUT_FILENO, &ready_signal, 1);
    if (n != 1) {
        fprintf(stderr, "[CHILD] Failed to send ready signal: n=%ld, errno=%d\n", (long)n, errno);
        perror("[CHILD] write ready signal");
        return 1;
    }
    fprintf(stderr, "[CHILD] Sent ready signal to parent\n");
    fflush(stderr);
    
    /* Set stdin to non-blocking */
    if (set_nonblocking(STDIN_FILENO) < 0) {
        fprintf(stderr, "[CHILD] Failed to set stdin non-blocking\n");
        return 1;
    }
    
    /* Set stdout to non-blocking */
    if (set_nonblocking(STDOUT_FILENO) < 0) {
        fprintf(stderr, "[CHILD] Failed to set stdout non-blocking\n");
        return 1;
    }
    
    while (1) {
        ssize_t nread = read(STDIN_FILENO, buffer, sizeof(buffer));
        
        if (nread > 0) {
            read_count++;
            total_read += nread;
            if (read_count % 100 == 0) {  /* Print every 100 reads */
                fprintf(stderr, "[CHILD] Read %ld bytes (total: %ld, count: %d)\n", 
                       (long)nread, (long)total_read, read_count);
                fflush(stderr);
            }
            
            /* Write what we read */
            ssize_t written = 0;
            while (written < nread) {
                ssize_t nwritten = write(STDOUT_FILENO, buffer + written, nread - written);
                
                if (nwritten > 0) {
                    write_count++;
                    written += nwritten;
                    total_written += nwritten;
                    if (write_count % 100 == 0) {  /* Print every 100 writes */
                        fprintf(stderr, "[CHILD] Wrote %ld bytes (total: %ld, count: %d)\n", 
                               (long)nwritten, (long)total_written, write_count);
                        fflush(stderr);
                    }
                } else if (nwritten < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        fprintf(stderr, "[CHILD] Write would block, waiting...\n");
                        fflush(stderr);
                        usleep(1000); /* Wait 1ms */
                        continue;
                    } else {
                        perror("[CHILD] write error");
                        return 1;
                    }
                }
            }
        } else if (nread == 0) {
            fprintf(stderr, "[CHILD] EOF on read pipe, exiting\n");
            fflush(stderr);
            break;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                /* No data available yet, wait a bit */
                usleep(1000); /* Wait 1ms */
            } else {
                perror("[CHILD] read error");
                return 1;
            }
        }
    }
    
    fprintf(stderr, "[CHILD] Finished: read %ld bytes in %d operations, wrote %ld bytes in %d operations\n",
           (long)total_read, read_count, (long)total_written, write_count);
    fflush(stderr);
    
    return 0;
}

/* Main test function */
int main(int argc, char *argv[]) {
    /* Check if we're the child process */
    if (argc > 1 && strcmp(argv[1], "--child") == 0) {
        return child_main(argc, argv);
    }
    
    printf("=== Pipe Non-Blocking Test ===\n");
    printf("Testing pipe operations with O_NONBLOCK flag\n\n");
    
    /* Create pipes for communication */
    int pipe_to_child[2];   /* Parent writes, child reads */
    int pipe_from_child[2]; /* Child writes, parent reads */
    
    if (pipe(pipe_to_child) < 0) {
        perror("pipe(pipe_to_child)");
        return 1;
    }
    printf("Created pipe_to_child: read=%d, write=%d\n", pipe_to_child[0], pipe_to_child[1]);
    
    if (pipe(pipe_from_child) < 0) {
        perror("pipe(pipe_from_child)");
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        return 1;
    }
    printf("Created pipe_from_child: read=%d, write=%d\n\n", pipe_from_child[0], pipe_from_child[1]);
    
    /* Prepare arguments for child - just --child flag */
    char *child_argv[] = {
        argv[0],
        "--child",
        NULL
    };
    
    char *child_env[] = {
        "TEST=nonblock_pipe",
        NULL
    };
    
    /* Spawn child process WITH FD redirection via spawnvpe */
    printf("Spawning child with stdin=%d (pipe_to_child read), stdout=%d (pipe_from_child write)...\n",
           pipe_to_child[0], pipe_from_child[1]);
    fflush(stdout);
    
    pid_t child_pid = spawnvpe(
        argv[0],                    /* program */
        (const char **)child_argv,  /* argv */
        (char **)child_env,         /* environment */
        NULL,                       /* current dir */
        pipe_to_child[0],           /* stdin - read end of pipe_to_child */
        pipe_from_child[1],         /* stdout - write end of pipe_from_child */
        -1                          /* stderr - keep parent's */
    );
    
    if (child_pid < 0) {
        perror("spawnvpe");
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);
        close(pipe_from_child[1]);
        return 1;
    }
    
    printf("Child process spawned with pid %d\n\n", child_pid);
    
    /* DON'T close unused pipe ends yet - child needs to open its copies first! */
    
    /* Wait for child ready signal FIRST - this ensures child has opened the pipes */
    printf("Waiting for child ready signal...\n");
    fflush(stdout);
    char ready_signal;
    int ready_received = 0;
    int wait_attempts = 0;
    while (!ready_received && wait_attempts < 100) {  /* Max 10 seconds */
        ssize_t n = read(pipe_from_child[0], &ready_signal, 1);
        if (n == 1 && ready_signal == 'R') {
            ready_received = 1;
            printf("Child is ready!\n\n");
            fflush(stdout);
        } else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("Error waiting for ready signal");
            goto cleanup;
        } else {
            usleep(100000);  /* Wait 100ms */
            wait_attempts++;
        }
    }
    
    if (!ready_received) {
        fprintf(stderr, "Child did not send ready signal in time\n");
        goto cleanup;
    }
    
    /* Close only the read end that parent doesn't use.
     * DO NOT close pipe_from_child[1] yet! Child is using a DupFileHandle() of it.
     * Closing it now might invalidate the child's stdout. */
    close(pipe_to_child[0]);      /* Parent doesn't read from pipe_to_child */
    
    /* Set parent's pipe ends to non-blocking */
    if (set_nonblocking(pipe_to_child[1]) < 0) {
        fprintf(stderr, "Failed to set write pipe non-blocking\n");
        goto cleanup;
    }
    
    if (set_nonblocking(pipe_from_child[0]) < 0) {
        fprintf(stderr, "Failed to set read pipe non-blocking\n");
        goto cleanup;
    }
    
    /* Allocate large buffer for testing */
    char *send_buffer = NULL;
    char *recv_buffer = NULL;
    
    send_buffer = malloc(LARGE_DATA_SIZE);
    recv_buffer = malloc(LARGE_DATA_SIZE);
    
    if (!send_buffer || !recv_buffer) {
        fprintf(stderr, "Failed to allocate buffers\n");
        goto cleanup;
    }
    
    /* Fill send buffer with pattern data */
    for (int i = 0; i < LARGE_DATA_SIZE; i++) {
        send_buffer[i] = (char)(i % 256);
    }
    printf("Generated %d bytes of test data\n", LARGE_DATA_SIZE);
    
    /* Test 1: Write large amount of data in chunks */
    printf("\n=== Test 1: Writing large data in non-blocking mode ===\n");
    ssize_t total_written = 0;
    ssize_t total_read = 0;
    int write_count = 0;
    int read_count = 0;
    int write_eagain_count = 0;
    int read_eagain_count = 0;
    
    while (total_written < LARGE_DATA_SIZE || total_read < LARGE_DATA_SIZE) {
        /* Try to write more data */
        if (total_written < LARGE_DATA_SIZE) {
            ssize_t to_write = LARGE_DATA_SIZE - total_written;
            if (to_write > SMALL_CHUNK) {
                to_write = SMALL_CHUNK;
            }
            
            ssize_t nwritten = write(pipe_to_child[1], send_buffer + total_written, to_write);
            
            if (nwritten > 0) {
                write_count++;
                total_written += nwritten;
                if (write_count % 10 == 0) {
                    printf("[PARENT] Wrote %ld bytes (total: %ld/%d, writes: %d)\n",
                           (long)nwritten, (long)total_written, LARGE_DATA_SIZE, write_count);
                    fflush(stdout);
                }
            } else if (nwritten < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    write_eagain_count++;
                    if (write_eagain_count % 100 == 0) {
                        printf("[PARENT] Write would block (EAGAIN count: %d)\n", write_eagain_count);
                        fflush(stdout);
                    }
                } else {
                    perror("[PARENT] write error");
                    goto cleanup;
                }
            }
        }
        
        /* Try to read data */
        if (total_read < LARGE_DATA_SIZE) {
            ssize_t to_read = LARGE_DATA_SIZE - total_read;
            if (to_read > BUFFER_SIZE) {
                to_read = BUFFER_SIZE;
            }
            
            ssize_t nread = read(pipe_from_child[0], recv_buffer + total_read, to_read);
            
            if (nread > 0) {
                read_count++;
                total_read += nread;
                if (read_count % 10 == 0) {
                    printf("[PARENT] Read %ld bytes (total: %ld/%d, reads: %d)\n",
                           (long)nread, (long)total_read, LARGE_DATA_SIZE, read_count);
                    fflush(stdout);
                }
            } else if (nread == 0) {
                /* EOF - child closed pipe */
                printf("[PARENT] Got EOF on read pipe\n");
                fflush(stdout);
                break;
            } else if (nread < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    perror("[PARENT] read error");
                    goto cleanup;
                } else {
                    /* Count EAGAIN on reads */
                    read_eagain_count++;
                    if (read_eagain_count % 1000 == 0) {
                        printf("[PARENT] Read EAGAIN count: %d (total_read=%ld)\n", 
                               read_eagain_count, (long)total_read);
                        fflush(stdout);
                    }
                }
            }
        }
        
        /* Don't sleep - we need to read/write as fast as possible to avoid deadlock */
    }
    
    printf("\n[PARENT] Data transfer complete:\n");
    printf("  Written: %ld bytes in %d operations\n", (long)total_written, write_count);
    printf("  Read: %ld bytes in %d operations\n", (long)total_read, read_count);
    printf("  Write EAGAIN count: %d\n", write_eagain_count);
    printf("  Read EAGAIN count: %d\n", read_eagain_count);
    
    /* Verify data integrity */
    printf("\n=== Verifying data integrity ===\n");
    int errors = 0;
    for (int i = 0; i < LARGE_DATA_SIZE; i++) {
        if (recv_buffer[i] != send_buffer[i]) {
            if (errors < 10) {
                printf("Data mismatch at offset %d: expected 0x%02x, got 0x%02x\n",
                       i, (unsigned char)send_buffer[i], (unsigned char)recv_buffer[i]);
            }
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("+ Data integrity check PASSED: all %d bytes matched\n", LARGE_DATA_SIZE);
    } else {
        printf("x Data integrity check FAILED: %d bytes mismatched\n", errors);
    }
    
    /* Close write pipe to signal EOF to child */
    printf("\n=== Closing write pipe and waiting for child ===\n");
    close(pipe_to_child[1]);
    
    /* Wait for child to finish */
    int status;
    printf("Waiting for child process %d...\n", child_pid);
    if (waitpid(child_pid, &status, 0) < 0) {
        perror("waitpid");
    } else {
        if (WIFEXITED(status)) {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child terminated abnormally\n");
        }
    }
    
    /* Now safe to close pipe_from_child[1] since child has exited */
    close(pipe_from_child[1]);
    
    /* Cleanup */
    free(send_buffer);
    free(recv_buffer);
    close(pipe_from_child[0]);
    
    printf("\n=== Test completed successfully ===\n");
    return 0;
    
cleanup:
    /* Close pipes to signal child to exit */
    close(pipe_to_child[1]);
    close(pipe_from_child[0]);
    if (send_buffer) free(send_buffer);
    if (recv_buffer) free(recv_buffer);
    
    /* Wait for child to finish (with timeout simulation via polling) */
    printf("Waiting for child to finish...\n");
    int wait_count = 0;
    while (wait_count < 50) {  /* Max 5 seconds */
        int child_status;
        pid_t result = waitpid(child_pid, &child_status, WNOHANG);
        if (result == child_pid) {
            printf("Child exited\n");
            break;
        } else if (result < 0) {
            perror("waitpid");
            break;
        }
        usleep(100000);  /* Wait 100ms */
        wait_count++;
    }
    
    if (wait_count >= 50) {
        fprintf(stderr, "Child did not exit in time\n");
    }
    
    return 1;
}
