#include <stdio.h>
#include <string.h>
#include <regex.h>

int main() {
    char *source = "___ abc123def ___ ghi456 ___";
    char *regexString = "[a-z]*([0-9]+)([a-z]*)";
    size_t maxMatches = 2;
    size_t maxGroups = 3;

    regex_t regexCompiled;
    regmatch_t groupArray[maxGroups];
    unsigned int m;
    char *cursor;

    if (regcomp(&regexCompiled, regexString, REG_EXTENDED)) {
        printf("Could not compile regular expression.\n");
        return 1;
    };

    m = 0;
    cursor = source;
    for (m = 0; m < maxMatches; m++) {
        if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
            break;  // No more matches

        unsigned int g = 0;
        unsigned int offset = 0;
        for (g = 0; g < maxGroups; g++) {
            if (groupArray[g].rm_so == (size_t) - 1)
                break;  // No more groups

            if (g == 0)
                offset = groupArray[g].rm_eo;

            char cursorCopy[strlen(cursor) + 1];
            strcpy(cursorCopy, cursor);
            cursorCopy[groupArray[g].rm_eo] = 0;
            printf("Match %u, Group %u: [%2u-%2u]: %s\n",
                   m, g, groupArray[g].rm_so, groupArray[g].rm_eo,
                   cursorCopy + groupArray[g].rm_so);
        }
        cursor += offset;
    }

    regfree(&regexCompiled);

    return 0;
}