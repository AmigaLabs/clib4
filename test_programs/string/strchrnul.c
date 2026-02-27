#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

int main(void) {
    char string[256];
    char c;

    printf("Input a string!\n");
    gets(string);
    printf("Input a character to look up for!\n");
    scanf("%c", &c);

    char *rp = strchrnul(string, c);

    printf("The 1th character '%c' that found, the next character is '%c'\n", *rp, *(rp + 1));

    return 0;
}