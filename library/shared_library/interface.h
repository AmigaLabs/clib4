#ifndef _SYS_INTERFACE_H_
#define _SYS_INTERFACE_H_

#include <proto/dos.h>

#include <arpa/inet.h>
#include <aio.h>
#include <argz.h>
#include <assert.h>
#include <complex.h>
#include <crypt.h>
#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <envz.h>
#include <err.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <ftw.h>
#include <getopt.h>
#include <glob.h>
#include <grp.h>
#include <iconv.h>
#include <ifaddrs.h>
#include <langinfo.h>
#include <libgen.h>
#include <locale.h>
#include <malloc.h>
#include <math.h>
#include <netdb.h>
#include <nl_types.h>
#include <poll.h>
#include <pthread.h>
#include <pwd.h>
#include <regex.h>
#include <resolv.h>
#include <search.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <termcap.h>
#include <termios.h>
#include <tgmath.h>
#include <time.h>
#include <uchar.h>
#include <ulimit.h>
#include <unistd.h>
#include <utime.h>
#include <wchar.h>
#include <wctype.h>
#include <wctype.h>
#include <sys/byteswap.h>
#include <sys/file.h>
#include <sys/iconvnls.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/syslog.h>
#include <sys/systeminfo.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/uio.h>

struct NameTranslationInfo;

#ifdef __GNUC__
#define ___VALIST __gnuc_va_list
#else
#define ___VALIST va_list
#endif

#include <stdarg.h>

struct Clib2IFace {
    int data[15]; //struct InterfaceData Data;                      //0-60

    unsigned long APICALL (*Obtain)(struct Clib2IFace *Self);       //64
    unsigned long APICALL (*Release)(struct Clib2IFace *Self);      //68
    void APICALL (*Expunge)(struct Clib2IFace *Self);               //72
    struct Clib2IFace *APICALL (*Clone)(struct Clib2IFace *Self);   //76

    /* internal */
    int  (* library_start)(char *argstr, int arglen, struct Library **_DOSBase, struct DOSIFace **_IDOS, int (* start_main)(int, char **), void (*__CTOR_LIST__[])(void), void (*__DTOR_LIST__[])(void)); //80
    void (* __getclib2)(void);                                      //84
    void (* __translate_amiga_to_unix_path_name)(void);             //88
    void (* __translate_unix_to_amiga_path_name)(void);             //92
    void (* __restore_path_name)(void);                             //96
    void (* internal1)(void);                                       //100
    void (* internal2)(void);                                       //104
    void (* internal3)(void);                                       //108
    void (* internal4)(void);                                       //112
    void (* internal5)(void);                                       //116

    /* aio.h */
    void (* aio_init) (const struct aioinit *init);                                                                                                  /* 120 */
    int (* aio_read) (struct aiocb *aiocbp);                                                                                                         /* 124 */
    int (* aio_write) (struct aiocb *aiocbp);                                                                                                        /* 128 */
    int (* lio_listio) (int mode, struct aiocb *const list[restrict], int nent, struct sigevent * sig);                                              /* 132 */
    int (* aio_error) (const struct aiocb *aiocbp);                                                                                                  /* 136 */
    ssize_t (* aio_return) (struct aiocb *aiocbp);                                                                                                   /* 140 */
    int (* aio_cancel) (int fildes, struct aiocb *aiocbp);                                                                                           /* 144 */
    int (* aio_suspend) (const struct aiocb *const list[], int nent, const struct timespec *timeout);                                                /* 148 */
    int (* aio_fsync) (int operation, struct aiocb *aiocbp);                                                                                         /* 152 */
    int (* aio_read64) (struct aiocb64 *aiocbp);                                                                                                     /* 156 */
    int (* aio_write64) (struct aiocb64 *aiocbp);                                                                                                    /* 160 */

    /* argz.h */
    error_t (* argz_create) (char *const argv[], char **argz, size_t *argz_len);                                                                     /* 164 */
    error_t (* argz_create_sep) (const char *string, int sep, char **argz, size_t *argz_len);                                                        /* 168 */
    size_t (* argz_count) (const char *argz, size_t argz_len);                                                                                       /* 172 */
    void (* argz_extract) (char *argz, size_t argz_len, char **argv);                                                                                /* 176 */
    void (* argz_stringify) (char *argz, size_t argz_len, int sep);                                                                                  /* 180 */
    error_t (* argz_add) (char **argz, size_t *argz_len, const char *str);                                                                           /* 184 */
    error_t (* argz_add_sep) (char **argz, size_t *argz_len, const char *str, int sep);                                                              /* 188 */
    error_t (* argz_append) (char **argz, size_t *argz_len, const char *buf, size_t buf_len);                                                        /* 192 */
    error_t (* argz_delete) (char **argz, size_t *argz_len, char *entry);                                                                            /* 196 */
    error_t (* argz_insert) (char **argz, size_t *argz_len, char *before, const char *entry);                                                        /* 200 */
    char * (* argz_next) (char *argz, size_t argz_len, const char *entry);                                                                           /* 204 */
    error_t (* argz_replace) (char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count);                            /* 208 */

    /* assert.h */
    void (* __assertion_failure) (const char *file_name, int line_number, const char *expression);                                                   /* 212 */

    /* complex.h */
    double complex (* cacos) (double complex z);                                                                                                     /* 216 */
    float complex (* cacosf) (float complex z);                                                                                                      /* 220 */
    long double complex (* cacosl) (long double complex z);                                                                                          /* 224 */
    double complex (* casin) (double complex z);                                                                                                     /* 228 */
    float complex (* casinf) (float complex z);                                                                                                      /* 232 */
    long double complex (* casinl) (long double complex z);                                                                                          /* 236 */
    double complex (* catan) (double complex z);                                                                                                     /* 240 */
    float complex (* catanf) (float complex z);                                                                                                      /* 244 */
    long double complex (* catanl) (long double complex z);                                                                                          /* 248 */
    double complex (* ccos) (double complex z);                                                                                                      /* 252 */
    float complex (* ccosf) (float complex z);                                                                                                       /* 256 */
    long double complex (* ccosl) (long double complex z);                                                                                           /* 260 */
    double complex (* csin) (double complex z);                                                                                                      /* 264 */
    float complex (* csinf) (float complex z);                                                                                                       /* 268 */
    long double complex (* csinl) (long double complex z);                                                                                           /* 272 */
    double complex (* ctan) (double complex z);                                                                                                      /* 276 */
    float complex (* ctanf) (float complex z);                                                                                                       /* 280 */
    long double complex (* ctanl) (long double complex z);                                                                                           /* 284 */
    double complex (* cacosh) (double complex z);                                                                                                    /* 288 */
    float complex (* cacoshf) (float complex z);                                                                                                     /* 292 */
    long double complex (* cacoshl) (long double complex z);                                                                                         /* 296 */
    double complex (* casinh) (double complex z);                                                                                                    /* 300 */
    float complex (* casinhf) (float complex z);                                                                                                     /* 304 */
    long double complex (* casinhl) (long double complex z);                                                                                         /* 308 */
    double complex (* catanh) (double complex z);                                                                                                    /* 312 */
    float complex (* catanhf) (float complex z);                                                                                                     /* 316 */
    long double complex (* catanhl) (long double complex z);                                                                                         /* 320 */
    double complex (* ccosh) (double complex z);                                                                                                     /* 324 */
    float complex (* ccoshf) (float complex z);                                                                                                      /* 328 */
    long double complex (* ccoshl) (long double complex z);                                                                                          /* 332 */
    double complex (* csinh) (double complex z);                                                                                                     /* 336 */
    float complex (* csinhf) (float complex z);                                                                                                      /* 340 */
    long double complex (* csinhl) (long double complex z);                                                                                          /* 344 */
    double complex (* ctanh) (double complex z);                                                                                                     /* 348 */
    float complex (* ctanhf) (float complex z);                                                                                                      /* 352 */
    long double complex (* ctanhl) (long double complex z);                                                                                          /* 356 */
    double complex (* cexp) (double complex z);                                                                                                      /* 360 */
    float complex (* cexpf) (float complex z);                                                                                                       /* 364 */
    long double complex (* cexpl) (long double complex z);                                                                                           /* 368 */
    double complex (* clog) (double complex z);                                                                                                      /* 372 */
    float complex (* clogf) (float complex z);                                                                                                       /* 376 */
    long double complex (* clogl) (long double complex z);                                                                                           /* 380 */
    double (* cabs) (double complex z);                                                                                                              /* 384 */
    float (* cabsf) (float complex z);                                                                                                               /* 388 */
    long double (* cabsl) (long double complex z);                                                                                                   /* 392 */
    double complex (* cpow) (double complex z, double complex c);                                                                                    /* 396 */
    float complex (* cpowf) (float complex z, float complex c);                                                                                      /* 400 */
    long double complex (* cpowl) (long double complex z, long double complex c);                                                                    /* 404 */
    double complex (* csqrt) (double complex z);                                                                                                     /* 408 */
    float complex (* csqrtf) (float complex z);                                                                                                      /* 412 */
    long double complex (* csqrtl) (long double complex z);                                                                                          /* 416 */
    double complex (* carg) (double complex z);                                                                                                      /* 420 */
    float complex (* cargf) (float complex z);                                                                                                       /* 424 */
    long double complex (* cargl) (long double complex z);                                                                                           /* 428 */
    double complex (* conj) (double complex z);                                                                                                      /* 432 */
    float complex (* conjf) (float complex z);                                                                                                       /* 436 */
    long double complex (* conjl) (long double complex z);                                                                                           /* 440 */
    double complex (* cproj) (double complex z);                                                                                                     /* 444 */
    float complex (* cprojf) (float complex z);                                                                                                      /* 448 */
    long double complex (* cprojl) (long double complex z);                                                                                          /* 452 */
    double (* cimag) (double complex z);                                                                                                             /* 456 */
    float (* cimagf) (float complex z);                                                                                                              /* 460 */
    long double (* cimagl) (long double complex z);                                                                                                  /* 464 */
    double (* creal) (double complex z);                                                                                                             /* 468 */
    float (* crealf) (float complex z);                                                                                                              /* 472 */
    long double (* creall) (long double complex z);                                                                                                  /* 476 */

    /* crypt.h */
    char * (* crypt_r) (const char *__phrase, const char *__salt, struct crypt_data * __restrict __data);                                            /* 480 */

    /* ctype.h */
    int (* isalnum) (int c);                                                                                                                         /* 484 */
    int (* isalpha) (int c);                                                                                                                         /* 488 */
    int (* iscntrl) (int c);                                                                                                                         /* 492 */
    int (* isdigit) (int c);                                                                                                                         /* 496 */
    int (* isgraph) (int c);                                                                                                                         /* 500 */
    int (* islower) (int c);                                                                                                                         /* 504 */
    int (* isprint) (int c);                                                                                                                         /* 508 */
    int (* ispunct) (int c);                                                                                                                         /* 512 */
    int (* isspace) (int c);                                                                                                                         /* 516 */
    int (* isupper) (int c);                                                                                                                         /* 520 */
    int (* isxdigit) (int c);                                                                                                                        /* 524 */
    int (* tolower) (int c);                                                                                                                         /* 528 */
    int (* toupper) (int c);                                                                                                                         /* 532 */
    int (* isblank) (int c);                                                                                                                         /* 536 */
    int (* isascii) (int c);                                                                                                                         /* 540 */
    int (* toascii) (int c);                                                                                                                         /* 544 */

