#define __USE_INLINE__
#include <proto/exec.h>
#include <proto/dos.h>

int main() {
    struct Clib4IFace *IClib4 = NULL;
    struct Library *Clib4Base = OpenLibrary("clib4.library", 0);
    if (Clib4Base) {
        IClib4 = (struct Clib4IFace *) GetInterface(Clib4Base, "main", 1, NULL);
        if (!IClib4) {
            Printf("Cannot get clib4 interface\n");
        }
    }
    else {
        Printf("Cannot open clib4.library\n");
    }

    if (IClib4) {
        DropInterface((struct Interface *) IClib4);
        IClib4 = NULL;
    }
    if (Clib4Base) {
        CloseLibrary(Clib4Base);
        Clib4Base = NULL;
    }
}