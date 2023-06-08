#ifndef __GETTEXT_H__
#define __GETTEXT_H__

#define MO_MAGIC 0x950412de

struct eval_s {
    unsigned long r;
    unsigned long n;
    int op;
};

struct binding {
    struct binding *next;
    int dirlen;
    volatile int active;
    char *domainname;
    char *dirname;
    char buf[];
};

#endif