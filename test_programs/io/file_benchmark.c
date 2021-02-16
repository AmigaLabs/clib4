/*
  Description: benchmark of fputc vs fwrite for writing an array
               of unsigned char values, one at a time.
  Author: Rob Stewart <robstewart57@gmail.com>
  Date: 20.07.2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUMCHARS 10000000
unsigned char chars[NUMCHARS];

void randChars()
{
    char randChar = ' ';
    int counter = 0;
    int randNum = 0;

    srand(time(NULL));
    for (counter = 0; counter < NUMCHARS; counter++)
    {
        randNum = 26 * (rand() / (RAND_MAX + 1.0));
        randNum = randNum + 97;
        randChar = (char)randNum;
        chars[counter] = randChar;
    }
}

int main(int argc, char **argv)
{
    clock_t begin, end;
    double time_spent;
    FILE *fwrite_out_all;
    FILE *fwrite_out;
    FILE *fwrite_out_fseek;
    FILE *fputc_out;
    fwrite_out_all = fopen("fwrite-out-all.txt", "w+");
    fwrite_out = fopen("fwrite-out.txt", "w+");
    fwrite_out_fseek = fopen("fwrite-out-fseek.txt", "w+");
    fputc_out = fopen("fputc-out.txt", "w+");
    randChars();

    int i;

    /* measure fwrite time, writing array at once */
    begin = clock();
    fwrite(chars, sizeof(chars[0]), sizeof(chars) / sizeof(chars[0]), fwrite_out_all);
    fclose(fwrite_out_all);
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("fwrite time write all at once:\t\t %f\n", time_spent);

    /* measure fwrite time */
    static int cnt = 0;
    begin = clock();
    for (i = 0; i < NUMCHARS; i++)
    {
        fwrite(&chars[i], sizeof(unsigned char), 1, fwrite_out);
        cnt++;
    }
    fclose(fwrite_out);
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("fwrite time one byte at a time:\t\t %f\n", time_spent);

    /* measure fwrite time, also using fseek */
    cnt = 0;
    begin = clock();
    for (i = 0; i < NUMCHARS; i++)
    {
        fseek(fwrite_out_fseek, sizeof(unsigned char) * cnt, SEEK_SET);
        fwrite(&chars[i], sizeof(unsigned char), 1, fwrite_out_fseek);
        cnt++;
    }
    fclose(fwrite_out_fseek);
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("fwrite+fseek time one byte at a time:\t %f\n", time_spent);

    /* measure fputc time */
    begin = clock();
    for (i = 0; i < NUMCHARS; i++)
    {
        fputc(chars[i], fputc_out);
    }
    fclose(fputc_out);
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("fputc time one byte at a time:\t\t %f\n", time_spent);

    return 0;
}