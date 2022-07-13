#include <stdlib.h>
#include <iconv.h>
#include <stdio.h>
#include <errno.h>

/* This test checks that the conversion to wchar_t stops correctly when
   the input is incomplete.  Based on a bug report from
   Tristan Gingold <gingold@adacore.com>.  */

int main() {
    iconv_t cd = iconv_open("wchar_t", "UTF-8");
    char inbuf[2] = {0xc2, 0xa0};
    wchar_t outbuf[10];

    char *inptr = inbuf;
    size_t inbytesleft = 1;
    char *outptr = (char *) outbuf;
    size_t outbytesleft = sizeof(outbuf);
    size_t r = iconv(cd, (char **) &inptr, &inbytesleft, &outptr, &outbytesleft);

    if (!(r == (size_t)(-1) && errno == EINVAL))
        printf("Error on iconv() r = %d\n", r);

    return 0;
}