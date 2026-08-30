/*
 * nftw_test.c — POSIX conformance test for nftw() and ftw()
 *
 * Covers:
 *  1.  Every entry of a tree is visited exactly once
 *      (regression: readdir() reports "." and ".." under unix path
 *       semantics, and walking into them never ends)
 *  2.  Pre-order (no FTW_DEPTH): a directory is reported as FTW_D before
 *      its contents
 *  3.  Post-order (FTW_DEPTH): a directory is reported as FTW_DP after
 *      its contents
 *  4.  A non-zero return from the callback ends the traversal at once, and
 *      nftw() returns that value
 *  5.  The third argument is a descriptor limit, not a depth limit: a tree
 *      deeper than it is still walked in full
 *  6.  ftwbuf->base and ftwbuf->level
 *  7.  FTW_PHYS: a symbolic link is reported as FTW_SL and not followed
 *  8.  Without FTW_PHYS a symbolic link back up the tree does not send the
 *      traversal round in circles
 *  9.  A path that does not exist gives -1 / ENOENT
 * 10.  ftw() likewise visits every entry exactly once
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/types.h>

/* ------------------------------------------------------------------ helpers */

static int passed = 0;
static int failed = 0;

#define PASS(name) do { printf("  [PASS] %s\n", name); passed++; } while(0)
#define FAIL(name, ...) do { printf("  [FAIL] %s: ", name); printf(__VA_ARGS__); printf("\n"); failed++; } while(0)
#define SKIP(name, ...) do { printf("  [SKIP] %s: ", name); printf(__VA_ARGS__); printf("\n"); } while(0)

#define ROOT "nftw_test_tree"

#define MAX_VISITS 512

struct visit {
    char path[512];
    int type;
    int base;
    int level;
};

static struct visit visits[MAX_VISITS];
static int nvisits = 0;
static int overflowed = 0;

/* Value the callback should return, and after how many visits. 0 disables. */
static int stop_after = 0;
static int stop_value = 0;

static void
reset_visits(void) {
    nvisits = 0;
    overflowed = 0;
    stop_after = 0;
    stop_value = 0;
}

static int
record(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    (void) sb;

    if (nvisits >= MAX_VISITS) {
        overflowed = 1;
        return 1; /* stop before running off the end */
    }

    strncpy(visits[nvisits].path, path, sizeof(visits[0].path) - 1);
    visits[nvisits].path[sizeof(visits[0].path) - 1] = '\0';
    visits[nvisits].type = typeflag;
    visits[nvisits].base = ftwbuf->base;
    visits[nvisits].level = ftwbuf->level;
    nvisits++;

    if (stop_after != 0 && nvisits == stop_after)
        return stop_value;

    return 0;
}

/* ftw() takes a callback with one argument less. */
static int nvisits_ftw = 0;

static int
record_ftw(const char *path, const struct stat *sb, int typeflag) {
    (void) path;
    (void) sb;
    (void) typeflag;

    if (nvisits_ftw >= MAX_VISITS)
        return 1;

    nvisits_ftw++;
    return 0;
}

/* Index of the first visit of path, or -1. */
static int
find_visit(const char *path) {
    int i;

    for (i = 0; i < nvisits; i++)
        if (strcmp(visits[i].path, path) == 0)
            return i;

    return -1;
}

static int
count_visits(const char *path) {
    int i, n = 0;

    for (i = 0; i < nvisits; i++)
        if (strcmp(visits[i].path, path) == 0)
            n++;

    return n;
}

static int
make_file(const char *path) {
    FILE *f = fopen(path, "w");

    if (f == NULL)
        return -1;

    fputs("x\n", f);
    fclose(f);

    return 0;
}

