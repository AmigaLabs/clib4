#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    // int inpipe[2];
    // int errpipe[2];

    // pipe(inpipe);
    // pipe(errpipe);

    if (argc == 1) {
        int outpipe[2];
        pipe(outpipe);

        fcntl(outpipe[0], F_SETFL, O_NONBLOCK);

        const char *args[] = {"spawnvpe_pipe", "hej", 0};

        int pid = spawnvpe("spawnvpe_pipe", args, 0, 0, /*inpipe[0]*/-1, outpipe[1], /*errpipe[2]*/-1);

        if (pid == -1) {
            printf("Failed to create a child.\n");
            return -1;
        }

        printf("I have created a child.\n");

        uint32_t options = WNOHANG;
        uint32_t status = 0;
        int rpid = -1;
        do
            rpid = waitpid(pid, &status, options);
        while (rpid == -1 && errno == EINTR);

        if (rpid != -1)
            printf("The child returned with status : %d\n", WEXITSTATUS(status));
        else
            perror("waitpid");

        printf("The child is talking: ");
        char buffer[4096];
        int nbytes;
        do {
            // printf("[read]\n");
            nbytes = read(outpipe[0], buffer, sizeof(buffer));
            // printf("[read succeded]\n");
            write(1, buffer, nbytes);
        } while (nbytes > 0);

        printf("--- [End of child speach].\n");

        close(outpipe[0]);
        close(outpipe[1]);
    } else {
        printf("I am the child\n");
        return 1;
    }
    return 0;
}
