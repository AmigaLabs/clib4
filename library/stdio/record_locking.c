/*
 * $Id: stdio_record_locking.c,v 1.21 2024-07-06 14:39:23 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <strings.h>
#include <limits.h>

/* System-wide global data structure all programs which use advisory file record
 * locking with this runtime library will use.
 */
struct FileLockSemaphore {
    struct SignalSemaphore fls_Semaphore; /* Standard signal semaphore part */
    UWORD fls_Size;                          /* Size of this data structure (may grow) */
    struct MinList fls_LockList;          /* List of lock nodes */
};

/* Each file which has regions locked is registered through the following
 * data structure.
 */
struct FileLockNode {
    struct MinNode fln_MinNode;             /* Standard node */
    struct MinList fln_LockedRegionList; /* List of locked regions */

    BPTR fln_FileParentDir; /* Refers to the file's parent directory */
    char fln_FileName[1];    /* Name of the file; together with the
											 * parent directory, this should uniquely
											 * identify the file.
											 */
};

/* A single locked region, as associated with a file. */
struct LockedRegionNode {
    struct MinNode lrn_MinNode; /* Standard node */
    _off64_t lrn_Start;            /* Where the region begins */
    _off64_t lrn_Stop;            /* Where the region ends (inclusive) */
    _off64_t lrn_Length;        /* Original length requested */
    pid_t lrn_Owner;            /* Which process owns the region */
    BOOL lrn_Shared;            /* Whether or not this region has been locked for shared access. */
};

static struct FileLockSemaphore *FileLockSemaphore;
static void release_file_lock_semaphore(struct FileLockSemaphore *fls);
static struct FileLockSemaphore *obtain_file_lock_semaphore(struct _clib4 *__clib4, BOOL shared);
static void remove_locked_region_node(struct FileLockSemaphore *fls, struct fd *fd, _off64_t start, _off64_t stop, _off64_t original_length);
static void delete_locked_region_node(struct LockedRegionNode *lrn);
static long create_locked_region_node(struct LockedRegionNode **result_ptr);
static void delete_file_lock_node(struct FileLockNode *fln);
static long create_file_lock_node(struct fd *fd, struct FileLockNode **result_ptr);
static long find_file_lock_node_by_file_handle(struct FileLockSemaphore *fls, BPTR file_handle, struct FileLockNode **result_ptr);
static long find_file_lock_node_by_drawer_and_name(struct FileLockSemaphore *fls, BPTR dir_lock, STRPTR file_name, struct FileLockNode **result_ptr);
static struct LockedRegionNode *find_colliding_region(struct FileLockNode *fln, _off64_t start, _off64_t stop, BOOL shared);
static void cleanup_locked_records(struct _clib4 *__clib4, struct fd *fd);

static void release_file_lock_semaphore(struct FileLockSemaphore *fls) {
    ENTER();

    SHOWPOINTER(fls);

    if (fls != NULL)
        ReleaseSemaphore((struct SignalSemaphore *) fls);

    LEAVE();
}

