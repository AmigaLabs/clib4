#include "align.h"

extern bool human;

static void print_human(bool read, int word, int shift, int align, clock_t ms) {
    char s[12];

    switch (word) {
        case 1:
            strcpy(s, " byte");
            break;
        case 2:
            strcpy(s, " half");
            break;
        case 4:
            strcpy(s, " word");
            break;
        case 8:
            strcpy(s, "dword");
            break;
        default:
            printf("Wrong size usage\n");
            exit(-22);
    }

    if (read) {
        printf("\tRead  (%s) :\t%Lf ms\n", s, (long double) ms / (1000 * 1000));
    } else {
        printf("\tWrite (%s) :\t%Lf ms\n", s, (long double) ms / (1000 * 1000));
    }
}

static void print_machine(bool read, int word, int shift, int align, clock_t ms) {
    //printf("Memory allocation alignment| Write alignment | read/write | load/store size | time\n");
    printf("%d| ", align);
    printf("%d: ", shift);

    if (read)
        printf("read ");
    else
        printf("write ");

    printf("%d %Lf\n", word, (long double) ms / (1000 * 1000));
}

void print(bool read, int word, int shift, int align, clock_t ms) {
    if (human)
        print_human(read, word, shift, align, ms);
    else
        print_machine(read, word, shift, align, ms);
}
