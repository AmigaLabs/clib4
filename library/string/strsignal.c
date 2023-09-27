/*
 * $Id: string_strsignal.c,v 1.0 2022-08-04 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <signal.h>

static const char map[] = {
        [SIGHUP]    = 1,
        [SIGINT]    = 2,
        [SIGQUIT]   = 3,
        [SIGILL]    = 4,
        [SIGTRAP]   = 5,
        [SIGABRT]   = 6,
        [SIGEMT]    = 7,
        [SIGFPE]    = 8,
        [SIGKILL]   = 9,
        [SIGBUS]    = 10,
        [SIGSEGV]   = 11,
        [SIGSYS]    = 12,
        [SIGPIPE]   = 13,
        [SIGALRM]   = 14,
        [SIGTERM]   = 15,
        [SIGURG]    = 16,
        [SIGSTOP]   = 17,
        [SIGTSTP]   = 18,
        [SIGCONT]   = 19,
        [SIGCHLD]   = 20,
        [SIGTTIN]   = 21,
        [SIGTTOU]   = 22,
        [SIGIO]     = 23,
        [SIGXCPU]   = 24,
        [SIGXFSZ]   = 25,
        [SIGVTALRM] = 26,
        [SIGPROF]   = 27,
        [SIGWINCH]  = 28,
        [SIGLOST]   = 29,
        [SIGUSR1]   = 30,
        [SIGUSR2]   = 31
};

#define sigmap(x) ((x) >= sizeof map ? (x) : map[(x)])

static const char strings[] =
        "Unknown signal\0"
        "Hangup\0"
        "Interrupt\0"
        "Quit\0"
        "Illegal instruction\0"
        "Trace/breakpoint trap\0"
        "Aborted\0"
        "Emulator trap\0"
        "Arithmetic exception\0"
        "Killed\0"
        "Bus error\0"
        "Segmentation fault\0"
        "Bad system call\0"
        "Broken pipe\0"
        "Timer signal from alarm\0"
        "Termination signal\0"
        "Urgent condition on socket\0"
        "Stop process\0"
        "Stop typed at terminal\0"
        "Continue if stopped\0"
        "Child stopped or terminated\0"
        "Stopped (tty input)\0"
        "Stopped (tty output)\0"
        "I/O now possible\0"
        "CPU time limit exceeded\0"
        "File size limit exceeded\0"
        "Virtual timer expired\0"
        "Profiling timer expired\0"
        "Window changed\0"
        "File lock lost\0"
        "User-defined signal 1\0"
        "User-defined signal 2\0"
        "";

char *
strsignal(int signum) {
    const char *s = strings;

    signum = sigmap(signum);
    if (signum - 1U >= NSIG-1) signum = 0;

    for (; signum--; s++) for (; *s; s++);

    return (char *) s;
}
