/*
 * $Id: unistd.h,v 1.23 2007-01-06 10:09:49 clib2devs Exp $
*/

#ifndef _UNISTD_H
#define _UNISTD_H

#include <features.h>

#ifndef _FCNTL_H
#include <fcntl.h>
#endif /* _FCNTL_H */

#ifndef _STDIO_H
#include <stdio.h>
#endif /* _STDIO_H */

#ifndef _STDINT_H
#include <stdint.h>
#endif /* _STDINT_H */

#include <sys/select.h>

__BEGIN_DECLS

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
extern char * optarg;

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
extern int ttyname_t(int,char *,size_t);
extern int execl(const char *path,const char *arg0,...);
extern int execle(const char *path,const char *arg0,...);
extern int execlp(const char *path,const char *arg0,...);
extern int execv(const char *path,char * const argv[]);
extern int execve(const char *path,char *const argv[],char *const envp[]);
extern int execvp(const char *command,char * const argv[]);

extern int spawnv(int mode, const char *file, const char **argv);
extern int spawnvp(int mode, const char *path, const char **argv);

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

#define _POSIX_THREADS              1
#define _POSIX_MONOTONIC_CLOCK      1

/* The following is for use with sysconf(). Only the implemented one */

#define _SC_ARG_MAX                 0
#define _SC_CLK_TCK                 2
#define _SC_OPEN_MAX                4
#define _SC_PAGESIZE                8
#define _SC_NPROCESSORS_CONF        9
#define _SC_TZNAME_MAX             20
#define _SC_THREAD_KEYS_MAX        21
#define _SC_HOST_NAME_MAX          65

extern long pathconf(const char *path,int name);
extern long fpathconf(int file_descriptor,int name);

extern ssize_t write(int fildes, const void *buf, size_t nbyte);

extern int pipe (int fd[2]);
extern int pipe2 (int fd[2], int flags);
extern ssize_t pread(int fd, void *buf, size_t nbytes, off_t offset);
extern ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset);

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
