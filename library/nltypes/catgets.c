/*
 * $Id: nltypes_catgets.c,v 1.0 2023-04-22 07:38:10 clib2devs Exp $
*/

#include <nl_types.h>

/* TODO - Not implemented */
char *catgets(nl_catd catalog, int set_number, int message_number, const char *message) {
    (void) (catalog);
    (void) (set_number);
    (void) (message_number);
    (void) (message);

    return "catalog not available";
}