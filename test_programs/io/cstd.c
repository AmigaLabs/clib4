#include <stdio.h>

int main() {
    FILE *f = fopen("test.c", "r");
    int ch;

    if (!f) {
        perror("fopen");
        return 1;
    }

    while ((ch = fgetc(f)) != EOF) {
        putchar(ch);
    }

    fclose(f);
    return 0;
}
