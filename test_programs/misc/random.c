#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN (32)

void print_hex(char *b) {
    int i;
    for (i = 0; i < LEN; i++) printf("%02x ", ((unsigned char *) b)[i]);
    printf("\n");
}

int main(int argc,char ** argv) {
    char state1[256], state2[256], tmp[256];
    initstate(42, state2, LEN);
    initstate(62, state1, LEN);
    printf("buffer before random():\n");
    print_hex(state1);
    printf("%10ld\n", random());
    printf("buffer after random():\n");
    print_hex(state1);

    setstate(state2); // Now we are free to copy data from state1
    printf("buffer after setstate():\n");
    print_hex(state1);
    memcpy(tmp, state1, 256);
    printf("copied to tmp\n");

    setstate(state1); // Go on with original sequence
    printf("next random():\n");
    printf("%10ld\n", random());
    printf("next random():\n");
    printf("%10ld\n", random());

    setstate(state2); // Again, this allows us to play with data in state1
    memcpy(state1, tmp, 256);
    setstate(state1);
    printf("back copy:\n");
    printf("random() after copy:\n");
    printf("%10ld\n", random());
    printf("next random():\n");
    printf("%10ld\n", random());

    return 0;
}