#define __USE_INLINE__
#include <proto/exec.h>
#include <proto/dos.h>

int main() {
    struct Clib2IFace *IClib2 = NULL;
    struct Library *Clib2Base = OpenLibrary("clib2.library", 0);
    if (Clib2Base) {
        IClib2 = (struct Clib2IFace *) GetInterface(Clib2Base, "main", 1, NULL);
        if (!IClib2) {
            Printf("Cannot get clib2 interface\n");
        }
    }
    else {
        Printf("Cannot open clib2.library\n");
    }

    if (IClib2) {
        DropInterface((struct Interface *) IClib2);
        IClib2 = NULL;
    }
    if (Clib2Base) {
        CloseLibrary(Clib2Base);
        Clib2Base = NULL;
    }
}