/*
 * $Id: __std.c,v 1.0 2023-10-27 12:04:22 clib4devs Exp $
*/

#include <stdio.h>
#include <dos.h>

FILE *
__stdin () {
    return (FILE *) __CLIB4->__iob[0];
}

FILE *
__stdout () {
    return (FILE *) __CLIB4->__iob[1];
}

FILE *
__stderr () {
    return (FILE *) __CLIB4->__iob[2];
}