/*
 * $Id: unistd.h,v 1.24 2023-04-04 10:09:49 clib4devs Exp $
*/

#ifndef _UNISTD_H
#define _UNISTD_H

#include <features.h>

#include <fcntl.h>
#include <stdint.h>

__BEGIN_DECLS

/* These may be used to determine what facilities are present at compile time.
   Their values can be obtained at run time from `sysconf'.  */

#ifdef __USE_XOPEN2K8
/* POSIX Standard approved as ISO/IEC 9945-1 as of September 2008.  */
# define _POSIX_VERSION	200809L
#elif defined __USE_XOPEN2K
/* POSIX Standard approved as ISO/IEC 9945-1 as of December 2001.  */
# define _POSIX_VERSION	200112L
#elif defined __USE_POSIX199506
/* POSIX Standard approved as ISO/IEC 9945-1 as of June 1995.  */
# define _POSIX_VERSION	199506L
#elif defined __USE_POSIX199309
/* POSIX Standard approved as ISO/IEC 9945-1 as of September 1993.  */
# define _POSIX_VERSION	199309L
#else
/* POSIX Standard approved as ISO/IEC 9945-1 as of September 1990.  */
# define _POSIX_VERSION	199009L
#endif

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#define F_ULOCK	0
#define F_LOCK	1
#define F_TLOCK	2
#define F_TEST	3

extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;

#define environ (*__environ())
extern char ***__environ (void);

extern int isatty(int file_descriptor);
extern int dup(int file_descriptor);
extern int dup2(int file_descriptor1, int file_descriptor2);
extern off_t lseek(int file_descriptor, off_t offset, int mode);
extern int access(const char * path_name, int mode);
extern int chown(const char * path_name, uid_t owner, gid_t group);
extern int fchown(int file_descriptor, uid_t owner, gid_t group);
extern int lchown(const char * path_name, uid_t owner, gid_t group);
extern int truncate(const char * path_name, off_t length);
extern int ftruncate(int file_descriptor, off_t length);
extern int getpagesize(void);
#ifdef __USE_LARGEFILE64
extern int ftruncate64(int file_descriptor, _off64_t length);
extern int truncate64(const char *path_name, _off64_t length);
extern off64_t lseek64(int fd, off64_t offset, int whence);
#endif

extern unsigned int alarm(unsigned seconds);
extern useconds_t ualarm(useconds_t usecs, useconds_t interval);
extern int link(const char * existing_path,const char * new_path);
extern int unlink(const char * path_name);
extern int symlink(const char * actual_path, const char * symbolic_path);
extern int readlink(const char * path_name, char * buffer, int buffer_size);
extern int chdir(const char * path_name);
extern int fchdir(int file_descriptor);
extern int lockf(int file_descriptor, int function, off_t size);
extern unsigned int sleep(unsigned int seconds);
extern int usleep(useconds_t microseconds);
extern int pause(void);
extern int getopt(int argc, char * const argv[], const char *opts);
extern pid_t getpid(void);
extern pid_t getppid(void);
extern pid_t getpgrp(void );
extern pid_t tcgetpgrp(int fildes);
extern int tcsetpgrp(int filedes, pid_t pgrp);
extern char *realpath(const char *file_name, char *resolved_name);
extern int fsync(int file_descriptor);
extern int fdatasync(int file_descriptor);
extern char *ttyname(int);
extern int ttyname_r(int file_descriptor,char *name,size_t buflen);
extern int execl(const char *path,const char *arg0,...);
extern int execle(const char *path,const char *arg0,...);
extern int execlp(const char *path,const char *arg0,...);
extern int execv(const char *path,char * const argv[]);
extern int execve(const char *path,char *const argv[],char *const envp[]);
extern int execvp(const char *command,char * const argv[]);
extern int getentropy(void *, size_t);

extern void swab(const void *bfrom, void *bto, ssize_t nbytes);
extern void swab24(const void *bfrom, void *bto, ssize_t nbytes);	/* Same as swab(), but operates on 24-bit words instead. */
extern void swab32(const void *bfrom, void *bto, ssize_t nbytes);	/* Same as swab(), but operates on 32-bit words instead. */
extern void swab64(const void *bfrom, void *bto, ssize_t nbytes);	/* Same as swab(), but operates on 64-bit words instead. */

#define	swab16(x)	swab(x)

extern void encrypt(char *block, int edflag);

extern int spawnv(int mode, const char *file, const char **argv);
extern int spawnve(int mode, const char *path, const char **argv, char * const envp[]);
extern int spawnvp(int mode, const char *path, const char **argv);
/* Non standard. Used to help linux ports */
extern int spawnvpe(const char *file, const char **argv, char **deltaenv, const char *dir, int fhin, int fhout, int fherr);

extern int profil(unsigned short *buffer, size_t bufSize, size_t offset, unsigned int scale);
extern long sysconf(int name);

