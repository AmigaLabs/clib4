#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int
main(void) {
    int fd[2];
    int fd1 = fcntl(STDIN_FILENO, F_GETFD);
    int fd2 = fcntl(STDOUT_FILENO, F_GETFD);
    int fd3 = fcntl(STDERR_FILENO, F_GETFD);
    printf("fd1 = %d - fd2 = %d - fd3 = %d\n", fd1, fd2, fd3);
    /*
    while ((fcntl(STDIN_FILENO, F_GETFD) == -1) ||
           (fcntl(STDOUT_FILENO, F_GETFD) == -1) ||
           (fcntl(STDERR_FILENO, F_GETFD) == -1))
        if (pipe(fd))
            return 1;
    */
    return 0;
}