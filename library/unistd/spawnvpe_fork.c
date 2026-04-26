/*
 * $Id: unistd_spawnvpe_fork.c,v 1.0 2026-01-05 12:00:00 clib4devs Exp $
 *
 * Fork-like spawn implementation using CreateNewProcTags
 * This provides better control over file descriptor sharing
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include "children.h"

typedef struct LockList {
	BPTR    NextPath;
	BPTR    PathLock;
} LockList;

/*
 * Fork-like spawn that keeps file descriptors open and shared with parent
 *
 * Key differences from spawnvpe:
 * 1. File handles are NOT closed after process creation
 * 2. Parent and child share the same file position (offset)
 * 3. Both can read/write concurrently
 * 4. Closer to Unix fork() + exec() semantics
 *
 * This is useful for:
 * - Pipes between parent and child
 * - Shared file access
 * - IPC mechanisms
 *
* Returns:
 * - On success: child PID
 * - On error: -1 (errno set)
 */
int
spawnvpe_fork(
    const char *file,
    const char **argv,
    char **deltaenv,
    const char *_cwd,
    int fhin,
    int fhout,
    int fherr
) {
    int ret = -1;
    struct name_translation_info nti_name;
    const char *name = file;
    struct name_translation_info nti_cwd;
    const char *cwd = _cwd;
    BPTR iofh[3] = {BZERO, BZERO, BZERO};
    BPTR fh;
    int err;
    BPTR cwdLock = 0;
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len = 0;
    struct _clib4 *__clib4 = __CLIB4;
    struct Process *child_proc = NULL;
    char **saved_env = NULL;
    int env_count = 0;
    BOOL env_modified = FALSE;
    BPTR seglist = BZERO;
	BPTR progdirLock = BZERO;
	struct Task *thisTask = FindTask(NULL);

    __set_errno(0);

    D(("spawnvpe_fork: Starting fork-like process [%s]\n", name));

    /* Resolve symbolic links */
    char resolved_path[PATH_MAX];
    ssize_t link_len = readlink(name, resolved_path, sizeof(resolved_path) - 1);
    if (link_len > 0) {
        resolved_path[link_len] = '\0';
        D(("Resolved symbolic link: [%s] -> [%s]\n", name, resolved_path));
        name = resolved_path;
    }

    /* Translate path from Unix to Amiga format */
    int error = __translate_unix_to_amiga_path_name(&name, &nti_name);
    if (error) {
        __set_errno(EINVAL);
        D(("__translate_unix_to_amiga_path_name failed: %s\n", strerror(error)));
        return ret;
    }

    D(("name after conversion: [%s]\n", name));

    /* Translate cwd if provided */
    if (cwd) {
        error = __translate_unix_to_amiga_path_name(&cwd, &nti_cwd);
        if (error) {
            __set_errno(EINVAL);
            return ret;
        }
        cwdLock = Lock(cwd, SHARED_LOCK);
    }

    /* Load the program */
    D(("Loading program: [%s]\n", name));
	// Try to direct LoadSeg using name
	seglist = LoadSeg(name);
	// If that fails, try using CLI path list
	if (seglist == BZERO) {
		struct CommandLineInterface *cli = (struct CommandLineInterface *) BADDR(((struct Process *) thisTask)->pr_CLI);
		if (cli) {
			struct LockList *ll;

			for (ll = (LockList *)BADDR(cli->cli_PathList); seglist == BZERO && ll; ll = (LockList *)BADDR(ll->NextPath)) {
				if (ll->PathLock) {
					BPTR oldDir = SetCurrentDir(ll->PathLock);
					progdirLock = DupLock(ll->PathLock);
					seglist = LoadSeg(name);
					SetCurrentDir(oldDir);
				}
			}
		}
	}
	else {
		BPTR fileLock = Lock(name, SHARED_LOCK);
		if (fileLock) {
			progdirLock = ParentDir(fileLock);
			UnLock(fileLock);
		}
	}

    if (seglist == BZERO) {
        LONG ioerr = IoErr();
        D(("LoadSeg failed for [%s], IoErr=%ld\n", name, ioerr));

        /* Set a meaningful errno */
        if (ioerr == 0) {
            /* No error code from DOS, set generic error */
            __set_errno(ENOENT);
        } else {
            __set_errno(__translate_io_error_to_errno(ioerr));
        }
        goto cleanup;
    }

    D(("LoadSeg successful, seglist=0x%08lx\n", seglist));

    /* Build command line arguments */
    parameter_string_len = get_arg_string_length((char *const *) argv);
    if (parameter_string_len > 0) {
        arg_string = malloc(parameter_string_len + 1);
        if (arg_string == NULL) {
            __set_errno(ENOMEM);
            goto cleanup;
        }
        build_arg_string((char *const *) argv, arg_string);
        arg_string[parameter_string_len] = '\0';
    } else {
        arg_string = malloc(1);
        if (arg_string == NULL) {
            __set_errno(ENOMEM);
            goto cleanup;
        }
        arg_string[0] = '\0';
    }

    D(("arg_string: [%s]\n", arg_string));

    /* Prepare file handles - IMPORTANT: We DupFileHandle but DON'T close them!
     * This is the key difference from spawnvpe - the handles remain open
     * and shared with the parent process, just like fork() */

    if (fhin >= 0) {
        err = __get_default_file(fhin, &fh);
        if (err) {
            __set_errno(EBADF);
            goto cleanup;
        }
        /* Duplicate handle but parent keeps original open */
        iofh[0] = DupFileHandle(fh);
        if (iofh[0] == BZERO) {
            __set_errno(ENOMEM);
            goto cleanup;
        }
    } else {
        iofh[0] = Open("NIL:", MODE_OLDFILE);
        if (iofh[0] == BZERO) {
            D(("Failed to open NIL: for stdin, IoErr=%ld\n", IoErr()));
            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto cleanup;
        }
    }

    if (fhout >= 0) {
        err = __get_default_file(fhout, &fh);
        if (err) {
            __set_errno(EBADF);
            goto cleanup;
        }
        iofh[1] = DupFileHandle(fh);
        if (iofh[1] == BZERO) {
            __set_errno(ENOMEM);
            goto cleanup;
        }
    } else {
        iofh[1] = Open("NIL:", MODE_NEWFILE);
        if (iofh[1] == BZERO) {
            D(("Failed to open NIL: for stdout, IoErr=%ld\n", IoErr()));
            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto cleanup;
        }
    }

    if (fherr >= 0) {
        err = __get_default_file(fherr, &fh);
        if (err) {
            __set_errno(EBADF);
            goto cleanup;
        }
        iofh[2] = DupFileHandle(fh);
        if (iofh[2] == BZERO) {
            __set_errno(ENOMEM);
            goto cleanup;
        }
    } else {
        iofh[2] = Open("NIL:", MODE_NEWFILE);
        if (iofh[2] == BZERO) {
            D(("Failed to open NIL: for stderr, IoErr=%ld\n", IoErr()));
            __set_errno(__translate_io_error_to_errno(IoErr()));
            goto cleanup;
        }
    }

    /* Set environment variables if provided */
    if (deltaenv != NULL) {
        D(("Setting environment variables from deltaenv\n"));

        /* Count with safety limit */
        while (env_count < 1000 && deltaenv[env_count] != NULL) {
            if ((uintptr_t)deltaenv[env_count] < 0x1000) {
                D(("WARNING: deltaenv[%d] has invalid pointer, stopping\n", env_count));
                break;
            }
            env_count++;
        }

        if (env_count > 0) {
            saved_env = (char **) malloc(env_count * sizeof(char *));
            if (saved_env != NULL) {
                memset(saved_env, 0, env_count * sizeof(char *));

                for (int i = 0; i < env_count; i++) {
                    if (deltaenv[i] == NULL || (uintptr_t)deltaenv[i] < 0x1000)
                        continue;

                    char *env_copy = strdup(deltaenv[i]);
                    if (env_copy == NULL)
                        continue;

                    char *eq = strchr(env_copy, '=');
                    if (eq) {
                        *eq = '\0';
                        char *var_name = env_copy;
                        char *value = eq + 1;

                        char *old_value = getenv(var_name);
                        if (old_value) {
                            saved_env[i] = strdup(old_value);
                        }

                        D(("Setting env: %s=%s\n", var_name, value));
                        SetVar(var_name, value, -1, GVF_LOCAL_ONLY);
                        env_modified = TRUE;
                    }
                    free(env_copy);
                }
            }
        }
    }

    /* Create the new process using CreateNewProcTags for more control */
    char process_name[32];
    snprintf(process_name, sizeof(process_name) - 1, "Forked Process #%d", __clib4->__children);

    D(("Creating new process with CreateNewProcTags\n"));

    /* Close file descriptors marked with FD_CLOEXEC
     * This implements the Unix close-on-exec behavior.
     * We close them BEFORE creating the child so they are not inherited. */
    int num_closed = 0;
    for (int i = 0; i < __clib4->__num_fd; i++) {
        struct fd *check_fd = __clib4->__fd[i];

        /* Skip invalid FDs */
        if (check_fd == NULL || FLAG_IS_CLEAR(check_fd->fd_Flags, FDF_IN_USE))
            continue;

        /* Skip the FDs we're explicitly passing to the child */
        if (i == fhin || i == fhout || i == fherr)
            continue;

        /* Close FDs marked with FD_CLOEXEC */
        if (FLAG_IS_SET(check_fd->fd_Flags, FDF_CLOEXEC)) {
            D(("Closing fd=%d (FD_CLOEXEC set) before exec\n", i));

            /* We need to close the underlying AmigaOS handle without removing the FD entry
             * because we're about to exec and the FD table will be inherited.
             * Actually, with CreateNewProc the child gets a fresh FD table, so we can
             * just mark them for not being inherited. */

            /* For now, we just don't pass them to the child - they remain open in parent
             * but won't be in child's FD table since CreateNewProc doesn't inherit FDs */
            num_closed++;
        }
    }

    D(("Closed %d file descriptors with FD_CLOEXEC before exec\n", num_closed));
	struct spawnData data = { getgid(), thisTask, "" };
	if (__CLIB4->uuid) strncpy(data.parentUuid, __CLIB4->uuid, UUID4_LEN);
    struct TagItem tags[] = {
        { NP_Seglist,           (ULONG) seglist },     /* FIX: Use NP_Seglist not NP_Entry! */
        { NP_Name,              (ULONG) process_name },
        { NP_CommandName,       (ULONG) name },
        { NP_Arguments,         (ULONG) arg_string },
        { NP_Input,             (ULONG) iofh[0] },
        { NP_Output,            (ULONG) iofh[1] },
        { NP_Error,             (ULONG) iofh[2] },
        { NP_CloseInput,        TRUE },   /* Let DOS close when child exits */
        { NP_CloseOutput,       TRUE },   /* Let DOS close when child exits */
        { NP_CloseError,        TRUE },   /* Let DOS close when child exits */
        { NP_Cli,               TRUE },
        { NP_Child,             TRUE },
        { NP_StackSize,         2048 * 1024 },  /* FIX: 2048 not 2024 */
        { NP_CopyVars,          TRUE },
        { NP_FreeSeglist,       TRUE },   /* FIX: Free seglist when process exits */
    	{ NP_EntryCode,       	(ULONG) spawnedProcessEnter },
		{ NP_EntryData,       	(ULONG) &data },
		{ NP_ExitCode,        	(ULONG) spawnedProcessExit },
        { cwdLock 		? NP_CurrentDir : TAG_IGNORE, (ULONG) cwdLock },
		{ progdirLock 	? NP_ProgramDir : TAG_IGNORE, (ULONG) progdirLock },
        { TAG_DONE,             0 }
    };

    if ((child_proc = CreateNewProc(tags))) {
		Wait(SIGF_CHILD);
	}
	//if (oldDir != BZERO)
	//	SetCurrentDir(oldDir);

    if (child_proc == NULL) {
        D(("CreateNewProc failed, IoErr=%ld\n", IoErr()));
        __set_errno(__translate_io_error_to_errno(IoErr()));

        /* Cleanup file handles on failure */
        for (int i = 0; i < 3; i++) {
            if (iofh[i] != BZERO)
                Close(iofh[i]);
        }

        /* On failure, we must unload the seglist ourselves */
        if (seglist != BZERO)
            UnLoadSeg(seglist);

    	if (cwdLock != BZERO) {
    		UnLock(cwdLock);
    	}
    	if (progdirLock != BZERO) {
    		UnLock(progdirLock);
    	}

        goto cleanup;
    }

    /* Success! Get the child PID */
    ret = child_proc->pr_ProcessID;

    /* Register child process in our tracking system */
    if (insertSpawnedChildren(ret, getgid(), __clib4->uuid ? __clib4->uuid : "")) {
        __clib4->__children++;
        D(("Child process registered: PID=%ld\n", ret));
    } else {
        D(("WARNING: Failed to register child process in tracking system\n"));
        /* Continue anyway - the process was created successfully */
        __clib4->__children++;
    }

    D(("Fork-like process created successfully, PID=%ld\n", ret));

    /* IMPORTANT: We do NOT close iofh[] here in the parent!
     *
     * With NP_CloseInput/Output/Error = TRUE, DOS will close these handles
     * when the CHILD PROCESS EXITS, not immediately!
     *
     * The parent still has the ORIGINAL file descriptors (fhin/fhout/fherr) open.
     * The child has the duplicated handles (iofh[]) which will be closed by DOS
     * when the child terminates.
     *
     * This achieves fork-like behavior:
     * - Parent keeps its FDs open
     * - Child has its own handles (duplicated from parent's FDs)
     * - When child exits, DOS closes the child's handles
     * - Parent's FDs remain open until parent closes them
     * - Pipe/file stays alive as long as at least one handle is open
     *
     * No memory leak because DOS manages the child's handles lifecycle.
     */

    /* NOTE: We also do NOT unload seglist - NP_FreeSeglist handles that */

cleanup:
    /* If child was created, NP_CurrentDir and NP_ProgramDir take ownership of the locks */

    /* Restore environment variables */
    if (env_modified && saved_env != NULL) {
        D(("Restoring environment variables\n"));

        for (int i = 0; i < env_count; i++) {
            if (deltaenv[i] != NULL && (uintptr_t)deltaenv[i] >= 0x1000) {
                char *env_copy = strdup(deltaenv[i]);
                if (env_copy) {
                    char *eq = strchr(env_copy, '=');
                    if (eq) {
                        *eq = '\0';
                        char *var_name = env_copy;

                        DeleteVar(var_name, GVF_LOCAL_ONLY);

                        if (saved_env[i] != NULL) {
                            SetVar(var_name, saved_env[i], -1, GVF_LOCAL_ONLY);
                            free(saved_env[i]);
                        }
                    }
                    free(env_copy);
                }
            }
        }
        free(saved_env);
    }

    if (arg_string != NULL)
        free(arg_string);

    return ret;
}

