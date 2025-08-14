/*
 * $Id: stdlib_setenv.c,v 1.12 2006-09-25 14:12:15 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

struct LocalVariable {
	struct LocalVariable *lv_Next;
	char *lv_Name;
};

CLIB_DESTRUCTOR(__setenv_exit) {
	ENTER();
	struct _clib4 *__clib4 = __CLIB4;
	/* Now for the local variables that may still be set. */
	if (__clib4->__lv_root != NULL) {
		do {
			D(("deleting variable '%s'", __clib4->__lv_root->lv_Name));
			DeleteVar(__clib4->__lv_root->lv_Name, 0);
		}
		while ((__clib4->__lv_root = __clib4->__lv_root->lv_Next) != NULL);
	}
	LEAVE();
}

int
setenv(const char *original_name, const char *original_value, int overwrite) {
	const char *name = original_name;
	const char *value = original_value;
	int result = ERROR;
	struct _clib4 *__clib4 = __CLIB4;

	/* Only deal with a pointer to environ, to work around subtle bugs with shared
		libraries and/or small data systems where the user declares his own 'environ'.  */
	char ***p_environ = &environ;
	static int alloced; /* if allocated space before */
	register char *C;
	int l_value, offset;

	ENTER();

	SHOWSTRING(original_name);
	SHOWSTRING(original_value);
	SHOWVALUE(overwrite);

	assert(original_name != NULL || original_value != NULL);

	if (original_name == NULL && original_value == NULL) {
		SHOWMSG("invalid parameters");
		__set_errno_r(__clib4, EFAULT);
		goto out;
	}

	MutexObtain(__clib4->__environment_lock);

	if (*value == '=') /* no `=' in value */
		++value;

	l_value = strlen(value);
	if ((C = getenv_r(__clib4, name, &offset))) {
		/* find if already exists */
		if (!overwrite) {
			MutexRelease(__clib4->__environment_lock);
			return 0;
		}
		if (strlen(C) >= (size_t) l_value) {
			/* old larger; copy over */
			while ((*C++ = *value++) != 0);
			MutexRelease(__clib4->__environment_lock);
			/* if we are changing the TZ environment variable, update timezone info */
			if (strcmp(name, "TZ") == 0)
				tzset();
			return 0;
		}
	}
	else {
		/* create new slot */
		register int cnt;
		register char **P;
		for (P = *p_environ, cnt = 0; *P; ++P, ++cnt)
			;
		if (alloced) {
			/* just increase size */
			*p_environ = (char**) realloc((char*) environ, (sizeof(char*) * (cnt + 2)));
			if (!*p_environ) {
				MutexRelease(__clib4->__environment_lock);
				return -1;
			}
		}
		else {
			/* get new space */
			alloced = 1; /* copy old entries into it */
			P = (char**) malloc((sizeof(char*) * (cnt + 2)));
			if (!P) {
				MutexRelease(__clib4->__environment_lock);
				return (-1);
			}
			bcopy((char*) *p_environ, (char*) P, cnt * sizeof(char*));
			*p_environ = P;
		}
		(*p_environ)[cnt + 1] = NULL;
		offset = cnt;
	}

	for (C = (char*) name; *C && *C != '='; ++C)
		; /* no `=' in name */

	if (!((*p_environ)[offset] = malloc((size_t) ((C - name) + l_value + 2)))) { /* name + `=' + value */
		MutexRelease(__clib4->__environment_lock);
		return -1;
	}

	for (C = (*p_environ)[offset]; ((*C = *name++)) && *C != '='; ++C)
		;
	for (*C++ = '='; (*C++ = *value++) != 0;)
		;
	MutexRelease(__clib4->__environment_lock);

	SHOWMSG("Variable set correctly");

	if (strcmp(name, "TZ") == 0) {
		SHOWMSG("Set TZ");
		tzset();
	}

	result = OK;

out:
	RETURN(result);
	return (result);
}
