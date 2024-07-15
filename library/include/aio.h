#ifndef _AIO_H
#define _AIO_H

#ifndef __USE_FILE_OFFSET64
#define _LARGEFILE64_SOURCE
#endif

#include <features.h>
#include <sys/types.h>
#include <signal.h>

__BEGIN_DECLS

/* Asynchronous I/O control block.  */
struct aiocb {
    int aio_fildes;		/* File descriptor.  */
    int aio_lio_opcode;		/* Operation to be performed.  */
    int aio_reqprio;		/* Request priority offset.  */
    volatile void *aio_buf;	/* Location of buffer.  */
    size_t aio_nbytes;		/* Length of transfer.  */
    struct sigevent aio_sigevent;	/* Signal number and value.  */

    /* Internal members.  */
    struct aiocb *__next_prio;
    int __abs_prio;
    int __policy;
    int __error_code;
    ssize_t __return_value;

#ifndef __USE_FILE_OFFSET64
    off_t aio_offset;		/* File offset.  */
    char __pad[sizeof (int64_t) - sizeof (off_t)];
#else
    int64_t aio_offset;		/* File offset.  */
#endif
    char __libc_reserved[32];
};

/* The same for the 64bit offsets.  Please note that the members aio_fildes
   to __return_value have to be the same in aiocb and aiocb64.  */
#ifdef __USE_LARGEFILE64
struct aiocb64 {
  int aio_fildes;		/* File descriptor.  */
  int aio_lio_opcode;		/* Operation to be performed.  */
  int aio_reqprio;		/* Request priority offset.  */
  volatile void *aio_buf;	/* Location of buffer.  */
  size_t aio_nbytes;		/* Length of transfer.  */
  struct sigevent aio_sigevent;	/* Signal number and value.  */

  /* Internal members.  */
  struct aiocb *__next_prio;
  int __abs_prio;
  int __policy;
  int __error_code;
  ssize_t __return_value;

  off64_t aio_offset;		/* File offset.  */
  char __glibc_reserved[32];
};
#endif


#ifdef __USE_GNU
/* To optimize the implementation one can use the following struct.  */
struct aioinit {
    int aio_threads;		/* Maximum number of threads.  */
    int aio_num;		/* Number of expected simultaneous requests.  */
    int aio_locks;		/* Not used.  */
    int aio_usedba;		/* Not used.  */
    int aio_debug;		/* Not used.  */
    int aio_numusers;		/* Not used.  */
    int aio_idle_time;		/* Number of seconds before idle thread terminates.  */
    int aio_reserved;
};
#endif


/* Return values of the aio_cancel function.  */
enum {
    AIO_CANCELED,
#define AIO_CANCELED AIO_CANCELED
    AIO_NOTCANCELED,
#define AIO_NOTCANCELED AIO_NOTCANCELED
    AIO_ALLDONE
#define AIO_ALLDONE AIO_ALLDONE
};


/* Operation codes for `aio_lio_opcode'.  */
enum {
    LIO_READ,
#define LIO_READ LIO_READ
    LIO_WRITE,
#define LIO_WRITE LIO_WRITE
    LIO_NOP
#define LIO_NOP LIO_NOP
};


/* Synchronization options for `lio_listio' function.  */
enum {
    LIO_WAIT,
#define LIO_WAIT LIO_WAIT
    LIO_NOWAIT
#define LIO_NOWAIT LIO_NOWAIT
};


/* Allow user to specify optimization.  */
#ifdef __USE_GNU
extern void aio_init(const struct aioinit *init);
#endif

/* Enqueue read request for given number of bytes and the given priority.  */
extern int aio_read(struct aiocb *aiocbp);
/* Enqueue write request for given number of bytes and the given priority.  */
extern int aio_write(struct aiocb *aiocbp);

/* Initiate list of I/O requests.  */
extern int lio_listio(int mode, struct aiocb *const list[], int nent, struct sigevent * sig);

/* Retrieve error status associated with AIOCBP.  */
extern int aio_error(const struct aiocb *aiocbp);
/* Return status associated with AIOCBP.  */
extern ssize_t aio_return(struct aiocb *aiocbp);

/* Try to cancel asynchronous I/O requests outstanding against file descriptor FILDES.  */
extern int aio_cancel(int fildes, struct aiocb *aiocbp);

/* Suspend calling thread until at least one of the asynchronous I/O
   operations referenced by LIST has completed.
   This function is a cancellation point and therefore not marked with.
*/
extern int aio_suspend(const struct aiocb *const list[], int nent, const struct timespec *timeout);

/* Force all operations associated with file desriptor described by
   `aio_fildes' member of AIOCBP.  */
extern int aio_fsync(int operation, struct aiocb *aiocbp);

#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
extern int aio_read64 (struct aiocb64 *aiocbp);
extern int aio_write64 (struct aiocb64 *aiocbp);
# define aio_error64 aio_error
# define aio_return64 aio_return
# define aio_cancel64 aio_cancel
# define aio_suspend64 aio_suspend
# define aio_fsync64 aio_fsync
# define lio_listio64 lio_listio
#endif

__END_DECLS


#endif
