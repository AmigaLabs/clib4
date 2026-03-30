/*
 * Minimal nested pthread_join test (no printf from threads)
 *
 * Same structure as test_pthread_join_nested.c but thread functions
 * do NO printf/fflush — only the main thread prints.
 * This eliminates stdio locking as a possible deadlock cause.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#define NUM_PARENTS         10
#define CHILDREN_PER_PARENT  3
#define GRANDCHILDREN        2
#define NUM_ITERATIONS       5

/* ---- Grandchild: leaf thread, exits immediately ---- */
typedef struct {
    int parent_id;
    int child_id;
    int grand_id;
} GrandchildArg;

static void *grandchild_func(void *arg) {
    GrandchildArg *ga = (GrandchildArg *)arg;
    return (void *)(long)(ga->parent_id * 1000 + ga->child_id * 10 + ga->grand_id);
}

/* ---- Child: creates grandchildren and joins them, or returns fast ---- */
typedef struct {
    int parent_id;
    int child_id;
    int create_grandchildren;
    volatile int error_count; /* set by thread on failure */
} ChildArg;

static void *child_func(void *arg) {
    ChildArg *ca = (ChildArg *)arg;
    ca->error_count = 0;

    if (ca->create_grandchildren) {
        pthread_t gthr[GRANDCHILDREN];
        GrandchildArg gargs[GRANDCHILDREN];

        for (int i = 0; i < GRANDCHILDREN; i++) {
            gargs[i].parent_id = ca->parent_id;
            gargs[i].child_id  = ca->child_id;
            gargs[i].grand_id  = i;
            int rc = pthread_create(&gthr[i], NULL, grandchild_func, &gargs[i]);
            if (rc != 0) {
                ca->error_count++;
                gargs[i].grand_id = -1;
            }
        }

        for (int i = 0; i < GRANDCHILDREN; i++) {
            if (gargs[i].grand_id == -1) continue;
            void *gret = NULL;
            int rc = pthread_join(gthr[i], &gret);
            if (rc != 0)
                ca->error_count++;
        }
    }

    return (void *)(long)(ca->parent_id * 100 + ca->child_id);
}

/* ---- Parent: creates children and joins them ---- */
typedef struct {
    int parent_id;
    volatile int error_count;
} ParentArg;

static void *parent_func(void *arg) {
    ParentArg *pa = (ParentArg *)arg;
    int pid = pa->parent_id;
    pthread_t cthr[CHILDREN_PER_PARENT];
    ChildArg cargs[CHILDREN_PER_PARENT];

    pa->error_count = 0;

    for (int i = 0; i < CHILDREN_PER_PARENT; i++) {
        cargs[i].parent_id = pid;
        cargs[i].child_id  = i;
        cargs[i].create_grandchildren = (i == 0);
        cargs[i].error_count = 0;

        int rc = pthread_create(&cthr[i], NULL, child_func, &cargs[i]);
        if (rc != 0) {
            cargs[i].child_id = -1;
            pa->error_count++;
        }
    }

    for (int i = 0; i < CHILDREN_PER_PARENT; i++) {
        if (cargs[i].child_id == -1) continue;
        void *cret = NULL;
        int rc = pthread_join(cthr[i], &cret);
        if (rc != 0)
            pa->error_count++;
        pa->error_count += cargs[i].error_count;
    }

    return (void *)(long)pid;
}

int main(void) {
    int total_failures = 0;

    printf("=== MINIMAL NESTED PTHREAD_JOIN TEST (no printf in threads) ===\n");
    printf("Parents=%d  Children/parent=%d  Grandchildren/child0=%d  Iterations=%d\n",
           NUM_PARENTS, CHILDREN_PER_PARENT, GRANDCHILDREN, NUM_ITERATIONS);
    printf("Total threads per iteration: %d\n",
           NUM_PARENTS + NUM_PARENTS * CHILDREN_PER_PARENT + NUM_PARENTS * GRANDCHILDREN);
    printf("===============================================================\n\n");
    fflush(stdout);

    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        pthread_t pthr[NUM_PARENTS];
        ParentArg pargs[NUM_PARENTS];
        int failures = 0;

        printf(">>> Iteration %d/%d <<<\n", iter + 1, NUM_ITERATIONS);
        fflush(stdout);

        for (int i = 0; i < NUM_PARENTS; i++) {
            pargs[i].parent_id = i;
            pargs[i].error_count = 0;
            int rc = pthread_create(&pthr[i], NULL, parent_func, &pargs[i]);
            if (rc != 0) {
                printf("MAIN: ERROR: pthread_create parent %d failed: %s\n",
                       i, strerror(rc));
                fflush(stdout);
                pargs[i].parent_id = -1;
                failures++;
            }
        }

        for (int i = 0; i < NUM_PARENTS; i++) {
            if (pargs[i].parent_id == -1) continue;
            void *pret = NULL;
            printf("MAIN: joining parent %d...\n", i);
            fflush(stdout);
            int rc = pthread_join(pthr[i], &pret);
            if (rc != 0) {
                printf("MAIN: ERROR: pthread_join parent %d failed: %s\n",
                       i, strerror(rc));
                failures++;
            } else {
                printf("MAIN: parent %d joined (ret=%ld, child_errors=%d)\n",
                       i, (long)pret, pargs[i].error_count);
                failures += pargs[i].error_count;
            }
            fflush(stdout);
        }

        total_failures += failures;
        printf(">>> Iteration %d complete (failures=%d) <<<\n\n", iter + 1, failures);
        fflush(stdout);
    }

    printf("=== ALL DONE. Total failures: %d ===\n", total_failures);
    return total_failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
