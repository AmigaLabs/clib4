#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static const char *__attribute__((used)) stackcookie = "$STACK: 3000000";

//////////////////////////////////////////////////////////////////////////////
// glibc functions

// From http://ctan.mackichan.com/macros/texinfo/texinfo/gnulib/lib/memchr.c .
// Current version at http://repo.or.cz/w/glibc.git/blob/HEAD:/string/memchr.c ,
// x86 asm at http://repo.or.cz/w/glibc.git/blob/HEAD:/sysdeps/x86_64/memchr.S

void *
glibc_memchr(void const *s, int c_in, size_t n) {
    typedef unsigned long int longword;
    const unsigned char *char_ptr;
    const longword *longword_ptr;
    longword repeated_one;
    longword repeated_c;
    unsigned register char c;
    c = (unsigned char) c_in;
    for (char_ptr = (const unsigned char *) s;
         n > 0 && (size_t) char_ptr % sizeof(longword) != 0;
         --n, ++char_ptr)
        if (*char_ptr == c)
            return (void *) char_ptr;
    longword_ptr = (const longword *) char_ptr;
    repeated_one = 0x01010101;
    repeated_c = c | (c << 8);
    repeated_c |= repeated_c << 16;
    if (0xffffffffU < (longword) -1) {
        repeated_one |= repeated_one << 31 << 1;
        repeated_c |= repeated_c << 31 << 1;
        if (8 < sizeof(longword)) {
            size_t i;
            for (i = 64; i < sizeof(longword) * 8; i *= 2) {
                repeated_one |= repeated_one << i;
                repeated_c |= repeated_c << i;
            }
        }
    }
    while (n >= sizeof(longword)) {
        longword longword1 = *longword_ptr ^ repeated_c;
        if ((((longword1 - repeated_one) & ~longword1)
             & (repeated_one << 7)) != 0)
            break;
        longword_ptr++;
        n -= sizeof(longword);
    }
    char_ptr = (const unsigned char *) longword_ptr;
    for (; n > 0; --n, ++char_ptr) {
        if (*char_ptr == c)
            return (void *) char_ptr;
    }
    return NULL;
}

// http://repo.or.cz/w/glibc.git/blob/HEAD:/string/strchr.c
char *
glibc_strchr(const char *s, int c_in) {
    const unsigned char *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int longword, magic_bits, charmask;
    unsigned register char c;
    c = (unsigned char) c_in;
    for (char_ptr = (const unsigned char *) s;
         ((unsigned long int) char_ptr & (sizeof(longword) - 1)) != 0;
         ++char_ptr)
        if (*char_ptr == c)
            return (char *) char_ptr;
        else if (*char_ptr == '\0')
            return NULL;
    longword_ptr = (unsigned long int *) char_ptr;
    switch (sizeof(longword)) {
        case 4:
            magic_bits = 0x7efefeffL;
            break;
        case 8:
            magic_bits = ((0x7efefefeL << 16) << 16) | 0xfefefeffL;
            break;
        default:
            abort();
    }
    charmask = c | (c << 8);
    charmask |= charmask << 16;
    if (sizeof(longword) > 4)
        charmask |= (charmask << 16) << 16;
    if (sizeof(longword) > 8)
        abort();
    for (;;) {
        longword = *longword_ptr++;
        if ((((longword + magic_bits)
              ^ ~longword)
             & ~magic_bits) != 0 ||
            ((((longword ^ charmask) + magic_bits) ^ ~(longword ^ charmask))
             & ~magic_bits) != 0) {
            const unsigned char *cp = (const unsigned char *) (longword_ptr - 1);
            if (*cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (*++cp == c)
                return (char *) cp;
            else if (*cp == '\0')
                return NULL;
            if (sizeof(longword) > 4) {
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
                if (*++cp == c)
                    return (char *) cp;
                else if (*cp == '\0')
                    return NULL;
            }
        }
    }
    return NULL;
}

// Done with glibc
//////////////////////////////////////////////////////////////////////////////


void *stupid_memchr(const void *s_in, int c, size_t n) {
    const unsigned char *s = (const unsigned char *) s_in;
    for (int i = 0; i < n; ++i)
        if (s[i] == c)
            return (void *) (s + i);
    return NULL;
}

char *stupid_strchr(char *s_in, int c) {
    while (*s_in && *s_in != c)
        s_in++;
    return *s_in == c ? s_in : NULL;
}

void fill_string(char *buf, int n, int xpos) {
    memset(buf, '.', n);
    buf[xpos] = 'x';
    buf[n] = '\0';  // for strstr
}

int64_t now_millis() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return ((int64_t) now.tv_sec * 1000) + (now.tv_usec / 1000);
}

int main() {
    const int STRLEN = 1000;

    char strs[STRLEN][STRLEN + 1];
    for (int i = 0; i < STRLEN; ++i)
        fill_string(strs[i], STRLEN, i);

    const int ITERS = 1000;
    int64_t start, stop;
    int delta;
    float time;

#define BENCH(fun, ...) \
      start = now_millis(); \
      for (int j = 0; j < ITERS; ++j) { \
        for (int i = 0; i < STRLEN; ++i) { \
          void* r = fun(strs[i], __VA_ARGS__); \
          if (r != strs[i] + i) { \
            printf("Bug!\n"); \
            exit(1); \
          } \
        } \
      } \
      stop = now_millis(); \
      delta = (int)(stop - start); \
      time = delta*1000 / (float)ITERS; \
      printf("%s: %.1fus\n", #fun, time) \

    BENCH(stupid_memchr, 'x', STRLEN);
    BENCH(glibc_memchr, 'x', STRLEN);
    BENCH(memchr, 'x', STRLEN);

    printf("\n");

    BENCH(stupid_strchr, 'x');
    BENCH(glibc_strchr, 'x');
    BENCH(strchr, 'x');
}