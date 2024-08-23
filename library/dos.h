/*
 * $Id: dos.h,v 2.0 2023-05-17 09:40:06 clib4devs Exp $
*/

#ifndef _DOS_H
#define _DOS_H

#include "include/features.h"

#include "include/stdio.h"
#include "include/stddef.h"
#include "include/signal.h"
#include "include/sys/resource.h"
#include "include/sys/syslimits.h"
#include "include/wchar.h"
#include "include/setjmp.h"
#include "include/resolv.h"
#include <exec/types.h>
#include <exec/lists.h>
#include <exec/semaphores.h>

/* Category name handling variables.  */
#define NUM_LOCALES                (LC_MAX + 1)
#define MAX_LOCALE_NAME_LEN        256
/* arc4Random stuff */
#define RANDOMDEV               "RANDOM:"
#define KEYSIZE                    128

__BEGIN_DECLS

typedef struct _wof_allocator_t wof_allocator_t;
typedef void (*signal_handler_t)(int sig);
typedef int32 BPTR;
typedef short BOOL;

struct ExitTrapNode {
    struct MinNode etn_MinNode;

    void (*etn_Function)(void);
};

struct mofile_s {
    struct mofile_s *next;
    char path[PATH_MAX];
    const char *plural_rule;
    unsigned long nplurals;
    void *map;
    size_t size;
};

struct arc4_stream {
    uint8_t i;
    uint8_t j;
    uint8_t s[256];
};

/*
 * Obtain the low level 'file' handle or socket ID bound to a file
 * descriptor. This function returns 0 for success and non-zero
 * otherwise (if, for example, the file descriptor value you
 * provided is not valid; errno will be set to the appropriate
 * error code, too). Put a pointer to the file handle variable you
 * want to be filled in into the second parameter to this function.
 */
extern int __get_default_file(int file_descriptor, long *file_ptr);

/*
 * If this function pointer is not NULL, it must point to a function which
 * figures out how much stack space is required to run the program. The
 * function return value replaces the value of the __stack_size variable
 * if it is not equal to zero.
 *
 * At the time this function is invoked, dos.library and utility.library
 * have already been opened for you.
 */
extern unsigned int (*__get_default_stack_size)(void);

/****************************************************************************/

/* A data structures used by the path translation routines below. */
struct name_translation_info {
    char substitute[PATH_MAX];
    char *original_name;
    int is_root;
};

/****************************************************************************/

extern int __translate_relative_path_name(char const **name_ptr, char *replace, size_t max_replace_len);
extern void __restore_path_name(char const **name_ptr, struct name_translation_info *nti);
extern int __translate_amiga_to_unix_path_name(char const **name_ptr, struct name_translation_info *nti);
extern int __translate_unix_to_amiga_path_name(char const **name_ptr, struct name_translation_info *nti);
extern int __translate_io_error_to_errno(LONG io_error);
extern void __print_termination_message(const char *termination_message);

/****************************************************************************/

extern int __execve_environ_init(char *const envp[]);
extern void __execve_environ_exit(char *const envp[]);

/****************************************************************************/

/*
 * The __execve_exit() function is called by execve() if the command
 * executed correctly and control should be returned to the shell. The
 * default behaviour is to eventually call exit(). You can, however,
 * replace __execve_exit() with a stub which does nothing at all. In
 * that case the execve() function will return control to the caller
 * instead.
 */

extern void __execve_exit(int return_code);

/*
 * wide char status structure
 */
struct _wchar {
    /* miscellaneous reentrant data */
    char *_strtok_last;
    _mbstate_t _mblen_state;
    _mbstate_t _wctomb_state;
    _mbstate_t _mbtowc_state;
    char _l64a_buf[8];
    int _getdate_err;
    _mbstate_t _mbrlen_state;
    _mbstate_t _mbrtowc_state;
    _mbstate_t _mbsrtowcs_state;
    _mbstate_t _wcrtomb_state;
    _mbstate_t _wcsrtombs_state;
};

/*
 * Initial _clib4 structure. This contains all fields used by current progream
 */

struct _clib4 {
    struct ExecIFace *IExec;    /* Main IExec interface */

    struct ElfIFace *IElf;