static struct FileLockSemaphore *
obtain_file_lock_semaphore(struct _clib4 *__clib4, BOOL shared) {
    struct FileLockSemaphore *result = NULL;

    ENTER();

    if (FileLockSemaphore == NULL && __clib4->__file_lock_semaphore_name != NULL && __clib4->__file_lock_semaphore_name[0] != '\0') {
        struct FileLockSemaphore *fls = NULL;
        char *semaphore_name_copy = NULL;

        /* We allocate the new semaphore first, so that we don't spend
           any time in Forbid() allocating memory. */
        semaphore_name_copy = AllocVecTags(strlen(__clib4->__file_lock_semaphore_name) + 1, AVT_Type, MEMF_SHARED, TAG_DONE);
        if (semaphore_name_copy != NULL) {
            strcpy(semaphore_name_copy, __clib4->__file_lock_semaphore_name);

            fls = AllocSysObjectTags(ASOT_SEMAPHORE,
                                     ASOSEM_Size, sizeof(*fls),
                                     ASOSEM_Name, semaphore_name_copy,
                                     ASOSEM_Public, FALSE, /* we want to add it ourselves */
                                     ASOSEM_Pri, 1,
                                     TAG_END);
        }

        SHOWMSG("try to find the locking semaphore");

        Forbid();

        FileLockSemaphore = (struct FileLockSemaphore *) FindSemaphore((STRPTR) __clib4->__file_lock_semaphore_name);
        if (FileLockSemaphore == NULL) {
            SHOWMSG("didn't find it; we're going to add our own");

            if (fls != NULL) {
                SHOWMSG("adding our own semaphore");

                fls->fls_Size = sizeof(*fls);
                NewList((struct List *) &fls->fls_LockList);

                AddSemaphore(&fls->fls_Semaphore);

                FileLockSemaphore = fls;
                fls = NULL;

                semaphore_name_copy = NULL;
            } else {
                SHOWMSG("not enough memory");
            }
        }

        Permit();

        if (FileLockSemaphore != NULL && FileLockSemaphore->fls_Size < sizeof(*FileLockSemaphore)) {
            SHOWMSG("semaphore found, but it's too short");

            SHOWVALUE(FileLockSemaphore->fls_Size);
            SHOWVALUE(sizeof(*FileLockSemaphore));

            FileLockSemaphore = NULL;
        }

        /* Release the memory allocated for the semaphore, in case
           we didn't need it after all. */
        if (fls != NULL) {
            FreeVec(semaphore_name_copy);
            FreeSysObject(ASOT_SEMAPHORE, fls);
        }
    }

    if (FileLockSemaphore != NULL) {
        SHOWMSG("got a semaphore, using it...");

        if (shared) {
            ObtainSemaphoreShared((struct SignalSemaphore *) FileLockSemaphore);
        } else {
            ObtainSemaphore((struct SignalSemaphore *) FileLockSemaphore);
        }

        result = FileLockSemaphore;
    } else {
        SHOWMSG("didn't get the semaphore");
    }

    RETURN(result);
    return (result);
}

static void
remove_locked_region_node(struct FileLockSemaphore *fls, struct fd *fd, _off64_t start, _off64_t UNUSED stop, _off64_t original_length) {
    ENTER();

    assert(fls != NULL && fd != NULL);

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_LOCKED)) {
        struct FileLockNode *which_lock = NULL;

        SHOWMSG("found a locked file");

        /* Find the locked file this descriptor
         * buffer belongs to.
         */
        if (find_file_lock_node_by_file_handle(fls, fd->fd_File, &which_lock) == OK) {
            struct LockedRegionNode *lrn;
            struct LockedRegionNode *lrn_next;
            pid_t this_task = getpid();

            assert(which_lock != NULL);

            D(("trying to unlock the region %ld..%ld", start, stop));

            /* Find the region to unlock and remove it. */
            for (lrn = (struct LockedRegionNode *) which_lock->fln_LockedRegionList.mlh_Head;
                 (lrn_next = (struct LockedRegionNode *) lrn->lrn_MinNode.mln_Succ) != NULL;
                 lrn = lrn_next) {
                if (lrn->lrn_Owner == this_task &&
                    lrn->lrn_Start == start &&
                    lrn->lrn_Length == original_length) {
                    SHOWMSG("unlocking all regions on this file");

                    Remove((struct Node *) lrn);
                    delete_locked_region_node(lrn);
                }
            }

            /* Check if there are any locked regions left.
             * If not, mark the entire file as unlocked.
             */
            if (IsMinListEmpty(&which_lock->fln_LockedRegionList)) {
                SHOWMSG("no more regions are locked; removing the file lock node");

                Remove((struct Node *) which_lock);
                delete_file_lock_node(which_lock);

                /* If this is an alias, move up to the real thing. */
                if (fd->fd_Original != NULL)
                    fd = fd->fd_Original;

                do {
                    CLEAR_FLAG(fd->fd_Flags, FDF_IS_LOCKED);
                } while ((fd = fd->fd_NextAlias) != NULL);
            }
        }
    }

    LEAVE();
}

static void
delete_locked_region_node(struct LockedRegionNode *lrn) {
    ENTER();

    SHOWPOINTER(lrn);

    if (lrn) {
        FreeVec(lrn);
        lrn = NULL;
    }

    LEAVE();
}

static long
create_locked_region_node(struct LockedRegionNode **result_ptr) {
    struct LockedRegionNode *lrn;
    LONG error = OK;

    ENTER();

    assert(result_ptr != NULL);

    lrn = AllocVecTags(sizeof(*lrn), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);
    if (lrn == NULL) {
        SHOWMSG("not enough memory for locked region node");

        error = ERROR_NO_FREE_STORE;
        goto out;
    }

    lrn->lrn_Owner = getpid();

out:

    (*result_ptr) = lrn;

    RETURN(error);
    return (error);
}