/* Remove a tree without relying on nftw(), which is what is under test. */
static void
remove_tree(const char *path) {
    struct stat st;

    if (lstat(path, &st) != 0)
        return;

    if (S_ISDIR(st.st_mode)) {
        DIR *dp;
        struct dirent *de;
        char child[512];

        dp = opendir(path);
        if (dp != NULL) {
            while ((de = readdir(dp)) != NULL) {
                if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                    continue;

                snprintf(child, sizeof(child), "%s/%s", path, de->d_name);
                remove_tree(child);
            }
            closedir(dp);
        }

        rmdir(path);
    } else {
        unlink(path);
    }
}

/*  The tree built for the tests:
 *
 *    nftw_test_tree/
 *      a.txt
 *      b.txt
 *      sub/
 *        c.txt
 *        deep/
 *          d.txt
 *
 *  That is 4 files and 3 directories, 7 entries in total.
 */
#define TREE_ENTRIES 7

static int
build_tree(void) {
    remove_tree(ROOT);

    if (mkdir(ROOT, 0755) != 0)
        return -1;
    if (mkdir(ROOT "/sub", 0755) != 0)
        return -1;
    if (mkdir(ROOT "/sub/deep", 0755) != 0)
        return -1;
    if (make_file(ROOT "/a.txt") != 0)
        return -1;
    if (make_file(ROOT "/b.txt") != 0)
        return -1;
    if (make_file(ROOT "/sub/c.txt") != 0)
        return -1;
    if (make_file(ROOT "/sub/deep/d.txt") != 0)
        return -1;

    return 0;
}

/* ------------------------------------------------------------------ test 1 */
static void
test_visits_each_entry_once(void) {
    const char *name = "every entry visited exactly once";
    int rc, i;

    reset_visits();

    rc = nftw(ROOT, record, 16, 0);
    if (rc != 0) {
        FAIL(name, "nftw() returned %d (errno=%s)", rc, strerror(errno));
        return;
    }
    if (overflowed) {
        FAIL(name, "more than %d visits: the traversal is not terminating", MAX_VISITS);
        return;
    }
    if (nvisits != TREE_ENTRIES) {
        FAIL(name, "visited %d entries, expected %d", nvisits, TREE_ENTRIES);
        return;
    }

    for (i = 0; i < nvisits; i++) {
        if (count_visits(visits[i].path) != 1) {
            FAIL(name, "'%s' visited more than once", visits[i].path);
            return;
        }
        if (strstr(visits[i].path, "/.") != NULL) {
            FAIL(name, "walked into a dot entry: '%s'", visits[i].path);
            return;
        }
    }

    PASS(name);
}

/* ------------------------------------------------------------------ test 2 */
static void
test_preorder(void) {
    const char *name = "pre-order: FTW_D before contents";
    int dir, child;

    reset_visits();

    if (nftw(ROOT, record, 16, 0) != 0) {
        FAIL(name, "nftw() failed: %s", strerror(errno));
        return;
    }

    dir = find_visit(ROOT "/sub");
    child = find_visit(ROOT "/sub/c.txt");

    if (dir < 0 || child < 0) {
        FAIL(name, "directory or child not visited");
        return;
    }
    if (visits[dir].type != FTW_D) {
        FAIL(name, "'%s' reported as type %d, expected FTW_D (%d)",
             ROOT "/sub", visits[dir].type, FTW_D);
        return;
    }
    if (dir > child) {
        FAIL(name, "directory reported after its contents");
        return;
    }

    PASS(name);
}

/* ------------------------------------------------------------------ test 3 */
static void
test_postorder(void) {
    const char *name = "FTW_DEPTH: FTW_DP after contents";
    int dir, child;

    reset_visits();

    if (nftw(ROOT, record, 16, FTW_DEPTH) != 0) {
        FAIL(name, "nftw() failed: %s", strerror(errno));
        return;
    }
    if (nvisits != TREE_ENTRIES) {
        FAIL(name, "visited %d entries, expected %d", nvisits, TREE_ENTRIES);
        return;
    }

    dir = find_visit(ROOT "/sub");
    child = find_visit(ROOT "/sub/c.txt");

    if (dir < 0 || child < 0) {
        FAIL(name, "directory or child not visited");
        return;
    }
    if (visits[dir].type != FTW_DP) {
        FAIL(name, "'%s' reported as type %d, expected FTW_DP (%d)",
             ROOT "/sub", visits[dir].type, FTW_DP);
        return;
    }
    if (dir < child) {
        FAIL(name, "directory reported before its contents");
        return;
    }

    PASS(name);
}