    struct Library *__LocaleBase;
    struct LocaleIFace *__ILocale;

    struct Library *__DiskfontBase;
    struct DiskfontIFace *__IDiskfont;

    struct Library *__TimezoneBase;
    struct TimezoneIFace *__ITimezone;

    struct Library *__SocketBase;
    struct SocketIFace *__ISocket;

    struct Library *__UserGroupBase;
    struct UserGroupIFace *__IUserGroup;

    /* CPU Family */
    uint32 cpufamily;
    uint32 hasAltivec;

    /* Used by setjmp/longjmp in main */
    jmp_buf __exit_jmp_buf;
    int __exit_value;

    /*
     * If set to TRUE, your program's process->pr_WindowPtr will be set to -1
     * when it starts. The process->pr_WindowPtr will be automatically restored
     * to the initial value before it exits.
     */
    BOOL __disable_dos_requesters;

    struct timeval clock;        /* Populated when clib starts with current time */
    struct rusage ru;            /* rusage struct used in rlimit function */
    struct _wchar *wide_status;    /* wide char functions status */

    /*
     * This variable controls the task priority of the program, when running.
     * It must be in the range -128..127 to be useful. By default, the task
     * priority will not be changed.
     */
    int __priority;

    char *__tzname[2];     /* Current timezone names.  */
    int __daylight;        /* If daylight-saving time is ever in use.  */
    long int __timezone;   /* Seconds west of UTC.  */
    int __dyntz;           /* Set to TRUE if created with malloc */
    struct SignalSemaphore *timezone_lock;

    /*
     * You can override the default break signal mask which is used by
     * __check_abort() and other functions. This must be done at link
     * time because the break signal checking is set up very early on
     * while the program startup code is preparing your code to be run.
     * In particular, this affects the socket I/O functionality which
     * configures the break signal only once. The default value of the
     * __break_signal_mask variable is SIGBREAKF_CTRL_C.
     */
    ULONG __break_signal_mask;

    /*
     * If set, Unix path names are translated to Amiga path
     * names (and the other way round). If you wish to disable this, set the
     * following variable to FALSE. Only the path name translation is affected
     * by setting this variable to FALSE. You will always get Unix-like behaviour
     * from certain functions regardless of whether the path names are translated
     * or not.
     */

    BOOL __unix_path_semantics;

    BOOL  __fully_initialized;
    int32_t __pipenum;
    void *__pipe_semaphore;
    BOOL  unused3;

    /* This is used with the dlopen(), dlclose() and dlsym() functions. */
    void  *__dl_root_handle; //Elf32_Handle
    int __elf_error_code;    // Elf32_Error

    /* This is the pointer to itself */
    struct Process *self;

    struct Locale *__default_locale;
    struct Locale *__locale_table[NUM_LOCALES];
    char __locale_name_table[NUM_LOCALES][MAX_LOCALE_NAME_LEN];
    char __lc_ctype[__LC_LAST];
    struct SignalSemaphore *locale_lock;

    /* used by setlocale */
    int _current_category;
    const char *_current_locale;
    int __mb_cur_max;

    /* used by tmpnam */
    int inc;

    /* Directories being scanned whose locks need to be freed when shutting down. */
    struct MinList __directory_list;
    struct SignalSemaphore *dirent_lock;

    /* Used by initstate/setstate */
    struct SignalSemaphore *__random_lock;
    int n;
    int i;
    int j;
    uint32_t *x;

    /* Used by rand_r */
    uint64_t __random_seed;

    /* Used by setitimer/getitimer */
    struct itimerval tmr_time;
    struct timeval tmr_start_time;
    struct Process *tmr_real_task;

    /* Used for shared version library */
    int _errno;
    int _h_errno;

    /*
     * Defaults for path delimiter (":") and the shell search path
     * ("/gcc/bin:/SDK/C:/SDK/Local/C:/C:.") as used by the execvp()
     * function.
     */
    const char *__default_path_delimiter;
    const char *__default_path;

    /* Set this to FALSE to disable all Ctrl+C checking in the program. */
    BOOL __check_abort_enabled;

    /* The file handle table. */
    struct iob **__iob;
    int __num_iob;

