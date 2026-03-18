/*
 * Test program to debug errno behavior
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

// Flag per verificare che il signal handler è stato chiamato
static volatile sig_atomic_t alarm_fired = 0;

// Signal handler per SIGALRM
// IMPORTANTE: I signal handler DEVONO salvare e ripristinare errno!
void alarm_handler(int sig) {
    int saved_errno = errno;  // Salva errno
    (void)sig;
    alarm_fired = 1;
    errno = saved_errno;  // Ripristina errno
}

int main(void) {
    struct timespec req = { 2, 0 };
    struct timespec rem;
    int result, saved_errno;

    printf("=== ERRNO DEBUG TEST ===\n\n");

    // Test 1: Direct errno access
    printf("Test 1: Direct errno manipulation\n");
    errno = 0;
    printf("  errno = 0, read back: %d\n", errno);
    errno = 42;
    printf("  errno = 42, read back: %d\n", errno);
    errno = EINVAL;
    printf("  errno = EINVAL(%d), read back: %d\n", EINVAL, errno);
    printf("\n");

    // Test 2: nanosleep with invalid input (should set errno)
    printf("Test 2: nanosleep with NULL req (should fail with EFAULT)\n");
    errno = 0;
    printf("  Before call: errno = %d\n", errno);
    result = nanosleep(NULL, &rem);
    saved_errno = errno;  // IMPORTANTE: Salvare errno IMMEDIATAMENTE dopo la syscall
    printf("  After call: result = %d, errno = %d (expected EFAULT=%d)\n",
           result, saved_errno, EFAULT);
    printf("\n");

    // Test 3: nanosleep with negative time (should set errno)
    printf("Test 3: nanosleep with negative time (should fail with EINVAL)\n");
    errno = 0;
    struct timespec bad_req = { -1, 0 };
    printf("  Before call: errno = %d\n", errno);
    result = nanosleep(&bad_req, &rem);
    saved_errno = errno;  // IMPORTANTE: Salvare errno IMMEDIATAMENTE dopo la syscall
    printf("  After call: result = %d, errno = %d (expected EINVAL=%d)\n",
           result, saved_errno, EINVAL);
    printf("\n");

    // Test 4: nanosleep interrupted by signal (should fail with EINTR)
    printf("Test 4: nanosleep interrupted by signal (should fail with EINTR)\n");

    // Installa il signal handler per SIGALRM
    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Non usare SA_RESTART per permettere EINTR
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        printf("  ERROR: Failed to install signal handler\n");
    } else {
        alarm_fired = 0;
        errno = 0;
        printf("  Before call: errno = %d\n", errno);

        // Imposta un alarm che scatterà dopo 1 secondo
        alarm(1);

        // Prova a dormire per 10 secondi (sarà interrotto dall'alarm)
        struct timespec long_req = { 10, 0 };
        printf("  Calling nanosleep(10 seconds) with alarm(1)...\n");
        result = nanosleep(&long_req, &rem);
        saved_errno = errno;  // IMPORTANTE: Salvare errno IMMEDIATAMENTE dopo la syscall

        printf("  After call: result = %d, errno = %d (expected EINTR=%d)\n",
               result, saved_errno, EINTR);
        printf("  Signal handler called: %s\n", alarm_fired ? "YES" : "NO");
        if (rem.tv_sec > 0 || rem.tv_nsec > 0) {
            printf("  Remaining time: %ld.%09ld seconds\n",
                   (long)rem.tv_sec, (long)rem.tv_nsec);
        }
    }
    printf("\n");

    // Test 5: Valid nanosleep (should succeed)
    printf("Test 5: nanosleep with valid time (should succeed)\n");
    errno = 0;
    printf("  Before call: errno = %d\n", errno);
    printf("  Sleeping for 1 second...\n");
    req.tv_sec = 1;
    req.tv_nsec = 0;
    result = nanosleep(&req, &rem);
    saved_errno = errno;  // IMPORTANTE: Salvare errno IMMEDIATAMENTE dopo la syscall
    printf("  After call: result = %d, errno = %d (expected 0)\n",
           result, saved_errno);

    return 0;
}
