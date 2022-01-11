#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(void)
{
    void *handle;
    double (*cosine)(double);
    const char *error;

    handle = dlopen("libm.so", RTLD_LAZY);
    if (!handle)
    {
        fprintf(stderr, "Error %s\n", dlerror());
        exit(EXIT_FAILURE);
    }

    dlerror(); /* Clear any existing error */

    cosine = (double (*)(double))dlsym(handle, "cos");
    printf("cosine handler = %x\n", cosine);

    error = dlerror();
    if (error != NULL)
    {
        fprintf(stderr, "Error %s\n", error);
        exit(EXIT_FAILURE);
    }

    printf("%f\n", (*cosine)(2.0));
    dlclose(handle);
    exit(EXIT_SUCCESS);
}