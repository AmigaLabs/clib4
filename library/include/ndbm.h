#ifndef _NDBM_H_
#define _NDBM_H_

#include <features.h>
#include <db.h>

/* Map dbm interface onto db(3). */
#define DBM_RDONLY	O_RDONLY

/* Flags to dbm_store(). */
#define DBM_INSERT      0
#define DBM_REPLACE     1

/*
 * The db(3) support for ndbm always appends this suffix to the
 * file name to avoid overwriting the user's original database.
 */
#define	DBM_SUFFIX	".db"

typedef struct {
	char *dptr;
	int dsize;
} datum;

typedef DB DBM;
#define	dbm_pagfno(a)	DBM_PAGFNO_NOT_AVAILABLE

__BEGIN_DECLS
int	 dbm_clearerr(DBM *);
void dbm_close(DBM *);
int	 dbm_delete(DBM *, datum);
int	 dbm_error(DBM *);
datum dbm_fetch(DBM *, datum);
datum dbm_firstkey(DBM *);
datum dbm_nextkey(DBM *);
DBM	*dbm_open(const char *, int, int);
int	 dbm_store(DBM *, datum, datum, int);
#if __BSD_VISIBLE
int dbm_dirfno(DBM *);
#endif
__END_DECLS

#endif /* !_NDBM_H_ */