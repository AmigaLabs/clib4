/*
 * $Id: regex_regfree.c,v 1.0 2022-03-21 18:52:18 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <regex.h>

#include "tre.h"

void
regfree(regex_t *preg) {
    tre_tnfa_t *tnfa;
    unsigned int i;
    tre_tnfa_transition_t *trans;

    tnfa = (void *) preg->TRE_REGEX_T_FIELD;
    if (!tnfa)
        return;

    for (i = 0; i < tnfa->num_transitions; i++)
        if (tnfa->transitions[i].state) {
            if (tnfa->transitions[i].tags)
                xfree(tnfa->transitions[i].tags);
            if (tnfa->transitions[i].neg_classes)
                xfree(tnfa->transitions[i].neg_classes);
        }
    if (tnfa->transitions)
        xfree(tnfa->transitions);

    if (tnfa->initial) {
        for (trans = tnfa->initial; trans->state; trans++) {
            if (trans->tags)
                xfree(trans->tags);
        }
        xfree(tnfa->initial);
    }

    if (tnfa->submatch_data) {
        for (i = 0; i < tnfa->num_submatches; i++)
            if (tnfa->submatch_data[i].parents)
                xfree(tnfa->submatch_data[i].parents);
        xfree(tnfa->submatch_data);
    }

    if (tnfa->tag_directions)
        xfree(tnfa->tag_directions);
    if (tnfa->firstpos_chars)
        xfree(tnfa->firstpos_chars);
    if (tnfa->minimal_tags)
        xfree(tnfa->minimal_tags);
    xfree(tnfa);
}
