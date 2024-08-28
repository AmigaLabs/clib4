/*
  $Id: uuid.c,v 1.01 2024-08-28 12:09:49 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include "uuid.h"

static uint64_t xorshift128plus(uint64_t *s) {
    /* http://xorshift.di.unimi.it/xorshift128plus.c */
    uint64_t s1 = s[0];
    const uint64_t s0 = s[1];
    s[0] = s0;
    s1 ^= s1 << 23;
    s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    return s[1] + s0;
}

extern struct TimerIFace *ITimer;

void uuid4_generate(char *_dst) {
    char *dst = _dst;
    static const char *template = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    static const char *chars = "0123456789abcdef";
    uint64_t seed[2] = {0, 0};
    struct RandomState state;
    DECLARE_UTILITYBASE();
    // DECLARE_TIMERBASE();

    GetSysTime((struct TimeVal *)&state);

    typedef union {
        int64_t big;
        struct {
            int32_t a;
            int32_t b;
        };
    } _seed;

    union {
        unsigned char b[16];
        uint64_t word[2];
    } s;

    const char *p;
    int i, n;

    _seed seed1 = { .a = Random(&state), .b = Random(&state) };
    _seed seed2 = { .a = Random(&state), .b = Random(&state) };
    seed[0] = seed1.big;
    seed[1] = seed2.big;

    /* get random */
    s.word[0] = xorshift128plus(seed);
    s.word[1] = xorshift128plus(seed);
    /* build string */
    p = template;
    i = 0;
    while (*p) {
        n = s.b[i >> 1];
        n = (i & 1) ? (n >> 4) : (n & 0xf);
        switch (*p) {
            case 'x'  :
                *dst = chars[n];
                i++;
                break;
            case 'y'  :
                *dst = chars[(n & 0x3) + 8];
                i++;
                break;
            default   :
                *dst = *p;
        }
        dst++, p++;
    }
    *dst = '\0';

    D(("[uuid4_generate :] result : <%s>\n", _dst));
}