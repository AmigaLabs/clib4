/*
 * $Id: signal_kill.c,v 1.8 2006-04-05 08:39:45 clib2devs Exp $
*/

#ifndef _SIGNAL_HEADERS_H
#include "signal_headers.h"
#endif /* _SIGNAL_HEADERS_H */

#include <unistd.h>

static APTR
hook_function(struct Hook *hook, APTR userdata, struct Process *process)
{
	uint32 pid = (uint32)userdata;
    (void) (hook);

	if (process->pr_ProcessID == pid)
	{
		return process;
	}

	return 0;
}

int 
kill(pid_t pid, int signal_number)
{
	struct Process *cli_process;
	int result = ERROR;

	ENTER();

	SHOWVALUE(pid);
	SHOWVALUE(signal_number);

	if (signal_number < 0 || signal_number >= NSIG)
	{
		__set_errno(EINVAL);
		return result;
	}

	if (pid > 0)
	{
		if (pid == getpid())
		{
			result = raise(signal_number);
		}
		else
		{
			struct Hook h = {{NULL, NULL}, (HOOKFUNC)hook_function, NULL, NULL};

			Forbid();

			int32 process = ProcessScan(&h, (CONST_APTR)pid, 0);
			if (process > 0)
			{
				cli_process = (struct Process *) process;
				
				SHOWMSG("found the process");

				result = 0;

				if (signal_number == SIGTERM || signal_number == SIGINT || signal_number == SIGQUIT)
				{
					Signal((struct Task *)cli_process, SIGBREAKF_CTRL_C);
				}
				else
				{
					SHOWMSG("but won't shut it down");
				}
			}
			else
			{
				SHOWMSG("didn't find the process");

				__set_errno(ESRCH);
			}

			Permit();
		}
	}

	RETURN(result);
	return (result);
}
