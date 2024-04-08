/*
 * $Id: posix_posix_spawn_internal.c,v 1.0 2024-04-07 12:04:24 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include "spawn_internal.h"

static int spawned_task() {
    struct Process *thisTask = (struct Process *) FindTask(NULL);
    struct _posix_spawn_task_params *taskParams = (struct _posix_spawn_task_params *) thisTask->pr_Task.tc_UserData;
    struct _clib4 *__clib4 = __CLIB4;
    int ret = 0;
    int action;
    int pipe[2] = {-1, -1};

    Printf("2) taskParams->pipes = %ld %ld\n", taskParams->pipe[0], taskParams->pipe[1]);
    pipe[0] = taskParams->pipe[0];
    pipe[1] = taskParams->pipe[1];

    if (taskParams->ad != NULL) {
        _posix_spawnattr_t *px_attrp = (_posix_spawnattr_t *) taskParams->ad->attrp;
        if (px_attrp) {
            if ((*px_attrp)->psa_flags & POSIX_SPAWN_RESETIDS) {
                setgid(getgid());
                setuid(getuid());
            }
            if ((*px_attrp)->psa_flags & POSIX_SPAWN_SETSIGMASK) {
                ret = sigprocmask(SIG_SETMASK, &(*px_attrp)->psa_sigmask, NULL);
                if (ret != 0)
                    goto fail;
            }
        }

        _posix_spawn_file_actions_t *px_sfap = (_posix_spawn_file_actions_t *) taskParams->ad->file_actions;
        if (px_sfap) {
            for (action = 0; action < taskParams->ad->file_actions_size; action++) {
                _psfa_action_t *psfa = &(*px_sfap)->psfa_act_acts[action];

                if (psfa->psfaa_filedes == taskParams->pipe[1]) {
                    ret = dup(taskParams->pipe[1]);
                    if (ret < 0)
                        goto fail;

                    close(taskParams->pipe[1]);
                    pipe[1] = ret;
                }

                switch (psfa->psfaa_type) {
                    case PSFA_OPEN:
                        ret = open(psfa->psfaa_openargs.psfao_path, psfa->psfaa_openargs.psfao_oflag,
                                   psfa->psfaa_openargs.psfao_mode);
                        if (ret < 0)
                            goto fail;

                        if (ret != psfa->psfaa_filedes) {
                            int fd = ret;
                            ret = dup2(ret, psfa->psfaa_filedes);

                            if (ret < 0) {
                                close(fd);
                                goto fail;
                            }
                        }
                        break;
                    case PSFA_CLOSE:
                        ret = close(psfa->psfaa_filedes);
                        if (ret != 0)
                            goto fail;
                        break;
                    case PSFA_DUP2:
                        ret = dup2(psfa->psfaa_filedes, psfa->psfaa_openargs.psfao_oflag);
                        if (ret != 0)
                            goto fail;
                        break;
                    case PSFA_CHDIR:
                        ret = chdir(psfa->psfaa_path);
                        if (ret != 0)
                            goto fail;
                        break;
                    case PSFA_FCHDIR:
                        ret = fchdir(psfa->psfaa_filedes);
                        if (ret != 0)
                            goto fail;
                        break;
                }
            }
        }
    }

    Printf("2) Calling execve\n");
    ret = execve(taskParams->path, taskParams->argv, taskParams->envp);
    Printf("2) Done: ret = %ld\n", ret);

    Printf("2) Write on pipe[1]\n");
    write(pipe[1], &ret, sizeof(ret));
    Signal(taskParams->parent, taskParams->startupSignal);
    Printf("2) close pipe[1]\n");
    close(pipe[1]);

    Printf("2) return -127\n");
    return -127;
fail:
    return ret;
}

int
__posix_spawn(pid_t *__restrict pid, const char *__restrict path, struct _posix_spawn_args_desc *ad,
              char *const argv[__restrict], char *const envp[__restrict]) {
    int pipe[2] = {-1, -1};
    int ret = pipe2(pipe, O_CLOEXEC);
    if (ret < 0)
        return ret;

    int8_t signalBit = AllocSignal (-1);
    int32_t startupSignal;
    if (signalBit != -1)
        startupSignal = 1 << signalBit;
    else {
        close(pipe[0]);
        close(pipe[1]);
        return ret;
    }

    struct _posix_spawn_task_params taskParams;
    taskParams.parent = FindTask (NULL);
    taskParams.envp = envp;
    taskParams.ad = ad;
    taskParams.argv = argv;
    taskParams.path = path;
    taskParams.pipe[0] = pipe[0];
    taskParams.pipe[1] = pipe[1];
    taskParams.startupSignal = startupSignal;

    BPTR fileIn  = Open("CONSOLE:", MODE_OLDFILE);
    BPTR fileOut = Open("CONSOLE:", MODE_OLDFILE);

    struct Process *child = CreateNewProcTags(
            NP_Name,                "SPAWN_INTERNAL",
            NP_Entry,               spawned_task,
            NP_Input,			    fileIn,
            NP_Output,			    fileOut,
            NP_CloseInput,		    TRUE,
            NP_CloseOutput,		    TRUE,
            NP_Child,               TRUE,
            NP_Cli,	                TRUE,
            NP_UserData,            (struct _posix_spawn_task_params *) &taskParams,
            NP_CloseOutput,         TRUE,
            TAG_END);

    //close(pipe[1]);

    if (!child) {
        Close(fileIn);
        Close(fileOut);
        return EFAULT;
    }
    *pid = ((struct Process *) child)->pr_ProcessID;
    Printf("1) Spawned process = %ld\n", pid);
    Printf("1) Wait for process to start\n");

    // Make sure process is started
    Wait(taskParams.startupSignal);
    Printf("1) Started\n");

    Printf("1) pipes = %ld %ld\n", pipe[0], pipe[1]);
    Printf("1) Now read from pipe[0]\n");
    if (read(pipe[0], &ret, sizeof(ret)) != sizeof(ret))
        ret = 0;
    Printf("1) ret = %ld\n", ret);
    close(pipe[0]);

    return ret;
}

int
_posix_spawn_file_actions_grow(_posix_spawn_file_actions_t *psactsp) {
    int new_alloc = (*psactsp)->psfa_act_alloc * 2;
    _posix_spawn_file_actions_t new_psacts;

    /*
     * XXX may want to impose an administrative limit here; POSIX does
     * XXX not provide for an administrative error return in this case,
     * XXX so it's probably acceptable to just fail catastrophically
     * XXX instead of implementing one.
     */
    if ((new_psacts = (_posix_spawn_file_actions_t) realloc((*psactsp), PSF_ACTIONS_SIZE(new_alloc))) == NULL) {
        return (ENOMEM);
    }
    new_psacts->psfa_act_alloc = new_alloc;
    *psactsp = new_psacts;

    return (0);
}