/* ------------------------------------------------------------------ test 4 */
static void
test_callback_stops_walk(void) {
    const char *name = "non-zero callback return ends the walk";
    int rc;

    reset_visits();
    stop_after = 2;
    stop_value = 42;

    rc = nftw(ROOT, record, 16, 0);

    if (rc != 42) {
        FAIL(name, "nftw() returned %d, expected the callback's 42", rc);
        return;
    }
    if (nvisits != 2) {
        FAIL(name, "%d entries visited after the callback said stop at 2", nvisits);
        return;
    }

    PASS(name);
}

/* ------------------------------------------------------------------ test 5 */
static void
test_fd_limit_is_not_a_depth_limit(void) {
    const char *name = "third argument is an fd limit, not a depth limit";
    const int depth = 12;
    char path[512];
    int i, len, rc;

    remove_tree(ROOT "_deep");

    if (mkdir(ROOT "_deep", 0755) != 0) {
        FAIL(name, "cannot create test directory: %s", strerror(errno));
        return;
    }

    len = snprintf(path, sizeof(path), "%s", ROOT "_deep");
    for (i = 0; i < depth; i++) {
        len += snprintf(path + len, sizeof(path) - len, "/d%d", i);
        if (mkdir(path, 0755) != 0) {
            FAIL(name, "cannot create '%s': %s", path, strerror(errno));
            goto done;
        }
    }

    reset_visits();

    /* An fd limit of 2 must not stop the walk after two levels. */
    rc = nftw(ROOT "_deep", record, 2, 0);
    if (rc != 0) {
        FAIL(name, "nftw() returned %d (errno=%s)", rc, strerror(errno));
        goto done;
    }
    if (nvisits != depth + 1) {
        FAIL(name, "visited %d directories of %d: the walk was cut short",
             nvisits, depth + 1);
        goto done;
    }
    if (find_visit(path) < 0) {
        FAIL(name, "deepest directory '%s' never visited", path);
        goto done;
    }

    PASS(name);
done:
    remove_tree(ROOT "_deep");
}

/* ------------------------------------------------------------------ test 6 */
static void
test_base_and_level(void) {
    const char *name = "ftwbuf->base and ftwbuf->level";
    int root, deep, file;

    reset_visits();

    if (nftw(ROOT, record, 16, 0) != 0) {
        FAIL(name, "nftw() failed: %s", strerror(errno));
        return;
    }

    root = find_visit(ROOT);
    deep = find_visit(ROOT "/sub/deep");
    file = find_visit(ROOT "/sub/deep/d.txt");

    if (root < 0 || deep < 0 || file < 0) {
        FAIL(name, "expected entries not visited");
        return;
    }

    if (visits[root].level != 0) {
        FAIL(name, "root level is %d, expected 0", visits[root].level);
        return;
    }
    if (visits[deep].level != 2) {
        FAIL(name, "'%s' level is %d, expected 2", visits[deep].path, visits[deep].level);
        return;
    }
    if (visits[file].level != 3) {
        FAIL(name, "'%s' level is %d, expected 3", visits[file].path, visits[file].level);
        return;
    }
    if (strcmp(visits[file].path + visits[file].base, "d.txt") != 0) {
        FAIL(name, "base %d of '%s' points at '%s', expected 'd.txt'",
             visits[file].base, visits[file].path, visits[file].path + visits[file].base);
        return;
    }

    PASS(name);
}

