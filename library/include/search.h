#ifndef _SEARCH_H
#define _SEARCH_H

#include <features.h>

#include <stddef.h>
#include <stdint.h>

typedef struct entry
{
    char *key;
    void *data;
} ENTRY;

typedef enum
{
    FIND,
    ENTER
} ACTION;

typedef enum
{
    preorder,
    postorder,
    endorder,
    leaf
} VISIT;

#ifdef _SEARCH_PRIVATE
typedef struct node
{
    char *key;
    struct node *llink, *rlink;
} node_t;
#endif

struct hsearch_data
{
    struct internal_head *htable;
    size_t htablesize;
};

__BEGIN_DECLS

extern int hcreate(size_t);
extern void hdestroy(void);
extern ENTRY *hsearch(ENTRY, ACTION);
extern int hcreate_r(size_t, struct hsearch_data *);
extern void hdestroy_r(struct hsearch_data *);
extern int hsearch_r(ENTRY, ACTION, ENTRY **, struct hsearch_data *);

extern void *lfind(const void *key, const void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));
extern void *lsearch(const void *key, void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));
extern void *tdelete(const void *vkey, void **vrootp, int (*compar)(const void *, const void *));
extern void tdestroy(void *vrootp, void (*freefct)(void *));
extern void *tfind(const void *, void **, int (*)(const void *, const void *));
extern void *tsearch(const void *, void **, int (*)(const void *, const void *));
extern void twalk(const void *root, void (*action)(const void *, VISIT, int));

__END_DECLS

#endif