    /* The file descriptor table. */
    struct fd **__fd;
    int __num_fd;

    /* This is filled in with a pointer to the name of the program being run. */
    char *__progname;
    BOOL free_program_name;

    char __current_path_name[PATH_MAX];

    /* Don't remove aligned otherwise you could have bad things on exit */
    struct AnchorPath *anchor __attribute__ ((aligned));

    /* This holds a mask of all signals whose delivery is currently blocked.
       The sigaddset(), sigblock(), sigprocmask() and sigsetmask() functions
       modify or query it. */
    int __signals_blocked;
    int local_signals_blocked;

    struct SignalSemaphore *stdio_lock;

    /* Wof Allocator main pointer */
    wof_allocator_t *__wof_allocator;
    void *unused6;

    /* Names of files and directories to delete when shutting down. */
    struct MinList __unlink_list;
    struct SignalSemaphore __unlink_semaphore;

    /* Local timer I/O. */
    struct MsgPort *__timer_port;
    BOOL __timer_busy;
    struct SignalSemaphore *__timer_semaphore;
    struct TimeRequest *__timer_request;
    struct Library *__TimerBase;
    struct TimerIFace *__ITimer;

    /* If the program's current directory was changed, here is where we find out about it. */
    BPTR __original_current_directory;
    BOOL __current_directory_changed;
    BOOL __unlock_current_directory;

    /* Memalign memory list */
    void *__memalign_pool;
    struct AVLNode *__memalign_tree;

    /*
     * This variable can be set up to contain the minimum stack size the program
     * should be launched with. If the startup code finds that there is not enough
     * stack space available to start with, it will attempt to allocate more and
     * then proceed to run your program.
     *
     * If this variable is set to 0 (the default) then no stack size test will
     * be performed upon startup.
     */
    unsigned int __stack_size;

    UBYTE __shell_escape_character;

    char **__argv;
    int __argc;

    const char *__file_lock_semaphore_name;

    struct WBStartup *__WBenchMsg;
    BOOL __no_standard_io;

    /* The pointer to the data made by tgetent is left here for tgetnum, tgetflag and tgetstr to find.  */
    char *term_entry;

    /* CPU cache line size; used to align I/O buffers for best performance. */
    ULONG __cache_line_size;

    /* File init fields */
    struct MsgPort *old_console_task;
    BOOL restore_console_task;
    BOOL restore_streams;
    BPTR old_output;
    BPTR old_input;
    BPTR output;
    BPTR input;

    BOOL __environment_allocated;

    /*
     * The following variables are part of libnet.a, which provides for
     * a BSD-Unix-like socket networking API. Traditionally, only one process
     * at a time may use the underlying bsdsocket.library base, but with a
     * multithreaded application you may want all of them to share the same
     * library base. As of this writing there is one single TCP/IP stack which
     * supports this feature (Roadshow) and it must be enabled early on. If
     * this worked out well you can test through the following variable which
     * will be set to TRUE:
     */
    BOOL __can_share_socket_library_base;

    /*
     * The next global variable is also part of the thread-safe libnet.a and
     * indicates that the TCP/IP stack will call the functions __set_errno()
     * and __set_h_errno() when it modifies the global errno and h_errno
     * variables, respectively. If you want to save the error codes for each
     * of the Processes in your multithreaded application then you should
     * override these functions with your. The following variable will be
     * set to TRUE if the __set_errno() and __set_h_errno() functions will
     * be used to change the corresponding variables:
     */
    BOOL __thread_safe_errno_h_errno;

    /*
     * The unlink() and remove() functions in libunix.a may return success even
     * though deletion failed because the file/directory/link in question is still
     * reported as being "in use". This is the default behaviour. If you want the
     * deletion to fail instead, set '__unlink_retries' to FALSE.
     */
    BOOL __unlink_retries;