static void
delete_file_lock_node(struct FileLockNode *fln) {
    ENTER();

    SHOWPOINTER(fln);

    if (fln != NULL) {
        UnLock(fln->fln_FileParentDir);
        FreeVec(fln);
        fln = NULL;
    }

    LEAVE();
}

static long
create_file_lock_node(struct fd *fd, struct FileLockNode **result_ptr) {
    struct FileLockNode *result = NULL;
    struct FileLockNode *fln = NULL;
    struct ExamineData *fib;
    LONG error = OK;

    ENTER();

    assert(fd != NULL && result_ptr != NULL);

    /* We store a lock on the file's parent directory
     * and the name of the file for later use in
     * comparisons.
     */
    fib = ExamineObjectTags(EX_FileHandleInput, fd->fd_File, TAG_DONE);
    if (fib == NULL) {
        SHOWMSG("couldn't examine file handle");

        error = IoErr();
        goto out;
    }

    fln = AllocVecTags(sizeof(*fln) + strlen(fib->Name), AVT_Type, MEMF_SHARED, AVT_ClearWithValue, 0, TAG_DONE);
    if (fln == NULL) {
        SHOWMSG("not enough memory for lock node");

        error = ERROR_NO_FREE_STORE;
        goto out;
    }

    fln->fln_FileParentDir = ParentOfFH(fd->fd_File);
    if (fln->fln_FileParentDir == BZERO) {
        SHOWMSG("couldn't get parent directory");

        error = IoErr();
        goto out;
    }

    strcpy(fln->fln_FileName, fib->Name);

    NewList((struct List *) &fln->fln_LockedRegionList);

    result = fln;
    fln = NULL;

out:
    FreeDosObject(DOS_EXAMINEDATA, fib);
    delete_file_lock_node(fln);

    (*result_ptr) = result;

    RETURN(error);
    return (error);
}

static long
find_file_lock_node_by_drawer_and_name(struct FileLockSemaphore *fls, BPTR dir_lock, STRPTR file_name, struct FileLockNode **result_ptr) {
    DECLARE_UTILITYBASE();
    struct FileLockNode *result = NULL;
    struct FileLockNode *fln;
    LONG status;
    LONG error;

    ENTER();

    assert(fls != NULL && dir_lock != BZERO && file_name != NULL && result_ptr != NULL);
    assert(UtilityBase != NULL);

#if DEBUG
    {
        char name[FILENAME_MAX];

        if (NameFromLock(dir_lock, name, sizeof(name)))
        {
            if (AddPart(name, file_name, sizeof(name)))
                D(("Looking for a lock on file |%s|", name));
        }
    }
#endif /* DEBUG */

    error = ERROR_OBJECT_NOT_FOUND;

    for (fln = (struct FileLockNode *) fls->fls_LockList.mlh_Head;
         fln->fln_MinNode.mln_Succ != NULL;
         fln = (struct FileLockNode *) fln->fln_MinNode.mln_Succ) {
        status = SameLock(fln->fln_FileParentDir, dir_lock);
        if (status == LOCK_SAME) {
            if (Stricmp((STRPTR) fln->fln_FileName, file_name) == SAME) {
                error = OK;

                result = fln;

                break;
            }
        }
    }

    if (result != NULL)
        SHOWMSG("found one");
    else
        SHOWMSG("didn't find one");

    (*result_ptr) = result;

    RETURN(error);
    return (error);
}

static long
find_file_lock_node_by_file_handle(struct FileLockSemaphore *fls, BPTR file_handle, struct FileLockNode **result_ptr) {
    struct ExamineData *this_fib;
    BPTR parent_dir = BZERO;
    LONG error;

    ENTER();

    assert(fls != NULL && file_handle != BZERO && result_ptr != NULL);

    (*result_ptr) = NULL;

    this_fib = ExamineObjectTags(EX_FileHandleInput, file_handle, TAG_DONE);
    if (this_fib == NULL) {
        SHOWMSG("couldn't examine file handle");

        error = IoErr();
        goto out;
    }

    /* Determine the file's parent directory and
     * name. These will be compared against the
     * global file lock data.
     */
    parent_dir = ParentOfFH(file_handle);
    if (parent_dir == BZERO) {
        SHOWMSG("couldn't get parent directory");

        error = IoErr();
        goto out;
    }

    error = find_file_lock_node_by_drawer_and_name(fls, parent_dir, this_fib->Name, result_ptr);
    if (error != OK) {
        SHOWMSG("couldn't find a lock node for the file");
        goto out;
    }

out:

    FreeDosObject(DOS_EXAMINEDATA, this_fib);
    UnLock(parent_dir);

    RETURN(error);
    return (error);
}

