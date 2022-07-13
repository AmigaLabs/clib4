#define _POSIX_SOURCE
#include <unistd.h>
#undef _POSIX_SOURCE
#include <stdio.h>

int main() {
    char cwd[BUFSIZ] = {0};

    if (chdir("T:") != 0)
        perror("chdir() error()");
    else {
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("getcwd() error");
        else
            printf("current working directory is: %s\n", cwd);
    }
}