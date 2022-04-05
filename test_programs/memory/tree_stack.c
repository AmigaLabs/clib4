#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>

/*
 * Tree destruction using a stack
 *
 * This source code is licensed under the GPLv3 license.
 *
 * Taken from Forester.
 */

#include <stdlib.h>

int __VERIFIER_nondet_int() {
    return rand() % 10000;
}

int main() {
    srand(time(NULL));

    struct TreeNode {
        struct TreeNode *left;
        struct TreeNode *right;
    };

    struct StackItem {
        struct StackItem *next;
        struct TreeNode *node;
    };

    struct TreeNode *root = malloc(sizeof(*root)), *n;
    root->left = NULL;
    root->right = NULL;

    while (__VERIFIER_nondet_int()) {
        n = root;
        while (n->left && n->right) {
            if (__VERIFIER_nondet_int())
                n = n->left;
            else
                n = n->right;
        }
        if (!n->left && __VERIFIER_nondet_int()) {
            n->left = malloc(sizeof(*n));
            n->left->left = NULL;
            n->left->right = NULL;
        }
        if (!n->right && __VERIFIER_nondet_int()) {
            n->right = malloc(sizeof(*n));
            n->right->left = NULL;
            n->right->right = NULL;
        }
    }

    n = NULL;

    struct StackItem *s = malloc(sizeof(*s)), *st;
    s->next = NULL;
    s->node = root;

    while (s != NULL) {
        st = s;
        s = s->next;
        n = st->node;
        free(st);
        if (n->left) {
            st = malloc(sizeof(*st));
            st->next = s;
            st->node = n->left;
            s = st;
        }
        if (n->right) {
            st = malloc(sizeof(*st));
            st->next = s;
            st->node = n->right;
            s = st;
        }
        free(n);
    }

    return 0;
}