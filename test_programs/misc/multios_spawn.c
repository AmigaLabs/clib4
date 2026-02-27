#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <process.h> /* Required for _spawnv */
#include <windows.h>
/* We make getpid() work in a similar
    way on Windows as it does on Linux */
#define getpid() GetCurrentProcessId()
#endif
#if defined(__linux__) || defined(__amigaos4__)
#include <unistd.h>
#include <limits.h>
#endif

void spawn_new_process(const char **argv);

int pid;

int main(int argc, char *argv[]) {
    pid = getpid();
    if (argc > 1 && strcmp(argv[1], "the_new_process") == 0) {
        printf("[%d] This is a new process, and not a fork.\n", pid);
    } else {
        printf("[%d] This is the original process.\n", pid);
        char *new_args[3] = {0};
        new_args[0] = argv[0];
        new_args[1] = "the_new_process";
        spawn_new_process((const char **) new_args);
    }
    return (0);
}

void spawn_new_process(const char **argv) {
#ifdef _WIN32
    /* This code block will also be reached on a
       64 bit version of a Windows desktop OS */
    _spawnv(_P_WAIT, argv[0], (const char * const *)argv);
#elif defined __amigaos4__
    spawnv(P_WAIT, argv[0], (const char **)argv);
#else // __linux__
    pid = getpid();

    /* Create copy of current process */
    pid = fork();

    /* The parent`s new pid will be 0 */
    if (pid != 0) {
        /* We are now in a child progress
           Execute different process */
        printf("[%d] Child (fork) process will call execv [%s].\n", pid, argv[0]);
        execv(argv[0], (char * const*) argv);

        /* This code will never be executed */
        printf("[%d] Child (fork) process is exiting.\n", pid);
        exit(EXIT_SUCCESS);
    }
#endif

    /* We are still in the original process */
    printf("[%d] Original process is exiting.\n", pid);
    exit(EXIT_SUCCESS);
}
