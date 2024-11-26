/*
 * $Id: unistd_execve.c,v 1.13 2006-10-02 07:16:06 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <dos/stdio.h>

/* This gets handed around when trying to locate a program or a script
   interpreter which knows how to do the job. */
struct program_info {
    struct DosResidentSeg *resident_command;    /* If not NULL, points to a valid resident command */
    BPTR home_dir;                              /* If not ZERO refers to the directory in which the command to be executed can be found */
    BPTR segment_list;                          /* If not ZERO refers to a command loaded into memory */
    char *program_name;                         /* Points to the name of the command */
    char *interpreter_name;                     /* If not NULL the name of the command interpreter to use */
    char *interpreter_args;                     /* If not NULL these are additional arguments to be passedto the command interpreter */
};

/****************************************************************************/

/* Try to find a resident command by name; returns a pointer to the Segment
   data structure ready to use, or NULL if none could be found */
static struct DosResidentSeg *
find_resident_command(const char *command_name) {
    struct DosResidentSeg *seg;

    /* This must be done under Forbid() since dos.library does not have
       a more sophisticated arbitration method for this yet... */
    Forbid();

    seg = FindSegment((STRPTR) command_name, NULL, FALSE);
    if (seg == NULL)
        seg = FindSegment((STRPTR) command_name, NULL, TRUE);

    if (seg != NULL) {
        /* Check if that's a disabled command or something else. */
        if ((seg->seg_UC < 0) && ((seg->seg_UC > CMD_INTERNAL) || (seg->seg_UC <= CMD_DISABLED))) {
            seg = NULL;
        } else {
            /* Unless it's a built-in command, mark it as having another user. */
            if (seg->seg_UC >= 0)
                seg->seg_UC++;
        }
    }

    Permit();

    return (seg);
}

