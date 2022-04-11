#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

int
main(void) {
    struct winsize ws;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

    printf("lines %d\n", ws.ws_row);
    printf("columns %d\n", ws.ws_col);

    return (0);
}