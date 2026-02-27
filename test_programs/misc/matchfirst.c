#define __USE_INLINE__
#include <stdio.h>
#include <stdlib.h>
#include <proto/dos.h>
#include <dos/obsolete.h>

void loadFile(const char *file) {
    char *filename = malloc(400);
    if (filename) {
        sprintf(filename, "%s%s/%s","PROGDIR:", "UserMaps", file);
        FILE *f = fopen(filename, "rb");
        if (!f)
            printf("Cannot open %s\n", filename);
        else {
            printf("File %s opened\n", filename);
            fclose(f);
        }
        free(filename);
    }
    else
        printf("Malloc error\n");
}

int main() {
    STRPTR temp = malloc(400);
    struct AnchorPath *myAnchor __attribute__ ((aligned));
    int i;

    myAnchor = AllocDosObjectTags(DOS_ANCHORPATH,
                                  ADO_Strlen, 1024,
                                  TAG_END);

    NameFromLock(GetProgramDir(), temp, 399);
    AddPart(temp, "UserMaps", 399);

    sprintf(temp, "%s/#?.map", temp);
    printf("PATTERN = %s\n", temp);
    myAnchor->ap_Strlen = 0;
    myAnchor->ap_BreakBits = 0;
    myAnchor->ap_Flags = 0;

    if ((i = MatchFirst(temp, myAnchor)) == 0) {
        Printf("FILE = %s\n", myAnchor->ap_Info.fib_FileName);
        loadFile(myAnchor->ap_Info.fib_FileName);
        while ((i = MatchNext(myAnchor)) == 0) {
            loadFile(myAnchor->ap_Info.fib_FileName);
        }
    }

    if (i != ERROR_NO_MORE_ENTRIES)
        printf("InitUserMap(): Match failed, error code %d...\n", i);

    MatchEnd(myAnchor);

    if (myAnchor)
        FreeDosObject(DOS_ANCHORPATH, myAnchor);

    if (temp)
        free(temp);
    return 0;
}
