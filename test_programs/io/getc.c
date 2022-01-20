/* getc example: money counter */
#include <stdio.h>
int main ()
{
    FILE * pFile;
    int c;
    int n = 0;
    pFile=fopen ("test.txt","r");
    if (pFile==NULL) perror ("Error opening file");
    else
    {
        do {
            c = getc (pFile);
            if (c == '$') n++;
        } while (c != EOF);
        fclose (pFile);
        printf ("File contains %d$.\n",n);
    }
    return 0;
}