extern void enableUnixPaths(void);
extern void disableUnixPaths(void);
extern void enableAltivec(void);
extern void disableAltivec(void);
extern void enableOptimizedFunctions(void);
extern void disableOptimizedFunctions(void);

extern char *getcwd(char * buffer, size_t buffer_size);
extern char *getwd(char *buf);
#ifdef _GNU_SOURCE
extern char *get_current_dir_name(void);
#endif

#undef gethostid
#undef gethostname

extern long gethostid();
extern int gethostname(const char *name, size_t len);
extern int getdomainname(char *name, size_t len);
extern int setdomainname(const char *name, size_t len);
extern char *getlogin(void);
extern int getlogin_r(char *buf, size_t bufsize);
extern char *crypt(const char *key, const char *salt);
extern gid_t getegid(void);
extern uid_t geteuid(void);
extern gid_t getgid(void);
extern int getgroups(int gidsetsize, gid_t grouplist[]);
extern char *getpass(const char *prompt);
extern uid_t getuid(void);
extern int initgroups(const char *name, gid_t basegid);
extern int setegid(gid_t gid);
extern int seteuid(uid_t uid);
extern int setgid(gid_t gid);
extern int setgroups(int ngroups, const gid_t *gidset);
extern int setregid(gid_t rgid, gid_t egid);
extern int setreuid(uid_t ruid, uid_t euid);
extern pid_t setsid(void);
extern int setuid(uid_t uid);
extern int setlogin(const char *name);

/* The following is for use with pathconf()/fpathconf() */
#define	_PC_LINK_MAX				0
#define	_PC_MAX_CANON				1
#define	_PC_MAX_INPUT				2
#define	_PC_NAME_MAX				3
#define	_PC_PATH_MAX				4
#define	_PC_PIPE_BUF				5
#define	_PC_CHOWN_RESTRICTED		6
#define	_PC_NO_TRUNC				7
#define	_PC_VDISABLE				8
#define	_PC_SYNC_IO					9
#define	_PC_ASYNC_IO				10
#define	_PC_PRIO_IO					11
#define	_PC_SOCK_MAXBUF				12
#define	_PC_FILESIZEBITS			13
#if 0
#define	_PC_REC_INCR_XFER_SIZE		14
#define	_PC_REC_MAX_XFER_SIZE		15
#define	_PC_REC_MIN_XFER_SIZE		16
#define	_PC_REC_XFER_ALIGN			17
#define	_PC_ALLOC_SIZE_MIN			18
#endif
#define	_PC_SYMLINK_MAX				19
#define	_PC_XATTR_EXISTS			20
#define	_PC_XATTR_ENABLED			21

/* Amiga Specific */
#define	_PC_DOSTYPE					300

#define _POSIX_MONOTONIC_CLOCK      1

/* The following is for use with sysconf(). Only the implemented one */

#define _SC_ARG_MAX                 0
#define _SC_CLK_TCK                 2
#define _SC_OPEN_MAX                4
#define _SC_PAGESIZE                8
#define _SC_NPROCESSORS_CONF        9
#define _SC_NPROCESSORS_ONLN       10
#define _SC_TZNAME_MAX             20
#define _SC_THREAD_KEYS_MAX        21
#define _SC_VERSION                29
#define _SC_HOST_NAME_MAX          65
#define _SC_GETPW_R_SIZE_MAX	   70
#define _SC_TTY_NAME_MAX           72

extern long pathconf(const char *path,int name);
extern long fpathconf(int file_descriptor,int name);

extern int pipe (int fd[2]);
extern int pipe2 (int fd[2], int flags);
extern ssize_t pread(int fd, void *buf, size_t nbytes, off_t offset);
extern ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset);

#ifdef __USE_LARGEFILE64
extern ssize_t pread64(int fd, void *buf, size_t nbytes, off64_t offset);
extern ssize_t pwrite64(int fd, const void *buf, size_t nbytes, off64_t offset);
#endif

#ifdef __USE_GNU
/* Evaluate EXPRESSION, and repeat as long as it returns -1 with `errno' set to EINTR.  */

# define TEMP_FAILURE_RETRY(exp) \
  ({                                                    \
    long int __result = 0;                              \
    do {                                                \
      __result = (long int)(exp);                       \
    } while ((__result == -1) && (errno == EINTR));     \
    __result;                                           \
  })
#endif

#define _P_WAIT         1
#define _P_NOWAIT       2
#define _P_OVERLAY      3
#define _P_NOWAITO      4
#define _P_DETACH       5

#define P_WAIT          _P_WAIT
#define P_NOWAIT        _P_NOWAIT
#define P_OVERLAY       _P_OVERLAY
#define P_NOWAITO       _P_NOWAITO
#define P_DETACH        _P_DETACH

#define WAIT_CHILD 1

__END_DECLS

#endif /* _UNISTD_H */
