#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <sys/param.h>
#include <stdlib.h>


// Times that a loop repeats. Increate so your test takes longer
#define MAX_CHUNK 128
#define BUFFER_SIZE 4096 * 1024
#define CHAR 'x'

extern void default_allocation8(int shift, int align);
extern void default_allocation16(int shift, int align);
extern void default_allocation32(int shift, int align);
extern void default_allocation64(int shift, int align);

// alloc.c
extern uint8_t *alloc(int shift, int align);

extern time_t measure(void (*f)(uint8_t *), uint8_t *buf);

// print.c
extern void print(bool read, int word, int shift, int align, clock_t diff);
