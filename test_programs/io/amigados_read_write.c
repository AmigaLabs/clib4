#define __USE_INLINE__
#include <proto/dos.h>
#include <proto/exec.h>
#include <dos/dos.h>

int main() {
    BPTR file = Open("test.c", MODE_OLDFILE);
    if (!file) return RETURN_FAIL;

    char buffer[1024];
    LONG bytesRead;

    while ((bytesRead = Read(file, buffer, sizeof(buffer))) > 0) {
        Write(Output(), buffer, bytesRead);
    }
    FPuts(Output(), "\n");  // force output
    FFlush(Output());

    Close(file);
    return RETURN_OK;
}
