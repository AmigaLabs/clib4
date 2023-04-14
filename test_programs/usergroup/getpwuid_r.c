#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <errno.h>

int main() {
    struct passwd pd;
    struct passwd *pwdptr = &pd;
    struct passwd *tempPwdPtr;
    char pwdbuffer[200];
    int pwdlinelen = sizeof(pwdbuffer);

    if ((getpwuid_r(0, pwdptr, pwdbuffer, pwdlinelen, &tempPwdPtr)) != 0)
        perror("getpwuid_r() error.");
    else {
        printf("\nThe user name is: %s\n", pd.pw_name);
        printf("The user id   is: %u\n", pd.pw_uid);
        printf("The group id  is: %u\n", pd.pw_gid);
        printf("The initial directory is:    %s\n", pd.pw_dir);
        printf("The initial user program is: %s\n", pd.pw_shell);
    }
}