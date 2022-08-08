#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iconv.h>
#include <errno.h>
#include <string.h>

/* Print the hexadecimal bytes. */

void
showhex(const char *what, const char *a, int len) {
    int i;
    printf("%s: ", what);
    for (i = 0; i < len; i++) {
        printf("%02X", (unsigned char) a[i]);
        if (i < len - 1) {
            printf(" ");
        }
    }
    printf("\n");
}

/* Display values, for the purpose of showing what this is doing. */

void
show_values(const char *before_after,
            const char *euc_start, int len_start,
            const char *utf8_start, int utf8len_start) {
    printf("%s:\n", before_after);
    showhex("EUC-JP string", euc_start, len_start);
    showhex("UTF-8 string", utf8_start, utf8len_start);
}

/* The names of the input and output encodings. */

const char *EUCSET = "EUC-JP";
const char *OUTSET = "UTF-8";

/* Initialize the library. */

iconv_t
initialize(void) {
    iconv_t conv_desc;
    conv_desc = iconv_open(OUTSET, EUCSET);
    if ((int) conv_desc == -1) {
        /* Initialization failure. */
        if (errno == EINVAL) {
            fprintf(stderr,
                    "Conversion from '%s' to '%s' is not supported.\n",
                    EUCSET, OUTSET);
        } else {
            fprintf(stderr, "Initialization failure: %s\n",
                    strerror(errno));
        }
        // exit ok
        exit(1);
    }
    return conv_desc;
}


/* Convert EUC into UTF-8 using the iconv library. */

char *
euc2utf8(iconv_t conv_desc, const char *euc) {
    size_t iconv_value;
    char *utf8;
    unsigned int len;
    unsigned int utf8len;
    /* The variables with "start" in their name are solely for display
       of what the function is doing. As iconv runs, it alters the
       values of the variables, so these are for keeping track of the
       start points and start lengths. */
    char *utf8start;
    const char *euc_start;
    int len_start;
    int utf8len_start;

    len = strlen(euc);
    if (!len) {
        fprintf(stderr, "Input string is empty.\n");
        return 0;
    }
    /* Assign enough space to put the UTF-8. */
    utf8len = 2 * len;
    utf8 = calloc(utf8len, sizeof(char));
    if (!utf8) {
        fprintf(stderr, "Calloc failed.\n");
        return 0;
    }
    /* Keep track of the variables. */
    len_start = len;
    utf8len_start = utf8len;
    utf8start = utf8;
    euc_start = euc;
    /* Display what is in the variables before calling iconv. */
    show_values("before",
                euc_start, len_start,
                utf8start, utf8len_start);
    iconv_value = iconv(conv_desc, &euc, &len, &utf8, &utf8len);
    /* Handle failures. */
    if (iconv_value == (size_t) - 1) {
        fprintf(stderr, "iconv failed: in string '%s', length %d, "
                        "out string '%s', length %d\n",
                euc, len, utf8start, utf8len);
        switch (errno) {
            /* See "man 3 iconv" for an explanation. */
            case EILSEQ:
                fprintf(stderr, "Invalid multibyte sequence.\n");
                break;
            case EINVAL:
                fprintf(stderr, "Incomplete multibyte sequence.\n");
                break;
            case E2BIG:
                fprintf(stderr, "No more room.\n");
                break;
            default:
                fprintf(stderr, "Error: %s.\n", strerror(errno));
        }
        // exit ok
        exit(1);
    }
    /* Display what is in the variables after calling iconv. */
    show_values("after",
                euc_start, len_start,
                utf8start, utf8len_start);
    return utf8start;
}

/* Close the connection with the library. */

void
finalize(iconv_t conv_desc) {
    int v;
    v = iconv_close(conv_desc);
    if (v != 0) {
        fprintf(stderr, "iconv_close failed: %s\n", strerror(errno));
        // exit ok
        exit(1);
    }
}

int main() {
    char *in_string = "\xB6\xE2ʸ\xC2\xCE";
    char *out_string;
    /* Conversion descriptor. */
    iconv_t conv_desc;

    conv_desc = initialize();
    out_string = euc2utf8(conv_desc, in_string);
    finalize(conv_desc);

    if (out_string) {
        printf("Final iconv output: %s\n", out_string);
        free(out_string);
    }
    return 0;
}