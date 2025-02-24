/* t_flock.c
   Demonstrate the use of flock() to place file locks.
*/
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char *argv[]) {
    int fd, lock;
    const char *lname;

    if (argc < 3 || strcmp(argv[1], "--help") == 0 ||
        strchr("sx", argv[2][0]) == NULL) {
        printf("%s file lock [sleep-time]\n"
               "    'lock' is 's' (shared) or 'x' (exclusive)\n"
               "        optionally followed by 'n' (nonblocking)\n"
               "    'sleep-time' specifies time to hold lock\n", argv[0]);
        exit(1);
    }

    lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
    if (argv[2][1] == 'n')
        lock |= LOCK_NB;

    fd = open(argv[1], O_RDONLY);               /* Open file to be locked */
    if (fd == -1) {
        printf("Cannot open file %s\n", argv[1]);
        exit(1);
    }

    lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";

    printf("PID %ld: requesting %s\n", (long) getpid(), lname);

    if (flock(fd, lock) == -1) {
        if (errno == EWOULDBLOCK) {
            printf("PID %ld: already locked - bye!\n", (long) getpid());
        }
        else {
            printf("flock (PID=%ld) %s\n", (long) getpid(), strerror(errno));
        }
        close(fd);
        exit(1);
    }

    printf("PID %ld: granted    %s\n", (long) getpid(), lname);

    sleep(5);

    printf("PID %ld: releasing  %s\n", (long) getpid(), lname);
    if (flock(fd, LOCK_UN) == -1) {
        printf("cannot unlock file\n");
    }

    close(fd);

    exit(0);
}