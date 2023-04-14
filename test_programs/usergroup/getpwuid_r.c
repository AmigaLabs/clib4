#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct passwd pd;
    struct passwd *pwdptr = &pd;
    struct passwd *tempPwdPtr;
    char *buf;
    size_t bufsize;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s userid\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)          /* Value was indeterminate */
        bufsize = 16384;        /* Should be more than enough */

    buf = malloc(bufsize);
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    if ((getpwuid_r(atoi(argv[1]), pwdptr, buf, bufsize, &tempPwdPtr)) != 0)
        perror("getpwuid_r() error.");
    else {
        printf("buf = %s\n", buf);
        printf("The user name is: %s\n", pd.pw_name);
        printf("The user id   is: %u\n", pd.pw_uid);
        printf("The group id  is: %u\n", pd.pw_gid);
        printf("The initial directory is:    %s\n", pd.pw_dir);
        printf("The initial user program is: %s\n", pd.pw_shell);
    }
}