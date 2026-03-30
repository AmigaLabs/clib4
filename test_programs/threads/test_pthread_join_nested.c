/*
 * Aggressive nested pthread_join test (v2 - diagnostic)
 *
 * Creates ~10 parent threads, each of which creates 3 child threads
 * (and some children create grandchildren). All levels call pthread_join
 * on their children. This stresses:
 *  - Hierarchical/nested thread creation and join
 *  - Concurrent join_list usage from multiple threads
 *  - Signal allocation/deallocation across thread lifetimes
 *  - Thread slot reuse across iterations
 *
 * v2: No usleep (all children return immediately), prints pthread_t IDs
 *     to help diagnose where hangs occur.
 *
 * Reproducer for PowerFox PR_JoinThread hang.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
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
    pthread_t self = pthread_self();
    printf("    [P%d-C%d-G%d] grandchild running (tid=%d)\n",
           ga->parent_id, ga->child_id, ga->grand_id, (int)self);
    fflush(stdout);
    /* Return immediately — no usleep — to stress StarterFunc exit path */
    return (void *)(long)(ga->parent_id * 1000 + ga->child_id * 10 + ga->grand_id);
}

/* ---- Child: creates grandchildren and joins them, or returns fast ---- */
typedef struct {
    int parent_id;
    int child_id;
    int create_grandchildren; /* nonzero => spawn grandchildren */
} ChildArg;

static void *child_func(void *arg) {
    ChildArg *ca = (ChildArg *)arg;
    int pid = ca->parent_id;
    int cid = ca->child_id;
    pthread_t self = pthread_self();

    printf("  [P%d-C%d] child running (tid=%d, grandchildren=%s)\n",
           pid, cid, (int)self, ca->create_grandchildren ? "yes" : "no");
    fflush(stdout);

    if (ca->create_grandchildren) {
        pthread_t gthr[GRANDCHILDREN];
        GrandchildArg gargs[GRANDCHILDREN];
        int i;

        for (i = 0; i < GRANDCHILDREN; i++) {
            gargs[i].parent_id = pid;
            gargs[i].child_id  = cid;
            gargs[i].grand_id  = i;
            int rc = pthread_create(&gthr[i], NULL, grandchild_func, &gargs[i]);
            if (rc != 0) {
                printf("  [P%d-C%d] ERROR: pthread_create grandchild %d failed: %s\n",
                       pid, cid, i, strerror(rc));
                fflush(stdout);
                gargs[i].grand_id = -1; /* mark as not created */
            } else {
                printf("  [P%d-C%d] created grandchild %d (tid=%d)\n",
                       pid, cid, i, (int)gthr[i]);
                fflush(stdout);
            }
        }

        for (i = 0; i < GRANDCHILDREN; i++) {
            if (gargs[i].grand_id == -1)
                continue;
            void *gret = NULL;
            printf("  [P%d-C%d] joining grandchild %d (tid=%d)...\n",
                   pid, cid, i, (int)gthr[i]);
            fflush(stdout);
            int rc = pthread_join(gthr[i], &gret);
            if (rc != 0) {
                printf("  [P%d-C%d] ERROR: pthread_join grandchild %d failed: %s\n",
                       pid, cid, i, strerror(rc));
            } else {
                printf("  [P%d-C%d] grandchild %d joined (ret=%ld)\n",
                       pid, cid, i, (long)gret);
            }
            fflush(stdout);
        }
    }
    /* No usleep for any child — return immediately */

    printf("  [P%d-C%d] child returning (tid=%d)\n", pid, cid, (int)self);
    fflush(stdout);
    return (void *)(long)(pid * 100 + cid);
}

/* ---- Parent: creates children and joins them ---- */
typedef struct {
    int parent_id;
} ParentArg;

