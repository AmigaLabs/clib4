/*
 * $Id: stdlib_random.c,v 1.0 2021-03-22 20:10:26 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */


uint32_t lcg31(uint32_t x) {
    return (1103515245 * x + 12345) & 0x7fffffff;
}

uint64_t lcg64(uint64_t x) {
    return 6364136223846793005ull * x + 1;
}

void *savestate() {
    __global_clib2->x[-1] = (__global_clib2->n << 16) | (__global_clib2->i << 8) | __global_clib2->j;
    return __global_clib2->x - 1;
}

void loadstate(uint32_t *state) {
    __global_clib2->x = state + 1;
    __global_clib2->n = __global_clib2->x[-1] >> 16;
    __global_clib2->i = (__global_clib2->x[-1] >> 8) & 0xff;
    __global_clib2->j = __global_clib2->x[-1] & 0xff;
}

long
random(void) {
    long k;

    ObtainSemaphore(__global_clib2->__random_lock);

    if (__global_clib2->n == 0) {
        k = __global_clib2->x[0] = lcg31(__global_clib2->x[0]);
        goto end;
    }
    __global_clib2->x[__global_clib2->i] += __global_clib2->x[__global_clib2->j];
    k = __global_clib2->x[__global_clib2->i]>>1;
    if (++__global_clib2->i == __global_clib2->n)
        __global_clib2->i = 0;
    if (++__global_clib2->j == __global_clib2->n)
        __global_clib2->j = 0;

end:
    ReleaseSemaphore(__global_clib2->__random_lock);

    return k;
}