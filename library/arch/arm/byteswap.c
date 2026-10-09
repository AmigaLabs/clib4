/*
 * byteswap.c - bswap16/24/32/64 for ARM.
 *
 * The PowerPC versions are assembler routines; on ARM the compiler's byte
 * swap builtins give the same results, and the prototypes are those of
 * sys/byteswap.h. bswap24 swaps the outer two bytes of the low 24 bits and
 * keeps the middle byte, the contract of the PowerPC routine it replaces.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <stdint.h>

extern uint16_t bswap16(uint16_t);
extern uint32_t bswap24(uint32_t);
extern uint32_t bswap32(uint32_t);
extern uint64_t bswap64(uint64_t);

uint16_t
bswap16(uint16_t x)
{
    return __builtin_bswap16(x);
}

uint32_t
bswap24(uint32_t x)
{
    return ((x & 0x000000ffU) << 16) | (x & 0x0000ff00U) | ((x >> 16) & 0x000000ffU);
}

uint32_t
bswap32(uint32_t x)
{
    return __builtin_bswap32(x);
}

uint64_t
bswap64(uint64_t x)
{
    return __builtin_bswap64(x);
}
