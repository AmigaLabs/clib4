#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termcap.h>
#include <unistd.h>


#define REGION_ROWS 3


static void print_text(const char *s) {
    if (s != NULL) {
        int i;

        printf("\"");
        for (i = 0; s[i]; ++i) {
            if (s[i] == '\033') {
                printf("\\E");
            } else if (s[i] == '\\') {
                printf("\\\\");
            } else if (s[i] >= '\x21' && s[i] <= '\x7E') {
                printf("%c", s[i]);
            } else {
                printf("\\x%02x", (unsigned char) s[i]);
            }
        }
        printf("\"");
    } else {
        printf("(null)");
    }
}


static int get_capability(const char *label, const char *name, const char **pptr, char **p_buf_ptr) {
    const char *ptr;

    ptr = tgetstr(name, p_buf_ptr);

    printf("%-22s (%s) = ", label, name);
    print_text(ptr);
    printf("\n");

    *pptr = ptr;
    return ptr != NULL;
}


int main() {
    char tent_buf[1024];
    char tget_buf[1024];
    char *tget_ptr = &tget_buf[0];

    /* capabilities. */
    const char *cm;
    const char *sf;
    const char *cs;

    int lines;
    int columns;
    int ret;

    ret = tgetent(tent_buf, getenv("TERM"));
    if (ret != 1) {
        printf("tgetent failed (%d).\n", ret);
        return 1;
    }

    lines = tgetnum("li");
    columns = tgetnum("co");
    printf("lines   (li) = %d\n", lines);
    printf("columns (co) = %d\n", columns);

    if (!get_capability("cursor move", "cm", &cm, &tget_ptr)) {
        return 1;
    }
    if (!get_capability("scroll forward", "sf", &sf, &tget_ptr)) {
        return 1;
    }
    if (!get_capability("change scroll region", "cs", &cs, &tget_ptr)) {
        return 1;
    }

    {
        const char *x;
        get_capability("cursor position format", "u6", &x, &tget_ptr);
        get_capability("query cursor position", "u7", &x, &tget_ptr);
    }

    /* sample. */
    {
        const char *o = tgoto(cs, 5, 3);

        printf("change scroll region (5,3) = ");
        print_text(o);
        printf("\n");
    }

    if (lines < REGION_ROWS + 2) {
        printf("this program requires at least %d lines, but got %d lines.\n", REGION_ROWS + 2, lines);
        return 1;
    }

    {
        int i;
        int top = lines - (REGION_ROWS + 2);
        int first = top + 1;
        int last = first + REGION_ROWS - 1;

        printf("\n");
        printf("change scroll region to (%d,%d)\n", first, last);
        printf("NOTE: use reset(1) if console seems broken.\n");

        /* move cursor to bottom-left corner and draw frame. */
        tputs(tgoto(cm, 0, lines - 1), 1, putchar);
        printf("line.%d ----\n", top + 0);
        for (i = 1; i <= REGION_ROWS; ++i) {
            printf("line.%d\n", top + i);
        }
        printf("line.%d ----", top + (REGION_ROWS + 2) - 1);
        fflush(stdout);

        /* change scroll region (last,first) and move cursor inside. */
        tputs(tgoto(cs, last, first), 1, putchar);
        tputs(tgoto(cm, 0, first), 1, putchar);

        sleep(1);
        for (i = 1; i <= 5; ++i) {
            if (i != 1) {
                printf("\n");
            }
            printf("test %d/5.", i);
            fflush(stdout);
            sleep(1);
        }

        /* restore scroll region and cursor position. */
        tputs(tgoto(cs, lines - 1, 0), 1, putchar);
        tputs(tgoto(cm, 0, lines - 1), 1, putchar);

        /* and scroll down. */
        tputs(sf, 1, putchar);
        sleep(1);
    }
    return 0;
}