/*
 * getsp.c - the stack pointer, for ARM.
 *
 * The PowerPC routine copies r1, the stack pointer. On ARM it is sp.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <stdint.h>

uintptr_t
__get_sp(void)
{
    uintptr_t sp;

    __asm__ __volatile__("mov %0, sp" : "=r"(sp));
    return sp;
}
