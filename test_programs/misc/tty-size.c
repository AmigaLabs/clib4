#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int
main(int argc, char **argv) {
    int rows = 40, cols = 40;
    struct winsize ws;

    if (argc == 3) {
        rows = atoi(argv[1]);
        cols = atoi(argv[2]);
    }

    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    printf("Actual Lines %d\n", ws.ws_row);
    printf("Actual Columns %d\n", ws.ws_col);
    printf("Actual XPixel %d\n", ws.ws_xpixel);
    printf("Actual YPixel %d\n", ws.ws_ypixel);

    ws.ws_row = rows;
    ws.ws_col = cols;

    ioctl(STDIN_FILENO, TIOCSWINSZ, &ws);
    printf("TIOCSWINSZ errno = %d\n", errno);

    printf("New Lines %d\n", ws.ws_row);
    printf("New Columns %d\n", ws.ws_col);
    printf("New XPixel %d\n", ws.ws_xpixel);
    printf("New YPixel %d\n", ws.ws_ypixel);
    return (0);
}