static void *parent_func(void *arg) {
    ParentArg *pa = (ParentArg *)arg;
    int pid = pa->parent_id;
    pthread_t self = pthread_self();
    pthread_t cthr[CHILDREN_PER_PARENT];
    ChildArg cargs[CHILDREN_PER_PARENT];
    int i;

    printf("[P%d] parent running (tid=%d), creating %d children\n",
           pid, (int)self, CHILDREN_PER_PARENT);
    fflush(stdout);

    for (i = 0; i < CHILDREN_PER_PARENT; i++) {
        cargs[i].parent_id = pid;
        cargs[i].child_id  = i;
        /* First child of each parent creates grandchildren */
        cargs[i].create_grandchildren = (i == 0);

        int rc = pthread_create(&cthr[i], NULL, child_func, &cargs[i]);
        if (rc != 0) {
            printf("[P%d] ERROR: pthread_create child %d failed: %s\n",
                   pid, i, strerror(rc));
            fflush(stdout);
            cargs[i].child_id = -1; /* mark as not created */
        } else {
            printf("[P%d] created child %d (tid=%d)\n", pid, i, (int)cthr[i]);
            fflush(stdout);
        }
    }

    /* Join children in order */
    for (i = 0; i < CHILDREN_PER_PARENT; i++) {
        if (cargs[i].child_id == -1)
            continue;
        void *cret = NULL;
        printf("[P%d] joining child %d (tid=%d)...\n", pid, i, (int)cthr[i]);
        fflush(stdout);
        int rc = pthread_join(cthr[i], &cret);
        if (rc != 0) {
            printf("[P%d] ERROR: pthread_join child %d (tid=%d) failed: %s\n",
                   pid, i, (int)cthr[i], strerror(rc));
        } else {
            printf("[P%d] child %d joined (ret=%ld)\n", pid, i, (long)cret);
        }
        fflush(stdout);
    }

    printf("[P%d] parent done (tid=%d)\n", pid, (int)self);
    fflush(stdout);
    return (void *)(long)pid;
}

int main(void) {
    int iter, i;
    int total_failures = 0;

    printf("=== AGGRESSIVE NESTED PTHREAD_JOIN TEST v2 ===\n");
    printf("Parents=%d  Children/parent=%d  Grandchildren/child0=%d  Iterations=%d\n",
           NUM_PARENTS, CHILDREN_PER_PARENT, GRANDCHILDREN, NUM_ITERATIONS);
    printf("Total threads per iteration: %d parents + %d children + %d grandchildren = %d\n",
           NUM_PARENTS,
           NUM_PARENTS * CHILDREN_PER_PARENT,
           NUM_PARENTS * GRANDCHILDREN,
           NUM_PARENTS + NUM_PARENTS * CHILDREN_PER_PARENT + NUM_PARENTS * GRANDCHILDREN);
    printf("NO usleep — all children return immediately\n");
    printf("============================================\n\n");
    fflush(stdout);

    for (iter = 0; iter < NUM_ITERATIONS; iter++) {
        pthread_t pthr[NUM_PARENTS];
        ParentArg pargs[NUM_PARENTS];
        int failures = 0;

        printf(">>> Iteration %d/%d <<<\n", iter + 1, NUM_ITERATIONS);
        fflush(stdout);

        /* Create all parent threads */
        for (i = 0; i < NUM_PARENTS; i++) {
            pargs[i].parent_id = i;
            int rc = pthread_create(&pthr[i], NULL, parent_func, &pargs[i]);
            if (rc != 0) {
                printf("MAIN: ERROR: pthread_create parent %d failed: %s\n",
                       i, strerror(rc));
                fflush(stdout);
                pargs[i].parent_id = -1;
                failures++;
            } else {
                printf("MAIN: created parent %d (tid=%d)\n", i, (int)pthr[i]);
                fflush(stdout);
            }
        }

        /* Join all parents */
        for (i = 0; i < NUM_PARENTS; i++) {
            if (pargs[i].parent_id == -1)
                continue;
            void *pret = NULL;
            printf("MAIN: joining parent %d (tid=%d)...\n", i, (int)pthr[i]);
            fflush(stdout);
            int rc = pthread_join(pthr[i], &pret);
            if (rc != 0) {
                printf("MAIN: ERROR: pthread_join parent %d failed: %s\n",
                       i, strerror(rc));
                failures++;
            } else {
                printf("MAIN: parent %d joined (ret=%ld)\n", i, (long)pret);
            }
            fflush(stdout);
        }

        printf(">>> Iteration %d complete (failures=%d) <<<\n\n", iter + 1, failures);
        fflush(stdout);
        total_failures += failures;
    }

    printf("=== ALL DONE: %d iterations, total failures=%d ===\n",
           NUM_ITERATIONS, total_failures);
    fflush(stdout);
    return total_failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
