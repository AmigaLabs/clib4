#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <iconv.h>

int main(int argc, char **argv) {
    char *in_buf = "안녕 Hi";

    size_t in_size = strlen(in_buf);

    size_t out_size = sizeof(wchar_t) * in_size * 4;
    char *out_buf = malloc(out_size);

    memset(out_buf, 0x00, out_size);

    // iconv --list
    //iconv_t ic = iconv_open("UTF-8" /*tocode*/, "EUC-KR" /*fromcode*/ );
    iconv_t ic = iconv_open("EUC-KR" /*tocode*/, "UTF-8" /*fromcode*/ );
    if (ic == (iconv_t) - 1) {
        fprintf(stderr, "not supported code \n");
        exit(1);
    }

    wprintf(L"input len = %ld, %s\n", in_size, in_buf);

    // 주의 iconv 의 inbuf, outbuf 는 이중 포인터이므로 별도의 pointer 를 선언하고 주소를 넘겨야 함.
    char *in_ptr = in_buf;
    char *out_ptr = out_buf;

    size_t out_buf_left = out_size;
    size_t result = iconv(ic, &in_ptr, &in_size, &out_ptr, &out_buf_left);
    printf("input len = %ld, output len=%ld %s\n", in_size, out_size - out_buf_left, out_buf);

    iconv_close(ic);

    if (result == -1) {
        perror("iconv failed : ");

        return 1;
    }

    FILE *fp = fopen("out.txt", "w");
    fwrite(out_buf, out_size - out_buf_left, 1, fp);
    fclose(fp);

    return 0;
}