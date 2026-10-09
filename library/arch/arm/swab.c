/*
 * swab.c - swap bytes in arrays, for ARM.
 *
 * The PowerPC routines are assembler. Here the same contracts in C: swab() swaps
 * each adjacent pair of bytes; swab24/32/64() reverse the bytes of each 3-, 4- or
 * 8-byte word. A trailing partial group is copied unchanged, and a final odd byte
 * of swab() is not copied, as POSIX leaves it.
 *
 * No part of this file is derived from any AmigaOS implementation.
 */

#include <unistd.h>

static void
swab_groups(const void *bfrom, void *bto, ssize_t nbytes, ssize_t group)
{
    const unsigned char *from = bfrom;
    unsigned char *to = bto;
    ssize_t whole = nbytes - (nbytes % group);
    ssize_t i, j;

    for (i = 0; i < whole; i += group)
        for (j = 0; j < group; j++)
            to[i + j] = from[i + group - 1 - j];
    for (; i < nbytes; i++)
        to[i] = from[i];
}

void
swab(const void *bfrom, void *bto, ssize_t nbytes)
{
    swab_groups(bfrom, bto, nbytes - (nbytes % 2), 2);
}

void
swab24(const void *bfrom, void *bto, ssize_t nbytes)
{
    swab_groups(bfrom, bto, nbytes, 3);
}

void
swab32(const void *bfrom, void *bto, ssize_t nbytes)
{
    swab_groups(bfrom, bto, nbytes, 4);
}

void
swab64(const void *bfrom, void *bto, ssize_t nbytes)
{
    swab_groups(bfrom, bto, nbytes, 8);
}