    /*
     * 'environ' is the default environment variable table as used by the execl(),
     * execv() and execvp() functions. This needs to be initialized before you
     * can use it. The table has the following form:
     *
     *    char ** environ =
     *    {
     *       "variable1=value",
     *       "variable2=value",
     *       NULL
     *    };
     *
     * Note that if you initialize the 'environ' table you will also have to
     * provide for a function which prepares its contents in execve() for use
     * by the command to be executed. That function is called
     * __execve_environ_init(). Should program execution fail, you need to
     * clean up after what __execve_environ_init() set up. To do this, call
     * __execve_environ_exit(). There are stubs in clib4 for these functions
     * which essentially do nothing at all. You will have to implement these
     * yourself if you want to use them.
     */
    char **__environment;

    /*
     * If you link against libunix.a then the default command line processing
     * function will attempt to expand every single wildcard parameter on the
     * command line into a series of file and directories names matching the
     * wildcards. The idea is to provide functionality which on Unix the
     * shell is responsible for. On AmigaDOS the shell commands need to perform
     * the expansion. However, if you are mixing AmigaDOS commands which expand
     * wildcard patterns with a shell that already does the job, you may run into
     * big trouble. To disable the expansion, declare the global variable named
     * "__expand_wildcard_args" in your code and have it set to FALSE. Because
     * the program startup code checks this variable early on, its value must
     * be available at that time, i.e. you cannot just set it differently in
     * your code lateron because by that time the startup code will have already
     * checked it.
     *
     * Note that the startup code will disable wildcard expansion if the local
     * shell environment variable "DISABLE_COMMANDLINE_WILDCARD_EXPANSION"
     * is set.
     */
    BOOL __expand_wildcard_args;

    signal_handler_t __signal_handler_table[NSIG];

    float __infinity;
    float __nan;
    int _gamma_signgam;

    BOOL __root_mode;
    int __root_uid;
    int __root_gid;
    int __root_euid;
    int __root_egid;

    /* atexit() - 32 functions are the minimum required by ISO 'C'. */
    struct ExitTrapNode exit_node_table[32];
    struct MinList exit_trap_list;
    size_t num_exit_nodes_used;
    BOOL atexit_blocked;

    struct arc4_stream rs;
    int rs_initialized;
    int rs_stired;
    int arc4_count;
    int rs_data_available;
    struct {
        struct timeval tv;
        pid_t pid;
        uint8_t rnd[KEYSIZE];
    } rdat;

    struct LocalVariable *__lv_root;

    int indent_level;
    int __debug_level;
    int previous_debug_level;

    struct StackSwapStruct stack_swap_struct;

    /* Wof Allocator memory semaphore */
    struct SignalSemaphore *memory_semaphore;
    int pipenum;

    /* gettext */
    struct SignalSemaphore *gettext_lock;
    struct mofile_s *g_mofile;
    char gettext_domain[NAME_MAX];
    void *bindings;

    /* getrandom */
    int randfd[2];

    unsigned char __ctype_table[2 * 128];

    /* termcap  */
    short __ospeed;
    /* If OSPEED is 0, we use this as the actual baud rate.  */
    int __tputs_baud_rate;
    char __PC;
    char *__UP;
    char *__BC;

    /* Syslog stuff */
    int syslog_openlog_flags;
    int syslog_mask;
    int syslog_facility;
    char syslog_ident[35];
    FILE *syslog_fd;

    /* Current process id */
    int processId;
    char *uuid;

    BPTR error;
    BPTR old_error;

    /* termcap */
    char tgoto_buf[50];

    struct __res_state _res_state;

    /* ttyname */
    char tty_file_name[_POSIX_TTY_NAME_MAX];

    /* Set of current actions.  If sa_handler for an entry is NULL, then
     * that signal is not currently handled by the sigaction handler.
     */
    struct sigaction action_array[NSIG];

    int __children;
    int __was_sig;
};

#ifndef __getClib4
extern struct _clib4 *__getClib4(void);
#endif

#undef __CLIB4
#define __CLIB4 __getClib4()

/*
 * Two functions control how this library uses the locale.library API to
 * perform string collation, character and time conversion, etc.
 *
 * __locale_init() opens locale.library and attempts to open the default
 * locale. If it succeeds, it will return 0 and -1 otherwise.
 *
 * __locale_exit() releases the default locale and closes locale.library.
 */
extern int __locale_init(struct _clib4 *__clib4);
extern void __locale_exit(struct _clib4 *__clib4);

__END_DECLS

#endif /* _DOS_H */
