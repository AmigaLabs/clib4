#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int
main(void) {
    struct winsize ws;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);

    printf("lines %d\n", ws.ws_row);
    printf("columns %d\n", ws.ws_col);

#if 0
    ws.ws_row = 150;
    ws.ws_col = 40;

    ioctl(STDIN_FILENO, TIOCSWINSZ, &ws);
    printf("errno TIOCSWINSZ = %d\n", errno);

    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    printf("errno TIOCGWINSZ = %d\n", errno);

    printf("lines %d\n", ws.ws_row);
    printf("columns %d\n", ws.ws_col);
#endif

    return (0);
}