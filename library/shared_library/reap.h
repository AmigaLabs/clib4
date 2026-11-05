/*
 * reap.h - what the rest of the library says to the part that deals with a program
 * closed from outside. See reap.c.
 */
#ifndef _REAP_H
#define _REAP_H

#include <exec/types.h>
#include <dos/dos.h>

struct _clib4;
struct Clib4Resource;

void __reap_init(void);
BOOL __reap_stop(void);

void __reap_program_add(struct _clib4 *__clib4);
void __reap_program_remove(struct _clib4 *__clib4);

BOOL __reap_track_file(struct _clib4 *__clib4, BPTR file);
BOOL __reap_track_lock(struct _clib4 *__clib4, BPTR lock);
BOOL __reap_track_dir(struct _clib4 *__clib4, APTR context);
void __reap_untracked(struct _clib4 *__clib4);

void __shm_detach_all(struct Clib4Resource *res, struct _clib4 *__clib4);

#endif /* _REAP_H */