    /* dirent.h */
    DIR * (* opendir) (const char * path_name);                                                                                                      /* 548 */
    struct dirent * (* readdir) (DIR * directory_pointer);                                                                                           /* 552 */
    void (* rewinddir) (DIR * directory_pointer);                                                                                                    /* 556 */
    int (* closedir) (DIR * directory_pointer);                                                                                                      /* 560 */
    DIR * (* fdopendir) (int);                                                                                                                       /* 564 */
    int (* alphasort) (const struct dirent **a, const struct dirent **b);                                                                            /* 568 */
    int (* dirfd) (DIR *dirp);                                                                                                                       /* 572 */
    int (* scandir) (const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));      /* 576 */
    int (* readdir_r) (DIR *dir, struct dirent *buf, struct dirent **result);                                                                        /* 580 */
    int (* readdir64_r) (DIR *dir, struct dirent *buf, struct dirent **result);                                                                      /* 584 */

    /* dlfcn.h */
    int (* dlclose) (void *__handle);                                                                                                                /* 588 */
    const char * (* dlerror) (void);                                                                                                                 /* 592 */
    void * (* dlopen) (const char *__path_name, int __mode);                                                                                         /* 596 */
    void * (* dlsym) (void *__handle, const char *__symbol_name);                                                                                    /* 600 */

    /* envz.h */
    char * (* envz_entry) (const char *envz, size_t envz_len, const char *name);                                                                     /* 604 */
    char * (* envz_get) (const char *envz, size_t envz_len, const char *name);                                                                       /* 608 */
    error_t (* envz_add) (char **envz, size_t *envz_len, const char *name, const char *value);                                                       /* 612 */
    error_t (* envz_merge) (char **envz, size_t *envz_len, const char *envz2, size_t envz2_len, int override);                                       /* 616 */
    void (* envz_remove) (char **envz, size_t *envz_len, const char *name);                                                                          /* 620 */
    void (* envz_strip) (char **envz, size_t *envz_len);                                                                                             /* 624 */

    /* err.h */
    void (* warn) (const char *, ...);                                                                                                               /* 628 */
    void (* vwarn) (const char *, va_list);                                                                                                          /* 632 */
    void (* warnx) (const char *, ...);                                                                                                              /* 636 */
    void (* vwarnx) (const char *, va_list);                                                                                                         /* 640 */
    void (* err) (int, const char *, ...);                                                                                                           /* 644 */
    void (* verr) (int, const char *, va_list);                                                                                                      /* 648 */
    void (* errx) (int, const char *, ...);                                                                                                          /* 652 */
    void (* verrx) (int, const char *, va_list);                                                                                                     /* 656 */

    /* fcntl.h */
    int (* open) (const char *path_name, int open_flag, ... /* mode_t mode */ );                                                                     /* 660 */
    int (* openat) (int fd, const char *filename, int flags, ...);                                                                                   /* 664 */
    int (* creat) (const char * path_name, mode_t mode);                                                                                             /* 668 */
    int (* close) (int file_descriptor);                                                                                                             /* 672 */
    ssize_t (* read) (int file_descriptor, void * buffer, size_t num_bytes);                                                                         /* 676 */
    ssize_t (* write) (int file_descriptor, const void * buffer, size_t num_bytes);                                                                  /* 680 */
    int (* fcntl) (int file_descriptor, int cmd, ... /* int arg */ );                                                                                /* 684 */

    /* fnmatch.h */
    int (* fnmatch) (const char *pattern, const char *string, int flags);                                                                            /* 688 */

    /* ftw.h */
    int (* ftw) (const char *path,int (*func)(const char *,const struct stat *,int),int depth);                                                      /* 692 */
    int (* nftw) (const char *path,int (*func)(const char *,const struct stat *,int,struct FTW *),int depth,int flags);                              /* 696 */

    /* getopt.h */
    int (* getopt_long) (int argc, char* const *argv, const char *optstring, const struct option *longopts, int *longindex);                         /* 700 */
    int (* getopt_long_only) (int argc, char* const *argv, const char *optstring, const struct option *longopts, int *longindex);                    /* 704 */

    /* glob.h */
    int (* glob) (const char *pattern, int flags, int (*errfunc) (const char *epath, int eerrno), glob_t *pglob);                                    /* 708 */
    void (* globfree) (glob_t *pglob);                                                                                                               /* 712 */

    /* grp.h */
    void (* endgrent) (void);                                                                                                                        /* 716 */
    struct group * (* getgrent) (void);                                                                                                              /* 720 */
    struct group * (* getgrgid) (gid_t gid);                                                                                                         /* 724 */
    struct group * (* getgrnam) (const char *name);                                                                                                  /* 728 */
    void (* setgrent) (void);                                                                                                                        /* 732 */

    /* iconv.h */
    iconv_t (* iconv_open) (const char *to, const char *from);                                                                                       /* 736 */
    size_t (* iconv) (iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);                                           /* 740 */
    int (* iconv_close) (iconv_t cd);                                                                                                                /* 744 */

    /* ifaddrs.h */
    int (* getifaddrs) (struct ifaddrs **ifap);                                                                                                      /* 748 */
    void (* freeifaddrs) (struct ifaddrs *ifa);                                                                                                      /* 752 */

    /* inttypes.h */
    intmax_t (* imaxabs) (intmax_t x);                                                                                                               /* 756 */
    imaxdiv_t (* imaxdiv) (intmax_t n,intmax_t d);                                                                                                   /* 760 */
    intmax_t (* strtoimax) (const char *str, char **ptr, int base);                                                                                  /* 764 */
    uintmax_t (* strtoumax) (const char *str, char **ptr, int base);                                                                                 /* 768 */
    intmax_t (* wcstoimax) (const wchar_t *str, wchar_t **ptr, int base);                                                                            /* 772 */
    uintmax_t (* wcstoumax) (const wchar_t *str, wchar_t **ptr, int base);                                                                           /* 776 */

    /* langinfo.h */
    char * (* nl_langinfo) (nl_item item);                                                                                                           /* 780 */

    /* libgen.h */
    char * (* basename) (const char *path);                                                                                                          /* 784 */
    char * (* dirname) (const char *path);                                                                                                           /* 788 */

    /* locale.h */
    char * (* setlocale) (int category, const char *locale);                                                                                         /* 792 */
    struct lconv * (* localeconv) (void);                                                                                                            /* 796 */

    /* malloc.h */
    void * (* memalign) (size_t alignment, size_t size);                                                                                             /* 800 */

    /* math.h */
    double (* acos) (double x);                                                                                                                      /* 804 */
    double (* asin) (double x);                                                                                                                      /* 808 */
    double (* atan) (double x);                                                                                                                      /* 812 */
    double (* atan2) (double y, double x);                                                                                                           /* 816 */
    double (* ceil) (double x);                                                                                                                      /* 820 */
    double (* cos) (double x);                                                                                                                       /* 824 */
    double (* cosh) (double x);                                                                                                                      /* 828 */
    void (* sincos) (double x, double *s, double *c);                                                                                                /* 832 */
    void (* sincosf) (float x, float *s, float *c);                                                                                                  /* 836 */
    void (* sincosl) (double x, double *s, double *c);                                                                                               /* 840 */
    double (* exp10) (double x);                                                                                                                     /* 844 */
    float (* exp10f) (float x);                                                                                                                      /* 848 */
    long double (* exp10l) (long double x);                                                                                                          /* 852 */
    double (* exp) (double x);                                                                                                                       /* 856 */
    double (* fabs) (double x);                                                                                                                      /* 860 */
    double (* floor) (double x);                                                                                                                     /* 864 */
    double (* fmod) (double x, double y);                                                                                                            /* 868 */
    double (* frexp) (double x, int *nptr);                                                                                                          /* 872 */
    double (* ldexp) (double x, int n);                                                                                                              /* 876 */
    double (* log) (double x);                                                                                                                       /* 880 */
    double (* log10) (double x);                                                                                                                     /* 884 */
    double (* modf) (double x, double *nptr);                                                                                                        /* 888 */
    double (* pow) (double x, double y);                                                                                                             /* 892 */
    double (* pow10) (double x);                                                                                                                     /* 896 */
    float (* pow10f) (float x);                                                                                                                      /* 900 */
    long double (* pow10l) (long double x);                                                                                                          /* 904 */
    double (* sin) (double x);                                                                                                                       /* 908 */
    double (* sinh) (double x);                                                                                                                      /* 912 */
    double (* sqrt) (double x);                                                                                                                      /* 916 */
    double (* tan) (double x);                                                                                                                       /* 920 */
    double (* tanh) (double x);                                                                                                                      /* 924 */
    double (* j0) (double x);                                                                                                                        /* 928 */
    double (* y0) (double x);                                                                                                                        /* 932 */
    float (* j0f) (float x);                                                                                                                         /* 936 */
    float (* y0f) (float x);                                                                                                                         /* 940 */
    double (* j1) (double x);                                                                                                                        /* 944 */
    double (* y1) (double x);                                                                                                                        /* 948 */
    float (* j1f) (float x);                                                                                                                         /* 952 */
    float (* y1f) (float x);                                                                                                                         /* 956 */
    double (* jn) (int n, double x);                                                                                                                 /* 960 */
    double (* yn) (int n, double x);                                                                                                                 /* 964 */
    float (* jnf) (int n, float x);                                                                                                                  /* 968 */
    float (* ynf) (int n, float x);                                                                                                                  /* 972 */
    int (* __fpclassify_float) (float x);                                                                                                            /* 976 */
    int (* __fpclassify_double) (double x);                                                                                                          /* 980 */
    int (* __fpclassify_long_double) (long double x);                                                                                                /* 984 */
    int (* __isfinite_float) (float x);                                                                                                              /* 988 */
    int (* __isfinite_double) (double x);                                                                                                            /* 992 */
    int (* __isfinite_long_double) (long double x);                                                                                                  /* 996 */
    int (* __signbit_float) (float x);                                                                                                               /* 1000 */
    int (* __signbit_double) (double x);                                                                                                             /* 1004 */
    int (* __signbit_long_double) (long double x);                                                                                                   /* 1008 */
    int (* __isnan) (double x);                                                                                                                      /* 1012 */
    int (* __isnanf) (float x);                                                                                                                      /* 1016 */
    int (* __isnanl) (long double x);                                                                                                                /* 1020 */
    int (* __isinf) (double x);                                                                                                                      /* 1024 */
    int (* __isinff) (float x);                                                                                                                      /* 1028 */
    int (* __isinfl) (long double x);                                                                                                                /* 1032 */
    int (* __isnormalf) (float x);                                                                                                                   /* 1036 */
    int (* __isnormal) (double x);                                                                                                                   /* 1040 */
    int (* __isnormall) (long double x);                                                                                                             /* 1044 */
    float (* acosf) (float x);                                                                                                                       /* 1048 */
    float (* asinf) (float x);                                                                                                                       /* 1052 */
    float (* atanf) (float x);                                                                                                                       /* 1056 */
    float (* atan2f) (float y, float x);                                                                                                             /* 1060 */
    float (* ceilf) (float x);                                                                                                                       /* 1064 */
    float (* cosf) (float x);                                                                                                                        /* 1068 */
    float (* coshf) (float x);                                                                                                                       /* 1072 */
    float (* expf) (float x);                                                                                                                        /* 1076 */
    float (* fabsf) (float x);                                                                                                                       /* 1080 */
    float (* floorf) (float x);                                                                                                                      /* 1084 */
    float (* fmodf) (float x, float y);                                                                                                              /* 1088 */
    float (* frexpf) (float x, int *eptr);                                                                                                           /* 1092 */
    float (* ldexpf) (float x, int exp);                                                                                                             /* 1096 */
    float (* logf) (float x);                                                                                                                        /* 1100 */
    float (* log10f) (float x);                                                                                                                      /* 1104 */
    float (* modff) (float x, float *iptr);                                                                                                          /* 1108 */
    float (* powf) (float x, float y);                                                                                                               /* 1112 */
    float (* sinf) (float x);                                                                                                                        /* 1116 */
    float (* sinhf) (float x);                                                                                                                       /* 1120 */
    float (* sqrtf) (float x);                                                                                                                       /* 1124 */
    float (* tanf) (float x);                                                                                                                        /* 1128 */
    float (* tanhf) (float x);                                                                                                                       /* 1132 */
    float (* acoshf) (float x);                                                                                                                      /* 1136 */
    float (* asinhf) (float x);                                                                                                                      /* 1140 */
    float (* atanhf) (float x);                                                                                                                      /* 1144 */
    float (* cbrtf) (float x);                                                                                                                       /* 1148 */
    float (* copysignf) (float x, float y);                                                                                                          /* 1152 */
    float (* erfcf) (float x);                                                                                                                       /* 1156 */
    float (* erff) (float x);                                                                                                                        /* 1160 */
    float (* expm1f) (float x);                                                                                                                      /* 1164 */
    float (* fdimf) (float x, float y);                                                                                                              /* 1168 */
    float (* fmaf) (float x, float y, float z);                                                                                                      /* 1172 */
    float (* fmaxf) (float x, float y);                                                                                                              /* 1176 */
    float (* fminf) (float x, float y);                                                                                                              /* 1180 */
    float (* hypotf) (float x, float y);                                                                                                             /* 1184 */
    float (* lgammaf) (float x);                                                                                                                     /* 1188 */
    float (* log1pf) (float x);                                                                                                                      /* 1192 */
    float (* logbf) (float x);                                                                                                                       /* 1196 */
    long long (* llrintf) (float x);                                                                                                                 /* 1200 */
    long (* lrintf) (float x);                                                                                                                       /* 1204 */
    long (* lroundf) (float x);                                                                                                                      /* 1208 */
    long long (* llround) (double x);                                                                                                                /* 1212 */
    long long (* llroundf) (float x);                                                                                                                /* 1216 */
    float (* nanf) (const char *tagp);                                                                                                               /* 1220 */
    float (* nearbyintf) (float x);                                                                                                                  /* 1224 */
    float (* nextafterf) (float x, float y);                                                                                                         /* 1228 */
    float (* nexttowardf) (float x, long double y);                                                                                                  /* 1232 */
    float (* remainderf) (float x, float p);                                                                                                         /* 1236 */
    float (* remquof) (float x, float y, int *quo);                                                                                                  /* 1240 */
    float (* rintf) (float x);                                                                                                                       /* 1244 */
    float (* roundf) (float x);                                                                                                                      /* 1248 */
    float (* scalblnf) (float x, long n);                                                                                                            /* 1252 */
    float (* scalbnf) (float x, int n);                                                                                                              /* 1256 */
    float (* tgammaf) (float x);                                                                                                                     /* 1260 */
    float (* truncf) (float x);                                                                                                                      /* 1264 */
    int (* ilogbf) (float x);                                                                                                                        /* 1268 */
    int (* finite) (double x);                                                                                                                       /* 1272 */
    double (* acosh) (double x);                                                                                                                     /* 1276 */
    double (* asinh) (double x);                                                                                                                     /* 1280 */
    double (* atanh) (double x);                                                                                                                     /* 1284 */
    double (* cbrt) (double x);                                                                                                                      /* 1288 */
    double (* copysign) (double x, double y);                                                                                                        /* 1292 */
    double (* erf) (double x);                                                                                                                       /* 1296 */
    double (* erfc) (double x);                                                                                                                      /* 1300 */
    double (* expm1) (double x);                                                                                                                     /* 1304 */
    double (* fdim) (double x, double y);                                                                                                            /* 1308 */
    double (* fma) (double x, double y, double z);                                                                                                   /* 1312 */
    double (* fmax) (double x, double y);                                                                                                            /* 1316 */
    double (* fmin) (double x, double y);                                                                                                            /* 1320 */
    double (* hypot) (double x, double y);                                                                                                           /* 1324 */
    double (* lgamma) (double x);                                                                                                                    /* 1328 */
    double (* log1p) (double x);                                                                                                                     /* 1332 */
    double (* logb) (double x);                                                                                                                      /* 1336 */
    long long (* llrint) (double x);                                                                                                                 /* 1340 */
    long int (* lrint) (double x);                                                                                                                   /* 1344 */
    long int (* lround) (double x);                                                                                                                  /* 1348 */
    double (* nan) (const char *tagp);                                                                                                               /* 1352 */
    double (* nearbyint) (double x);                                                                                                                 /* 1356 */
    double (* nexttoward) (double x, long double y);                                                                                                 /* 1360 */
    double (* nextafter) (double x, double y);                                                                                                       /* 1364 */
    double (* remainder) (double x, double p);                                                                                                       /* 1368 */
    double (* remquo) (double x, double y, int *quo);                                                                                                /* 1372 */
    double (* rint) (double x);                                                                                                                      /* 1376 */
    double (* round) (double x);                                                                                                                     /* 1380 */
    double (* scalbln) (double x, long n);                                                                                                           /* 1384 */
    double (* scalbn) (double x, int n);                                                                                                             /* 1388 */
    double (* tgamma) (double x);                                                                                                                    /* 1392 */
    double (* trunc) (double x);                                                                                                                     /* 1396 */
    int (* ilogb) (double x);                                                                                                                        /* 1400 */
    float (* exp2f) (float x);                                                                                                                       /* 1404 */
    double (* exp2) (double x);                                                                                                                      /* 1408 */
    long double (* exp2l) (long double x);                                                                                                           /* 1412 */
    double (* log2) (double x);                                                                                                                      /* 1416 */
    float (* log2f) (float x);                                                                                                                       /* 1420 */
    long double (* log2l) (long double x);                                                                                                           /* 1424 */
    long double (* acosl) (long double x);                                                                                                           /* 1428 */
    long double (* asinl) (long double x);                                                                                                           /* 1432 */
    long double (* atanl) (long double x);                                                                                                           /* 1436 */
    long double (* atan2l) (long double y, long double x);                                                                                           /* 1440 */
    long double (* ceill) (long double x);                                                                                                           /* 1444 */
    long double (* cosl) (long double x);                                                                                                            /* 1448 */
    long double (* coshl) (long double x);                                                                                                           /* 1452 */
    long double (* expl) (long double x);                                                                                                            /* 1456 */
    long double (* fabsl) (long double x);                                                                                                           /* 1460 */
    long double (* floorl) (long double x);                                                                                                          /* 1464 */
    long double (* fmodl) (long double x, long double y);                                                                                            /* 1468 */
    long double (* frexpl) (long double x, int *nptr);                                                                                               /* 1472 */
    long double (* ldexpl) (long double x, int n);                                                                                                   /* 1476 */
    long double (* logl) (long double x);                                                                                                            /* 1480 */
    long double (* log10l) (long double x);                                                                                                          /* 1484 */
    long double (* modfl) (long double x, long double *iptr);                                                                                        /* 1488 */
    long double (* powl) (long double x, long double y);                                                                                             /* 1492 */
    long double (* sinl) (long double x);                                                                                                            /* 1496 */
    long double (* sinhl) (long double x);                                                                                                           /* 1500 */
    long double (* sqrtl) (long double x);                                                                                                           /* 1504 */
    long double (* tanl) (long double x);                                                                                                            /* 1508 */
    long double (* tanhl) (long double x);                                                                                                           /* 1512 */
    long double (* acoshl) (long double x);                                                                                                          /* 1516 */
    long double (* asinhl) (long double x);                                                                                                          /* 1520 */
    long double (* atanhl) (long double x);                                                                                                          /* 1524 */
    long double (* cbrtl) (long double x);                                                                                                           /* 1528 */
    long double (* copysignl) (long double x, long double y);                                                                                        /* 1532 */
    long double (* erfcl) (long double x);                                                                                                           /* 1536 */
    long double (* erfl) (long double x);                                                                                                            /* 1540 */
    long double (* expm1l) (long double x);                                                                                                          /* 1544 */
    long double (* fdiml) (long double x, long double y);                                                                                            /* 1548 */
    long double (* fmal) (long double x, long double y, long double z);                                                                              /* 1552 */
    long double (* fmaxl) (long double x, long double y);                                                                                            /* 1556 */
    long double (* fminl) (long double x, long double y);                                                                                            /* 1560 */
    long double (* hypotl) (long double x, long double y);                                                                                           /* 1564 */
    long double (* lgammal) (long double x);                                                                                                         /* 1568 */
    long double (* log1pl) (long double x);                                                                                                          /* 1572 */
    long double (* logbl) (long double x);                                                                                                           /* 1576 */
    long long int (* llrintl) (long double x);                                                                                                       /* 1580 */
    long int (* lrintl) (long double x);                                                                                                             /* 1584 */
    long long int (* llroundl) (long double x);                                                                                                      /* 1588 */
    long int (* lroundl) (long double x);                                                                                                            /* 1592 */
    long double (* nanl) (const char *tagp);                                                                                                         /* 1596 */
    long double (* nearbyintl) (long double x);                                                                                                      /* 1600 */
    long double (* nextafterl) (long double x, long double y);                                                                                       /* 1604 */
    long double (* nexttowardl) (long double x, long double y);                                                                                      /* 1608 */
    long double (* remainderl) (long double x, long double p);                                                                                       /* 1612 */
    long double (* remquol) (long double x, long double y, int *quo);                                                                                /* 1616 */
    long double (* rintl) (long double x);                                                                                                           /* 1620 */
    long double (* roundl) (long double x);                                                                                                          /* 1624 */
    long double (* scalblnl) (long double x, long int n);                                                                                            /* 1628 */
    long double (* scalbnl) (long double x, int n);                                                                                                  /* 1632 */
    long double (* tgammal) (long double x);                                                                                                         /* 1636 */
    long double (* truncl) (long double x);                                                                                                          /* 1640 */
    int (* ilogbl) (long double x);                                                                                                                  /* 1644 */

    /* netdb.h */
    struct hostent * (* gethostbyaddr) (const void *addr, socklen_t len, int type);                                                                  /* 1648 */
    struct hostent * (* gethostbyname) (const char *name);                                                                                           /* 1652 */
    struct netent * (* getnetbyaddr) (in_addr_t net, int type);                                                                                      /* 1656 */
    struct netent * (* getnetbyname) (const char *name);                                                                                             /* 1660 */
    struct protoent * (* getprotobyname) (const char *name);                                                                                         /* 1664 */
    struct protoent * (* getprotobynumber) (int proto);                                                                                              /* 1668 */
    struct servent * (* getservbyname) (const char *name, const char *proto);                                                                        /* 1672 */
    struct servent * (* getservbyport) (int port, const char *proto);                                                                                /* 1676 */
    void (* herror) (const char *);                                                                                                                  /* 1680 */
    const char * (* hstrerror) (int);                                                                                                                /* 1684 */
    int (* gethostbyname_r) (const char *, struct hostent *, char *, size_t, struct hostent **, int *);                                              /* 1688 */
    int (* gethostbyname2_r) (const char *, int, struct hostent *, char *, size_t, struct hostent **, int *);                                        /* 1692 */
    struct hostent * (* gethostbyname2) (const char *, int);                                                                                         /* 1696 */
    int (* gethostbyaddr_r) (const void *, socklen_t, int, struct hostent *, char *, size_t, struct hostent **, int *);                              /* 1700 */
    int (* getservbyport_r) (int, const char *, struct servent *, char *, size_t, struct servent **);                                                /* 1704 */
    int (* getservbyname_r) (const char *, const char *, struct servent *, char *, size_t, struct servent **);                                       /* 1708 */
    const char * (* gai_strerror) (int ecode);                                                                                                       /* 1712 */
    int (* getaddrinfo) (const char *name, const char *service, const struct addrinfo *req, struct addrinfo **pai);                                  /* 1716 */
    void (* freeaddrinfo) (struct addrinfo *ai);                                                                                                     /* 1720 */
    int (* getnameinfo) (const struct sockaddr *sa, socklen_t salen, char *node, socklen_t nodelen, char *service, socklen_t servicelen, int flags); /* 1724 */

    /* nl_types.h */
    nl_catd (* catopen) (const char* name, int flag);                                                                                                /* 1728 */
    char* (* catgets) (nl_catd catalog, int set_number, int msg_number, const char* msg);                                                            /* 1732 */
    int (* catclose) (nl_catd catalog);                                                                                                              /* 1736 */

    /* poll.h */
    int (* poll) (struct pollfd *fds, nfds_t nfds, int timeout);                                                                                     /* 1740 */

    /* pthread.h */
    int (* pthread_attr_init) (pthread_attr_t *attr);                                                                                                /* 1744 */
    int (* pthread_attr_destroy) (pthread_attr_t *attr);                                                                                             /* 1748 */
    int (* pthread_attr_getdetachstate) (const pthread_attr_t *attr, int *detachstate);                                                              /* 1752 */
    int (* pthread_attr_setdetachstate) (pthread_attr_t *attr, int detachstate);                                                                     /* 1756 */
    int (* pthread_attr_getstack) (const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);                                                 /* 1760 */
    int (* pthread_attr_setstack) (pthread_attr_t *attr, void *stackaddr, size_t stacksize);                                                         /* 1764 */
    int (* pthread_attr_getstacksize) (const pthread_attr_t *attr, size_t *stacksize);                                                               /* 1768 */
    int (* pthread_attr_setstacksize) (pthread_attr_t *attr, size_t stacksize);                                                                      /* 1772 */
    int (* pthread_attr_getschedparam) (const pthread_attr_t *attr, struct sched_param *param);                                                      /* 1776 */
    int (* pthread_attr_setschedparam) (pthread_attr_t *attr, const struct sched_param *param);                                                      /* 1780 */
    int (* pthread_attr_getschedpolicy) (const pthread_attr_t *attr, int *policy);                                                                   /* 1784 */
    int (* pthread_attr_setschedpolicy) (pthread_attr_t *attr, int policy);                                                                          /* 1788 */
    int (* pthread_attr_getinheritsched) (const pthread_attr_t *attr, int *inheritsched);                                                            /* 1792 */
    int (* pthread_attr_setinheritsched) (pthread_attr_t *attr, int inheritsched);                                                                   /* 1796 */
    int (* pthread_attr_getscope) (const pthread_attr_t *attr, int *contentionscope);                                                                /* 1800 */
    int (* pthread_attr_setscope) (pthread_attr_t *attr, int contentionscope);                                                                       /* 1804 */
    int (* pthread_attr_setguardsize) (pthread_attr_t *attr, size_t guardsize);                                                                      /* 1808 */
    int (* pthread_attr_getguardsize) (const pthread_attr_t *attr, size_t *guardsize);                                                               /* 1812 */
    int (* pthread_create) (pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);                                       /* 1816 */
    int (* pthread_detach) (pthread_t thread);                                                                                                       /* 1820 */
    int (* pthread_equal) (pthread_t t1, pthread_t t2);                                                                                              /* 1824 */
    void (* pthread_exit) (void *value_ptr);                                                                                                         /* 1828 */
    int (* pthread_join) (pthread_t thread, void **value_ptr);                                                                                       /* 1832 */
    pthread_t (* pthread_self) (void);                                                                                                               /* 1836 */
    int (* pthread_cancel) (pthread_t thread);                                                                                                       /* 1840 */
    int (* pthread_setcancelstate) (int state, int *oldstate);                                                                                       /* 1844 */
    int (* pthread_setcanceltype) (int type, int *oldtype);                                                                                          /* 1848 */
    void (* pthread_testcancel) (void);                                                                                                              /* 1852 */
    int (* pthread_once) (pthread_once_t *once_control, void (*init_routine)(void));                                                                 /* 1856 */
    int (* pthread_setschedprio) (pthread_t thread, int prio);                                                                                       /* 1860 */
    int (* pthread_setschedparam) (pthread_t thread, int policy, const struct sched_param *param);                                                   /* 1864 */
    int (* pthread_getschedparam) (pthread_t thread, int *policy, struct sched_param *param);                                                        /* 1868 */
    int (* pthread_setconcurrency) (int level);                                                                                                      /* 1872 */
    int (* pthread_getconcurrency) (void);                                                                                                           /* 1876 */
    int (* pthread_key_create) (pthread_key_t *key, void (*destructor)(void *));                                                                     /* 1880 */
    int (* pthread_key_delete) (pthread_key_t key);                                                                                                  /* 1884 */
    int (* pthread_setspecific) (pthread_key_t key, const void *value);                                                                              /* 1888 */
    void * (* pthread_getspecific) (pthread_key_t key);                                                                                              /* 1892 */
    int (* pthread_mutexattr_init) (pthread_mutexattr_t *attr);                                                                                      /* 1896 */
    int (* pthread_mutexattr_destroy) (pthread_mutexattr_t *attr);                                                                                   /* 1900 */
    int (* pthread_mutexattr_getpshared) (const pthread_mutexattr_t *attr, int *pshared);                                                            /* 1904 */
    int (* pthread_mutexattr_setpshared) (pthread_mutexattr_t *attr, int pshared);                                                                   /* 1908 */
    int (* pthread_mutexattr_gettype) (pthread_mutexattr_t *attr, int *kind);                                                                        /* 1912 */
    int (* pthread_mutexattr_settype) (pthread_mutexattr_t *attr, int kind);                                                                         /* 1916 */
    int (* pthread_mutex_init) (pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);                                                            /* 1920 */
    int (* pthread_mutex_destroy) (pthread_mutex_t *mutex);                                                                                          /* 1924 */
    int (* pthread_mutex_lock) (pthread_mutex_t *mutex);                                                                                             /* 1928 */
    int (* pthread_mutex_timedlock) (pthread_mutex_t *mutex, const struct timespec *abstime);                                                        /* 1932 */
    int (* pthread_mutex_trylock) (pthread_mutex_t *mutex);                                                                                          /* 1936 */
    int (* pthread_mutex_unlock) (pthread_mutex_t *mutex);                                                                                           /* 1940 */
    int (* pthread_condattr_init) (pthread_condattr_t *attr);                                                                                        /* 1944 */
    int (* pthread_condattr_destroy) (pthread_condattr_t *attr);                                                                                     /* 1948 */
    int (* pthread_condattr_getpshared) (const pthread_condattr_t *attr, int *pshared);                                                              /* 1952 */
    int (* pthread_condattr_setpshared) (pthread_condattr_t *attr, int pshared);                                                                     /* 1956 */
    int (* pthread_cond_init) (pthread_cond_t *cond, const pthread_condattr_t *attr);                                                                /* 1960 */
    int (* pthread_cond_destroy) (pthread_cond_t *cond);                                                                                             /* 1964 */
    int (* pthread_cond_wait) (pthread_cond_t *cond, pthread_mutex_t *mutex);                                                                        /* 1968 */
    int (* pthread_cond_timedwait) (pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);                                   /* 1972 */
    int (* pthread_cond_signal) (pthread_cond_t *cond);                                                                                              /* 1976 */
    int (* pthread_cond_broadcast) (pthread_cond_t *cond);                                                                                           /* 1980 */
    int (* pthread_barrierattr_init) (pthread_barrierattr_t *attr);                                                                                  /* 1984 */
    int (* pthread_barrierattr_destroy) (pthread_barrierattr_t *attr);                                                                               /* 1988 */
    int (* pthread_barrierattr_getpshared) (const pthread_barrierattr_t *attr, int *pshared);                                                        /* 1992 */
    int (* pthread_barrierattr_setpshared) (pthread_barrierattr_t *attr, int pshared);                                                               /* 1996 */
    int (* pthread_barrier_init) (pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);                                /* 2000 */
    int (* pthread_barrier_destroy) (pthread_barrier_t *barrier);                                                                                    /* 2004 */
    int (* pthread_barrier_wait) (pthread_barrier_t *barrier);                                                                                       /* 2008 */
    int (* pthread_rwlockattr_init) (pthread_rwlockattr_t *attr);                                                                                    /* 2012 */
    int (* pthread_rwlockattr_destroy) (pthread_rwlockattr_t *attr);                                                                                 /* 2016 */
    int (* pthread_rwlockattr_getpshared) (const pthread_rwlockattr_t *attr, int *pshared);                                                          /* 2020 */
    int (* pthread_rwlockattr_setpshared) (pthread_rwlockattr_t *attr, int pshared);                                                                 /* 2024 */
    int (* pthread_rwlock_init) (pthread_rwlock_t *lock, const pthread_rwlockattr_t *attr);                                                          /* 2028 */
    int (* pthread_rwlock_destroy) (pthread_rwlock_t *lock);                                                                                         /* 2032 */
    int (* pthread_rwlock_tryrdlock) (pthread_rwlock_t *lock);                                                                                       /* 2036 */
    int (* pthread_rwlock_trywrlock) (pthread_rwlock_t *lock);                                                                                       /* 2040 */
    int (* pthread_rwlock_rdlock) (pthread_rwlock_t *lock);                                                                                          /* 2044 */
    int (* pthread_rwlock_timedrdlock) (pthread_rwlock_t *lock, const struct timespec *abstime);                                                     /* 2048 */
    int (* pthread_rwlock_wrlock) (pthread_rwlock_t *lock);                                                                                          /* 2052 */
    int (* pthread_rwlock_timedwrlock) (pthread_rwlock_t *lock, const struct timespec *abstime);                                                     /* 2056 */
    int (* pthread_rwlock_unlock) (pthread_rwlock_t *lock);                                                                                          /* 2060 */
    int (* pthread_spin_init) (pthread_spinlock_t *lock, int pshared);                                                                               /* 2064 */
    int (* pthread_spin_destroy) (pthread_spinlock_t *lock);                                                                                         /* 2068 */
    int (* pthread_spin_lock) (pthread_spinlock_t *lock);                                                                                            /* 2072 */
    int (* pthread_spin_trylock) (pthread_spinlock_t *lock);                                                                                         /* 2076 */
    int (* pthread_spin_unlock) (pthread_spinlock_t *lock);                                                                                          /* 2080 */
    int (* pthread_setname_np) (pthread_t thread, const char *name);                                                                                 /* 2084 */
    int (* pthread_getname_np) (pthread_t thread, char *name, size_t len);                                                                           /* 2088 */
    int (* pthread_cond_timedwait_relative_np) (pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *reltime);                       /* 2092 */
    int (* pthread_getattr_np) (pthread_t thread, pthread_attr_t *attr);                                                                             /* 2096 */
    void (* pthread_cleanup_push) (void (*routine)(void *), void *arg);                                                                              /* 2100 */
    void (* pthread_cleanup_pop) (int execute);                                                                                                      /* 2104 */
    int (* pthread_kill) (pthread_t thread, int sig);                                                                                                /* 2108 */

    /* pwd.h */
    void (* endpwent) (void);                                                                                                                        /* 2112 */
    struct passwd * (* getpwent) (void);                                                                                                             /* 2116 */
    struct passwd * (* getpwnam) (const char *name);                                                                                                 /* 2120 */
    int (* getpwnam_r) (const char *name, struct passwd *pw, char *buffer, size_t bufsize, struct passwd **result);                                  /* 2124 */
    struct passwd * (* getpwuid) (uid_t uid);                                                                                                        /* 2128 */
    int (* getpwuid_r) (uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);                                        /* 2132 */
    void (* setpwent) (void);                                                                                                                        /* 2136 */

    /* regex.h */
    int (* regcomp) (regex_t * preg, const char * regex, int cflags);                                                                                /* 2140 */
    int (* regexec) (const regex_t *, const char *, size_t, regmatch_t *, int);                                                                      /* 2144 */
    void (* regfree) (regex_t *preg);                                                                                                                /* 2148 */
    size_t (* regerror) (int e, const regex_t * preg, char * buf, size_t size);                                                                      /* 2152 */

    /* resolv.h */
    int (* res_init) (void);                                                                                                                         /* 2156 */
    int (* res_query) (const char *dname, int class, int type, unsigned char *answer, int anslen);                                                   /* 2160 */
    int (* res_querydomain) (const char *name, const char *domain,int class, int type, unsigned char *answer,int anslen);                            /* 2164 */
    int (* res_search) (const char *dname, int class, int type, unsigned char *answer, int anslen);                                                  /* 2168 */
    int (* res_mkquery) (int, const char *, int, int, const unsigned char *, int, const unsigned char *, unsigned char *, int);                      /* 2172 */
    int (* res_send) (const unsigned char *msg, int msglen, unsigned char *answer,int anslen);                                                       /* 2176 */
    int (* dn_comp) (unsigned char *exp_dn, unsigned char *comp_dn,int length, unsigned char **dnptrs, unsigned char **lastdnptr);                   /* 2180 */
    int (* dn_expand) (unsigned char *msg, unsigned char *eomorig,unsigned char *comp_dn, char *exp_dn,int length);                                  /* 2184 */
    int (* dn_skipname) (const unsigned char *, const unsigned char *);                                                                              /* 2188 */

    /* sched.h */
    int (* sched_get_priority_max) (int);                                                                                                            /* 2192 */
    int (* sched_get_priority_min) (int);                                                                                                            /* 2196 */
    int (* sched_yield) (void);                                                                                                                      /* 2200 */

    /* search.h */
    int (* hcreate) (size_t);                                                                                                                        /* 2204 */
    void (* hdestroy) (void);                                                                                                                        /* 2208 */
    ENTRY * (* hsearch) (ENTRY, ACTION);                                                                                                             /* 2212 */
    int (* hcreate_r) (size_t, struct hsearch_data *);                                                                                               /* 2216 */
    void (* hdestroy_r) (struct hsearch_data *);                                                                                                     /* 2220 */
    int (* hsearch_r) (ENTRY, ACTION, ENTRY **, struct hsearch_data *);                                                                              /* 2224 */
    void * (* lfind) (const void *key, const void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                     /* 2228 */
    void * (* lsearch) (const void *key, void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                         /* 2232 */
    void * (* tdelete) (const void *vkey, void **vrootp, int (*compar)(const void *, const void *));                                                 /* 2236 */
    void (* tdestroy) (void *vrootp, void (*freefct)(void *));                                                                                       /* 2240 */
    void * (* tfind) (const void *, void **, int (*)(const void *, const void *));                                                                   /* 2244 */
    void * (* tsearch) (const void *, void **, int (*)(const void *, const void *));                                                                 /* 2248 */
    void (* twalk) (const void *root, void (*action)(const void *, VISIT, int));                                                                     /* 2252 */

    /* semaphore.h */
    int (* sem_init) (sem_t *sem, int pshared, unsigned int value);                                                                                  /* 2256 */
    int (* sem_destroy) (sem_t *sem);                                                                                                                /* 2260 */
    int (* sem_trywait) (sem_t *sem);                                                                                                                /* 2264 */
    int (* sem_wait) (sem_t *sem);                                                                                                                   /* 2268 */
    int (* sem_timedwait) (sem_t *sem, const struct timespec *abstime);                                                                              /* 2272 */
    int (* sem_post) (sem_t *sem);                                                                                                                   /* 2276 */
    sem_t * (* sem_open) (const char *name, int oflag, mode_t mode, unsigned int value);                                                             /* 2280 */
    int (* sem_close) (sem_t *sem);                                                                                                                  /* 2284 */
    int (* sem_unlink) (const char *name);                                                                                                           /* 2288 */
    int (* sem_getvalue) (sem_t *sem, int *sval);                                                                                                    /* 2292 */

    /* setjmp.h */
    int (* setjmp) (jmp_buf __env);                                                                                                                  /* 2296 */
    int (* __sigsetjmp) (struct __jmp_buf_tag __env[1], int __savemask);                                                                             /* 2300 */
    int (* _setjmp) (struct __jmp_buf_tag __env[1]);                                                                                                 /* 2304 */
    void (* longjmp) (jmp_buf __env, int __val) __attribute__ ((__noreturn__));                                                                      /* 2308 */
    void (* _longjmp) (struct __jmp_buf_tag __env[1], int __val) __attribute__ ((__noreturn__));                                                     /* 2312 */
    void (* siglongjmp) (sigjmp_buf __env, int __val) __attribute__ ((__noreturn__));                                                                /* 2316 */
    int (* __sigjmp_save) (jmp_buf __env, int __savemask);                                                                                           /* 2320 */

    /* signal.h */
    _sig_func_ptr (* signal) (int, _sig_func_ptr);;                                                                                                  /* 2324 */
    int (* raise) (int sig);                                                                                                                         /* 2328 */
    int (* sigmask) (int signum);                                                                                                                    /* 2332 */
    int (* sigblock) (int signal_mask);                                                                                                              /* 2336 */
    int (* sigsetmask) (int signal_mask);                                                                                                            /* 2340 */
    int (* sigprocmask) (int how, const sigset_t *set, sigset_t *oset);                                                                              /* 2344 */
    int (* sigismember) (const sigset_t *set, int sig);                                                                                              /* 2348 */
    int (* sigemptyset) (sigset_t *set);                                                                                                             /* 2352 */
    int (* sigfillset) (sigset_t *set);                                                                                                              /* 2356 */
    int (* sigdelset) (sigset_t *set, int sig);                                                                                                      /* 2360 */
    int (* sigaddset) (sigset_t *set, int sig);                                                                                                      /* 2364 */
    int (* kill) (pid_t pid, int signal_number);                                                                                                     /* 2368 */
    int (* sigaction) (int sig, const struct sigaction *act, struct sigaction *oact);                                                                /* 2372 */

    /* stdio.h */
    void (* perror) (const char *s);                                                                                                                 /* 2376 */
    FILE * (* fopen) (const char *filename, const char *mode);                                                                                       /* 2380 */
    int (* fclose) (FILE *stream);                                                                                                                   /* 2384 */
    int (* fflush) (FILE *stream);                                                                                                                   /* 2388 */
    FILE * (* freopen) (const char *filename, const char *mode, FILE *stream);                                                                       /* 2392 */
    int (* setvbuf) (FILE *stream, char *buf, int bufmode, size_t size);                                                                             /* 2396 */
    void (* setbuf) (FILE *stream, char *buf);                                                                                                       /* 2400 */
    int (* fseek) (FILE *stream, long int offset, int wherefrom);                                                                                    /* 2404 */
    long int (* ftell) (FILE *stream);                                                                                                               /* 2408 */
    void (* rewind) (FILE *stream);                                                                                                                  /* 2412 */
    int (* fgetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 2416 */
    int (* fsetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 2420 */
    int (* fgetc) (FILE *stream);                                                                                                                    /* 2424 */
    int (* getc) (FILE *stream);                                                                                                                     /* 2428 */
    int (* getchar) (void);                                                                                                                          /* 2432 */
    int (* ungetc) (int c, FILE *stream);                                                                                                            /* 2436 */
    int (* fputc) (int c, FILE *stream);                                                                                                             /* 2440 */
    int (* putc) (int c, FILE *stream);                                                                                                              /* 2444 */
    int (* putchar) (int c);                                                                                                                         /* 2448 */
    char * (* fgets) (char *s, int n, FILE *stream);                                                                                                 /* 2452 */
    char * (* gets) (char *s);                                                                                                                       /* 2456 */
    int (* fscanf) (FILE *stream, const char *format, ...);                                                                                          /* 2460 */
    int (* scanf) (const char *format, ...);                                                                                                         /* 2464 */
    int (* sscanf) (const char *s, const char *format, ...);                                                                                         /* 2468 */
    int (* fputs) (const char *s, FILE *stream);                                                                                                     /* 2472 */
    int (* puts) (const char *s);                                                                                                                    /* 2476 */
    int (* dprintf) (int fd, const char *format, ...);                                                                                               /* 2480 */
    int (* fprintf) (FILE *stream, const char *format, ...);                                                                                         /* 2484 */
    int (* printf) (const char *format, ...);                                                                                                        /* 2488 */
    int (* sprintf) (char *s, const char *format, ...);                                                                                              /* 2492 */
    int (* vdprintf) (int fd, const char *format, va_list arg);                                                                                      /* 2496 */
    int (* vfprintf) (FILE *stream, const char *format, va_list arg);                                                                                /* 2500 */
    int (* vprintf) (const char *format, va_list arg);                                                                                               /* 2504 */
    int (* vsprintf) (char *s, const char *format, va_list arg);                                                                                     /* 2508 */
    size_t (* fread) (void *ptr, size_t element_size, size_t count, FILE *stream);                                                                   /* 2512 */
    size_t (* fwrite) (const void *ptr, size_t element_size, size_t count, FILE *stream);                                                            /* 2516 */
    int (* feof) (FILE *stream);                                                                                                                     /* 2520 */
    int (* ferror) (FILE *stream);                                                                                                                   /* 2524 */
    void (* clearerr) (FILE *stream);                                                                                                                /* 2528 */
    int (* rename) (const char *oldname, const char *newname);                                                                                       /* 2532 */
    int (* remove) (const char *filename);                                                                                                           /* 2536 */
    FILE * (* tmpfile) (void);                                                                                                                       /* 2540 */
    char * (* tmpnam) (char *buf);                                                                                                                   /* 2544 */
    char * (* tempnam) (const char *dir, const char *pfx);                                                                                           /* 2548 */
    char * (* ctermid) (char *);                                                                                                                     /* 2552 */
    int (* __flush) (FILE *stream);                                                                                                                  /* 2556 */
    int (* __unlockfile) (FILE *stream, int c);                                                                                                      /* 2560 */
    int (* getc_unlocked) (FILE *stream);                                                                                                            /* 2564 */
    int (* getchar_unlocked) (void);                                                                                                                 /* 2568 */
    int (* putc_unlocked) (int c, FILE *stream);                                                                                                     /* 2572 */
    int (* putchar_unlocked) (int c);                                                                                                                /* 2576 */
    ssize_t (* getline) (char **lineptr, size_t *n, FILE *stream);                                                                                   /* 2580 */
    ssize_t (* getdelim) (char **lineptr, size_t *n, int delim, FILE *stream);                                                                       /* 2584 */
    FILE * (* fdopen) (int file_descriptor, const char *type);                                                                                       /* 2588 */
    int (* fileno) (FILE *file);                                                                                                                     /* 2592 */
    int (* asprintf) (char **ret, const char *format, ...);                                                                                          /* 2596 */
    int (* vsnprintf) (char *s, size_t size, const char *format, va_list arg);                                                                       /* 2600 */
    int (* pclose) (FILE *stream);                                                                                                                   /* 2604 */
    FILE * (* popen) (const char *command, const char *type);                                                                                        /* 2608 */
    int (* fseeko) (FILE *stream, off_t offset, int wherefrom);                                                                                      /* 2612 */
    off_t (* ftello) (FILE *stream);                                                                                                                 /* 2616 */
    void (* flockfile) (FILE *file);                                                                                                                 /* 2620 */
    void (* funlockfile) (FILE *file);                                                                                                               /* 2624 */
    int (* ftrylockfile) (FILE *file);                                                                                                               /* 2628 */
    int (* vasprintf) (char **ret, const char *format, va_list arg);                                                                                 /* 2632 */
    int (* vfscanf) (FILE *stream, const char *format, va_list arg);                                                                                 /* 2636 */
    int (* vsscanf) (const char *s, const char *format, va_list arg);                                                                                /* 2640 */
    int (* vscanf) (const char *format, va_list arg);                                                                                                /* 2644 */
    int (* snprintf) (char *s, size_t size, const char *format, ...);                                                                                /* 2648 */

    /* stdio_ext.h */
    void (* _flushlbf) (void);                                                                                                                       /* 2652 */
    int (* __fsetlocking) (FILE *, int);                                                                                                             /* 2656 */
    int (* __fwriting) (FILE *);                                                                                                                     /* 2660 */
    int (* __freading) (FILE *);                                                                                                                     /* 2664 */
    int (* __freadable) (FILE *);                                                                                                                    /* 2668 */
    int (* __fwritable) (FILE *);                                                                                                                    /* 2672 */
    int (* __flbf) (FILE *);                                                                                                                         /* 2676 */
    size_t (* __fbufsize) (FILE *);                                                                                                                  /* 2680 */
    size_t (* __fpending) (FILE *);                                                                                                                  /* 2684 */
    int (* __fpurge) (FILE *);                                                                                                                       /* 2688 */
    void (* fpurge) (FILE *);                                                                                                                        /* 2692 */
    void (* __fseterr) (FILE *);                                                                                                                     /* 2696 */
    size_t (* __freadahead) (FILE *);                                                                                                                /* 2700 */
    void (* __freadptrinc) (FILE *f, size_t inc);                                                                                                    /* 2704 */

    /* stdlib.h */
    int * (* __mb_cur_max) (void);                                                                                                                  /* 2708 */
    int (* mblen) (const char *s, size_t n);                                                                                                         /* 2712 */
    int (* mbtowc) (wchar_t *pwc, const char *s, size_t n);                                                                                          /* 2716 */
    int (* _mbtowc_r) (wchar_t *pwc, const char *s, size_t n, _mbstate_t *st);                                                                       /* 2720 */
    size_t (* _mbstowcs_r) (wchar_t *pwcs, const char *s, size_t n, mbstate_t *state);                                                               /* 2724 */
    int (* wctomb) (char *s, wchar_t wchar);                                                                                                         /* 2728 */
    int (* _wctomb_r) (char *s, wchar_t wchar, mbstate_t *state);                                                                                    /* 2732 */
    size_t (* mbstowcs) (wchar_t *ws, const char *s, size_t wn);                                                                                     /* 2736 */
    size_t (* wcstombs) (char *s, const wchar_t *pwcs, size_t n);                                                                                    /* 2740 */
    size_t (* _wcstombs_r) (char *s, const wchar_t *pwcs, size_t n, mbstate_t *state);                                                               /* 2744 */
    void * (* malloc) (size_t size);                                                                                                                 /* 2748 */
    void * (* calloc) (size_t num_elements, size_t element_size);                                                                                    /* 2752 */
    void (* free) (void *ptr);                                                                                                                       /* 2756 */
    void * (* realloc) (void *ptr, size_t size);                                                                                                     /* 2760 */
    void * (* valloc) (size_t size);                                                                                                                 /* 2764 */
    void * (* aligned_alloc) (size_t alignment, size_t size);                                                                                        /* 2768 */
    int (* posix_memalign) (void **memptr, size_t alignment, size_t size);                                                                           /* 2772 */
    int (* abs) (int x);                                                                                                                             /* 2776 */
    long (* labs) (long x);                                                                                                                          /* 2780 */
    div_t (* div) (int n, int d);                                                                                                                    /* 2784 */
    ldiv_t (* ldiv) (long n, long d);                                                                                                                /* 2788 */
    int (* rand) (void);                                                                                                                             /* 2792 */
    void (* srand) (unsigned int seed);                                                                                                              /* 2796 */
    long (* random) (void);                                                                                                                          /* 2800 */
    void (* srandom) (unsigned int seed);                                                                                                            /* 2804 */
    char * (* initstate) (unsigned int seed, char *state, size_t size);                                                                              /* 2808 */
    char * (* setstate) (const char *state);                                                                                                         /* 2812 */
    char * (* l64a) (long x0);                                                                                                                       /* 2816 */
    long (* a64l) (const char *s);                                                                                                                   /* 2820 */
    void (* setkey) (const char *key);                                                                                                               /* 2824 */
    int (* system) (const char *command);                                                                                                            /* 2828 */
    void (* exit) (int status);                                                                                                                      /* 2832 */
    void (* abort) (void);                                                                                                                           /* 2836 */
    int (* atexit) (void (*)(void));                                                                                                                 /* 2840 */
    char * (* getenv) (const char *name);                                                                                                            /* 2844 */
    void * (* bsearch) (const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value));           /* 2848 */
    void (* qsort) (void *base, size_t count, size_t size, int (*compare)(const void *element1, const void *element2));                              /* 2852 */
    double (* strtod) (const char *str, char **ptr);                                                                                                 /* 2856 */
    long (* strtol) (const char *str, char **ptr, int base);                                                                                         /* 2860 */
    unsigned long (* strtoul) (const char *str, char **ptr, int base);                                                                               /* 2864 */
    quad_t (* strtoq) (const char *nptr, char **endptr, register int base);                                                                          /* 2868 */
    double (* atof) (const char *str);                                                                                                               /* 2872 */
    int (* atoi) (const char *str);                                                                                                                  /* 2876 */
    long (* atol) (const char *str);                                                                                                                 /* 2880 */
    char * (* itoa) (int value, char *buffer, int base);                                                                                             /* 2884 */
    char * (* lltoa) (int64_t ll, char *buffer, int radix);                                                                                          /* 2888 */
    void (* _exit) (int status);                                                                                                                     /* 2892 */
    int (* rand_r) (unsigned int *seed);                                                                                                             /* 2896 */
    char * (* mktemp) (char *name_template);                                                                                                         /* 2900 */
    int (* mkstemp) (char *name_template);                                                                                                           /* 2904 */
    int (* unsetenv) (const char *name);                                                                                                             /* 2908 */
    int (* setenv) (const char *name, const char *value, int overwrite);                                                                             /* 2912 */
    int (* putenv) (const char *string);                                                                                                             /* 2916 */
    char * (* mkdtemp) (char *name_template);                                                                                                        /* 2920 */
    const char * (* getexecname) (void);                                                                                                             /* 2924 */
    long long (* strtoll) (const char *str, char **ptr, int base);                                                                                   /* 2928 */
    long double (* strtold) (const char *str, char **ptr);                                                                                           /* 2932 */
    unsigned long long (* strtoull) (const char *str, char **ptr, int base);                                                                         /* 2936 */
    long long (* atoll) (const char *str);                                                                                                           /* 2940 */
    long long (* llabs) (long long x);                                                                                                               /* 2944 */
    lldiv_t (* lldiv) (long long n, long long d);                                                                                                    /* 2948 */
    float (* strtof) (const char *str, char **ptr);                                                                                                  /* 2952 */
    float (* atoff) (const char *nptr);                                                                                                              /* 2956 */
    void (* _Exit) (int status);                                                                                                                     /* 2960 */
    char * (* ecvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2964 */
    char * (* fcvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2968 */
    char * (* gcvt) (double x, int n, char *b);                                                                                                      /* 2972 */
    char * (* secure_getenv) (const char *name);                                                                                                     /* 2976 */
    void * (* reallocarray) (void *ptr, size_t m, size_t n);                                                                                         /* 2980 */
    void (* qsort_r) (void *, size_t, size_t, int (*)(const void *, const void *, void *), void *);                                                  /* 2984 */
    uint32_t (* arc4random) (void);                                                                                                                  /* 2988 */
    void (* arc4random_buf) (void *buf, size_t nbytes);                                                                                              /* 2992 */
    uint32_t (* arc4random_uniform) (uint32_t upper_bound);                                                                                          /* 2996 */
    void (* arc4random_stir) (void);                                                                                                                 /* 3000 */
    void (* arc4random_addrandom) (unsigned char *dat, int datlen);                                                                                  /* 3004 */

    /* string.h */
    char * (* strerror) (int error_number);                                                                                                          /* 3008 */
    char * (* strcat) (char *dest, const char *src);                                                                                                 /* 3012 */
    char * (* strncat) (char *dest, const char *src, size_t n);                                                                                      /* 3016 */
    int (* strcmp) (const char *s1, const char * s2);                                                                                                /* 3020 */
    int (* strncmp) (const char *s1, const char *s2, size_t n);                                                                                      /* 3024 */
    char * (* strcpy) (char *dest, const char *src);                                                                                                 /* 3028 */
    char * (* strncpy) (char *dest, const char *src, size_t n);                                                                                      /* 3032 */
    size_t (* strnlen) (const char *s, size_t maxlen);                                                                                               /* 3036 */
    size_t (* strlen) (const char *s);                                                                                                               /* 3040 */
    char * (* strchr) (const char *s, int c);                                                                                                        /* 3044 */
    char * (* strrchr) (const char *s, int c);                                                                                                       /* 3048 */
    size_t (* strspn) (const char *s, const char *set);                                                                                              /* 3052 */
    size_t (* strcspn) (const char *s, const char *set);                                                                                             /* 3056 */
    char * (* strpbrk) (const char *s, const char *set);                                                                                             /* 3060 */
    char * (* strtok) (char *str, const char *set);                                                                                                  /* 3064 */
    char * (* strstr) (const char *src, const char *sub);                                                                                            /* 3068 */
    char * (* strnstr) (const char *src, const char *sub, size_t len);                                                                               /* 3072 */
    char * (* strsep) (char ** src, const char *delim);                                                                                              /* 3076 */
    char * (* stpcpy) (char *dest, const char *src);                                                                                                 /* 3080 */
    char * (* stpncpy) (char *dest, const char *src, size_t len);                                                                                    /* 3084 */
    size_t (* stccpy) (char * dest, const char * src, size_t n);                                                                                     /* 3088 */
    int (* strcoll) (const char *s1, const char *s2);                                                                                                /* 3092 */
    size_t (* strxfrm) (char *dest, const char *src, size_t len);                                                                                    /* 3096 */
    void * (* memmove) (void *dest, const void * src, size_t len);                                                                                   /* 3100 */
    void * (* memchr) (const void * ptr, int val, size_t len);                                                                                       /* 3104 */
    int (* memcmp) (const void *ptr1, const void *ptr2, size_t len);                                                                                 /* 3108 */
    void * (* memcpy) (void *dest, const void *src, size_t len);                                                                                     /* 3112 */
    void * (* memset) (void *ptr, int val, size_t len);                                                                                              /* 3116 */
    int (* strerror_r) (int error,char * buffer,size_t buffer_size);                                                                                 /* 3120 */
    char * (* strdup) (const char *s);                                                                                                               /* 3124 */
    char * (* strndup) (const char *str, size_t max);                                                                                                /* 3128 */
    void (* bcopy) (const void *from,void *to,size_t len);                                                                                           /* 3132 */
    void (* bzero) (void *m,size_t len);                                                                                                             /* 3136 */
    void (* explicit_bzero) (void *m, size_t len);                                                                                                   /* 3140 */
    int (* bcmp) (const void *a,const void *b,size_t len);                                                                                           /* 3144 */
    size_t (* strlcpy) (char *dst, const char *src, size_t siz);                                                                                     /* 3148 */
    size_t (* strlcat) (char *dst, const char *src, size_t siz);                                                                                     /* 3152 */
    char * (* strtok_r) (char *str, const char *separator_set,char ** state_ptr);                                                                    /* 3156 */
    char * (* strsignal) (int);                                                                                                                      /* 3160 */
    void * (* memccpy) (void *dst0, const void *src0, int endchar0, size_t len0);                                                                    /* 3164 */
    char * (* index) (const char *s, int c);                                                                                                         /* 3168 */
    char * (* rindex) (const char *s, int c);                                                                                                        /* 3172 */
    int (* strverscmp) (const char *s1, const char *s2);                                                                                             /* 3176 */
    char * (* strchrnul) (const char *s, int c_in);                                                                                                  /* 3180 */
    char * (* strcasestr) (const char *haystack, const char *needle);                                                                                /* 3184 */
    void * (* memmem) (const void *h0, size_t k, const void *n0, size_t l);                                                                          /* 3188 */
    void * (* memrchr) (const void *s, int c, size_t n);                                                                                             /* 3192 */
    void * (* mempcpy) (void *dst0, const void *src0, size_t len0);                                                                                  /* 3196 */

    /* strings.h */
    int (* strcasecmp) (const char *s1, const char *s2);                                                                                             /* 3200 */
    int (* strncasecmp) (const char *s1, const char *s2, size_t len);                                                                                /* 3204 */
    int (* ffs) (int i);                                                                                                                             /* 3208 */
    int (* ffsl) (long i);                                                                                                                           /* 3212 */
    int (* ffsll) (long long i);                                                                                                                     /* 3216 */

    /* termcap.h */
    int (* tgetent) (char *buffer, const char *termtype);                                                                                            /* 3220 */
    int (* tgetnum) (const char *name);                                                                                                              /* 3224 */
    int (* tgetflag) (const char *name);                                                                                                             /* 3228 */
    char * (* tgetstr) (const char *name, char **area);                                                                                              /* 3232 */
    void (* tputs) (const char *str, int nlines, int (*outfun) (int));                                                                               /* 3236 */
    char * (* tparam) (const char *string, char *outstring, int leb, int arg0, int arg1, int arg2, int arg3);                                        /* 3240 */
    char * (* tgoto) (const char *cstring, int hpos, int vpos);                                                                                      /* 3244 */

    /* termios.h */
    int (* tcgetattr) (int file_descriptor, struct termios *tios_p);                                                                                 /* 3248 */
    int (* tcsetattr) (int file_descriptor, int action, struct termios *tios_p);                                                                     /* 3252 */
    int (* tcsendbreak) (int file_descriptor, int duration);                                                                                         /* 3256 */
    int (* tcdrain) (int file_descriptor);                                                                                                           /* 3260 */
    int (* tcflush) (int file_descriptor, int what);                                                                                                 /* 3264 */
    int (* tcflow) (int file_descriptor, int action);                                                                                                /* 3268 */
    int (* cfmakeraw) (struct termios *tios_p);                                                                                                      /* 3272 */
    speed_t (* cfgetispeed) (const struct termios *tios_p);                                                                                          /* 3276 */
    speed_t (* cfgetospeed) (const struct termios *tios_p);                                                                                          /* 3280 */
    int (* cfsetispeed) (struct termios *tios_p, speed_t input_speed);                                                                               /* 3284 */
    int (* cfsetospeed) (struct termios *tios_p, speed_t output_speed);                                                                              /* 3288 */

    /* time.h */
    clock_t (* clock) (void);                                                                                                                        /* 3292 */
    time_t (* time) (time_t *t);                                                                                                                     /* 3296 */
    char * (* asctime) (const struct tm *tm);                                                                                                        /* 3300 */
    char * (* ctime) (const time_t *t);                                                                                                              /* 3304 */
    struct tm * (* gmtime) (const time_t *t);                                                                                                        /* 3308 */
    struct tm * (* localtime) (const time_t *t);                                                                                                     /* 3312 */
    time_t (* mktime) (struct tm *tm);                                                                                                               /* 3316 */
    double (* difftime) (time_t t1, time_t t0);                                                                                                      /* 3320 */
    size_t (* strftime) (char *s, size_t maxsize, const char *format, const struct tm *tm);                                                          /* 3324 */
    char * (* strptime) (const char *buf, const char *fmt, struct tm *timeptr);                                                                      /* 3328 */
    char * (* asctime_r) (const struct tm *tm, char *buffer);                                                                                        /* 3332 */
    char * (* ctime_r) (const time_t *tptr, char *buffer);                                                                                           /* 3336 */
    struct tm * (* gmtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                   /* 3340 */
    struct tm * (* localtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                /* 3344 */
    void (* tzset) (void);                                                                                                                           /* 3348 */
    int (* nanosleep) (const struct timespec *req, struct timespec *rem);                                                                            /* 3352 */
    int (* clock_gettime) (clockid_t clk_id, struct timespec *t);                                                                                    /* 3356 */
    int (* clock_settime) (clockid_t clk_id, const struct timespec *t);                                                                              /* 3360 */
    int (* clock_getres) (clockid_t clock_id, struct timespec *res);                                                                                 /* 3364 */
    int (* clock_nanosleep) (clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);                                /* 3368 */
    unsigned long long (* rdtsc) (void);                                                                                                             /* 3372 */
    int (* clock_gettime64) (clockid_t clock_id, struct timespec64 *tp);                                                                             /* 3376 */

    /* uchar.h */
    size_t (* c16rtomb) (char *__restrict, char16_t, mbstate_t *__restrict);                                                                         /* 3380 */
    size_t (* mbrtoc16) (char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 3384 */
    size_t (* c32rtomb) (char *__restrict, char32_t, mbstate_t *__restrict);                                                                         /* 3388 */
    size_t (* mbrtoc32) (char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 3392 */

    /* ulimit.h */
    long (* ulimit) (int cmd,long newlimit);                                                                                                         /* 3396 */

    /* unistd.h */
    int (* isatty) (int file_descriptor);                                                                                                            /* 3400 */
    int (* dup) (int file_descriptor);                                                                                                               /* 3404 */
    int (* dup2) (int file_descriptor1, int file_descriptor2);                                                                                       /* 3408 */
    off_t (* lseek) (int file_descriptor, off_t offset, int mode);                                                                                   /* 3412 */
    int (* access) (const char * path_name, int mode);                                                                                               /* 3416 */
    int (* chown) (const char * path_name, uid_t owner, gid_t group);                                                                                /* 3420 */
    int (* fchown) (int file_descriptor, uid_t owner, gid_t group);                                                                                  /* 3424 */
    int (* lchown) (const char * path_name, uid_t owner, gid_t group);                                                                               /* 3428 */
    int (* truncate) (const char * path_name, off_t length);                                                                                         /* 3432 */
    int (* ftruncate) (int file_descriptor, off_t length);                                                                                           /* 3436 */
    int (* getpagesize) (void);                                                                                                                      /* 3440 */
    int (* ftruncate64) (int file_descriptor, _off64_t length);                                                                                      /* 3444 */
    int (* truncate64) (const char *path_name, _off64_t length);                                                                                     /* 3448 */
    off64_t (* lseek64) (int fd, off64_t offset, int whence);                                                                                        /* 3452 */
    unsigned int (* alarm) (unsigned seconds);                                                                                                       /* 3456 */
    useconds_t (* ualarm) (useconds_t usecs, useconds_t interval);                                                                                   /* 3460 */
    int (* link) (const char * existing_path,const char * new_path);                                                                                 /* 3464 */
    int (* unlink) (const char * path_name);                                                                                                         /* 3468 */
    int (* symlink) (const char * actual_path, const char * symbolic_path);                                                                          /* 3472 */
    int (* readlink) (const char * path_name, char * buffer, int buffer_size);                                                                       /* 3476 */
    int (* chdir) (const char * path_name);                                                                                                          /* 3480 */
    int (* fchdir) (int file_descriptor);                                                                                                            /* 3484 */
    int (* lockf) (int file_descriptor, int function, off_t size);                                                                                   /* 3488 */
    unsigned int (* sleep) (unsigned int seconds);                                                                                                   /* 3492 */
    int (* usleep) (useconds_t microseconds);                                                                                                        /* 3496 */
    int (* pause) (void);                                                                                                                            /* 3500 */
    int (* getopt) (int argc, char * const argv[], const char *opts);                                                                                /* 3504 */
    pid_t (* getpid) (void);                                                                                                                         /* 3508 */
    pid_t (* getppid) (void);                                                                                                                        /* 3512 */
    pid_t (* getpgrp) (void );                                                                                                                       /* 3516 */
    pid_t (* tcgetpgrp) (int fildes);                                                                                                                /* 3520 */
    int (* tcsetpgrp) (int filedes, pid_t pgrp);                                                                                                     /* 3524 */
    char * (* realpath) (const char *file_name, char *resolved_name);                                                                                /* 3528 */
    int (* fsync) (int file_descriptor);                                                                                                             /* 3532 */
    int (* fdatasync) (int file_descriptor);                                                                                                         /* 3536 */
    char * (* ttyname) (int);                                                                                                                        /* 3540 */
    int (* ttyname_r) (int file_descriptor,char *name,size_t buflen);                                                                                /* 3544 */
    int (* execl) (const char *path,const char *arg0,...);                                                                                           /* 3548 */
    int (* execle) (const char *path,const char *arg0,...);                                                                                          /* 3552 */
    int (* execlp) (const char *path,const char *arg0,...);                                                                                          /* 3556 */
    int (* execv) (const char *path,char * const argv[]);                                                                                            /* 3560 */
    int (* execve) (const char *path,char *const argv[],char *const envp[]);                                                                         /* 3564 */
    int (* execvp) (const char *command,char * const argv[]);                                                                                        /* 3568 */
    void (* encrypt) (char *block, int edflag);                                                                                                      /* 3572 */
    int (* spawnv) (int mode, const char *file, const char **argv);                                                                                  /* 3576 */
    int (* spawnvp) (int mode, const char *path, const char **argv);                                                                                 /* 3580 */
    long (* sysconf) (int name);                                                                                                                     /* 3584 */
    void (* enableUnixPaths) (void);                                                                                                                 /* 3588 */
    void (* disableUnixPaths) (void);                                                                                                                /* 3592 */
    void (* enableAltivec) (void);                                                                                                                   /* 3596 */
    void (* disableAltivec) (void);                                                                                                                  /* 3600 */
    void (* enableOptimizedFunctions) (void);                                                                                                        /* 3604 */
    void (* disableOptimizedFunctions) (void);                                                                                                       /* 3608 */
    char * (* getcwd) (char * buffer, size_t buffer_size);                                                                                           /* 3612 */
    char * (* getwd) (char *buf);                                                                                                                    /* 3616 */
    char * (* get_current_dir_name) (void);                                                                                                          /* 3620 */
    long (* gethostid) ();                                                                                                                           /* 3624 */
    int (* gethostname) (const char *name, size_t len);                                                                                              /* 3628 */
    int (* getdomainname) (char *name, size_t len);                                                                                                  /* 3632 */
    int (* setdomainname) (const char *name, size_t len);                                                                                            /* 3636 */
    char * (* getlogin) (void);                                                                                                                      /* 3640 */
    int (* getlogin_r) (char *buf, size_t bufsize);                                                                                                  /* 3644 */
    char * (* crypt) (const char *key, const char *salt);                                                                                            /* 3648 */
    gid_t (* getegid) (void);                                                                                                                        /* 3652 */
    uid_t (* geteuid) (void);                                                                                                                        /* 3656 */
    gid_t (* getgid) (void);                                                                                                                         /* 3660 */
    int (* getgroups) (int gidsetsize, gid_t grouplist[]);                                                                                           /* 3664 */
    char * (* getpass) (const char *prompt);                                                                                                         /* 3668 */
    uid_t (* getuid) (void);                                                                                                                         /* 3672 */
    int (* initgroups) (const char *name, gid_t basegid);                                                                                            /* 3676 */
    int (* setegid) (gid_t gid);                                                                                                                     /* 3680 */
    int (* seteuid) (uid_t uid);                                                                                                                     /* 3684 */
    int (* setgid) (gid_t gid);                                                                                                                      /* 3688 */
    int (* setgroups) (int ngroups, const gid_t *gidset);                                                                                            /* 3692 */
    int (* setregid) (gid_t rgid, gid_t egid);                                                                                                       /* 3696 */
    int (* setreuid) (uid_t ruid, uid_t euid);                                                                                                       /* 3700 */
    pid_t (* setsid) (void);                                                                                                                         /* 3704 */
    int (* setuid) (uid_t uid);                                                                                                                      /* 3708 */
    int (* setlogin) (const char *name);                                                                                                             /* 3712 */
    long (* pathconf) (const char *path,int name);                                                                                                   /* 3716 */
    long (* fpathconf) (int file_descriptor,int name);                                                                                               /* 3720 */
    int (* pipe) (int fd[2]);                                                                                                                        /* 3724 */
    int (* pipe2) (int fd[2], int flags);                                                                                                            /* 3728 */
    ssize_t (* pread) (int fd, void *buf, size_t nbytes, off_t offset);                                                                              /* 3732 */
    ssize_t (* pwrite) (int fd, const void *buf, size_t nbytes, off_t offset);                                                                       /* 3736 */
    ssize_t (* pread64) (int fd, void *buf, size_t nbytes, off64_t offset);                                                                          /* 3740 */
    ssize_t (* pwrite64) (int fd, const void *buf, size_t nbytes, off64_t offset);                                                                   /* 3744 */

    /* utime.h */
    int (* utime) (const char * path_name,const struct utimbuf * times);                                                                             /* 3748 */

    /* wchar.h */
    wint_t (* btowc) (int c);                                                                                                                        /* 3752 */
    int (* wctob) (wint_t c);                                                                                                                        /* 3756 */
    wchar_t * (* wcscat) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3760 */
    wchar_t * (* wcsncat) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3764 */
    int (* wcscmp) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3768 */
    int (* wcsncmp) (const wchar_t *s1, const wchar_t *s2, size_t n);                                                                                /* 3772 */
    wchar_t * (* wcscpy) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3776 */
    wchar_t * (* wcsncpy) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3780 */
    size_t (* wcslen) (const wchar_t *s);                                                                                                            /* 3784 */
    wchar_t * (* wcschr) (const wchar_t *s, wchar_t c);                                                                                              /* 3788 */
    size_t (* wcsspn) (const wchar_t *s, const wchar_t *set);                                                                                        /* 3792 */
    wchar_t * (* wcspbrk) (const wchar_t *s, const wchar_t *set);                                                                                    /* 3796 */
    wchar_t * (* wcstok) (wchar_t *str, const wchar_t *delim, wchar_t **ptr);                                                                        /* 3800 */
    wchar_t * (* wcsstr) (const wchar_t *big, const wchar_t *little);                                                                                /* 3804 */
    int (* wcswidth) (const wchar_t *pwcs, size_t n);                                                                                                /* 3808 */
    int (* wcwidth) (const wchar_t wc);                                                                                                              /* 3812 */
    double (* wcstod) (const wchar_t *str, wchar_t **ptr);                                                                                           /* 3816 */
    long (* wcstol) (const wchar_t *str, wchar_t **ptr, int base);                                                                                   /* 3820 */
    unsigned long (* wcstoul) (const wchar_t *str, wchar_t **ptr, int base);                                                                         /* 3824 */
    float (* wcstof) (const wchar_t *nptr, wchar_t **endptr);                                                                                        /* 3828 */
    int (* wscoll) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3832 */
    size_t (* wcsxfrm) (wchar_t *dest, const wchar_t *src, size_t len);                                                                              /* 3836 */
    wchar_t * (* wmemchr) (const wchar_t *ptr, wchar_t val, size_t len);                                                                             /* 3840 */
    int (* wmemcmp) (const wchar_t *ptr1, const wchar_t *ptr2, size_t len);                                                                          /* 3844 */
    wchar_t * (* wmemcpy) (wchar_t *dest, const wchar_t *src, size_t len);                                                                           /* 3848 */
    wchar_t * (* wmemmove) (wchar_t *dest, const wchar_t *src, size_t len);                                                                          /* 3852 */
    wchar_t * (* wmemset) (wchar_t *ptr, int val, size_t len);                                                                                       /* 3856 */
    int (* fwide) (FILE *stream, int orient);                                                                                                        /* 3860 */
    wint_t (* fgetwc) (FILE *stream);                                                                                                                /* 3864 */
    wint_t (* getwc) (FILE *stream);                                                                                                                 /* 3868 */
    wint_t (* getwchar) (void);                                                                                                                      /* 3872 */
    wint_t (* ungetwc) (wint_t wc, FILE *fp);                                                                                                        /* 3876 */
    wchar_t * (* fgetws) (wchar_t *s, int n, FILE *stream);                                                                                          /* 3880 */
    int (* fwscanf) (FILE *stream, const wchar_t *format, ...);                                                                                      /* 3884 */
    int (* swscanf) (const wchar_t *s, const wchar_t *format, ...);                                                                                  /* 3888 */
    int (* vfwscanf) (FILE *f, const wchar_t *format, va_list ap);                                                                                   /* 3892 */
    int (* vswscanf) (const wchar_t *s, const wchar_t *format, va_list ap);                                                                          /* 3896 */
    int (* vwscanf) (const wchar_t *s, va_list ap);                                                                                                  /* 3900 */
    int (* wscanf) (const wchar_t *format, ...);                                                                                                     /* 3904 */
    wint_t (* fputwc) (wchar_t c, FILE *stream);                                                                                                     /* 3908 */
    wint_t (* putwc) (wchar_t c, FILE *stream);                                                                                                      /* 3912 */
    wint_t (* putwchar) (wchar_t c);                                                                                                                 /* 3916 */
    wint_t (* fputws) (const wchar_t *s, FILE *stream);                                                                                              /* 3920 */
    int (* fwprintf) (FILE *stream, const wchar_t *format, ...);                                                                                     /* 3924 */
    int (* swprintf) (wchar_t *restrict s, size_t l, const wchar_t *restrict fmt, ...);                                                              /* 3928 */
    int (* vfwprintf) (FILE *stream, const wchar_t *format, va_list arg);                                                                            /* 3932 */
    int (* vswprintf) (wchar_t *s, size_t maxlen, const wchar_t *format, va_list arg);                                                               /* 3936 */
    int (* vwprintf) (const wchar_t *format, va_list arg);                                                                                           /* 3940 */
    int (* wprintf) (const wchar_t *format, ...);                                                                                                    /* 3944 */
    size_t (* wcsftime) (wchar_t *s, size_t maxsize, const wchar_t *format, const struct tm *timeptr);                                               /* 3948 */
    size_t (* mbrlen) (const char *restrict s, size_t n, mbstate_t *restrict ps);                                                                    /* 3952 */
    size_t (* mbrtowc) (wchar_t *pwc, const char *src, size_t n, mbstate_t *ps);                                                                     /* 3956 */
    int (* mbsinit) (const mbstate_t *ps);                                                                                                           /* 3960 */
    size_t (* mbsrtowcs) (wchar_t *restrict dst, const char **restrict src, size_t len, mbstate_t *restrict ps);                                     /* 3964 */
    size_t (* wcrtomb) (char *restrict s, wchar_t wc, mbstate_t *restrict ps);                                                                       /* 3968 */
    int (* wcscoll) (const wchar_t *ws1, const wchar_t *ws2);                                                                                        /* 3972 */
    size_t (* wcscspn) (const wchar_t *s, const wchar_t *c);                                                                                         /* 3976 */
    wchar_t * (* wcsrchr) (const wchar_t *ws, wchar_t wc);                                                                                           /* 3980 */
    size_t (* wcsrtombs) (char *restrict dst, const wchar_t **restrict src, size_t len, mbstate_t *restrict ps);                                     /* 3984 */
    long long (* wcstoll) (const wchar_t *str, wchar_t **ptr, int base);                                                                             /* 3988 */
    unsigned long long (* wcstoull) (const wchar_t *str, wchar_t **ptr, int base);                                                                   /* 3992 */
    long double (* wcstold) (const wchar_t *nptr, wchar_t **endptr);                                                                                 /* 3996 */
    size_t (* mbsnrtowcs) (wchar_t *dst, const char **restrict src, size_t nmc, size_t len, mbstate_t *restrict ps);                                 /* 4000 */
    size_t (* wcsnrtombs) (char *restrict dst, const wchar_t **restrict src, size_t nwc, size_t len, mbstate_t *restrict ps);                        /* 4004 */
    wchar_t * (* wcsdup) (const wchar_t *src);                                                                                                       /* 4008 */
    size_t (* wcsnlen) (const wchar_t *src, size_t n);                                                                                               /* 4012 */
    wchar_t * (* wcpcpy) (wchar_t *dst, const wchar_t *src);                                                                                         /* 4016 */
    wchar_t * (* wcpncpy) (wchar_t *dst, const wchar_t *src, size_t len);                                                                            /* 4020 */
    int (* wcscasecmp) (const wchar_t *l, const wchar_t *r);                                                                                         /* 4024 */
    int (* wcscasecmp_l) (const wchar_t *l, const wchar_t *r, locale_t loc);                                                                         /* 4028 */
    int (* wcsncasecmp) (const wchar_t *l, const wchar_t *r, size_t n);                                                                              /* 4032 */
    int (* wcsncasecmp_l) (const wchar_t *l, const wchar_t *r, size_t n, locale_t loc);                                                              /* 4036 */
    int (* wcscoll_l) (const wchar_t *ws1, const wchar_t *ws2, locale_t loc);                                                                        /* 4040 */
    size_t (* wcsxfrm_l) (wchar_t *dest, const wchar_t *src, size_t len, locale_t loc);                                                              /* 4044 */

    /* wctype.h */
    int (* iswalnum) (wint_t c);                                                                                                                     /* 4048 */
    int (* iswalpha) (wint_t c);                                                                                                                     /* 4052 */
    int (* iswcntrl) (wint_t c);                                                                                                                     /* 4056 */
    int (* iswdigit) (wint_t c);                                                                                                                     /* 4060 */
    int (* iswxdigit) (wint_t c);                                                                                                                    /* 4064 */
    int (* iswgraph) (wint_t c);                                                                                                                     /* 4068 */
    int (* iswpunct) (wint_t c);                                                                                                                     /* 4072 */
    int (* iswprint) (wint_t c);                                                                                                                     /* 4076 */
    int (* iswlower) (wint_t c);                                                                                                                     /* 4080 */
    int (* iswupper) (wint_t c);                                                                                                                     /* 4084 */
    int (* iswspace) (wint_t c);                                                                                                                     /* 4088 */
    int (* iswblank) (wint_t c);                                                                                                                     /* 4092 */
    wint_t (* towlower) (wint_t c);                                                                                                                  /* 4096 */
    wint_t (* towupper) (wint_t c);                                                                                                                  /* 4100 */
    wctype_t (* wctype) (const char *property);                                                                                                      /* 4104 */
    int (* iswctype) (wint_t c, wctype_t desc);                                                                                                      /* 4108 */
    wctrans_t (* wctrans) (const char *property);                                                                                                    /* 4112 */
    wint_t (* towctrans) (wint_t c, wctrans_t desc);                                                                                                 /* 4116 */

    /* arpa/inet.h */
    in_addr_t (* inet_addr) (const char *cp);                                                                                                        /* 4120 */
    int (* inet_aton) (const char *cp, struct in_addr *pin);                                                                                         /* 4124 */
    in_addr_t (* inet_lnaof) (struct in_addr in);                                                                                                    /* 4128 */
    struct in_addr (* inet_makeaddr) (in_addr_t net, in_addr_t lna);                                                                                 /* 4132 */
    in_addr_t (* inet_netof) (struct in_addr in);                                                                                                    /* 4136 */
    in_addr_t (* inet_network) (const char *cp);                                                                                                     /* 4140 */
    char * (* inet_ntoa) (struct in_addr in);                                                                                                        /* 4144 */
    const char * (* inet_ntop) (int af, const void *src, char *dst, socklen_t cnt);                                                                  /* 4148 */
    int (* inet_pton) (int af, const char *src, void *dst);                                                                                          /* 4152 */

    /* arpa/nameser.h */
    unsigned (* ns_get16) (const unsigned char *);                                                                                                   /* 4156 */
    unsigned long (* ns_get32) (const unsigned char *);                                                                                              /* 4160 */
    void (* ns_put16) (unsigned, unsigned char *);                                                                                                   /* 4164 */
    void (* ns_put32) (unsigned long, unsigned char *);                                                                                              /* 4168 */
    int (* ns_initparse) (const unsigned char *, int, ns_msg *);                                                                                     /* 4172 */
    int (* ns_parserr) (ns_msg *, ns_sect, int, ns_rr *);                                                                                            /* 4176 */
    int (* ns_skiprr) (const unsigned char *, const unsigned char *, ns_sect, int);                                                                  /* 4180 */
    int (* ns_name_uncompress) (const unsigned char *, const unsigned char *, const unsigned char *, char *, size_t);                                /* 4184 */

    /* net/if.h */
    unsigned int (* if_nametoindex) (const char *ifname);                                                                                            /* 4188 */
    char * (* if_indextoname) (unsigned int ifindex, char *ifname);                                                                                  /* 4192 */

    /* netinet/in.h */
    int (* bindresvport) (int sd, struct sockaddr_in *sa);                                                                                           /* 4196 */
    int (* bindresvport6) (int sd, struct sockaddr_in6 *sa);                                                                                         /* 4200 */

    /* sys/byteswap.h */
    __CONST_FUNC uint16_t (* bswap16) (uint16_t);                                                                                                    /* 4204 */
    __CONST_FUNC uint32_t (* bswap24) (uint32_t);                                                                                                    /* 4208 */
    __CONST_FUNC uint32_t (* bswap32) (uint32_t);                                                                                                    /* 4212 */
    __CONST_FUNC uint64_t (* bswap64) (uint64_t);                                                                                                    /* 4216 */
    void * (* swab) (void *from,void *to,ssize_t nbytes);                                                                                            /* 4220 */
    void * (* swab24) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 24-bit words instead. */                              /* 4224 */
    void * (* swab32) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 32-bit words instead. */                              /* 4228 */
    void * (* swab64) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 64-bit words instead. */                              /* 4232 */

    /* sys/file.h */
    int (* flock) (int fd, int op);                                                                                                                  /* 4236 */

    /* sys/ioctl.h */
    int (* ioctl) (int fildes, int request, ... /* arg */);                                                                                          /* 4240 */

    /* sys/ipc.h */
    key_t (* ftok) (const char *path, int id);                                                                                                       /* 4244 */

    /* sys/mman.h */
    void * (* mmap) (void *addr, size_t len, int prot, int flags, int fd, off_t offset);                                                             /* 4248 */
    int (* munmap) (void *map, size_t length);                                                                                                       /* 4252 */
    int (* msync) (void *addr, size_t len, int flags);                                                                                               /* 4256 */

    /* sys/mount.h */
    int (* statfs) (const char *path, struct statfs *buf);                                                                                           /* 4260 */
    int (* fstatfs) (int fd, struct statfs *buf);                                                                                                    /* 4264 */

    /* sys/msg.h */
    int (* _msgctl) (int msqid, int cmd, struct msqid_ds *buf);                                                                                      /* 4268 */
    int (* _msgget) (key_t key, int msgflg);                                                                                                         /* 4272 */
    ssize_t (* _msgrcv) (int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);                                                              /* 4276 */
    int (* _msgsnd) (int msqid, const void *msgp, size_t msgsz, int msgflg);                                                                         /* 4280 */
    int (* _msgsnap) (int msqid, void *buf, size_t bufsz, long msgtyp);                                                                              /* 4284 */
    int (* _msgids) (int *buf, size_t nids, size_t  *pnids);                                                                                         /* 4288 */

    /* sys/resource.h */
    int (* getrlimit) (int resource, struct rlimit *rlp);                                                                                            /* 4292 */
    int (* setrlimit) (int resource, const struct rlimit *rlp);                                                                                      /* 4296 */
    int (* getrusage) (int who, struct rusage *usage);                                                                                               /* 4300 */

    /* sys/select.h */
    int (* select) (int nfds, fd_set *readfds,fd_set *writefds, fd_set *errorfds,struct timeval *timeout);                                           /* 4304 */
    int (* waitselect) (int num_fds,fd_set *read_fds,fd_set *write_fds,fd_set *except_fds,struct timeval *timeout,unsigned long * signal_mask);      /* 4308 */

    /* sys/sem.h */
    int (* _semctl) (int semid, int semnum, int cmd, union semun aun);                                                                               /* 4312 */
    int (* _semget) (key_t key, int nsems, int flags);                                                                                               /* 4316 */
    int (* _semop) (int semid, const struct sembuf *ops, int nops);                                                                                  /* 4320 */
    long int (* _semids) (long int *buf, uint32_t nids, long unsigned int *idcnt);                                                                   /* 4324 */
    int (* _semtimedop) (int semid, const struct sembuf *ops, int nops, struct timespec *to);                                                        /* 4328 */

    /* sys/shm.h */
    void * (* _shmat) (int shmid, const void *shmaddr, int shmflg);                                                                                  /* 4332 */
    int (* _shmctl) (int shmid, int cmd, struct shmid_ds *buf);                                                                                      /* 4336 */
    int (* _shmdt) (const void *shmaddr);                                                                                                            /* 4340 */
    int (* _shmget) (key_t key, size_t size, int shmflg);                                                                                            /* 4344 */
    int (* _shmids) (int *buf, size_t nids, size_t *pnids);                                                                                          /* 4348 */

    /* sys/socket.h */
    int (* accept) (int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);                                                                         /* 4352 */
    int (* bind) (int socket, const struct sockaddr *address, socklen_t address_len);                                                                /* 4356 */
    int (* connect) (int socket, const struct sockaddr *address, socklen_t address_len);                                                             /* 4360 */
    int (* getpeername) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 4364 */
    int (* getsockname) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 4368 */
    int (* getsockopt) (int socket, int level, int option_name, void *option_value, socklen_t *option_len);                                          /* 4372 */
    int (* listen) (int socket, int backlog);                                                                                                        /* 4376 */
    ssize_t (* recv) (int socket, void *buffer, size_t length, int flags);                                                                           /* 4380 */
    ssize_t (* recvfrom) (int socket, void *buffer, size_t length,int flags, struct sockaddr *address,socklen_t *address_len);                       /* 4384 */
    ssize_t (* recvmsg) (int socket, struct msghdr *message, int flags);                                                                             /* 4388 */
    ssize_t (* send) (int socket, const void *buffer, size_t length, int flags);                                                                     /* 4392 */
    ssize_t (* sendmsg) (int socket, const struct msghdr *message, int flags);                                                                       /* 4396 */
    ssize_t (* sendto) (int socket, const void *message, size_t length,int flags, const struct sockaddr *dest_addr,socklen_t dest_len);              /* 4400 */
    int (* setsockopt) (int socket, int level, int option_name, const void *option_value, socklen_t option_len);                                     /* 4404 */
    int (* shutdown) (int socket, int how);                                                                                                          /* 4408 */
    int (* socket) (int domain, int type, int protocol);                                                                                             /* 4412 */
    int (* socketpair) (int domain, int type, int protocol, int fds[2]);                                                                             /* 4416 */

    /* sys/stat.h */
    int (* stat) (const char * path_name, struct stat * buffer);                                                                                     /* 4420 */
    int (* fstat) (int file_descriptor, struct stat * buffer);                                                                                       /* 4424 */
    int (* lstat) (const char * path_name, struct stat * buffer);                                                                                    /* 4428 */
    int (* chmod) (const char * path_name, mode_t mode);                                                                                             /* 4432 */
    int (* fchmod) (int file_descriptor, mode_t mode);                                                                                               /* 4436 */
    int (* mkdir) (const char * path_name, mode_t mode);                                                                                             /* 4440 */
    int (* rmdir) (const char * path_name);                                                                                                          /* 4444 */
    mode_t (* umask) (mode_t new_mask);                                                                                                              /* 4448 */

    /* sys/statvfs.h */
    int (* statvfs) (const char *, struct statvfs *);                                                                                                /* 4452 */
    int (* fstatvfs) (int, struct statvfs *);                                                                                                        /* 4456 */

    /* sys/syslog.h */
    void (* closelog) (void);                                                                                                                        /* 4460 */
    void (* openlog) (const char *ident, int opt, int facility);                                                                                     /* 4464 */
    int (* setlogmask) (int maskpri);                                                                                                                /* 4468 */
    void (* syslog) (int priority, const char *message, ...);                                                                                        /* 4472 */
    void (* vsyslog) (int priority, const char *message, va_list args);                                                                              /* 4476 */

    /* sys/systeminfo.h */
    long (* sysinfo) (int cmd,char *buf,long buflen);                                                                                                /* 4480 */

    /* sys/time.h */
    int (* gettimeofday) (struct timeval *tp, struct timezone *tzp);                                                                                 /* 4484 */
    int (* settimeofday) (const struct timeval *, const struct timezone *);                                                                          /* 4488 */
    int (* utimes) (const char *path, const struct timeval *tvp);                                                                                    /* 4492 */
    int (* getitimer) (int which, struct itimerval *curr_value);                                                                                     /* 4496 */
    int (* setitimer) (int which, const struct itimerval *new_value, struct itimerval *old_value);                                                   /* 4500 */

    /* sys/timeb.h */
    int (* ftime) (struct timeb *);                                                                                                                  /* 4504 */

    /* sys/times.h */
    clock_t (* times) (struct tms *tp);                                                                                                              /* 4508 */

    /* sys/uio.h */
    ssize_t (* readv) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                   /* 4512 */
    ssize_t (* writev) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                  /* 4516 */

    /* sys/utsname.h */
    int (* uname) (struct utsname *);                                                                                                                /* 4520 */

};

#ifdef __PIC__
#define Clib2Call2(function, offset)     \
   asm(".section	\".text\"        \n\
	    .align 2                     \n\
	    .globl " #function "         \n\
	    .type	" #function ", @function \n\
" #function ":                       \n\
	    li  %r12," #offset "         \n\
	    b __Clib2Call@plt            \n\
	    .size	" #function ", .-" #function)
#elif !defined(__PIC__)
#define Clib2Call2(function, offset)     \
   asm(".section	\".text\"        \n\
	    .align 2                     \n\
	    .globl " #function "         \n\
	    .type	" #function ", @function \n\
" #function ":                       \n\
	    li  %r12," #offset "         \n\
	    b __Clib2Call                \n\
	    .size	" #function ", .-" #function)
#endif

#define Clib2Call(x...) Clib2Call2(x)

#endif