static struct LockedRegionNode *
find_colliding_region(struct FileLockNode *fln, _off64_t start, _off64_t stop, BOOL shared) {
    struct LockedRegionNode *result = NULL;
    struct LockedRegionNode *lrn;
    pid_t this_task = getpid();

    ENTER();

    assert(fln != NULL && start <= stop);

    SHOWVALUE(start);
    SHOWVALUE(stop);
    SHOWVALUE(shared);

    /* This routine looks for a locked region that overlaps
     * with the specified region. It returns a pointer to the
     * region that would collide with the specified region if
     * a new lock were to be added.
     */
    for (lrn = (struct LockedRegionNode *) fln->fln_LockedRegionList.mlh_Head;
         lrn->lrn_MinNode.mln_Succ != NULL;
         lrn = (struct LockedRegionNode *) lrn->lrn_MinNode.mln_Succ) {
        /* Do the regions overlap? */
        if (lrn->lrn_Start <= stop && start <= lrn->lrn_Stop) {
            /* Two shared regions may always overlap.
             * How about the rest?
             */
            if (NOT shared || NOT
            lrn->lrn_Shared)
            {
                /* The lock owner may add as many exclusive
                 * or shared locks to the same region as
                 * necessary.
                 */
                if (lrn->lrn_Owner == this_task) {
                    /*kprintf("lock collision [%ld..%ld] : [%ld..%ld]\n",start,stop,lrn->lrn_Start,lrn->lrn_Stop);*/
                    continue;
                }

                /* So we found a region that would
                 * cause a collision.
                 */
                result = lrn;
                break;
            }
        }
    }

    RETURN(result);
    return (result);
}


static void
cleanup_locked_records(struct _clib4 *__clib4, struct fd *fd) {
    ENTER();

    assert(fd != NULL);

    /* This routine removes all locked regions from a file
     * before it is eventually closed.
     */

    if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_LOCKED)) {
        struct FileLockSemaphore *fls;

        fls = obtain_file_lock_semaphore(__clib4, FALSE);
        if (fls != NULL) {
            BPTR file_handle = fd->fd_File;
            struct FileLockNode *which_lock = NULL;
            pid_t this_task = getpid();
            LONG error;

            error = find_file_lock_node_by_file_handle(fls, file_handle, &which_lock);
            if (error == OK) {
                struct LockedRegionNode *lrn_this;
                struct LockedRegionNode *lrn_next;

                assert(which_lock != NULL);

                SHOWMSG("unlocking all regions on this file");

                for (lrn_this = (struct LockedRegionNode *) which_lock->fln_LockedRegionList.mlh_Head;
                     (lrn_next = (struct LockedRegionNode *) lrn_this->lrn_MinNode.mln_Succ) != NULL;
                     lrn_this = lrn_next) {
                    if (lrn_this->lrn_Owner == this_task) {
                        Remove((struct Node *) lrn_this);

                        delete_locked_region_node(lrn_this);
                    }
                }

                if (IsMinListEmpty(&which_lock->fln_LockedRegionList)) {
                    SHOWMSG("no more regions are locked; removing the file lock node");

                    Remove((struct Node *) which_lock);

                    delete_file_lock_node(which_lock);
                }
            }

            if (error == OK || error == ERROR_OBJECT_NOT_FOUND) {
                /* If this is an alias, move up to the real thing. */
                if (fd->fd_Original != NULL)
                    fd = fd->fd_Original;

                do {
                    CLEAR_FLAG(fd->fd_Flags, FDF_IS_LOCKED);
                } while ((fd = fd->fd_NextAlias) != NULL);
            }

            release_file_lock_semaphore(fls);
        }
    }

    LEAVE();
}

