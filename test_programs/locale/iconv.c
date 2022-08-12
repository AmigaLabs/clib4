#include <stdlib.h>
#include <errno.h>
#include <iconv.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // create transcoder from iso-8859-1 to utf8
    iconv_t foo = iconv_open("UTF-8", "ISO-8859-1");
    if ((int) foo == -1) {
        if (errno == EINVAL) {
            fprintf(stderr, "Conversion is not supported");
        } else {
            fprintf(stderr, "Initialization failure:\n");
        }
    }
    // calloc fills memory with 0 bytes. we alloc two -
    // one for the 'ö' and one for the ending delimeter
    char *iso = calloc(2, sizeof(char));
    iso[0] = 0xF6; // iso-8859-1 'ö'

    // the converted string can be four times larger
    // then the original, as the largest known char width is 4 bytes.
    char *converted = calloc(5, sizeof(char));

    // we need to store an additional pointer that targets the
    // start of converted. (iconv modifies the original 'converted')
    char *converted_start = converted;

    size_t ibl = 2; // len of iso
    size_t obl = 5; // len of converted

    // do it!
    int ret = iconv(foo, &iso, &ibl, &converted, &obl);

    // if iconv fails it returns -1
    if (ret == (iconv_t) - 1) {
        perror("iconv");
        iconv_close(foo);
        return 1;
    } else {
        // other wise the number of converted bytes
        printf("%i bytes converted\n", ret);
        printf("result: '%s'\n", converted_start);
        iconv_close(foo);
        return 0;
    }
}