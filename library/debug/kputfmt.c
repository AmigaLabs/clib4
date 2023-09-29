/*
 * $Id: debug_kputfmt.c,v 1.4 2006-01-08 12:04:22 clib4devs Exp $
*/

#include "debug_headers.h"

/* Count the number of parameters that need to be passed on the stack. If a
   parameter table is provided, pick up the parameters and store them in
   the table. The table layout follows the format string and can include
   both 16 and 32 bit data. */
STATIC size_t
process_var_args(char *format_string, va_list argument_list, unsigned short *table) {
    size_t num_args = 0;
    char len;
    char c;

    while ((c = (*format_string++)) != '\0') {
        if (c != '%')
            continue;

        /* Process the flags; only '-' is allowed here. */
        if ((*format_string) == '-')
            format_string++;

        /* Process the field width; this must be a string of digits. */
        c = (*format_string);
        while ('0' <= c && c <= '9') {
            format_string++;

            c = (*format_string);
        }

        /* Process the size limit field, if any. */
        if ((*format_string) == '.') {
            format_string++;

            c = (*format_string);
            while ('0' <= c && c <= '9') {
                format_string++;

                c = (*format_string);
            }
        }

        /* Take care of the parameter length, if any. */
        len = (*format_string);
        if (len == 'l')
            format_string++;

        c = (*format_string++);
        if (c == '\0')
            break;

        /* Check if any of the following parameters might come from the
           stack, and if so, convert them into the form expected by the
           operating system. */
        switch (c) {
            /* 32 bit pointers (BSTR or STRPTR) */
            case 'b':
            case 's':

                if (table != NULL)
                    (*(unsigned long *) &table[num_args]) = (unsigned long) va_arg(argument_list,
                char *);

                num_args += 2;
                break;

                /* Single character */
            case 'c':

                /* NOTE: va_arg() is quietly assumed not to be able to expand 'char' type
                         parameters. They appear to default to 'int' and should show up as such
                         on the stack (or wherever they ended up). We therefore assume that they
                         should be picked up as type 'int' rather than as type 'char'. */

                /* Numeric constant, which is assumed to be of type int. */
            case 'd':
            case 'u':
            case 'x':

                if (table != NULL) {
                    if (len == 'l')
                        (*(unsigned long *) &table[num_args]) = (unsigned long) va_arg(argument_list,
                    int);
                    else
                    table[num_args] = (unsigned short) va_arg(argument_list,
                    int);
                }

                if (len == 'l')
                    num_args += 2;
                else
                    num_args++;

                break;

                /* Anything else is taken as is... */
            default:

                break;
        }
    }

    return (num_args);
}

STATIC VOID ASM
raw_put_char(REG(d0, UBYTE c))
{
    kputc(c);
}

VOID
KPutFmt(const char *format_string, va_list argument_list) {
    if (format_string != NULL) {
        size_t num_args;

        /* The following assumes that GCC extensions are available. */

        num_args = process_var_args((char *) format_string, argument_list, NULL);
        if (num_args > 0) {
            unsigned short table[num_args];

            (void) process_var_args((char *) format_string, argument_list, table);

            KDoFmt(format_string, (APTR) table, (APTR) raw_put_char, NULL);
        } else {
            KDoFmt(format_string, NULL, (APTR) raw_put_char, NULL);
        }
    }
}