/* ------------------------------------------------------------------ test 7 */
static void
test_phys_does_not_follow_links(void) {
    const char *name = "FTW_PHYS: symlink reported as FTW_SL, not followed";
    int link;

    remove_tree(ROOT "/link");

    if (symlink("sub", ROOT "/link") != 0) {
        SKIP(name, "symlink() unavailable here: %s", strerror(errno));
        return;
    }

    reset_visits();

    if (nftw(ROOT, record, 16, FTW_PHYS) != 0) {
        FAIL(name, "nftw() failed: %s", strerror(errno));
        goto done;
    }

    link = find_visit(ROOT "/link");
    if (link < 0) {
        FAIL(name, "the symbolic link was not visited at all");
        goto done;
    }
    if (visits[link].type != FTW_SL) {
        FAIL(name, "link reported as type %d, expected FTW_SL (%d)",
             visits[link].type, FTW_SL);
        goto done;
    }
    if (find_visit(ROOT "/link/c.txt") >= 0) {
        FAIL(name, "the link was followed despite FTW_PHYS");
        goto done;
    }

    PASS(name);
done:
    remove_tree(ROOT "/link");
}

/* ------------------------------------------------------------------ test 8 */
static void
test_link_loop_terminates(void) {
    const char *name = "a symlink loop does not trap the traversal";
    int rc;

    remove_tree(ROOT "/loop");

    /* A link from inside the tree back to its root. */
    if (symlink("..", ROOT "/sub/loop") != 0) {
        SKIP(name, "symlink() unavailable here: %s", strerror(errno));
        return;
    }

    reset_visits();

    rc = nftw(ROOT, record, 16, 0);

    if (overflowed) {
        FAIL(name, "more than %d visits: the traversal went round in circles",
             MAX_VISITS);
        goto done;
    }
    if (rc != 0) {
        FAIL(name, "nftw() returned %d (errno=%s)", rc, strerror(errno));
        goto done;
    }

    PASS(name);
done:
    remove_tree(ROOT "/sub/loop");
}

/* ------------------------------------------------------------------ test 9 */
static void
test_missing_path(void) {
    const char *name = "missing path gives -1 / ENOENT";
    int rc;

    reset_visits();
    errno = 0;

    rc = nftw("nftw_test_no_such_directory", record, 16, 0);

    if (rc != -1) {
        FAIL(name, "nftw() returned %d, expected -1", rc);
        return;
    }
    if (errno != ENOENT) {
        FAIL(name, "errno is %d (%s), expected ENOENT", errno, strerror(errno));
        return;
    }

    PASS(name);
}

/* ----------------------------------------------------------------- test 10 */
static void
test_ftw_visits_each_entry_once(void) {
    const char *name = "ftw() visits every entry exactly once";
    int rc;

    nvisits_ftw = 0;

    rc = ftw(ROOT, record_ftw, 16);
    if (rc != 0) {
        FAIL(name, "ftw() returned %d (errno=%s)", rc, strerror(errno));
        return;
    }
    if (nvisits_ftw != TREE_ENTRIES) {
        FAIL(name, "visited %d entries, expected %d", nvisits_ftw, TREE_ENTRIES);
        return;
    }

    PASS(name);
}

/* ------------------------------------------------------------------ main */
int
main(void) {
    printf("=== POSIX nftw()/ftw() tests ===\n\n");

    if (build_tree() != 0) {
        printf("Cannot build the test tree: %s\n", strerror(errno));
        return 1;
    }

    test_visits_each_entry_once();
    test_preorder();
    test_postorder();
    test_callback_stops_walk();
    test_fd_limit_is_not_a_depth_limit();
    test_base_and_level();
    test_phys_does_not_follow_links();
    test_link_loop_terminates();
    test_missing_path();
    test_ftw_visits_each_entry_once();

    remove_tree(ROOT);

    printf("\n=== Results: %d passed, %d failed ===\n", passed, failed);
    return (failed > 0) ? 1 : 0;
}