int
__handle_record_locking(struct _clib4 *__clib4, int cmd, struct flock *l, struct fd *fd, int *error_ptr) {
    struct FileLockSemaphore *fls = NULL;
    BPTR file_handle = fd->fd_File;
    struct LockedRegionNode *lrn = NULL;
    struct FileLockNode *fln = NULL;
    struct ExamineData *fib = NULL;
    BOOL fib_is_valid = FALSE;
    BPTR parent_dir = BZERO;
    _off64_t seek_position;
    _off64_t current_position;
    int result = ERROR;
    _off64_t original_len;
    LONG error = OK;
    _off64_t start = 0;
    _off64_t len = 0;
    _off64_t stop;

    ENTER();

    /* This routine implements advisory file segment locking
     * similar to 4.4BSD, but not quite the same. The functionality
     * is a subset, somewhat similar to the functionality offered
     * by the AmigaDOS LockRecord() and UnlockRecord() functions.
     * This means for example that every unlock request must
     * match the size and position of the corresponding locking
     * request.
     *
     * This implementation was chosen because not every Amiga
     * filing system implements record locking and Samba
     * absolutely requires this functionality to work.
     */
    assert(l != NULL && fd != NULL);
    assert(F_RDLCK <= l->l_type && l->l_type <= F_WRLCK);
    assert(SEEK_SET <= l->l_whence && l->l_whence <= SEEK_END);
    assert(error_ptr != NULL);
    assert(FLAG_IS_CLEAR(fd->fd_Flags, FDF_STDIO));

    /* Remember to unlock any records before closing the file. */
    fd->fd_Cleanup = cleanup_locked_records;

    if ((cmd == F_SETLK || cmd == F_SETLKW) && (l->l_type != F_UNLCK)) {
        SHOWMSG("this is a lock request");

        error = create_file_lock_node(fd, &fln);
        if (error != OK) {
            SHOWMSG("could not create lock node");
            goto out;
        }

        error = create_locked_region_node(&lrn);
        if (error != OK) {
            SHOWMSG("could not create region node");
            goto out;
        }
    } else {
        SHOWMSG("this is not a lock request");
        goto out;
    }

    original_len = l->l_len;

    /* Now calculate the position of the first byte to lock and the number
     * of bytes to lock.
     */
    switch (l->l_whence) {
        case SEEK_SET:

            SHOWMSG("SEEK_SET");

            start = l->l_start;

            if (l->l_len == 0)
                len = LONG_MAX;
            else
                len = l->l_len;

            SHOWVALUE(start);
            SHOWVALUE(len);

            break;

        case SEEK_CUR:

            SHOWMSG("SEEK_CUR");

            seek_position = GetFilePosition(file_handle);
            if (seek_position == GETPOSITION_ERROR && IoErr() != OK) {
                SHOWMSG("could not obtain current seek position");

                error = IoErr();
                goto out;
            }

            current_position = (_off64_t) seek_position;

            start = current_position + l->l_start;

            if (l->l_len == 0)
                len = LONG_MAX;
            else
                len = l->l_len;

            SHOWVALUE(start);
            SHOWVALUE(len);

            break;

        case SEEK_END:

            SHOWMSG("SEEK_END");
            fib = ExamineObjectTags(EX_FileHandleInput, file_handle, TAG_DONE);
            if (fib == NULL) {
                SHOWMSG("could not examine file");

                error = IoErr();
                goto out;
            }

            fib_is_valid = TRUE;

            start = (_off64_t) fib->FileSize + l->l_start;

            if (l->l_len == 0)
                len = LONG_MAX;
            else
                len = l->l_len;

            SHOWVALUE(start);
            SHOWVALUE(len);

            break;
    }

    SHOWVALUE(start);
    SHOWVALUE(len);

    if (start < 0) {
        SHOWMSG("invalid start");

        (*error_ptr) = EINVAL;
        goto out;
    }

    if (len < 0) {
        start += len;
        if (start < 0) {
            SHOWMSG("invalid start");

            (*error_ptr) = EINVAL;
            goto out;
        }

        stop = start - len - 1;
        if (stop < start)
            stop = LONG_MAX;
    } else {
        stop = start - 1 + len;
        if (stop < start - 1) /* Check for overflow */
            stop = LONG_MAX;
    }

    if (l->l_type == F_UNLCK) {
        SHOWMSG("F_UNLCK");

        fls = obtain_file_lock_semaphore(__clib4, FALSE);
        if (fls == NULL) {
            SHOWMSG("couldn't obtain file locking semaphore");
            (*error_ptr) = EBADF;
            goto out;
        }

        D(("unlocking %ld..%ld", start, stop));

        remove_locked_region_node(fls, fd, start, stop, original_len);
    } else if (cmd == F_SETLKW) {
        struct FileLockNode *existing_fln;
        BOOL locked;
        BOOL shared;

        SHOWMSG("F_SETLKW");

        D(("  locking %ld..%ld", start, stop));
        fib = ExamineObjectTags(EX_FileHandleInput, file_handle, TAG_DONE);
        if (NOT fib_is_valid && fib == NULL)
        {
            SHOWMSG("couldn't read this file's name");

            error = IoErr();
            goto out;
        }

        parent_dir = ParentOfFH(file_handle);
        if (parent_dir == BZERO) {
            SHOWMSG("couldn't get a lock on the file's parent directory");

            error = IoErr();
            goto out;
        }

        shared = (BOOL)(l->l_type == F_RDLCK);

        if (shared)
            D(("this is a shared lock; waiting for completion"));
        else
            D(("this is an exclusive lock; waiting for completion"));

        /* Shared locks require readable files, exclusive locks require
           writable files. */
        if ((shared && FLAG_IS_CLEAR(fd->fd_Flags, FDF_READ)) ||
            (NOT
            shared && FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE)))
        {
            (*error_ptr) = EBADF;
            goto out;
        }

        lrn->lrn_Start = start;
        lrn->lrn_Stop = stop;
        lrn->lrn_Length = original_len;
        lrn->lrn_Shared = shared;

        /* Retry until we manage to lock the record. */
        locked = FALSE;

        do {
            fls = obtain_file_lock_semaphore(__clib4, FALSE);
            if (fls == NULL) {
                SHOWMSG("couldn't obtain file locking semaphore");
                (*error_ptr) = EBADF;
                goto out;
            }

            if (find_file_lock_node_by_drawer_and_name(fls, parent_dir, fib->Name, &existing_fln) == OK) {
                SHOWMSG("that file is already locked by someone");

                if (find_colliding_region(existing_fln, start, stop, shared) == NULL) {
                    SHOWMSG("but the locks don't collide");

                    AddTail((struct List *) &existing_fln->fln_LockedRegionList, (struct Node *) lrn);
                    lrn = NULL;

                    locked = TRUE;
                } else {
                    (*error_ptr) = EWOULDBLOCK;
                    SHOWMSG("and the locks collide");
                }
            } else {
                SHOWMSG("nobody has any locks on this file");

                AddTail((struct List *) &fls->fls_LockList, (struct Node *) fln);
                AddTail((struct List *) &fln->fln_LockedRegionList, (struct Node *) lrn);

                fln = NULL;
                lrn = NULL;

                locked = TRUE;
            }

            release_file_lock_semaphore(fls);
            fls = NULL;

            if (NOT locked) {
                const int rand_max = RAND_MAX / 65536;
                int num_random_ticks;

                if (__clib4->__check_abort_enabled && (SetSignal(0, 0) & __clib4->__break_signal_mask) != 0) {
                    SHOWMSG("lock polling loop stopped");

                    delete_file_lock_node(fln);
                    fln = NULL;

                    delete_locked_region_node(lrn);
                    lrn = NULL;

                    UnLock(parent_dir);

                    parent_dir = BZERO;

                    (*error_ptr) = EINTR;

                    __check_abort_f(__clib4);

                    goto out;
                }

                /* Wait a little before retrying
                 * the locking operation. We add
                 * a little randomness here to
                 * reduce the likelihood of two
                 * competing processes trying to
                 * lock the same file at the
                 * same time.
                 */

                num_random_ticks = ((TICKS_PER_SECOND / 2) * (rand() / 65536)) / rand_max;

                if (num_random_ticks > 0) {
                    Delay(num_random_ticks);
                }
            }
        } while (NOT
        locked);

        SHOWMSG("the file now has a lock set");

        /* If this is an alias, move up to the real thing. */
        if (fd->fd_Original != NULL)
            fd = fd->fd_Original;

        do {
            SET_FLAG(fd->fd_Flags, FDF_IS_LOCKED);
        } while ((fd = fd->fd_NextAlias) != NULL);
    } else if (cmd == F_SETLK) {
        BOOL shared = (BOOL)(l->l_type == F_RDLCK);
        struct FileLockNode *existing_fln = NULL;

        SHOWMSG("F_SETLK");

        if (shared)
            D(("this is a shared lock"));
        else
            D(("this is an exclusive lock"));

        /* Shared locks require readable files, exclusive locks require
           writable files. */
        if ((shared && FLAG_IS_CLEAR(fd->fd_Flags, FDF_READ)) ||
            (NOT
            shared && FLAG_IS_CLEAR(fd->fd_Flags, FDF_WRITE)))
        {
            (*error_ptr) = EBADF;
            goto out;
        }

        lrn->lrn_Start = start;
        lrn->lrn_Stop = stop;
        lrn->lrn_Length = original_len;
        lrn->lrn_Shared = shared;

        fls = obtain_file_lock_semaphore(__clib4, FALSE);
        if (fls == NULL) {
            SHOWMSG("couldn't obtain file locking semaphore");
            (*error_ptr) = EBADF;
            goto out;
        }

        error = find_file_lock_node_by_file_handle(fls, file_handle, &existing_fln);
        if (error == OK) {
            SHOWMSG("that file is already locked by someone else");

            if (find_colliding_region(existing_fln, start, stop, shared) != NULL) {
                SHOWMSG("and the locks collide");

                (*error_ptr) = EACCES;
                goto out;
            }

            SHOWMSG("but the locks don't collide");

            AddTail((struct List *) &existing_fln->fln_LockedRegionList, (struct Node *) lrn);
            lrn = NULL;
        } else {
            if (error != ERROR_OBJECT_NOT_FOUND)
                goto out;

            SHOWMSG("nobody has any locks on this file");

            AddTail((struct List *) &fls->fls_LockList, (struct Node *) fln);
            AddTail((struct List *) &fln->fln_LockedRegionList, (struct Node *) lrn);

            fln = NULL;
            lrn = NULL;
        }

        SHOWMSG("the file now has a lock set");

        /* If this is an alias, move up to the real thing. */
        if (fd->fd_Original != NULL)
            fd = fd->fd_Original;

        do {
            SET_FLAG(fd->fd_Flags, FDF_IS_LOCKED);
        } while ((fd = fd->fd_NextAlias) != NULL);
    } else if (cmd == F_GETLK) {
        struct FileLockNode *fln_found = NULL;

        SHOWMSG("F_GETLK");

        fls = obtain_file_lock_semaphore(__clib4, TRUE);
        if (fls == NULL) {
            SHOWMSG("couldn't obtain file locking semaphore");
            (*error_ptr) = EBADF;
            goto out;
        }

        SHOWMSG("checking for possible lock collision");

        error = find_file_lock_node_by_file_handle(fls, file_handle, &fln_found);
        if (error == OK) {
            BOOL shared = (BOOL)(l->l_type == F_RDLCK);
            struct LockedRegionNode *lrn_found;

            SHOWMSG("somebody has locked this file");

            lrn_found = find_colliding_region(fln_found, start, stop, shared);
            if (lrn_found != NULL) {
                SHOWMSG("there is a possible lock collision");

                l->l_type = (lrn_found->lrn_Shared ? F_RDLCK : F_WRLCK);
                l->l_whence = SEEK_SET;
                l->l_start = lrn_found->lrn_Start;
                l->l_len = lrn_found->lrn_Length;
                l->l_pid = lrn_found->lrn_Owner;

                SHOWVALUE(l->l_type);
                SHOWVALUE(l->l_whence);
                SHOWVALUE(l->l_start);
                SHOWVALUE(l->l_len);
                SHOWVALUE(l->l_pid);
            } else {
                SHOWMSG("there is no lock collision");

                l->l_type = F_UNLCK;
            }
        } else {
            if (error != ERROR_OBJECT_NOT_FOUND)
                goto out;

            SHOWMSG("nobody has locked this file");

            l->l_type = F_UNLCK;
        }
    }

    result = OK;

out:

    delete_file_lock_node(fln);
    delete_locked_region_node(lrn);

    release_file_lock_semaphore(fls);

    FreeDosObject(DOS_EXAMINEDATA, fib);
    UnLock(parent_dir);

    if (result != 0 && error != OK) {
        SetIoErr(error);

        (*error_ptr) = __translate_io_error_to_errno(error);
    }

    RETURN(result);
    return (result);
}
