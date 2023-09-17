#include <stdlib.h>
#include <stdio.h>
#include <fts.h>
#include <string.h>

int compare(const FTSENT **, const FTSENT **);
void indent(int i);

int main(int argc, char *const argv[]) {
    FTS *file_system = NULL;
    FTSENT * node    = NULL;

    if (argc < 2) {
        printf("Usage: %s <path-spec>\n", argv[0]);
        exit(255);
    }

    file_system = fts_open(argv + 1, FTS_COMFOLLOW | FTS_NOCHDIR, &compare);

    if (NULL != file_system) {
        while ((node = fts_read(file_system)) != NULL) {
            switch (node->fts_info) {
                case FTS_D :
                case FTS_F :
                case FTS_SL:
                    indent(node->fts_level);
                    printf("%s\n", node->fts_name);
                    break;
                default:
                    break;
            }
        }
        fts_close(file_system);
    }
    return 0;
}

int compare(const FTSENT **one, const FTSENT **two) {
    return (strcmp((*one)->fts_name, (*two)->fts_name));
}

void indent(int i) {
    for (; i > 0; i--)
        printf("    ");
}