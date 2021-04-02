/*
 * $Id: stdlib_rand48.c,v 1.0 2021-02-23 10:36:26 apalmate Exp $
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

#include <inttypes.h>

static uint64_t 
iterate48(unsigned short subi[3])
{
    const uint64_t a = UINT64_C(0x5DEECE66D);
    const unsigned c = 13;
    const uint64_t mask = UINT64_C(0xFFFFFFFFFFFF); // 48 bits

    uint64_t x = ((uint64_t)subi[0] << 32) | ((uint32_t)subi[1] << 16) | subi[2];

    x *= a;
    x += c;
    x &= mask;

    subi[0] = (x >> 32) & 0xFFFF;
    subi[1] = (x >> 16) & 0xFFFF;
    subi[2] = (x >> 0) & 0XFFFF;

    return x;
}

double 
erand48(unsigned short subi[3])
{
    uint64_t r = iterate48(subi);
    return ((double)r) / 281474976710655.;
}

long 
jrand48(unsigned short subi[3])
{
    return ((int64_t)iterate48(subi)) >> 16;
}

long 
nrand48(unsigned short subi[3])
{
    return iterate48(subi) >> 17;
}