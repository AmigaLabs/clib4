/*
 * $Id: unistd_spawnvpe.c,v 1.0 2024-06-20 18:26:47 clib4devs Exp $
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

int
spawnvpe(
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
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len = 0;
    struct _clib4 *__clib4 = __CLIB4;
    struct Task *me = (struct Task *) __clib4->self;
    BPTR seglist;
    char **saved_env = NULL;
    int env_count = 0;
    BOOL env_modified = FALSE;

    __set_errno(0);

    D(("Starting new process [%s]\n", name));

    /* Check if the file is a symbolic link and resolve it BEFORE translation */
    char resolved_path[PATH_MAX];
    ssize_t link_len = readlink(name, resolved_path, sizeof(resolved_path) - 1);
    if (link_len > 0) {
        /* It's a symbolic link, use the resolved path */
        resolved_path[link_len] = '\0';
        D(("Resolved symbolic link: [%s] -> [%s]\n", name, resolved_path));
        name = resolved_path;
    } else {
        /* Not a symbolic link or readlink failed, use original path */
        D(("Not a symbolic link or readlink failed (errno=%d): using original path\n", errno));
    }

    /* Now translate the path (either original or resolved) from Unix to Amiga format */
    int error = __translate_unix_to_amiga_path_name(&name, &nti_name);
    if (error) {
        __set_errno(EINVAL);
        D(("__translate_unix_to_amiga_path_name failed: %s\n", strerror(error)));
        return ret;
    }

    D(("name after conversion: [%s]\n", name));

    if (cwd) {
        error = __translate_unix_to_amiga_path_name(&cwd, &nti_cwd);
        if (error) {
            __set_errno(EINVAL);
            D(("__translate_unix_to_amiga_path_name failed: %s\n", strerror(error)));
            return ret;
        }
    }

    D(("cwd after conversion: [%s]\n", cwd));

    BPTR cwdLock = cwd ? Lock(cwd, SHARED_LOCK) : 0; //DupLock(GetCurrentDir());

    parameter_string_len = get_arg_string_length((char *const *) argv);

    D(("parameter_string_len: [%ld]\n", parameter_string_len));

    arg_string = malloc(parameter_string_len + 1);
    if (arg_string == NULL) {
        __set_errno(ENOMEM);
        if (cwdLock != BZERO)
            UnLock(cwdLock);
        return ret;
    }

    if (parameter_string_len > 0) {
        build_arg_string((char *const *) argv, &arg_string[arg_string_len]);
        arg_string_len += parameter_string_len;
    }

    D(("arg_string_len: [%ld]\n", arg_string_len));

    /* Add a NUL, to be nice... */
    arg_string[arg_string_len] = '\0';

    D(("arg_string: [%s]\n", arg_string));

    int full_command_len = strlen(name) + 1 + arg_string_len + 1; // '\0'
    char *full_command = (char*)malloc(full_command_len);
    char process_name[32] = {0};
    snprintf(full_command, full_command_len, "%s %s", name, arg_string);
    snprintf(process_name, NAMELEN - 1, "Spawned Process #%d", __clib4->__children);

    D(("Command to execute: [%s]\n", full_command));

    if (fhin >= 0) {
        err = __get_default_file(fhin, &fh);
        if (err) {
            __set_errno(EBADF);
            if (cwdLock != BZERO)
                UnLock(cwdLock);
            free(arg_string);
            free(full_command);
            return ret;
        }
        iofh[0] = DupFileHandle(fh); // This will be closed by ST/CNPT
    }
    else {
        iofh[0] = Open("NIL:", MODE_OLDFILE);
    }

    if (fhout >= 0) {
        err = __get_default_file(fhout, &fh);
        if (err) {
            __set_errno(EBADF);
            if (cwdLock != BZERO)
                UnLock(cwdLock);
            free(arg_string);
            free(full_command);
            Close(iofh[0]);
            return ret;
        }
        iofh[1] = DupFileHandle(fh); // This will be closed by ST/CNPT
    }
    else {
        iofh[1] = Open("NIL:", MODE_OLDFILE);
    }

    if (fherr >= 0) {
        err = __get_default_file(fherr, &fh);
        if (err) {
            __set_errno(EBADF);
            if (cwdLock != BZERO)
                UnLock(cwdLock);
            free(arg_string);
            free(full_command);
            Close(iofh[0]);
            Close(iofh[1]);
            return ret;
        }
        iofh[2] = DupFileHandle(fh); // This will be closed by ST/CNPT
    }
    else {
        iofh[2] = Open("NIL:", MODE_OLDFILE);
    }

	/* If deltaenv is provided, temporarily set environment variables
	* so that NP_CopyVars (default TRUE) will copy them to the child process */
	if (deltaenv != NULL) {
		D(("Setting environment variables from deltaenv\n"));

		/* Count environment variables */
		while (deltaenv[env_count] != NULL) {
			env_count++;
		}

		if (env_count > 0) {
			/* Allocate space to save old environment values */
			saved_env = (char **) malloc(env_count * sizeof(char *));
			if (saved_env != NULL) {
				/* Set each environment variable, saving old values */
				for (int i = 0; i < env_count; i++) {
					saved_env[i] = NULL;

					char *env_copy = strdup(deltaenv[i]);
					if (env_copy) {
						char *eq = strchr(env_copy, '=');
						if (eq) {
							*eq = '\0';
							char *name = env_copy;
							char *value = eq + 1;

							/* Save old value if it exists */
							char *old_value = getenv(name);
							if (old_value) {
								saved_env[i] = strdup(old_value);
							}

                            D(("Setting env: %s=%s\n", name, value));
							SetVar(name, value, -1, GVF_LOCAL_ONLY);
							env_modified = TRUE;
						}
						free(env_copy);
                        env_copy = NULL;
					}
				}
			}
		}
	}


    D(("(*)Calling SystemTags.\n"));

	struct spawnData data = { getgid(), FindTask(NULL), "" };
	if (__CLIB4->uuid) strncpy(data.parentUuid, __CLIB4->uuid, UUID4_LEN);
    ret = SystemTags(full_command,
                    NP_NotifyOnDeathSigTask, me,
                    SYS_Input,          iofh[0],
                    SYS_Output,         iofh[1],
                    SYS_Error,          iofh[2],
                    NP_CloseError,      TRUE,
                    SYS_UserShell,      TRUE,
                    SYS_Asynch,         TRUE,
                    NP_Child,           TRUE,
                    NP_StackSize,       2024 * 1024,
                    cwdLock ? NP_CurrentDir : TAG_SKIP, cwdLock,
                    NP_Name,            process_name,
                    NP_EntryCode,       spawnedProcessEnter,
                    NP_EntryData,       &data,
                    NP_ExitCode,        spawnedProcessExit,
                    NP_CopyVars,        TRUE,
                    TAG_DONE);

    if (ret != 0) {
        D(("System/CreateNewProc failed. Return value: [%ld]\n", ret));

        __set_errno(__translate_io_error_to_errno(IoErr()));

        /* SystemTags failed. Clean up file handles */
        for (int i = 0; i < 3; i++) {
            Close(iofh[i]);
        }
    }
    else {
        D(("System/CreateNewProc succeeded. Return value: [%ld]\n", ret));

        __clib4->__children++;

        /*
         * If mode is set as P_NOWAIT we can retrieve process id calling IoErr()
         * just after SystemTags. In this case spawnv will return pid
         * IoErr() must be called IMMEDIATELY after SystemTags() == no other DOS calls inbetween
         */

        pid_t pid = IoErr();
        ret = pid;
    }

	/* Restore environment variables if we modified them */
	if (env_modified && saved_env != NULL) {
		D(("Restoring environment variables\n"));

		for (int i = 0; i < env_count; i++) {
			if (deltaenv[i] != NULL) {
				char *env_copy = strdup(deltaenv[i]);
				if (env_copy) {
					char *eq = strchr(env_copy, '=');
					if (eq) {
						*eq = '\0';
						char *name = env_copy;

						DeleteVar(name, GVF_LOCAL_ONLY);
						if (saved_env[i] != NULL) {
							free(saved_env[i]);
                            saved_env[i] = NULL;
						}
					}
                    free(env_copy);
                    env_copy = NULL;
                }
            }
        }
		free(saved_env);
        saved_env = NULL;
	}

    free(full_command);
    full_command = NULL;

    D(("System/CreateNewProc completed. Return value: [%ld]\n", ret));

    return ret;
}