/* Try to read the first line of a script file */
static int
get_first_script_line(const char *path, char **line_ptr) {
    BPTR script_file;
    int result = -1;
    char *script_line = NULL;
    size_t script_line_size = 0;
    size_t script_line_length = 0;
    LONG c;
    struct _clib4 *__clib4 = __CLIB4;

    (*line_ptr) = NULL;

    script_file = Open((STRPTR) path, MODE_OLDFILE);
    if (script_file == BZERO) {
        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    /* Make file access a little more robust. */
    SetFileHandleAttrTags(script_file, FH_BufferMode, BUF_LINE, FH_BufferSize, 1024, TAG_DONE);

    while ((c = FGetC(script_file)) != -1) {
        /* Still enough room in the buffer? We always reserve
           enough memory for the next character and a NUL
           to terminate the string with. */
        if (script_line_length + 2 > script_line_size) {
            char *new_script_line;

            /* Give the user a chance to bail out. */
            if (__clib4->__check_abort_enabled && (SetSignal(0, 0) & __clib4->__break_signal_mask) != 0) {
                __set_errno(EAGAIN);
                goto out;
            }

            /* Always reserve a little more memory than needed,
               and one extra byte to allow us to to NUL-terminate
               the string. */
            new_script_line = realloc(script_line, script_line_length + 10);
            if (new_script_line == NULL) {
                __set_errno(ENOMEM);
                goto out;
            }

            script_line = new_script_line;
            script_line_size = script_line_length + 10;
        }

        /* Stop when we hit a line feed or an unprintable character */
        if (c == '\n' || (c < ' ' && c != '\t' && c != '\r') || (c >= 128 && c < 160))
            break;

        script_line[script_line_length++] = c;
    }

    /* Check for read error */
    if (c == -1 && IoErr() != 0) {
        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    /* Provide for NUL-termination. */
    if (script_line_size > 0) {
        /* Also strip all trailing blank spaces; that includes
           line feed and carriage return characters. */
        while (script_line_length > 0 && isspace(script_line[script_line_length - 1]))
            script_line_length--;
    }

    script_line[script_line_length] = '\0';

    (*line_ptr) = script_line;
    script_line = NULL;

    result = 0;

out:

    if (script_file != BZERO)
        Close(script_file);

    if (script_line != NULL)
        free(script_line);

    return (result);
}

/* Release all the resources allocate for the program information, as produced
   by the find_command() function */
static void
free_program_info(struct program_info *pi) {
    if (pi != NULL) {
        if (pi->resident_command != NULL) {
            Forbid();

            if (pi->resident_command->seg_UC > 0)
                pi->resident_command->seg_UC--;

            Permit();
        }

        if (pi->interpreter_name != NULL)
            free(pi->interpreter_name);

        if (pi->interpreter_args != NULL)
            free(pi->interpreter_args);

        if (pi->program_name != NULL)
            free(pi->program_name);

        if (pi->home_dir != BZERO)
            UnLock(pi->home_dir);

        if (pi->segment_list != BZERO)
            UnLoadSeg(pi->segment_list);

        free(pi);
    }
}

/* Try to find a command by name; if the name does not include any path
   information, try the dos.library resident command list */
static int
find_command(const char *path, struct program_info **result_ptr) {
    struct name_translation_info nti;
    char *script_line = NULL;
    struct program_info *pi;
    APTR old_window_ptr;
    int result = -1;
    BPTR old_dir = BZERO;
    BOOL found_path_separator;
    BOOL found_volume_separator;
    const char *p;
    int error;
    char c;
    struct _clib4 *__clib4 = __CLIB4;

    (*result_ptr) = NULL;

    /* We don't want to show any dos.library requesters while we
       are looking */
    old_window_ptr = __set_process_window((APTR) - 1);

    pi = __malloc_r(__clib4, sizeof(*pi));
    if (pi == NULL) {
        __set_errno(ENOMEM);
        goto out;
    }

    memset(pi, 0, sizeof(*pi));

    error = __translate_unix_to_amiga_path_name(&path, &nti);
    if (error != 0) {
        __set_errno(error);
        goto out;
    }

    /* Check if the path name uses separator characters, which
       indicate that the command should be located along a
       relative or absolute path. */
    found_path_separator = found_volume_separator = FALSE;

    p = path;

    while ((c = (*p++)) != '\0') {
        if (c == '/')
            found_path_separator = TRUE;

        if (c == ':')
            found_volume_separator = TRUE;
    }

    /* No relative or absolute path given? Try the resident command list. */
    if (!found_path_separator && !found_volume_separator) {
        /* Try to find the command on the resident list */
        pi->resident_command = find_resident_command(path);
        if (pi->resident_command != NULL) {
            pi->program_name = strdup(path);
            if (pi->program_name == NULL) {
                __set_errno(ENOMEM);
                goto out;
            }
        }
    }

    /* No resident command found? Try the file system. */
    if (pi->resident_command == NULL) {
        struct MsgPort *file_system;
        struct DevProc *dvp = NULL;
        BOOL done = FALSE;
        LONG io_err;

        /* Now for the simple stuff. Find a command or command script file
           under the path name given. Handle multi-volume assignments, such as
           referring to "C:", gracefully */
        file_system = GetFileSysPort();

        do {
            /* Give the user a chance to bail out. */
            if (__clib4->__check_abort_enabled && (SetSignal(0, 0) & __clib4->__break_signal_mask) != 0) {
                error = EAGAIN;
                break;
            }

            if (found_volume_separator) {
                dvp = GetDeviceProc((STRPTR) path, dvp);
                if (dvp != NULL) {
                    SetFileSysPort(dvp->dvp_Port);

                    old_dir = SetCurrentDir(dvp->dvp_Lock);
                }
            }

            /* First try: let's assume that that the file is
               executable */
            pi->segment_list = LoadSeg((STRPTR) path);
            if (pi->segment_list != BZERO) {
                /* Also remember the name of the command */
                pi->program_name = strdup(path);
                if (pi->program_name != NULL)
                    done = TRUE;
                else
                    error = ENOMEM;
            }

            io_err = IoErr();

            /* If that didn't work and we might be dealing with a script
               file, have a closer look at it. */
            if (error == 0 && !done &&
                (io_err == ERROR_OBJECT_NOT_FOUND || io_err == ERROR_OBJECT_WRONG_TYPE || io_err == ERROR_BAD_HUNK)) {
                /* Could that be an ARexx or shell script? */
                if (get_first_script_line(path, &script_line) == 0) {
                    if (strncmp(script_line, "/*", 2) == SAME) {
                        /* That's an ARexx script (or a 'C'/C++/Java program, but we support only ARexx here) */
                        pi->interpreter_name = strdup("RX");
                        if (pi->interpreter_name != NULL)
                            done = TRUE;
                        else
                            error = ENOMEM;
                    } else if (strncmp(script_line, "#!", 2) == SAME) {
                        char *name;
                        char *args;

                        /* That's probably a shell script */
                        name = &script_line[2];
                        while ((*name) != '\0' && isspace(*name))
                            name++;

                        /* Do we have a command name? */
                        if ((*name) != '\0') {
                            /* Find out if there are any script parameters */
                            args = name;
                            while ((*args) != '\0' && !isspace(*args))
                                args++;

                            if ((*args) != '\0') {
                                (*args++) = '\0';

                                while ((*args) != '\0' && isspace(*args))
                                    args++;
                            }

                            /* Remember the parameters, if any */
                            if ((*args) != '\0') {
                                pi->interpreter_args = strdup(args);
                                if (pi->interpreter_args == NULL)
                                    error = ENOMEM;
                            }

                            /* And remember the interpreter name. */
                            if (error == 0) {
                                pi->interpreter_name = strdup(name);
                                if (pi->interpreter_name != NULL)
                                    done = TRUE;
                                else
                                    error = ENOMEM;
                            }
                        }
                    }

                    free(script_line);
                    script_line = NULL;
                }
            }

            /* If that still didn't work, check if the file has
               the "script" protection bit set. */
            if (error == 0 && !done) {
                BPTR file_lock;

                file_lock = Lock((STRPTR) path, SHARED_LOCK);
                if (file_lock != BZERO) {
                    struct ExamineData *status = ExamineObjectTags(EX_LockInput, file_lock, TAG_DONE);
                    if (status) {
                        if (status->Protection & EXDB_SCRIPT) {
                            /* That's an AmigaDOS script */
                            pi->interpreter_name = strdup("Execute");
                            if (pi->interpreter_name != NULL)
                                done = TRUE;
                            else
                                error = ENOMEM;
                        }
                        FreeDosObject(DOS_EXAMINEDATA, status);
                    } else {
                        error = __translate_io_error_to_errno(IoErr());
                    }

                    UnLock(file_lock);
                }
            }

            /* If we found what we came for, also try to get a lock on
               the command/script home directory, so that "PROGDIR:"
               will work. */
            if (done) {
                BPTR file_lock;

                /* Remember where that file came from so that
                   "PROGDIR:" will work. */
                file_lock = Lock((STRPTR) path, SHARED_LOCK);
                if (file_lock != BZERO) {
                    pi->home_dir = ParentDir(file_lock);
                    if (pi->home_dir == BZERO)
                        error = __translate_io_error_to_errno(IoErr());

                    UnLock(file_lock);
                } else {
                    error = __translate_io_error_to_errno(IoErr());
                }
            }

            if (dvp != NULL)
                SetCurrentDir(old_dir);
        } while (!done && error == 0 && dvp != NULL && (dvp->dvp_Flags & DVPF_MULTIASSIGN));

        SetFileSysPort(file_system);

        if (dvp != NULL)
            FreeDeviceProc(dvp);

        if (error == 0 && !done)
            error = ENOENT;

        if (error != 0) {
            __set_errno(error);
            goto out;
        }
    }

    (*result_ptr) = pi;
    pi = NULL;

    result = 0;

out:

    if (script_line != NULL)
        free(script_line);

    if (pi != NULL)
        free_program_info(pi);

    __set_process_window(old_window_ptr);

    return (result);
}

/* Scan the string, looking for characters which need to be
   escape with a '*' if that string is to be quoted and the
   contents should remain in the same form */
static size_t
count_extra_escape_chars(const char *string, size_t len) {
    size_t count = 0;
    size_t i;
    char c;

    for (i = 0; i < len; i++) {
        c = (*string++);
        if (c == '\"' || c == '*' || c == '\n')
            count++;
    }

    return (count);
}

/* Scan a string for characters which may require that the string
   should be quoted */
STATIC BOOL
string_needs_quoting(const char *string, size_t len) {
    BOOL result = FALSE;
    size_t i;
    char c;

    for (i = 0; i < len; i++) {
        c = (*string++);
        if (c == ' ' || ((unsigned char) c) == 0xA0 || c == '\t' || c == '\n' || c == '\"') {
            result = TRUE;
            break;
        }
    }

    return (result);
}

/* Figure out how many characters would go into a string composed of
   individual arguments. This takes into account the lengths of
   the individual argument strings, the separator characters, the
   quote characters and any escape characters. */
static size_t
get_arg_string_length(char *const argv[]) {
    size_t result = 0;
    size_t i, len;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
       the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++) {
        s = (char *) argv[i];

        len = strlen(s);
        if (len > 0) {
            if ((*s) != '\"') {
                if (string_needs_quoting(s, len))
                    len += 1 + count_extra_escape_chars(s, len) + 1;
            }

            if (result == 0)
                result = len;
            else
                result = result + 1 + len;
        }
    }

    return (result);
}

/* Put together an argument string from a list of individual
   components, quoting characters, escape characters and
   separator characters. You're supposed to have enough memory
   reserved for the whole string to fit */
static void
build_arg_string(char *const argv[], char *arg_string) {
    BOOL first_char = TRUE;
    size_t i, j, len;
    char *s;

    /* The first argv[] element is skipped; it does not contain part of
       the command line but holds the name of the program to be run. */
    for (i = 1; argv[i] != NULL; i++) {
        s = (char *) argv[i];

        len = strlen(s);
        if (len > 0) {
            if (first_char)
                first_char = FALSE;
            else
                (*arg_string++) = ' ';

            if ((*s) != '\"' && string_needs_quoting(s, len)) {
                (*arg_string++) = '\"';

                for (j = 0; j < len; j++) {
                    if (s[j] == '\"' || s[j] == '*') {
                        (*arg_string++) = '*';
                        (*arg_string++) = s[j];
                    } else if (s[j] == '\n') {
                        (*arg_string++) = '*';
                        (*arg_string++) = 'N';
                    } else {
                        (*arg_string++) = s[j];
                    }
                }

                (*arg_string++) = '\"';
            } else {
                memcpy(arg_string, s, len);
                arg_string += len;
            }
        }
    }
}

/* NOTE: This is not an execve() function which works like you might expect it
         to do on a Unix-like system. Specifically, the command which will
         be executed does not replace the currently running program. That
         command will be executed first, and when it has returned the currently
         running program will exit. Also, if you need to redirect the standard
         input/output/error streams you will have to do this using the
         equivalent AmigaDOS functions (Open(), SelectInput(), SelectOutput()
         and SelectErrorOutput()), restoring these streams before your program
         exits. What this execve() function does is very similar to how the
         built-in AmigaDOS shell works, but it is much more limited in what
         it can do. */

int execve(const char *path, char *const argv[], char *const envp[]) {
    char old_program_name[256];
    int result = -1;
    struct program_info *pi;
    char *arg_string = NULL;
    size_t arg_string_len = 0;
    size_t parameter_string_len;
    BOOL success = FALSE;
    BOOL clean_up_env = FALSE;
    BPTR old_dir;

    /* We begin by trying to find the command to execute */
    if (find_command((char *) path, &pi) != 0)
        goto out;

    /* We'll need to know how much memory to reserve for the
       parameters anyway */
    parameter_string_len = get_arg_string_length(argv);

    /* Do we have to use a script interpreter? */
    if (pi->interpreter_name != NULL) {
        struct program_info *pi_interpreter;
        size_t path_len = strlen(path);

        /* Now try to find the command corresponding to the
           interpreter given */
        if (find_command(pi->interpreter_name, &pi_interpreter) != 0)
            goto out;

        /* We only try to resolve the name once. If this is still
           not a command we can launch, we chicken out */
        if (pi_interpreter->interpreter_name != NULL) {
            free_program_info(pi_interpreter);
            goto out;
        }

        /* Just remember the arguments that need to be passed
           to the interpreter */
        pi_interpreter->interpreter_args = pi->interpreter_args;
        pi->interpreter_args = NULL;

        free_program_info(pi);
        pi = pi_interpreter;

        /* Reserve as much memory as is required for the
           interpreter's parameters and the command's
           arguments */
        if (pi->interpreter_args != NULL) {
            size_t interpreter_args_len = strlen(pi->interpreter_args);

            arg_string = malloc(interpreter_args_len + 1 + path_len + 1 + parameter_string_len + 1 + 1);
            if (arg_string == NULL) {
                __set_errno(ENOMEM);
                goto out;
            }

            memcpy(arg_string, pi->interpreter_args, interpreter_args_len);
            arg_string_len = interpreter_args_len;
            arg_string[arg_string_len++] = ' ';

            memcpy(&arg_string[arg_string_len], path, path_len);
            arg_string_len += path_len;
            arg_string[arg_string_len++] = ' ';
        } else {
            arg_string = malloc(path_len + 1 + parameter_string_len + 1 + 1);
            if (arg_string == NULL) {
                __set_errno(ENOMEM);
                goto out;
            }

            memcpy(arg_string, path, path_len);
            arg_string_len = path_len;
            arg_string[arg_string_len++] = ' ';
        }
    } else {
        arg_string = malloc(parameter_string_len + 1 + 1);
        if (arg_string == NULL) {
            __set_errno(ENOMEM);
            goto out;
        }
    }

    /* Any command parameters to take care of? */
    if (parameter_string_len > 0) {
        build_arg_string(argv, &arg_string[arg_string_len]);
        arg_string_len += parameter_string_len;
    }

    /* Add the terminating new line character and a NUL,
       to be nice... */
    arg_string[arg_string_len++] = '\n';
    arg_string[arg_string_len] = '\0';

    /* Almost ready: prepare the environment data so that it can
       be used by the command to be executed. */
    if (envp != NULL) {
        if (__execve_environ_init(envp) != 0)
            goto out;

        clean_up_env = TRUE;
    }

    /* Change the shell's program name */
    GetCliProgramName(old_program_name, sizeof(old_program_name));
    SetCliProgramName(pi->program_name);

    /* Change the command's home directory, so that "PROGDIR:"
       can be used */
    old_dir = SetProgramDir(pi->home_dir);

    /* Reset the break signal before the program starts */
    SetSignal(0, SIGBREAKF_CTRL_C);

    /* Now try to run the program with the accumulated parameters */
    result = RunCommand((pi->resident_command != NULL) ? pi->resident_command->seg_Seg : pi->segment_list,
                        Cli()->cli_DefaultStack * sizeof(LONG), arg_string, arg_string_len);

    /* Restore the home directory */
    SetProgramDir(old_dir);

    /* Restore the program name */
    SetCliProgramName(old_program_name);

    /* Did we launch the program? */
    if (result == -1) {
        __set_errno(__translate_io_error_to_errno(IoErr()));
        goto out;
    }

    /* Looks good, doesn't it? */
    success = TRUE;

out:

    /* Clean up... */
    if (clean_up_env)
        __execve_environ_exit(envp);

    if (pi != NULL)
        free_program_info(pi);

    if (arg_string != NULL)
        free(arg_string);

    /* If things went well, we can actually quit now. */
    if (success)
        __execve_exit(result);

    /* Just in case somebody wants to quit... */
    __check_abort();

    return (result);
}
