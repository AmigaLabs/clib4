#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int main(void)
{
    FILE    *stream;
    wchar_t  wcs[100];

    if (NULL == (stream = fopen("README", "r"))) {
        printf("Unable to open: \"README\"\n");
        exit(1);
    }

    errno = 0;
    if (NULL == fgetws(wcs, 100, stream)) {
        if (EILSEQ == errno) {
            printf("An invalid wide character was encountered.\n");
            exit(1);
        }
        else if (feof(stream))
            printf("End of file reached.\n");
        else
            perror("Read error.\n");
    }
    wprintf(L"wcs = \"%ls\"\n", wcs);
    fclose(stream);
    return 0;

    /************************************************************
       Assuming the file fgetws.dat contains:

       This test string should not return -1

       The output should be similar to:

       wcs = "This test string should not return -1"
    ************************************************************/
}