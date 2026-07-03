#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <pthread.h>

static void try_sym(void *h, const char *name)
{
    dlerror(); /* clear */
    void *p = dlsym(h, name);
    const char *err = dlerror();
    printf("  dlsym(%-22s) = %p%s%s\n", name, p,
           p ? "" : "   <-- NOT FOUND",
           err ? "  err=" : "");
    if (err) printf("      (%s)\n", err);
}

static void test_handle(const char *label, void *h)
{
    printf("=== %s -> handle=%p ===\n", label, h);
    if (!h) {
        const char *err = dlerror();
        printf("  (open failed: %s)\n", err ? err : "unknown");
        return;
    }
    /* Known-good baseline (clib4's own test uses these) */
    try_sym(h, "malloc");
    try_sym(h, "free");
    /* The OS.File POSIX functions that ctypes fails to find */
    try_sym(h, "getcwd");
    try_sym(h, "mkdir");
    try_sym(h, "lstat");
    try_sym(h, "stat");
    try_sym(h, "open");
    try_sym(h, "close");
    try_sym(h, "read");
    try_sym(h, "opendir");
    printf("\n");
}

/* Worker-thread version: OS.File's ctypes runs on a ChromeWorker (a separate
 * pthread/Task), NOT the main task.  The C test above runs on the main task and
 * succeeds; if dlopen/dlsym behave differently on a secondary pthread, that is
 * the difference between the working C test and the failing ctypes call. */
static void *thread_main(void *arg)
{
    (void)arg;
    printf("########## FROM SECONDARY pthread (worker-like) ##########\n");
    void *h = dlopen("PROGDIR:SObjs/libc.so", RTLD_LAZY);
    test_handle("[thread] dlopen(\"PROGDIR:SObjs/libc.so\", RTLD_LAZY)", h);
    /* Also try opening with flags 0, exactly like ctypes' PR_LoadLibraryWithFlags(spec, 0) */
    void *h0 = dlopen("PROGDIR:SObjs/libc.so", 0);
    test_handle("[thread] dlopen(\"PROGDIR:SObjs/libc.so\", 0)  <-- ctypes uses flags 0", h0);
    return NULL;
}

int main(void)
{
    printf("clib4 dlsym test for OS.File symbols\n\n");

    /* KEY TEST: resolve symbols from the process itself (RTLD_DEFAULT == NULL
     * handle) WITHOUT any dlopen.  xpcshell/powerfox are statically linked with
     * clib4, so libc symbols are already in the process.  If this works, the
     * NSPR fix is: fall back to dlsym(RTLD_DEFAULT, name) when the libc.so
     * dlopen handle is dead. */
    printf("=== dlsym(RTLD_DEFAULT, ...) [no dlopen, process self] ===\n");
    try_sym(RTLD_DEFAULT, "malloc");
    try_sym(RTLD_DEFAULT, "getcwd");
    try_sym(RTLD_DEFAULT, "mkdir");
    try_sym(RTLD_DEFAULT, "lstat");
    printf("\n");

    /* The exact string OS.File/ctypes uses on AmigaOS. */
    test_handle("dlopen(\"PROGDIR:SObjs/libc.so\", RTLD_LAZY)",
                dlopen("PROGDIR:SObjs/libc.so", RTLD_LAZY));

    /* ctypes opens with flags 0 (PR_LoadLibraryWithFlags(spec, 0)); test that
     * exact mode on the main task too. */
    test_handle("dlopen(\"PROGDIR:SObjs/libc.so\", 0)  <-- ctypes uses flags 0",
                dlopen("PROGDIR:SObjs/libc.so", 0));

    /* Variants, in case the path/flags matter. */
    test_handle("dlopen(\"PROGDIR:SObjs/libc.so\", RTLD_NOW|RTLD_GLOBAL)",
                dlopen("PROGDIR:SObjs/libc.so", RTLD_NOW | RTLD_GLOBAL));

    /* dlopen(NULL) == main program's own symbols (may be how some builds
     * expect POSIX fns to resolve). */
    test_handle("dlopen(NULL, RTLD_LAZY) [self]",
                dlopen(NULL, RTLD_LAZY));

    /* Now repeat from a secondary pthread, like the OS.File ChromeWorker. */
    pthread_t th;
    if (pthread_create(&th, NULL, thread_main, NULL) == 0) {
        pthread_join(th, NULL);
    } else {
        printf("pthread_create FAILED\n");
    }

    printf("DONE\n");
    return 0;
}
