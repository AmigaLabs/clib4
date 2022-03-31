/* Benchmark for some string functions
   Copyright (C) 2001, 2002 Free Software Foundation, Inc.
   Written by Stephane Carrez (stcarrez@nerim.fr)

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file with other programs, and to distribute
those programs without any restriction coming from the use of this
file.  (The General Public License restrictions do apply in other
respects; for example, they cover modification of the file, and
distribution when not linked into another program.)

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

void bench_string();

#define MAX_LENGTH 64
#define TIMES 10000000

/* Benchmark the walk of a single linked list having 100 elements.  */
void
bench_string() {
    int res;
    char *p;

    /* strcpy with a constant string.  */
    clock_t begin = clock();
    for (int i = 0; i < TIMES; i++) {
        char buf[MAX_LENGTH];
        strcpy(buf, "0");
        strcpy(buf, "0123456789abcdef");
        strcpy(buf, "0123456789abcdef0123456789abcdef");
        strcat(buf, "0");
        strcat(buf, "0123456789abcdef");
        strcat(buf, "0123456789abcdef");

        strcpy(buf, "0");
        res = strlen(buf);
        strcat(buf, "0123456789abcdef");
        res = strlen(buf);
        strcat(buf, "0123456789abcdef");
        res = strlen(buf);
        res = strcmp(buf, "0123456789abcdef0123456789abcdef");
        p = strrchr(buf, '0');
        p = strstr(buf, "f0123456789abcdef");
    }
    clock_t end = clock();
    double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("%lf\n", time_spent);

}

/* Main, run the benchmarks.  */
int
main() {
    bench_string();
    return 0;
}
