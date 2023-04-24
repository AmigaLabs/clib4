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
    void (* aio_init) (const struct aioinit *init);                                                                                                  /* 116 */
    int (* aio_read) (struct aiocb *aiocbp);                                                                                                         /* 120 */
    int (* aio_write) (struct aiocb *aiocbp);                                                                                                        /* 124 */
    int (* lio_listio) (int mode, struct aiocb *const list[restrict], int nent, struct sigevent * sig);                                              /* 128 */
    int (* aio_error) (const struct aiocb *aiocbp);                                                                                                  /* 132 */
    ssize_t (* aio_return) (struct aiocb *aiocbp);                                                                                                   /* 136 */
    int (* aio_cancel) (int fildes, struct aiocb *aiocbp);                                                                                           /* 140 */
    int (* aio_suspend) (const struct aiocb *const list[], int nent, const struct timespec *timeout);                                                /* 144 */
    int (* aio_fsync) (int operation, struct aiocb *aiocbp);                                                                                         /* 148 */
    int (* aio_read64) (struct aiocb64 *aiocbp);                                                                                                     /* 152 */
    int (* aio_write64) (struct aiocb64 *aiocbp);                                                                                                    /* 156 */

    /* argz.h */
    error_t (* argz_create) (char *const argv[], char **argz, size_t *argz_len);                                                                     /* 160 */
    error_t (* argz_create_sep) (const char *string, int sep, char **argz, size_t *argz_len);                                                        /* 164 */
    size_t (* argz_count) (const char *argz, size_t argz_len);                                                                                       /* 168 */
    void (* argz_extract) (char *argz, size_t argz_len, char **argv);                                                                                /* 172 */
    void (* argz_stringify) (char *argz, size_t argz_len, int sep);                                                                                  /* 176 */
    error_t (* argz_add) (char **argz, size_t *argz_len, const char *str);                                                                           /* 180 */
    error_t (* argz_add_sep) (char **argz, size_t *argz_len, const char *str, int sep);                                                              /* 184 */
    error_t (* argz_append) (char **argz, size_t *argz_len, const char *buf, size_t buf_len);                                                        /* 188 */
    error_t (* argz_delete) (char **argz, size_t *argz_len, char *entry);                                                                            /* 192 */
    error_t (* argz_insert) (char **argz, size_t *argz_len, char *before, const char *entry);                                                        /* 196 */
    char * (* argz_next) (char *argz, size_t argz_len, const char *entry);                                                                           /* 200 */
    error_t (* argz_replace) (char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count);                            /* 204 */

    /* assert.h */
    void (* __assertion_failure) (const char *file_name, int line_number, const char *expression);                                                   /* 208 */

    /* complex.h */
    double complex (* cacos) (double complex z);                                                                                                     /* 212 */
    float complex (* cacosf) (float complex z);                                                                                                      /* 216 */
    long double complex (* cacosl) (long double complex z);                                                                                          /* 220 */
    double complex (* casin) (double complex z);                                                                                                     /* 224 */
    float complex (* casinf) (float complex z);                                                                                                      /* 228 */
    long double complex (* casinl) (long double complex z);                                                                                          /* 232 */
    double complex (* catan) (double complex z);                                                                                                     /* 236 */
    float complex (* catanf) (float complex z);                                                                                                      /* 240 */
    long double complex (* catanl) (long double complex z);                                                                                          /* 244 */
    double complex (* ccos) (double complex z);                                                                                                      /* 248 */
    float complex (* ccosf) (float complex z);                                                                                                       /* 252 */
    long double complex (* ccosl) (long double complex z);                                                                                           /* 256 */
    double complex (* csin) (double complex z);                                                                                                      /* 260 */
    float complex (* csinf) (float complex z);                                                                                                       /* 264 */
    long double complex (* csinl) (long double complex z);                                                                                           /* 268 */
    double complex (* ctan) (double complex z);                                                                                                      /* 272 */
    float complex (* ctanf) (float complex z);                                                                                                       /* 276 */
    long double complex (* ctanl) (long double complex z);                                                                                           /* 280 */
    double complex (* cacosh) (double complex z);                                                                                                    /* 284 */
    float complex (* cacoshf) (float complex z);                                                                                                     /* 288 */
    long double complex (* cacoshl) (long double complex z);                                                                                         /* 292 */
    double complex (* casinh) (double complex z);                                                                                                    /* 296 */
    float complex (* casinhf) (float complex z);                                                                                                     /* 300 */
    long double complex (* casinhl) (long double complex z);                                                                                         /* 304 */
    double complex (* catanh) (double complex z);                                                                                                    /* 308 */
    float complex (* catanhf) (float complex z);                                                                                                     /* 312 */
    long double complex (* catanhl) (long double complex z);                                                                                         /* 316 */
    double complex (* ccosh) (double complex z);                                                                                                     /* 320 */
    float complex (* ccoshf) (float complex z);                                                                                                      /* 324 */
    long double complex (* ccoshl) (long double complex z);                                                                                          /* 328 */
    double complex (* csinh) (double complex z);                                                                                                     /* 332 */
    float complex (* csinhf) (float complex z);                                                                                                      /* 336 */
    long double complex (* csinhl) (long double complex z);                                                                                          /* 340 */
    double complex (* ctanh) (double complex z);                                                                                                     /* 344 */
    float complex (* ctanhf) (float complex z);                                                                                                      /* 348 */
    long double complex (* ctanhl) (long double complex z);                                                                                          /* 352 */
    double complex (* cexp) (double complex z);                                                                                                      /* 356 */
    float complex (* cexpf) (float complex z);                                                                                                       /* 360 */
    long double complex (* cexpl) (long double complex z);                                                                                           /* 364 */
    double complex (* clog) (double complex z);                                                                                                      /* 368 */
    float complex (* clogf) (float complex z);                                                                                                       /* 372 */
    long double complex (* clogl) (long double complex z);                                                                                           /* 376 */
    double (* cabs) (double complex z);                                                                                                              /* 380 */
    float (* cabsf) (float complex z);                                                                                                               /* 384 */
    long double (* cabsl) (long double complex z);                                                                                                   /* 388 */
    double complex (* cpow) (double complex z, double complex c);                                                                                    /* 392 */
    float complex (* cpowf) (float complex z, float complex c);                                                                                      /* 396 */
    long double complex (* cpowl) (long double complex z, long double complex c);                                                                    /* 400 */
    double complex (* csqrt) (double complex z);                                                                                                     /* 404 */
    float complex (* csqrtf) (float complex z);                                                                                                      /* 408 */
    long double complex (* csqrtl) (long double complex z);                                                                                          /* 412 */
    double complex (* carg) (double complex z);                                                                                                      /* 416 */
    float complex (* cargf) (float complex z);                                                                                                       /* 420 */
    long double complex (* cargl) (long double complex z);                                                                                           /* 424 */
    double complex (* conj) (double complex z);                                                                                                      /* 428 */
    float complex (* conjf) (float complex z);                                                                                                       /* 432 */
    long double complex (* conjl) (long double complex z);                                                                                           /* 436 */
    double complex (* cproj) (double complex z);                                                                                                     /* 440 */
    float complex (* cprojf) (float complex z);                                                                                                      /* 444 */
    long double complex (* cprojl) (long double complex z);                                                                                          /* 448 */
    double (* cimag) (double complex z);                                                                                                             /* 452 */
    float (* cimagf) (float complex z);                                                                                                              /* 456 */
    long double (* cimagl) (long double complex z);                                                                                                  /* 460 */
    double (* creal) (double complex z);                                                                                                             /* 464 */
    float (* crealf) (float complex z);                                                                                                              /* 468 */
    long double (* creall) (long double complex z);                                                                                                  /* 472 */

    /* crypt.h */
    char * (* crypt_r) (const char *__phrase, const char *__salt, struct crypt_data * __restrict __data);                                            /* 476 */

    /* ctype.h */
    int (* isalnum) (int c);                                                                                                                         /* 480 */
    int (* isalpha) (int c);                                                                                                                         /* 484 */
    int (* iscntrl) (int c);                                                                                                                         /* 488 */
    int (* isdigit) (int c);                                                                                                                         /* 492 */
    int (* isgraph) (int c);                                                                                                                         /* 496 */
    int (* islower) (int c);                                                                                                                         /* 500 */
    int (* isprint) (int c);                                                                                                                         /* 504 */
    int (* ispunct) (int c);                                                                                                                         /* 508 */
    int (* isspace) (int c);                                                                                                                         /* 512 */
    int (* isupper) (int c);                                                                                                                         /* 516 */
    int (* isxdigit) (int c);                                                                                                                        /* 520 */
    int (* tolower) (int c);                                                                                                                         /* 524 */
    int (* toupper) (int c);                                                                                                                         /* 528 */
    int (* isblank) (int c);                                                                                                                         /* 532 */
    int (* isascii) (int c);                                                                                                                         /* 536 */
    int (* toascii) (int c);                                                                                                                         /* 540 */

    /* dirent.h */
    DIR * (* opendir) (const char * path_name);                                                                                                      /* 544 */
    struct dirent * (* readdir) (DIR * directory_pointer);                                                                                           /* 548 */
    void (* rewinddir) (DIR * directory_pointer);                                                                                                    /* 552 */
    int (* closedir) (DIR * directory_pointer);                                                                                                      /* 556 */
    DIR * (* fdopendir) (int);                                                                                                                       /* 560 */
    int (* alphasort) (const struct dirent **a, const struct dirent **b);                                                                            /* 564 */
    int (* dirfd) (DIR *dirp);                                                                                                                       /* 568 */
    int (* scandir) (const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));      /* 572 */
    int (* readdir_r) (DIR *dir, struct dirent *buf, struct dirent **result);                                                                        /* 576 */
    int (* readdir64_r) (DIR *dir, struct dirent *buf, struct dirent **result);                                                                      /* 580 */

    /* dlfcn.h */
    int (* dlclose) (void *__handle);                                                                                                                /* 584 */
    const char * (* dlerror) (void);                                                                                                                 /* 588 */
    void * (* dlopen) (const char *__path_name, int __mode);                                                                                         /* 592 */
    void * (* dlsym) (void *__handle, const char *__symbol_name);                                                                                    /* 596 */

    /* envz.h */
    char * (* envz_entry) (const char *envz, size_t envz_len, const char *name);                                                                     /* 600 */
    char * (* envz_get) (const char *envz, size_t envz_len, const char *name);                                                                       /* 604 */
    error_t (* envz_add) (char **envz, size_t *envz_len, const char *name, const char *value);                                                       /* 608 */
    error_t (* envz_merge) (char **envz, size_t *envz_len, const char *envz2, size_t envz2_len, int override);                                       /* 612 */
    void (* envz_remove) (char **envz, size_t *envz_len, const char *name);                                                                          /* 616 */
    void (* envz_strip) (char **envz, size_t *envz_len);                                                                                             /* 620 */

    /* err.h */
    void (* warn) (const char *, ...);                                                                                                               /* 624 */
    void (* vwarn) (const char *, va_list);                                                                                                          /* 628 */
    void (* warnx) (const char *, ...);                                                                                                              /* 632 */
    void (* vwarnx) (const char *, va_list);                                                                                                         /* 636 */
    void (* err) (int, const char *, ...);                                                                                                           /* 640 */
    void (* verr) (int, const char *, va_list);                                                                                                      /* 644 */
    void (* errx) (int, const char *, ...);                                                                                                          /* 648 */
    void (* verrx) (int, const char *, va_list);                                                                                                     /* 652 */

    /* fcntl.h */
    int (* open) (const char *path_name, int open_flag, ... /* mode_t mode */ );                                                                     /* 656 */
    int (* openat) (int fd, const char *filename, int flags, ...);                                                                                   /* 660 */
    int (* creat) (const char * path_name, mode_t mode);                                                                                             /* 664 */
    int (* close) (int file_descriptor);                                                                                                             /* 668 */
    ssize_t (* read) (int file_descriptor, void * buffer, size_t num_bytes);                                                                         /* 672 */
    ssize_t (* write) (int file_descriptor, const void * buffer, size_t num_bytes);                                                                  /* 676 */
    int (* fcntl) (int file_descriptor, int cmd, ... /* int arg */ );                                                                                /* 680 */

    /* fnmatch.h */
    int (* fnmatch) (const char *pattern, const char *string, int flags);                                                                            /* 684 */

    /* ftw.h */
    int (* ftw) (const char *path,int (*func)(const char *,const struct stat *,int),int depth);                                                      /* 688 */
    int (* nftw) (const char *path,int (*func)(const char *,const struct stat *,int,struct FTW *),int depth,int flags);                              /* 692 */

    /* getopt.h */
    int (* getopt_long) (int argc, char* const *argv, const char *optstring, const struct option *longopts, int *longindex);                         /* 696 */
    int (* getopt_long_only) (int argc, char* const *argv, const char *optstring, const struct option *longopts, int *longindex);                    /* 700 */

    /* glob.h */
    int (* glob) (const char *pattern, int flags, int (*errfunc) (const char *epath, int eerrno), glob_t *pglob);                                    /* 704 */
    void (* globfree) (glob_t *pglob);                                                                                                               /* 708 */

    /* grp.h */
    void (* endgrent) (void);                                                                                                                        /* 712 */
    struct group * (* getgrent) (void);                                                                                                              /* 716 */
    struct group * (* getgrgid) (gid_t gid);                                                                                                         /* 720 */
    struct group * (* getgrnam) (const char *name);                                                                                                  /* 724 */
    void (* setgrent) (void);                                                                                                                        /* 728 */

    /* iconv.h */
    iconv_t (* iconv_open) (const char *to, const char *from);                                                                                       /* 732 */
    size_t (* iconv) (iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);                                           /* 736 */
    int (* iconv_close) (iconv_t cd);                                                                                                                /* 740 */

    /* ifaddrs.h */
    int (* getifaddrs) (struct ifaddrs **ifap);                                                                                                      /* 744 */
    void (* freeifaddrs) (struct ifaddrs *ifa);                                                                                                      /* 748 */

    /* inttypes.h */
    intmax_t (* imaxabs) (intmax_t x);                                                                                                               /* 752 */
    imaxdiv_t (* imaxdiv) (intmax_t n,intmax_t d);                                                                                                   /* 756 */
    intmax_t (* strtoimax) (const char *str, char **ptr, int base);                                                                                  /* 760 */
    uintmax_t (* strtoumax) (const char *str, char **ptr, int base);                                                                                 /* 764 */
    intmax_t (* wcstoimax) (const wchar_t *str, wchar_t **ptr, int base);                                                                            /* 768 */
    uintmax_t (* wcstoumax) (const wchar_t *str, wchar_t **ptr, int base);                                                                           /* 772 */

    /* langinfo.h */
    char * (* nl_langinfo) (nl_item item);                                                                                                           /* 776 */

    /* libgen.h */
    char * (* basename) (const char *path);                                                                                                          /* 780 */
    char * (* dirname) (const char *path);                                                                                                           /* 784 */

    /* locale.h */
    char * (* setlocale) (int category, const char *locale);                                                                                         /* 788 */
    struct lconv * (* localeconv) (void);                                                                                                            /* 792 */

    /* malloc.h */
    void * (* memalign) (size_t alignment, size_t size);                                                                                             /* 796 */

    /* math.h */
    double (* acos) (double x);                                                                                                                      /* 800 */
    double (* asin) (double x);                                                                                                                      /* 804 */
    double (* atan) (double x);                                                                                                                      /* 808 */
    double (* atan2) (double y, double x);                                                                                                           /* 812 */
    double (* ceil) (double x);                                                                                                                      /* 816 */
    double (* cos) (double x);                                                                                                                       /* 820 */
    double (* cosh) (double x);                                                                                                                      /* 824 */
    void (* sincos) (double x, double *s, double *c);                                                                                                /* 828 */
    void (* sincosf) (float x, float *s, float *c);                                                                                                  /* 832 */
    void (* sincosl) (double x, double *s, double *c);                                                                                               /* 836 */
    double (* exp10) (double x);                                                                                                                     /* 840 */
    float (* exp10f) (float x);                                                                                                                      /* 844 */
    long double (* exp10l) (long double x);                                                                                                          /* 848 */
    double (* exp) (double x);                                                                                                                       /* 852 */
    double (* fabs) (double x);                                                                                                                      /* 856 */
    double (* floor) (double x);                                                                                                                     /* 860 */
    double (* fmod) (double x, double y);                                                                                                            /* 864 */
    double (* frexp) (double x, int *nptr);                                                                                                          /* 868 */
    double (* ldexp) (double x, int n);                                                                                                              /* 872 */
    double (* log) (double x);                                                                                                                       /* 876 */
    double (* log10) (double x);                                                                                                                     /* 880 */
    double (* modf) (double x, double *nptr);                                                                                                        /* 884 */
    double (* pow) (double x, double y);                                                                                                             /* 888 */
    double (* pow10) (double x);                                                                                                                     /* 892 */
    float (* pow10f) (float x);                                                                                                                      /* 896 */
    long double (* pow10l) (long double x);                                                                                                          /* 900 */
    double (* sin) (double x);                                                                                                                       /* 904 */
    double (* sinh) (double x);                                                                                                                      /* 908 */
    double (* sqrt) (double x);                                                                                                                      /* 912 */
    double (* tan) (double x);                                                                                                                       /* 916 */
    double (* tanh) (double x);                                                                                                                      /* 920 */
    double (* j0) (double x);                                                                                                                        /* 924 */
    double (* y0) (double x);                                                                                                                        /* 928 */
    float (* j0f) (float x);                                                                                                                         /* 932 */
    float (* y0f) (float x);                                                                                                                         /* 936 */
    double (* j1) (double x);                                                                                                                        /* 940 */
    double (* y1) (double x);                                                                                                                        /* 944 */
    float (* j1f) (float x);                                                                                                                         /* 948 */
    float (* y1f) (float x);                                                                                                                         /* 952 */
    double (* jn) (int n, double x);                                                                                                                 /* 956 */
    double (* yn) (int n, double x);                                                                                                                 /* 960 */
    float (* jnf) (int n, float x);                                                                                                                  /* 964 */
    float (* ynf) (int n, float x);                                                                                                                  /* 968 */
    int (* __fpclassify_float) (float x);                                                                                                            /* 972 */
    int (* __fpclassify_double) (double x);                                                                                                          /* 976 */
    int (* __fpclassify_long_double) (long double x);                                                                                                /* 980 */
    int (* __isfinite_float) (float x);                                                                                                              /* 984 */
    int (* __isfinite_double) (double x);                                                                                                            /* 988 */
    int (* __isfinite_long_double) (long double x);                                                                                                  /* 992 */
    int (* __signbit_float) (float x);                                                                                                               /* 996 */
    int (* __signbit_double) (double x);                                                                                                             /* 1000 */
    int (* __signbit_long_double) (long double x);                                                                                                   /* 1004 */
    int (* __isnan) (double x);                                                                                                                      /* 1008 */
    int (* __isnanf) (float x);                                                                                                                      /* 1012 */
    int (* __isnanl) (long double x);                                                                                                                /* 1016 */
    int (* __isinf) (double x);                                                                                                                      /* 1020 */
    int (* __isinff) (float x);                                                                                                                      /* 1024 */
    int (* __isinfl) (long double x);                                                                                                                /* 1028 */
    int (* __isnormalf) (float x);                                                                                                                   /* 1032 */
    int (* __isnormal) (double x);                                                                                                                   /* 1036 */
    int (* __isnormall) (long double x);                                                                                                             /* 1040 */
    float (* acosf) (float x);                                                                                                                       /* 1044 */
    float (* asinf) (float x);                                                                                                                       /* 1048 */
    float (* atanf) (float x);                                                                                                                       /* 1052 */
    float (* atan2f) (float y, float x);                                                                                                             /* 1056 */
    float (* ceilf) (float x);                                                                                                                       /* 1060 */
    float (* cosf) (float x);                                                                                                                        /* 1064 */
    float (* coshf) (float x);                                                                                                                       /* 1068 */
    float (* expf) (float x);                                                                                                                        /* 1072 */
    float (* fabsf) (float x);                                                                                                                       /* 1076 */
    float (* floorf) (float x);                                                                                                                      /* 1080 */
    float (* fmodf) (float x, float y);                                                                                                              /* 1084 */
    float (* frexpf) (float x, int *eptr);                                                                                                           /* 1088 */
    float (* ldexpf) (float x, int exp);                                                                                                             /* 1092 */
    float (* logf) (float x);                                                                                                                        /* 1096 */
    float (* log10f) (float x);                                                                                                                      /* 1100 */
    float (* modff) (float x, float *iptr);                                                                                                          /* 1104 */
    float (* powf) (float x, float y);                                                                                                               /* 1108 */
    float (* sinf) (float x);                                                                                                                        /* 1112 */
    float (* sinhf) (float x);                                                                                                                       /* 1116 */
    float (* sqrtf) (float x);                                                                                                                       /* 1120 */
    float (* tanf) (float x);                                                                                                                        /* 1124 */
    float (* tanhf) (float x);                                                                                                                       /* 1128 */
    float (* acoshf) (float x);                                                                                                                      /* 1132 */
    float (* asinhf) (float x);                                                                                                                      /* 1136 */
    float (* atanhf) (float x);                                                                                                                      /* 1140 */
    float (* cbrtf) (float x);                                                                                                                       /* 1144 */
    float (* copysignf) (float x, float y);                                                                                                          /* 1148 */
    float (* erfcf) (float x);                                                                                                                       /* 1152 */
    float (* erff) (float x);                                                                                                                        /* 1156 */
    float (* expm1f) (float x);                                                                                                                      /* 1160 */
    float (* fdimf) (float x, float y);                                                                                                              /* 1164 */
    float (* fmaf) (float x, float y, float z);                                                                                                      /* 1168 */
    float (* fmaxf) (float x, float y);                                                                                                              /* 1172 */
    float (* fminf) (float x, float y);                                                                                                              /* 1176 */
    float (* hypotf) (float x, float y);                                                                                                             /* 1180 */
    float (* lgammaf) (float x);                                                                                                                     /* 1184 */
    float (* log1pf) (float x);                                                                                                                      /* 1188 */
    float (* logbf) (float x);                                                                                                                       /* 1192 */
    long long (* llrintf) (float x);                                                                                                                 /* 1196 */
    long (* lrintf) (float x);                                                                                                                       /* 1200 */
    long (* lroundf) (float x);                                                                                                                      /* 1204 */
    long long (* llround) (double x);                                                                                                                /* 1208 */
    long long (* llroundf) (float x);                                                                                                                /* 1212 */
    float (* nanf) (const char *tagp);                                                                                                               /* 1216 */
    float (* nearbyintf) (float x);                                                                                                                  /* 1220 */
    float (* nextafterf) (float x, float y);                                                                                                         /* 1224 */
    float (* nexttowardf) (float x, long double y);                                                                                                  /* 1228 */
    float (* remainderf) (float x, float p);                                                                                                         /* 1232 */
    float (* remquof) (float x, float y, int *quo);                                                                                                  /* 1236 */
    float (* rintf) (float x);                                                                                                                       /* 1240 */
    float (* roundf) (float x);                                                                                                                      /* 1244 */
    float (* scalblnf) (float x, long n);                                                                                                            /* 1248 */
    float (* scalbnf) (float x, int n);                                                                                                              /* 1252 */
    float (* tgammaf) (float x);                                                                                                                     /* 1256 */
    float (* truncf) (float x);                                                                                                                      /* 1260 */
    int (* ilogbf) (float x);                                                                                                                        /* 1264 */
    int (* finite) (double x);                                                                                                                       /* 1268 */
    double (* acosh) (double x);                                                                                                                     /* 1272 */
    double (* asinh) (double x);                                                                                                                     /* 1276 */
    double (* atanh) (double x);                                                                                                                     /* 1280 */
    double (* cbrt) (double x);                                                                                                                      /* 1284 */
    double (* copysign) (double x, double y);                                                                                                        /* 1288 */
    double (* erf) (double x);                                                                                                                       /* 1292 */
    double (* erfc) (double x);                                                                                                                      /* 1296 */
    double (* expm1) (double x);                                                                                                                     /* 1300 */
    double (* fdim) (double x, double y);                                                                                                            /* 1304 */
    double (* fma) (double x, double y, double z);                                                                                                   /* 1308 */
    double (* fmax) (double x, double y);                                                                                                            /* 1312 */
    double (* fmin) (double x, double y);                                                                                                            /* 1316 */
    double (* hypot) (double x, double y);                                                                                                           /* 1320 */
    double (* lgamma) (double x);                                                                                                                    /* 1324 */
    double (* log1p) (double x);                                                                                                                     /* 1328 */
    double (* logb) (double x);                                                                                                                      /* 1332 */
    long long (* llrint) (double x);                                                                                                                 /* 1336 */
    long int (* lrint) (double x);                                                                                                                   /* 1340 */
    long int (* lround) (double x);                                                                                                                  /* 1344 */
    double (* nan) (const char *tagp);                                                                                                               /* 1348 */
    double (* nearbyint) (double x);                                                                                                                 /* 1352 */
    double (* nexttoward) (double x, long double y);                                                                                                 /* 1356 */
    double (* nextafter) (double x, double y);                                                                                                       /* 1360 */
    double (* remainder) (double x, double p);                                                                                                       /* 1364 */
    double (* remquo) (double x, double y, int *quo);                                                                                                /* 1368 */
    double (* rint) (double x);                                                                                                                      /* 1372 */
    double (* round) (double x);                                                                                                                     /* 1376 */
    double (* scalbln) (double x, long n);                                                                                                           /* 1380 */
    double (* scalbn) (double x, int n);                                                                                                             /* 1384 */
    double (* tgamma) (double x);                                                                                                                    /* 1388 */
    double (* trunc) (double x);                                                                                                                     /* 1392 */
    int (* ilogb) (double x);                                                                                                                        /* 1396 */
    float (* exp2f) (float x);                                                                                                                       /* 1400 */
    double (* exp2) (double x);                                                                                                                      /* 1404 */
    long double (* exp2l) (long double x);                                                                                                           /* 1408 */
    double (* log2) (double x);                                                                                                                      /* 1412 */
    float (* log2f) (float x);                                                                                                                       /* 1416 */
    long double (* log2l) (long double x);                                                                                                           /* 1420 */
    long double (* acosl) (long double x);                                                                                                           /* 1424 */
    long double (* asinl) (long double x);                                                                                                           /* 1428 */
    long double (* atanl) (long double x);                                                                                                           /* 1432 */
    long double (* atan2l) (long double y, long double x);                                                                                           /* 1436 */
    long double (* ceill) (long double x);                                                                                                           /* 1440 */
    long double (* cosl) (long double x);                                                                                                            /* 1444 */
    long double (* coshl) (long double x);                                                                                                           /* 1448 */
    long double (* expl) (long double x);                                                                                                            /* 1452 */
    long double (* fabsl) (long double x);                                                                                                           /* 1456 */
    long double (* floorl) (long double x);                                                                                                          /* 1460 */
    long double (* fmodl) (long double x, long double y);                                                                                            /* 1464 */
    long double (* frexpl) (long double x, int *nptr);                                                                                               /* 1468 */
    long double (* ldexpl) (long double x, int n);                                                                                                   /* 1472 */
    long double (* logl) (long double x);                                                                                                            /* 1476 */
    long double (* log10l) (long double x);                                                                                                          /* 1480 */
    long double (* modfl) (long double x, long double *iptr);                                                                                        /* 1484 */
    long double (* powl) (long double x, long double y);                                                                                             /* 1488 */
    long double (* sinl) (long double x);                                                                                                            /* 1492 */
    long double (* sinhl) (long double x);                                                                                                           /* 1496 */
    long double (* sqrtl) (long double x);                                                                                                           /* 1500 */
    long double (* tanl) (long double x);                                                                                                            /* 1504 */
    long double (* tanhl) (long double x);                                                                                                           /* 1508 */
    long double (* acoshl) (long double x);                                                                                                          /* 1512 */
    long double (* asinhl) (long double x);                                                                                                          /* 1516 */
    long double (* atanhl) (long double x);                                                                                                          /* 1520 */
    long double (* cbrtl) (long double x);                                                                                                           /* 1524 */
    long double (* copysignl) (long double x, long double y);                                                                                        /* 1528 */
    long double (* erfcl) (long double x);                                                                                                           /* 1532 */
    long double (* erfl) (long double x);                                                                                                            /* 1536 */
    long double (* expm1l) (long double x);                                                                                                          /* 1540 */
    long double (* fdiml) (long double x, long double y);                                                                                            /* 1544 */
    long double (* fmal) (long double x, long double y, long double z);                                                                              /* 1548 */
    long double (* fmaxl) (long double x, long double y);                                                                                            /* 1552 */
    long double (* fminl) (long double x, long double y);                                                                                            /* 1556 */
    long double (* hypotl) (long double x, long double y);                                                                                           /* 1560 */
    long double (* lgammal) (long double x);                                                                                                         /* 1564 */
    long double (* log1pl) (long double x);                                                                                                          /* 1568 */
    long double (* logbl) (long double x);                                                                                                           /* 1572 */
    long long int (* llrintl) (long double x);                                                                                                       /* 1576 */
    long int (* lrintl) (long double x);                                                                                                             /* 1580 */
    long long int (* llroundl) (long double x);                                                                                                      /* 1584 */
    long int (* lroundl) (long double x);                                                                                                            /* 1588 */
    long double (* nanl) (const char *tagp);                                                                                                         /* 1592 */
    long double (* nearbyintl) (long double x);                                                                                                      /* 1596 */
    long double (* nextafterl) (long double x, long double y);                                                                                       /* 1600 */
    long double (* nexttowardl) (long double x, long double y);                                                                                      /* 1604 */
    long double (* remainderl) (long double x, long double p);                                                                                       /* 1608 */
    long double (* remquol) (long double x, long double y, int *quo);                                                                                /* 1612 */
    long double (* rintl) (long double x);                                                                                                           /* 1616 */
    long double (* roundl) (long double x);                                                                                                          /* 1620 */
    long double (* scalblnl) (long double x, long int n);                                                                                            /* 1624 */
    long double (* scalbnl) (long double x, int n);                                                                                                  /* 1628 */
    long double (* tgammal) (long double x);                                                                                                         /* 1632 */
    long double (* truncl) (long double x);                                                                                                          /* 1636 */
    int (* ilogbl) (long double x);                                                                                                                  /* 1640 */

    /* netdb.h */
    struct hostent * (* gethostbyaddr) (const void *addr, socklen_t len, int type);                                                                  /* 1644 */
    struct hostent * (* gethostbyname) (const char *name);                                                                                           /* 1648 */
    struct netent * (* getnetbyaddr) (in_addr_t net, int type);                                                                                      /* 1652 */
    struct netent * (* getnetbyname) (const char *name);                                                                                             /* 1656 */
    struct protoent * (* getprotobyname) (const char *name);                                                                                         /* 1660 */
    struct protoent * (* getprotobynumber) (int proto);                                                                                              /* 1664 */
    struct servent * (* getservbyname) (const char *name, const char *proto);                                                                        /* 1668 */
    struct servent * (* getservbyport) (int port, const char *proto);                                                                                /* 1672 */
    void (* herror) (const char *);                                                                                                                  /* 1676 */
    const char * (* hstrerror) (int);                                                                                                                /* 1680 */
    int (* gethostbyname_r) (const char *, struct hostent *, char *, size_t, struct hostent **, int *);                                              /* 1684 */
    int (* gethostbyname2_r) (const char *, int, struct hostent *, char *, size_t, struct hostent **, int *);                                        /* 1688 */
    struct hostent * (* gethostbyname2) (const char *, int);                                                                                         /* 1692 */
    int (* gethostbyaddr_r) (const void *, socklen_t, int, struct hostent *, char *, size_t, struct hostent **, int *);                              /* 1696 */
    int (* getservbyport_r) (int, const char *, struct servent *, char *, size_t, struct servent **);                                                /* 1700 */
    int (* getservbyname_r) (const char *, const char *, struct servent *, char *, size_t, struct servent **);                                       /* 1704 */
    const char * (* gai_strerror) (int ecode);                                                                                                       /* 1708 */
    int (* getaddrinfo) (const char *name, const char *service, const struct addrinfo *req, struct addrinfo **pai);                                  /* 1712 */
    void (* freeaddrinfo) (struct addrinfo *ai);                                                                                                     /* 1716 */
    int (* getnameinfo) (const struct sockaddr *sa, socklen_t salen, char *node, socklen_t nodelen, char *service, socklen_t servicelen, int flags); /* 1720 */

    /* nl_types.h */
    nl_catd (* catopen) (const char* name, int flag);                                                                                                /* 1724 */
    char* (* catgets) (nl_catd catalog, int set_number, int msg_number, const char* msg);                                                            /* 1728 */
    int (* catclose) (nl_catd catalog);                                                                                                              /* 1732 */

    /* poll.h */
    int (* poll) (struct pollfd *fds, nfds_t nfds, int timeout);                                                                                     /* 1736 */

    /* pthread.h */
    int (* pthread_attr_init) (pthread_attr_t *attr);                                                                                                /* 1740 */
    int (* pthread_attr_destroy) (pthread_attr_t *attr);                                                                                             /* 1744 */
    int (* pthread_attr_getdetachstate) (const pthread_attr_t *attr, int *detachstate);                                                              /* 1748 */
    int (* pthread_attr_setdetachstate) (pthread_attr_t *attr, int detachstate);                                                                     /* 1752 */
    int (* pthread_attr_getstack) (const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);                                                 /* 1756 */
    int (* pthread_attr_setstack) (pthread_attr_t *attr, void *stackaddr, size_t stacksize);                                                         /* 1760 */
    int (* pthread_attr_getstacksize) (const pthread_attr_t *attr, size_t *stacksize);                                                               /* 1764 */
    int (* pthread_attr_setstacksize) (pthread_attr_t *attr, size_t stacksize);                                                                      /* 1768 */
    int (* pthread_attr_getschedparam) (const pthread_attr_t *attr, struct sched_param *param);                                                      /* 1772 */
    int (* pthread_attr_setschedparam) (pthread_attr_t *attr, const struct sched_param *param);                                                      /* 1776 */
    int (* pthread_attr_getschedpolicy) (const pthread_attr_t *attr, int *policy);                                                                   /* 1780 */
    int (* pthread_attr_setschedpolicy) (pthread_attr_t *attr, int policy);                                                                          /* 1784 */
    int (* pthread_attr_getinheritsched) (const pthread_attr_t *attr, int *inheritsched);                                                            /* 1788 */
    int (* pthread_attr_setinheritsched) (pthread_attr_t *attr, int inheritsched);                                                                   /* 1792 */
    int (* pthread_attr_getscope) (const pthread_attr_t *attr, int *contentionscope);                                                                /* 1796 */
    int (* pthread_attr_setscope) (pthread_attr_t *attr, int contentionscope);                                                                       /* 1800 */
    int (* pthread_attr_setguardsize) (pthread_attr_t *attr, size_t guardsize);                                                                      /* 1804 */
    int (* pthread_attr_getguardsize) (const pthread_attr_t *attr, size_t *guardsize);                                                               /* 1808 */
    int (* pthread_create) (pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);                                       /* 1812 */
    int (* pthread_detach) (pthread_t thread);                                                                                                       /* 1816 */
    int (* pthread_equal) (pthread_t t1, pthread_t t2);                                                                                              /* 1820 */
    void (* pthread_exit) (void *value_ptr);                                                                                                         /* 1824 */
    int (* pthread_join) (pthread_t thread, void **value_ptr);                                                                                       /* 1828 */
    pthread_t (* pthread_self) (void);                                                                                                               /* 1832 */
    int (* pthread_cancel) (pthread_t thread);                                                                                                       /* 1836 */
    int (* pthread_setcancelstate) (int state, int *oldstate);                                                                                       /* 1840 */
    int (* pthread_setcanceltype) (int type, int *oldtype);                                                                                          /* 1844 */
    void (* pthread_testcancel) (void);                                                                                                              /* 1848 */
    int (* pthread_once) (pthread_once_t *once_control, void (*init_routine)(void));                                                                 /* 1852 */
    int (* pthread_setschedprio) (pthread_t thread, int prio);                                                                                       /* 1856 */
    int (* pthread_setschedparam) (pthread_t thread, int policy, const struct sched_param *param);                                                   /* 1860 */
    int (* pthread_getschedparam) (pthread_t thread, int *policy, struct sched_param *param);                                                        /* 1864 */
    int (* pthread_setconcurrency) (int level);                                                                                                      /* 1868 */
    int (* pthread_getconcurrency) (void);                                                                                                           /* 1872 */
    int (* pthread_key_create) (pthread_key_t *key, void (*destructor)(void *));                                                                     /* 1876 */
    int (* pthread_key_delete) (pthread_key_t key);                                                                                                  /* 1880 */
    int (* pthread_setspecific) (pthread_key_t key, const void *value);                                                                              /* 1884 */
    void * (* pthread_getspecific) (pthread_key_t key);                                                                                              /* 1888 */
    int (* pthread_mutexattr_init) (pthread_mutexattr_t *attr);                                                                                      /* 1892 */
    int (* pthread_mutexattr_destroy) (pthread_mutexattr_t *attr);                                                                                   /* 1896 */
    int (* pthread_mutexattr_getpshared) (const pthread_mutexattr_t *attr, int *pshared);                                                            /* 1900 */
    int (* pthread_mutexattr_setpshared) (pthread_mutexattr_t *attr, int pshared);                                                                   /* 1904 */
    int (* pthread_mutexattr_gettype) (pthread_mutexattr_t *attr, int *kind);                                                                        /* 1908 */
    int (* pthread_mutexattr_settype) (pthread_mutexattr_t *attr, int kind);                                                                         /* 1912 */
    int (* pthread_mutex_init) (pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);                                                            /* 1916 */
    int (* pthread_mutex_destroy) (pthread_mutex_t *mutex);                                                                                          /* 1920 */
    int (* pthread_mutex_lock) (pthread_mutex_t *mutex);                                                                                             /* 1924 */
    int (* pthread_mutex_timedlock) (pthread_mutex_t *mutex, const struct timespec *abstime);                                                        /* 1928 */
    int (* pthread_mutex_trylock) (pthread_mutex_t *mutex);                                                                                          /* 1932 */
    int (* pthread_mutex_unlock) (pthread_mutex_t *mutex);                                                                                           /* 1936 */
    int (* pthread_condattr_init) (pthread_condattr_t *attr);                                                                                        /* 1940 */
    int (* pthread_condattr_destroy) (pthread_condattr_t *attr);                                                                                     /* 1944 */
    int (* pthread_condattr_getpshared) (const pthread_condattr_t *attr, int *pshared);                                                              /* 1948 */
    int (* pthread_condattr_setpshared) (pthread_condattr_t *attr, int pshared);                                                                     /* 1952 */
    int (* pthread_cond_init) (pthread_cond_t *cond, const pthread_condattr_t *attr);                                                                /* 1956 */
    int (* pthread_cond_destroy) (pthread_cond_t *cond);                                                                                             /* 1960 */
    int (* pthread_cond_wait) (pthread_cond_t *cond, pthread_mutex_t *mutex);                                                                        /* 1964 */
    int (* pthread_cond_timedwait) (pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);                                   /* 1968 */
    int (* pthread_cond_signal) (pthread_cond_t *cond);                                                                                              /* 1972 */
    int (* pthread_cond_broadcast) (pthread_cond_t *cond);                                                                                           /* 1976 */
    int (* pthread_barrierattr_init) (pthread_barrierattr_t *attr);                                                                                  /* 1980 */
    int (* pthread_barrierattr_destroy) (pthread_barrierattr_t *attr);                                                                               /* 1984 */
    int (* pthread_barrierattr_getpshared) (const pthread_barrierattr_t *attr, int *pshared);                                                        /* 1988 */
    int (* pthread_barrierattr_setpshared) (pthread_barrierattr_t *attr, int pshared);                                                               /* 1992 */
    int (* pthread_barrier_init) (pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);                                /* 1996 */
    int (* pthread_barrier_destroy) (pthread_barrier_t *barrier);                                                                                    /* 2000 */
    int (* pthread_barrier_wait) (pthread_barrier_t *barrier);                                                                                       /* 2004 */
    int (* pthread_rwlockattr_init) (pthread_rwlockattr_t *attr);                                                                                    /* 2008 */
    int (* pthread_rwlockattr_destroy) (pthread_rwlockattr_t *attr);                                                                                 /* 2012 */
    int (* pthread_rwlockattr_getpshared) (const pthread_rwlockattr_t *attr, int *pshared);                                                          /* 2016 */
    int (* pthread_rwlockattr_setpshared) (pthread_rwlockattr_t *attr, int pshared);                                                                 /* 2020 */
    int (* pthread_rwlock_init) (pthread_rwlock_t *lock, const pthread_rwlockattr_t *attr);                                                          /* 2024 */
    int (* pthread_rwlock_destroy) (pthread_rwlock_t *lock);                                                                                         /* 2028 */
    int (* pthread_rwlock_tryrdlock) (pthread_rwlock_t *lock);                                                                                       /* 2032 */
    int (* pthread_rwlock_trywrlock) (pthread_rwlock_t *lock);                                                                                       /* 2036 */
    int (* pthread_rwlock_rdlock) (pthread_rwlock_t *lock);                                                                                          /* 2040 */
    int (* pthread_rwlock_timedrdlock) (pthread_rwlock_t *lock, const struct timespec *abstime);                                                     /* 2044 */
    int (* pthread_rwlock_wrlock) (pthread_rwlock_t *lock);                                                                                          /* 2048 */
    int (* pthread_rwlock_timedwrlock) (pthread_rwlock_t *lock, const struct timespec *abstime);                                                     /* 2052 */
    int (* pthread_rwlock_unlock) (pthread_rwlock_t *lock);                                                                                          /* 2056 */
    int (* pthread_spin_init) (pthread_spinlock_t *lock, int pshared);                                                                               /* 2060 */
    int (* pthread_spin_destroy) (pthread_spinlock_t *lock);                                                                                         /* 2064 */
    int (* pthread_spin_lock) (pthread_spinlock_t *lock);                                                                                            /* 2068 */
    int (* pthread_spin_trylock) (pthread_spinlock_t *lock);                                                                                         /* 2072 */
    int (* pthread_spin_unlock) (pthread_spinlock_t *lock);                                                                                          /* 2076 */
    int (* pthread_setname_np) (pthread_t thread, const char *name);                                                                                 /* 2080 */
    int (* pthread_getname_np) (pthread_t thread, char *name, size_t len);                                                                           /* 2084 */
    int (* pthread_cond_timedwait_relative_np) (pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *reltime);                       /* 2088 */
    int (* pthread_getattr_np) (pthread_t thread, pthread_attr_t *attr);                                                                             /* 2092 */
    void (* pthread_cleanup_push) (void (*routine)(void *), void *arg);                                                                              /* 2096 */
    void (* pthread_cleanup_pop) (int execute);                                                                                                      /* 2100 */
    int (* pthread_kill) (pthread_t thread, int sig);                                                                                                /* 2104 */

    /* pwd.h */
    void (* endpwent) (void);                                                                                                                        /* 2108 */
    struct passwd * (* getpwent) (void);                                                                                                             /* 2112 */
    struct passwd * (* getpwnam) (const char *name);                                                                                                 /* 2116 */
    int (* getpwnam_r) (const char *name, struct passwd *pw, char *buffer, size_t bufsize, struct passwd **result);                                  /* 2120 */
    struct passwd * (* getpwuid) (uid_t uid);                                                                                                        /* 2124 */
    int (* getpwuid_r) (uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);                                        /* 2128 */
    void (* setpwent) (void);                                                                                                                        /* 2132 */

    /* regex.h */
    int (* regcomp) (regex_t * preg, const char * regex, int cflags);                                                                                /* 2136 */
    int (* regexec) (const regex_t *, const char *, size_t, regmatch_t *, int);                                                                      /* 2140 */
    void (* regfree) (regex_t *preg);                                                                                                                /* 2144 */
    size_t (* regerror) (int e, const regex_t * preg, char * buf, size_t size);                                                                      /* 2148 */

    /* resolv.h */
    int (* res_init) (void);                                                                                                                         /* 2152 */
    int (* res_query) (const char *dname, int class, int type, unsigned char *answer, int anslen);                                                   /* 2156 */
    int (* res_querydomain) (const char *name, const char *domain,int class, int type, unsigned char *answer,int anslen);                            /* 2160 */
    int (* res_search) (const char *dname, int class, int type, unsigned char *answer, int anslen);                                                  /* 2164 */
    int (* res_mkquery) (int, const char *, int, int, const unsigned char *, int, const unsigned char *, unsigned char *, int);                      /* 2168 */
    int (* res_send) (const unsigned char *msg, int msglen, unsigned char *answer,int anslen);                                                       /* 2172 */
    int (* dn_comp) (unsigned char *exp_dn, unsigned char *comp_dn,int length, unsigned char **dnptrs, unsigned char **lastdnptr);                   /* 2176 */
    int (* dn_expand) (unsigned char *msg, unsigned char *eomorig,unsigned char *comp_dn, char *exp_dn,int length);                                  /* 2180 */
    int (* dn_skipname) (const unsigned char *, const unsigned char *);                                                                              /* 2184 */

    /* sched.h */
    int (* sched_get_priority_max) (int);                                                                                                            /* 2188 */
    int (* sched_get_priority_min) (int);                                                                                                            /* 2192 */
    int (* sched_yield) (void);                                                                                                                      /* 2196 */

    /* search.h */
    int (* hcreate) (size_t);                                                                                                                        /* 2200 */
    void (* hdestroy) (void);                                                                                                                        /* 2204 */
    ENTRY * (* hsearch) (ENTRY, ACTION);                                                                                                             /* 2208 */
    int (* hcreate_r) (size_t, struct hsearch_data *);                                                                                               /* 2212 */
    void (* hdestroy_r) (struct hsearch_data *);                                                                                                     /* 2216 */
    int (* hsearch_r) (ENTRY, ACTION, ENTRY **, struct hsearch_data *);                                                                              /* 2220 */
    void * (* lfind) (const void *key, const void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                     /* 2224 */
    void * (* lsearch) (const void *key, void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                         /* 2228 */
    void * (* tdelete) (const void *vkey, void **vrootp, int (*compar)(const void *, const void *));                                                 /* 2232 */
    void (* tdestroy) (void *vrootp, void (*freefct)(void *));                                                                                       /* 2236 */
    void * (* tfind) (const void *, void **, int (*)(const void *, const void *));                                                                   /* 2240 */
    void * (* tsearch) (const void *, void **, int (*)(const void *, const void *));                                                                 /* 2244 */
    void (* twalk) (const void *root, void (*action)(const void *, VISIT, int));                                                                     /* 2248 */

    /* semaphore.h */
    int (* sem_init) (sem_t *sem, int pshared, unsigned int value);                                                                                  /* 2252 */
    int (* sem_destroy) (sem_t *sem);                                                                                                                /* 2256 */
    int (* sem_trywait) (sem_t *sem);                                                                                                                /* 2260 */
    int (* sem_wait) (sem_t *sem);                                                                                                                   /* 2264 */
    int (* sem_timedwait) (sem_t *sem, const struct timespec *abstime);                                                                              /* 2268 */
    int (* sem_post) (sem_t *sem);                                                                                                                   /* 2272 */
    sem_t * (* sem_open) (const char *name, int oflag, mode_t mode, unsigned int value);                                                             /* 2276 */
    int (* sem_close) (sem_t *sem);                                                                                                                  /* 2280 */
    int (* sem_unlink) (const char *name);                                                                                                           /* 2284 */
    int (* sem_getvalue) (sem_t *sem, int *sval);                                                                                                    /* 2288 */

    /* setjmp.h */
    int (* setjmp) (jmp_buf __env);                                                                                                                  /* 2292 */
    int (* __sigsetjmp) (struct __jmp_buf_tag __env[1], int __savemask);                                                                             /* 2296 */
    int (* _setjmp) (struct __jmp_buf_tag __env[1]);                                                                                                 /* 2300 */
    void (* longjmp) (jmp_buf __env, int __val) __attribute__ ((__noreturn__));                                                                      /* 2304 */
    void (* _longjmp) (struct __jmp_buf_tag __env[1], int __val) __attribute__ ((__noreturn__));                                                     /* 2308 */
    void (* siglongjmp) (sigjmp_buf __env, int __val) __attribute__ ((__noreturn__));                                                                /* 2312 */
    int (* __sigjmp_save) (jmp_buf __env, int __savemask);                                                                                           /* 2316 */

    /* signal.h */
    _sig_func_ptr (* signal) (int, _sig_func_ptr);;                                                                                                  /* 2320 */
    int (* raise) (int sig);                                                                                                                         /* 2324 */
    int (* sigmask) (int signum);                                                                                                                    /* 2328 */
    int (* sigblock) (int signal_mask);                                                                                                              /* 2332 */
    int (* sigsetmask) (int signal_mask);                                                                                                            /* 2336 */
    int (* sigprocmask) (int how, const sigset_t *set, sigset_t *oset);                                                                              /* 2340 */
    int (* sigismember) (const sigset_t *set, int sig);                                                                                              /* 2344 */
    int (* sigemptyset) (sigset_t *set);                                                                                                             /* 2348 */
    int (* sigfillset) (sigset_t *set);                                                                                                              /* 2352 */
    int (* sigdelset) (sigset_t *set, int sig);                                                                                                      /* 2356 */
    int (* sigaddset) (sigset_t *set, int sig);                                                                                                      /* 2360 */
    int (* kill) (pid_t pid, int signal_number);                                                                                                     /* 2364 */
    int (* sigaction) (int sig, const struct sigaction *act, struct sigaction *oact);                                                                /* 2368 */

    /* stdio.h */
    void (* perror) (const char *s);                                                                                                                 /* 2372 */
    FILE * (* fopen) (const char *filename, const char *mode);                                                                                       /* 2376 */
    int (* fclose) (FILE *stream);                                                                                                                   /* 2380 */
    int (* fflush) (FILE *stream);                                                                                                                   /* 2384 */
    FILE * (* freopen) (const char *filename, const char *mode, FILE *stream);                                                                       /* 2388 */
    int (* setvbuf) (FILE *stream, char *buf, int bufmode, size_t size);                                                                             /* 2392 */
    void (* setbuf) (FILE *stream, char *buf);                                                                                                       /* 2396 */
    int (* fseek) (FILE *stream, long int offset, int wherefrom);                                                                                    /* 2400 */
    long int (* ftell) (FILE *stream);                                                                                                               /* 2404 */
    void (* rewind) (FILE *stream);                                                                                                                  /* 2408 */
    int (* fgetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 2412 */
    int (* fsetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 2416 */
    int (* fgetc) (FILE *stream);                                                                                                                    /* 2420 */
    int (* getc) (FILE *stream);                                                                                                                     /* 2424 */
    int (* getchar) (void);                                                                                                                          /* 2428 */
    int (* ungetc) (int c, FILE *stream);                                                                                                            /* 2432 */
    int (* fputc) (int c, FILE *stream);                                                                                                             /* 2436 */
    int (* putc) (int c, FILE *stream);                                                                                                              /* 2440 */
    int (* putchar) (int c);                                                                                                                         /* 2444 */
    char * (* fgets) (char *s, int n, FILE *stream);                                                                                                 /* 2448 */
    char * (* gets) (char *s);                                                                                                                       /* 2452 */
    int (* fscanf) (FILE *stream, const char *format, ...);                                                                                          /* 2456 */
    int (* scanf) (const char *format, ...);                                                                                                         /* 2460 */
    int (* sscanf) (const char *s, const char *format, ...);                                                                                         /* 2464 */
    int (* fputs) (const char *s, FILE *stream);                                                                                                     /* 2468 */
    int (* puts) (const char *s);                                                                                                                    /* 2472 */
    int (* dprintf) (int fd, const char *format, ...);                                                                                               /* 2476 */
    int (* fprintf) (FILE *stream, const char *format, ...);                                                                                         /* 2480 */
    int (* printf) (const char *format, ...);                                                                                                        /* 2484 */
    int (* sprintf) (char *s, const char *format, ...);                                                                                              /* 2488 */
    int (* vdprintf) (int fd, const char *format, va_list arg);                                                                                      /* 2492 */
    int (* vfprintf) (FILE *stream, const char *format, va_list arg);                                                                                /* 2496 */
    int (* vprintf) (const char *format, va_list arg);                                                                                               /* 2500 */
    int (* vsprintf) (char *s, const char *format, va_list arg);                                                                                     /* 2504 */
    size_t (* fread) (void *ptr, size_t element_size, size_t count, FILE *stream);                                                                   /* 2508 */
    size_t (* fwrite) (const void *ptr, size_t element_size, size_t count, FILE *stream);                                                            /* 2512 */
    int (* feof) (FILE *stream);                                                                                                                     /* 2516 */
    int (* ferror) (FILE *stream);                                                                                                                   /* 2520 */
    void (* clearerr) (FILE *stream);                                                                                                                /* 2524 */
    int (* rename) (const char *oldname, const char *newname);                                                                                       /* 2528 */
    int (* remove) (const char *filename);                                                                                                           /* 2532 */
    FILE * (* tmpfile) (void);                                                                                                                       /* 2536 */
    char * (* tmpnam) (char *buf);                                                                                                                   /* 2540 */
    char * (* tempnam) (const char *dir, const char *pfx);                                                                                           /* 2544 */
    char * (* ctermid) (char *);                                                                                                                     /* 2548 */
    int (* __flush) (FILE *stream);                                                                                                                  /* 2552 */
    int (* __unlockfile) (FILE *stream, int c);                                                                                                      /* 2556 */
    int (* getc_unlocked) (FILE *stream);                                                                                                            /* 2560 */
    int (* getchar_unlocked) (void);                                                                                                                 /* 2564 */
    int (* putc_unlocked) (int c, FILE *stream);                                                                                                     /* 2568 */
    int (* putchar_unlocked) (int c);                                                                                                                /* 2572 */
    ssize_t (* getline) (char **lineptr, size_t *n, FILE *stream);                                                                                   /* 2576 */
    ssize_t (* getdelim) (char **lineptr, size_t *n, int delim, FILE *stream);                                                                       /* 2580 */
    FILE * (* fdopen) (int file_descriptor, const char *type);                                                                                       /* 2584 */
    int (* fileno) (FILE *file);                                                                                                                     /* 2588 */
    int (* asprintf) (char **ret, const char *format, ...);                                                                                          /* 2592 */
    int (* vsnprintf) (char *s, size_t size, const char *format, va_list arg);                                                                       /* 2596 */
    int (* pclose) (FILE *stream);                                                                                                                   /* 2600 */
    FILE * (* popen) (const char *command, const char *type);                                                                                        /* 2604 */
    int (* fseeko) (FILE *stream, off_t offset, int wherefrom);                                                                                      /* 2608 */
    off_t (* ftello) (FILE *stream);                                                                                                                 /* 2612 */
    void (* flockfile) (FILE *file);                                                                                                                 /* 2616 */
    void (* funlockfile) (FILE *file);                                                                                                               /* 2620 */
    int (* ftrylockfile) (FILE *file);                                                                                                               /* 2624 */
    int (* vasprintf) (char **ret, const char *format, va_list arg);                                                                                 /* 2628 */
    int (* vfscanf) (FILE *stream, const char *format, va_list arg);                                                                                 /* 2632 */
    int (* vsscanf) (const char *s, const char *format, va_list arg);                                                                                /* 2636 */
    int (* vscanf) (const char *format, va_list arg);                                                                                                /* 2640 */
    int (* snprintf) (char *s, size_t size, const char *format, ...);                                                                                /* 2644 */

    /* stdio_ext.h */
    void (* _flushlbf) (void);                                                                                                                       /* 2648 */
    int (* __fsetlocking) (FILE *, int);                                                                                                             /* 2652 */
    int (* __fwriting) (FILE *);                                                                                                                     /* 2656 */
    int (* __freading) (FILE *);                                                                                                                     /* 2660 */
    int (* __freadable) (FILE *);                                                                                                                    /* 2664 */
    int (* __fwritable) (FILE *);                                                                                                                    /* 2668 */
    int (* __flbf) (FILE *);                                                                                                                         /* 2672 */
    size_t (* __fbufsize) (FILE *);                                                                                                                  /* 2676 */
    size_t (* __fpending) (FILE *);                                                                                                                  /* 2680 */
    int (* __fpurge) (FILE *);                                                                                                                       /* 2684 */
    void (* fpurge) (FILE *);                                                                                                                        /* 2688 */
    void (* __fseterr) (FILE *);                                                                                                                     /* 2692 */
    size_t (* __freadahead) (FILE *);                                                                                                                /* 2696 */
    void (* __freadptrinc) (FILE *f, size_t inc);                                                                                                    /* 2700 */

    /* stdlib.h */
    int * (* __mb_cur_max) (void);                                                                                                                  /* 2704 */
    int (* mblen) (const char *s, size_t n);                                                                                                         /* 2708 */
    int (* mbtowc) (wchar_t *pwc, const char *s, size_t n);                                                                                          /* 2712 */
    int (* _mbtowc_r) (wchar_t *pwc, const char *s, size_t n, _mbstate_t *st);                                                                       /* 2716 */
    size_t (* _mbstowcs_r) (wchar_t *pwcs, const char *s, size_t n, mbstate_t *state);                                                               /* 2720 */
    int (* wctomb) (char *s, wchar_t wchar);                                                                                                         /* 2724 */
    int (* _wctomb_r) (char *s, wchar_t wchar, mbstate_t *state);                                                                                    /* 2728 */
    size_t (* mbstowcs) (wchar_t *ws, const char *s, size_t wn);                                                                                     /* 2732 */
    size_t (* wcstombs) (char *s, const wchar_t *pwcs, size_t n);                                                                                    /* 2736 */
    size_t (* _wcstombs_r) (char *s, const wchar_t *pwcs, size_t n, mbstate_t *state);                                                               /* 2740 */
    void * (* malloc) (size_t size);                                                                                                                 /* 2744 */
    void * (* calloc) (size_t num_elements, size_t element_size);                                                                                    /* 2748 */
    void (* free) (void *ptr);                                                                                                                       /* 2752 */
    void * (* realloc) (void *ptr, size_t size);                                                                                                     /* 2756 */
    void * (* valloc) (size_t size);                                                                                                                 /* 2760 */
    void * (* aligned_alloc) (size_t alignment, size_t size);                                                                                        /* 2764 */
    int (* posix_memalign) (void **memptr, size_t alignment, size_t size);                                                                           /* 2768 */
    int (* abs) (int x);                                                                                                                             /* 2772 */
    long (* labs) (long x);                                                                                                                          /* 2776 */
    div_t (* div) (int n, int d);                                                                                                                    /* 2780 */
    ldiv_t (* ldiv) (long n, long d);                                                                                                                /* 2784 */
    int (* rand) (void);                                                                                                                             /* 2788 */
    void (* srand) (unsigned int seed);                                                                                                              /* 2792 */
    long (* random) (void);                                                                                                                          /* 2796 */
    void (* srandom) (unsigned int seed);                                                                                                            /* 2800 */
    char * (* initstate) (unsigned int seed, char *state, size_t size);                                                                              /* 2804 */
    char * (* setstate) (const char *state);                                                                                                         /* 2808 */
    char * (* l64a) (long x0);                                                                                                                       /* 2812 */
    long (* a64l) (const char *s);                                                                                                                   /* 2816 */
    void (* setkey) (const char *key);                                                                                                               /* 2820 */
    int (* system) (const char *command);                                                                                                            /* 2824 */
    void (* exit) (int status);                                                                                                                      /* 2828 */
    void (* abort) (void);                                                                                                                           /* 2832 */
    int (* atexit) (void (*)(void));                                                                                                                 /* 2836 */
    char * (* getenv) (const char *name);                                                                                                            /* 2840 */
    void * (* bsearch) (const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value));           /* 2844 */
    void (* qsort) (void *base, size_t count, size_t size, int (*compare)(const void *element1, const void *element2));                              /* 2848 */
    double (* strtod) (const char *str, char **ptr);                                                                                                 /* 2852 */
    long (* strtol) (const char *str, char **ptr, int base);                                                                                         /* 2856 */
    unsigned long (* strtoul) (const char *str, char **ptr, int base);                                                                               /* 2860 */
    quad_t (* strtoq) (const char *nptr, char **endptr, register int base);                                                                          /* 2864 */
    double (* atof) (const char *str);                                                                                                               /* 2868 */
    int (* atoi) (const char *str);                                                                                                                  /* 2872 */
    long (* atol) (const char *str);                                                                                                                 /* 2876 */
    char * (* itoa) (int value, char *buffer, int base);                                                                                             /* 2880 */
    char * (* lltoa) (int64_t ll, char *buffer, int radix);                                                                                          /* 2884 */
    void (* _exit) (int status);                                                                                                                     /* 2888 */
    int (* rand_r) (unsigned int *seed);                                                                                                             /* 2892 */
    char * (* mktemp) (char *name_template);                                                                                                         /* 2896 */
    int (* mkstemp) (char *name_template);                                                                                                           /* 2900 */
    int (* unsetenv) (const char *name);                                                                                                             /* 2904 */
    int (* setenv) (const char *name, const char *value, int overwrite);                                                                             /* 2908 */
    int (* putenv) (const char *string);                                                                                                             /* 2912 */
    char * (* mkdtemp) (char *name_template);                                                                                                        /* 2916 */
    const char * (* getexecname) (void);                                                                                                             /* 2920 */
    long long (* strtoll) (const char *str, char **ptr, int base);                                                                                   /* 2924 */
    long double (* strtold) (const char *str, char **ptr);                                                                                           /* 2928 */
    unsigned long long (* strtoull) (const char *str, char **ptr, int base);                                                                         /* 2932 */
    long long (* atoll) (const char *str);                                                                                                           /* 2936 */
    long long (* llabs) (long long x);                                                                                                               /* 2940 */
    lldiv_t (* lldiv) (long long n, long long d);                                                                                                    /* 2944 */
    float (* strtof) (const char *str, char **ptr);                                                                                                  /* 2948 */
    float (* atoff) (const char *nptr);                                                                                                              /* 2952 */
    void (* _Exit) (int status);                                                                                                                     /* 2956 */
    char * (* ecvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2960 */
    char * (* fcvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2964 */
    char * (* gcvt) (double x, int n, char *b);                                                                                                      /* 2968 */
    char * (* secure_getenv) (const char *name);                                                                                                     /* 2972 */
    void * (* reallocarray) (void *ptr, size_t m, size_t n);                                                                                         /* 2976 */
    void (* qsort_r) (void *, size_t, size_t, int (*)(const void *, const void *, void *), void *);                                                  /* 2980 */
    uint32_t (* arc4random) (void);                                                                                                                  /* 2984 */
    void (* arc4random_buf) (void *buf, size_t nbytes);                                                                                              /* 2988 */
    uint32_t (* arc4random_uniform) (uint32_t upper_bound);                                                                                          /* 2992 */
    void (* arc4random_stir) (void);                                                                                                                 /* 2996 */
    void (* arc4random_addrandom) (unsigned char *dat, int datlen);                                                                                  /* 3000 */

    /* string.h */
    char * (* strerror) (int error_number);                                                                                                          /* 3004 */
    char * (* strcat) (char *dest, const char *src);                                                                                                 /* 3008 */
    char * (* strncat) (char *dest, const char *src, size_t n);                                                                                      /* 3012 */
    int (* strcmp) (const char *s1, const char * s2);                                                                                                /* 3016 */
    int (* strncmp) (const char *s1, const char *s2, size_t n);                                                                                      /* 3020 */
    char * (* strcpy) (char *dest, const char *src);                                                                                                 /* 3024 */
    char * (* strncpy) (char *dest, const char *src, size_t n);                                                                                      /* 3028 */
    size_t (* strnlen) (const char *s, size_t maxlen);                                                                                               /* 3032 */
    size_t (* strlen) (const char *s);                                                                                                               /* 3036 */
    char * (* strchr) (const char *s, int c);                                                                                                        /* 3040 */
    char * (* strrchr) (const char *s, int c);                                                                                                       /* 3044 */
    size_t (* strspn) (const char *s, const char *set);                                                                                              /* 3048 */
    size_t (* strcspn) (const char *s, const char *set);                                                                                             /* 3052 */
    char * (* strpbrk) (const char *s, const char *set);                                                                                             /* 3056 */
    char * (* strtok) (char *str, const char *set);                                                                                                  /* 3060 */
    char * (* strstr) (const char *src, const char *sub);                                                                                            /* 3064 */
    char * (* strnstr) (const char *src, const char *sub, size_t len);                                                                               /* 3068 */
    char * (* strsep) (char ** src, const char *delim);                                                                                              /* 3072 */
    char * (* stpcpy) (char *dest, const char *src);                                                                                                 /* 3076 */
    char * (* stpncpy) (char *dest, const char *src, size_t len);                                                                                    /* 3080 */
    size_t (* stccpy) (char * dest, const char * src, size_t n);                                                                                     /* 3084 */
    int (* strcoll) (const char *s1, const char *s2);                                                                                                /* 3088 */
    size_t (* strxfrm) (char *dest, const char *src, size_t len);                                                                                    /* 3092 */
    void * (* memmove) (void *dest, const void * src, size_t len);                                                                                   /* 3096 */
    void * (* memchr) (const void * ptr, int val, size_t len);                                                                                       /* 3100 */
    int (* memcmp) (const void *ptr1, const void *ptr2, size_t len);                                                                                 /* 3104 */
    void * (* memcpy) (void *dest, const void *src, size_t len);                                                                                     /* 3108 */
    void * (* memset) (void *ptr, int val, size_t len);                                                                                              /* 3112 */
    int (* strerror_r) (int error,char * buffer,size_t buffer_size);                                                                                 /* 3116 */
    char * (* strdup) (const char *s);                                                                                                               /* 3120 */
    char * (* strndup) (const char *str, size_t max);                                                                                                /* 3124 */
    void (* bcopy) (const void *from,void *to,size_t len);                                                                                           /* 3128 */
    void (* bzero) (void *m,size_t len);                                                                                                             /* 3132 */
    void (* explicit_bzero) (void *m, size_t len);                                                                                                   /* 3136 */
    int (* bcmp) (const void *a,const void *b,size_t len);                                                                                           /* 3140 */
    size_t (* strlcpy) (char *dst, const char *src, size_t siz);                                                                                     /* 3144 */
    size_t (* strlcat) (char *dst, const char *src, size_t siz);                                                                                     /* 3148 */
    char * (* strtok_r) (char *str, const char *separator_set,char ** state_ptr);                                                                    /* 3152 */
    char * (* strsignal) (int);                                                                                                                      /* 3156 */
    void * (* memccpy) (void *dst0, const void *src0, int endchar0, size_t len0);                                                                    /* 3160 */
    char * (* index) (const char *s, int c);                                                                                                         /* 3164 */
    char * (* rindex) (const char *s, int c);                                                                                                        /* 3168 */
    int (* strverscmp) (const char *s1, const char *s2);                                                                                             /* 3172 */
    char * (* strchrnul) (const char *s, int c_in);                                                                                                  /* 3176 */
    char * (* strcasestr) (const char *haystack, const char *needle);                                                                                /* 3180 */
    void * (* memmem) (const void *h0, size_t k, const void *n0, size_t l);                                                                          /* 3184 */
    void * (* memrchr) (const void *s, int c, size_t n);                                                                                             /* 3188 */
    void * (* mempcpy) (void *dst0, const void *src0, size_t len0);                                                                                  /* 3192 */

    /* strings.h */
    int (* strcasecmp) (const char *s1, const char *s2);                                                                                             /* 3196 */
    int (* strncasecmp) (const char *s1, const char *s2, size_t len);                                                                                /* 3200 */
    int (* ffs) (int i);                                                                                                                             /* 3204 */
    int (* ffsl) (long i);                                                                                                                           /* 3208 */
    int (* ffsll) (long long i);                                                                                                                     /* 3212 */

    /* termcap.h */
    int (* tgetent) (char *buffer, const char *termtype);                                                                                            /* 3216 */
    int (* tgetnum) (const char *name);                                                                                                              /* 3220 */
    int (* tgetflag) (const char *name);                                                                                                             /* 3224 */
    char * (* tgetstr) (const char *name, char **area);                                                                                              /* 3228 */
    void (* tputs) (const char *str, int nlines, int (*outfun) (int));                                                                               /* 3232 */
    char * (* tparam) (const char *string, char *outstring, int leb, int arg0, int arg1, int arg2, int arg3);                                        /* 3236 */
    char * (* tgoto) (const char *cstring, int hpos, int vpos);                                                                                      /* 3240 */

    /* termios.h */
    int (* tcgetattr) (int file_descriptor, struct termios *tios_p);                                                                                 /* 3244 */
    int (* tcsetattr) (int file_descriptor, int action, struct termios *tios_p);                                                                     /* 3248 */
    int (* tcsendbreak) (int file_descriptor, int duration);                                                                                         /* 3252 */
    int (* tcdrain) (int file_descriptor);                                                                                                           /* 3256 */
    int (* tcflush) (int file_descriptor, int what);                                                                                                 /* 3260 */
    int (* tcflow) (int file_descriptor, int action);                                                                                                /* 3264 */
    int (* cfmakeraw) (struct termios *tios_p);                                                                                                      /* 3268 */
    speed_t (* cfgetispeed) (const struct termios *tios_p);                                                                                          /* 3272 */
    speed_t (* cfgetospeed) (const struct termios *tios_p);                                                                                          /* 3276 */
    int (* cfsetispeed) (struct termios *tios_p, speed_t input_speed);                                                                               /* 3280 */
    int (* cfsetospeed) (struct termios *tios_p, speed_t output_speed);                                                                              /* 3284 */

    /* time.h */
    clock_t (* clock) (void);                                                                                                                        /* 3288 */
    time_t (* time) (time_t *t);                                                                                                                     /* 3292 */
    char * (* asctime) (const struct tm *tm);                                                                                                        /* 3296 */
    char * (* ctime) (const time_t *t);                                                                                                              /* 3300 */
    struct tm * (* gmtime) (const time_t *t);                                                                                                        /* 3304 */
    struct tm * (* localtime) (const time_t *t);                                                                                                     /* 3308 */
    time_t (* mktime) (struct tm *tm);                                                                                                               /* 3312 */
    double (* difftime) (time_t t1, time_t t0);                                                                                                      /* 3316 */
    size_t (* strftime) (char *s, size_t maxsize, const char *format, const struct tm *tm);                                                          /* 3320 */
    char * (* strptime) (const char *buf, const char *fmt, struct tm *timeptr);                                                                      /* 3324 */
    char * (* asctime_r) (const struct tm *tm, char *buffer);                                                                                        /* 3328 */
    char * (* ctime_r) (const time_t *tptr, char *buffer);                                                                                           /* 3332 */
    struct tm * (* gmtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                   /* 3336 */
    struct tm * (* localtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                /* 3340 */
    void (* tzset) (void);                                                                                                                           /* 3344 */
    int (* nanosleep) (const struct timespec *req, struct timespec *rem);                                                                            /* 3348 */
    int (* clock_gettime) (clockid_t clk_id, struct timespec *t);                                                                                    /* 3352 */
    int (* clock_settime) (clockid_t clk_id, const struct timespec *t);                                                                              /* 3356 */
    int (* clock_getres) (clockid_t clock_id, struct timespec *res);                                                                                 /* 3360 */
    int (* clock_nanosleep) (clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);                                /* 3364 */
    unsigned long long (* rdtsc) (void);                                                                                                             /* 3368 */
    int (* clock_gettime64) (clockid_t clock_id, struct timespec64 *tp);                                                                             /* 3372 */

    /* uchar.h */
    size_t (* c16rtomb) (char *__restrict, char16_t, mbstate_t *__restrict);                                                                         /* 3376 */
    size_t (* mbrtoc16) (char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 3380 */
    size_t (* c32rtomb) (char *__restrict, char32_t, mbstate_t *__restrict);                                                                         /* 3384 */
    size_t (* mbrtoc32) (char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 3388 */

    /* ulimit.h */
    long (* ulimit) (int cmd,long newlimit);                                                                                                         /* 3392 */

    /* unistd.h */
    int (* isatty) (int file_descriptor);                                                                                                            /* 3396 */
    int (* dup) (int file_descriptor);                                                                                                               /* 3400 */
    int (* dup2) (int file_descriptor1, int file_descriptor2);                                                                                       /* 3404 */
    off_t (* lseek) (int file_descriptor, off_t offset, int mode);                                                                                   /* 3408 */
    int (* access) (const char * path_name, int mode);                                                                                               /* 3412 */
    int (* chown) (const char * path_name, uid_t owner, gid_t group);                                                                                /* 3416 */
    int (* fchown) (int file_descriptor, uid_t owner, gid_t group);                                                                                  /* 3420 */
    int (* lchown) (const char * path_name, uid_t owner, gid_t group);                                                                               /* 3424 */
    int (* truncate) (const char * path_name, off_t length);                                                                                         /* 3428 */
    int (* ftruncate) (int file_descriptor, off_t length);                                                                                           /* 3432 */
    int (* getpagesize) (void);                                                                                                                      /* 3436 */
    int (* ftruncate64) (int file_descriptor, _off64_t length);                                                                                      /* 3440 */
    int (* truncate64) (const char *path_name, _off64_t length);                                                                                     /* 3444 */
    off64_t (* lseek64) (int fd, off64_t offset, int whence);                                                                                        /* 3448 */
    unsigned int (* alarm) (unsigned seconds);                                                                                                       /* 3452 */
    useconds_t (* ualarm) (useconds_t usecs, useconds_t interval);                                                                                   /* 3456 */
    int (* link) (const char * existing_path,const char * new_path);                                                                                 /* 3460 */
    int (* unlink) (const char * path_name);                                                                                                         /* 3464 */
    int (* symlink) (const char * actual_path, const char * symbolic_path);                                                                          /* 3468 */
    int (* readlink) (const char * path_name, char * buffer, int buffer_size);                                                                       /* 3472 */
    int (* chdir) (const char * path_name);                                                                                                          /* 3476 */
    int (* fchdir) (int file_descriptor);                                                                                                            /* 3480 */
    int (* lockf) (int file_descriptor, int function, off_t size);                                                                                   /* 3484 */
    unsigned int (* sleep) (unsigned int seconds);                                                                                                   /* 3488 */
    int (* usleep) (useconds_t microseconds);                                                                                                        /* 3492 */
    int (* pause) (void);                                                                                                                            /* 3496 */
    int (* getopt) (int argc, char * const argv[], const char *opts);                                                                                /* 3500 */
    pid_t (* getpid) (void);                                                                                                                         /* 3504 */
    pid_t (* getppid) (void);                                                                                                                        /* 3508 */
    pid_t (* getpgrp) (void );                                                                                                                       /* 3512 */
    pid_t (* tcgetpgrp) (int fildes);                                                                                                                /* 3516 */
    int (* tcsetpgrp) (int filedes, pid_t pgrp);                                                                                                     /* 3520 */
    char * (* realpath) (const char *file_name, char *resolved_name);                                                                                /* 3524 */
    int (* fsync) (int file_descriptor);                                                                                                             /* 3528 */
    int (* fdatasync) (int file_descriptor);                                                                                                         /* 3532 */
    char * (* ttyname) (int);                                                                                                                        /* 3536 */
    int (* ttyname_r) (int file_descriptor,char *name,size_t buflen);                                                                                /* 3540 */
    int (* execl) (const char *path,const char *arg0,...);                                                                                           /* 3544 */
    int (* execle) (const char *path,const char *arg0,...);                                                                                          /* 3548 */
    int (* execlp) (const char *path,const char *arg0,...);                                                                                          /* 3552 */
    int (* execv) (const char *path,char * const argv[]);                                                                                            /* 3556 */
    int (* execve) (const char *path,char *const argv[],char *const envp[]);                                                                         /* 3560 */
    int (* execvp) (const char *command,char * const argv[]);                                                                                        /* 3564 */
    void (* encrypt) (char *block, int edflag);                                                                                                      /* 3568 */
    int (* spawnv) (int mode, const char *file, const char **argv);                                                                                  /* 3572 */
    int (* spawnvp) (int mode, const char *path, const char **argv);                                                                                 /* 3576 */
    long (* sysconf) (int name);                                                                                                                     /* 3580 */
    void (* enableUnixPaths) (void);                                                                                                                 /* 3584 */
    void (* disableUnixPaths) (void);                                                                                                                /* 3588 */
    void (* enableAltivec) (void);                                                                                                                   /* 3592 */
    void (* disableAltivec) (void);                                                                                                                  /* 3596 */
    void (* enableOptimizedFunctions) (void);                                                                                                        /* 3600 */
    void (* disableOptimizedFunctions) (void);                                                                                                       /* 3604 */
    char * (* getcwd) (char * buffer, size_t buffer_size);                                                                                           /* 3608 */
    char * (* getwd) (char *buf);                                                                                                                    /* 3612 */
    char * (* get_current_dir_name) (void);                                                                                                          /* 3616 */
    long (* gethostid) ();                                                                                                                           /* 3620 */
    int (* gethostname) (const char *name, size_t len);                                                                                              /* 3624 */
    int (* getdomainname) (char *name, size_t len);                                                                                                  /* 3628 */
    int (* setdomainname) (const char *name, size_t len);                                                                                            /* 3632 */
    char * (* getlogin) (void);                                                                                                                      /* 3636 */
    int (* getlogin_r) (char *buf, size_t bufsize);                                                                                                  /* 3640 */
    char * (* crypt) (const char *key, const char *salt);                                                                                            /* 3644 */
    gid_t (* getegid) (void);                                                                                                                        /* 3648 */
    uid_t (* geteuid) (void);                                                                                                                        /* 3652 */
    gid_t (* getgid) (void);                                                                                                                         /* 3656 */
    int (* getgroups) (int gidsetsize, gid_t grouplist[]);                                                                                           /* 3660 */
    char * (* getpass) (const char *prompt);                                                                                                         /* 3664 */
    uid_t (* getuid) (void);                                                                                                                         /* 3668 */
    int (* initgroups) (const char *name, gid_t basegid);                                                                                            /* 3672 */
    int (* setegid) (gid_t gid);                                                                                                                     /* 3676 */
    int (* seteuid) (uid_t uid);                                                                                                                     /* 3680 */
    int (* setgid) (gid_t gid);                                                                                                                      /* 3684 */
    int (* setgroups) (int ngroups, const gid_t *gidset);                                                                                            /* 3688 */
    int (* setregid) (gid_t rgid, gid_t egid);                                                                                                       /* 3692 */
    int (* setreuid) (uid_t ruid, uid_t euid);                                                                                                       /* 3696 */
    pid_t (* setsid) (void);                                                                                                                         /* 3700 */
    int (* setuid) (uid_t uid);                                                                                                                      /* 3704 */
    int (* setlogin) (const char *name);                                                                                                             /* 3708 */
    long (* pathconf) (const char *path,int name);                                                                                                   /* 3712 */
    long (* fpathconf) (int file_descriptor,int name);                                                                                               /* 3716 */
    int (* pipe) (int fd[2]);                                                                                                                        /* 3720 */
    int (* pipe2) (int fd[2], int flags);                                                                                                            /* 3724 */
    ssize_t (* pread) (int fd, void *buf, size_t nbytes, off_t offset);                                                                              /* 3728 */
    ssize_t (* pwrite) (int fd, const void *buf, size_t nbytes, off_t offset);                                                                       /* 3732 */
    ssize_t (* pread64) (int fd, void *buf, size_t nbytes, off64_t offset);                                                                          /* 3736 */
    ssize_t (* pwrite64) (int fd, const void *buf, size_t nbytes, off64_t offset);                                                                   /* 3740 */

    /* utime.h */
    int (* utime) (const char * path_name,const struct utimbuf * times);                                                                             /* 3744 */

    /* wchar.h */
    wint_t (* btowc) (int c);                                                                                                                        /* 3748 */
    int (* wctob) (wint_t c);                                                                                                                        /* 3752 */
    wchar_t * (* wcscat) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3756 */
    wchar_t * (* wcsncat) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3760 */
    int (* wcscmp) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3764 */
    int (* wcsncmp) (const wchar_t *s1, const wchar_t *s2, size_t n);                                                                                /* 3768 */
    wchar_t * (* wcscpy) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3772 */
    wchar_t * (* wcsncpy) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3776 */
    size_t (* wcslen) (const wchar_t *s);                                                                                                            /* 3780 */
    wchar_t * (* wcschr) (const wchar_t *s, wchar_t c);                                                                                              /* 3784 */
    size_t (* wcsspn) (const wchar_t *s, const wchar_t *set);                                                                                        /* 3788 */
    wchar_t * (* wcspbrk) (const wchar_t *s, const wchar_t *set);                                                                                    /* 3792 */
    wchar_t * (* wcstok) (wchar_t *str, const wchar_t *delim, wchar_t **ptr);                                                                        /* 3796 */
    wchar_t * (* wcsstr) (const wchar_t *big, const wchar_t *little);                                                                                /* 3800 */
    int (* wcswidth) (const wchar_t *pwcs, size_t n);                                                                                                /* 3804 */
    int (* wcwidth) (const wchar_t wc);                                                                                                              /* 3808 */
    double (* wcstod) (const wchar_t *str, wchar_t **ptr);                                                                                           /* 3812 */
    long (* wcstol) (const wchar_t *str, wchar_t **ptr, int base);                                                                                   /* 3816 */
    unsigned long (* wcstoul) (const wchar_t *str, wchar_t **ptr, int base);                                                                         /* 3820 */
    float (* wcstof) (const wchar_t *nptr, wchar_t **endptr);                                                                                        /* 3824 */
    int (* wscoll) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3828 */
    size_t (* wcsxfrm) (wchar_t *dest, const wchar_t *src, size_t len);                                                                              /* 3832 */
    wchar_t * (* wmemchr) (const wchar_t *ptr, wchar_t val, size_t len);                                                                             /* 3836 */
    int (* wmemcmp) (const wchar_t *ptr1, const wchar_t *ptr2, size_t len);                                                                          /* 3840 */
    wchar_t * (* wmemcpy) (wchar_t *dest, const wchar_t *src, size_t len);                                                                           /* 3844 */
    wchar_t * (* wmemmove) (wchar_t *dest, const wchar_t *src, size_t len);                                                                          /* 3848 */
    wchar_t * (* wmemset) (wchar_t *ptr, int val, size_t len);                                                                                       /* 3852 */
    int (* fwide) (FILE *stream, int orient);                                                                                                        /* 3856 */
    wint_t (* fgetwc) (FILE *stream);                                                                                                                /* 3860 */
    wint_t (* getwc) (FILE *stream);                                                                                                                 /* 3864 */
    wint_t (* getwchar) (void);                                                                                                                      /* 3868 */
    wint_t (* ungetwc) (wint_t wc, FILE *fp);                                                                                                        /* 3872 */
    wchar_t * (* fgetws) (wchar_t *s, int n, FILE *stream);                                                                                          /* 3876 */
    int (* fwscanf) (FILE *stream, const wchar_t *format, ...);                                                                                      /* 3880 */
    int (* swscanf) (const wchar_t *s, const wchar_t *format, ...);                                                                                  /* 3884 */
    int (* vfwscanf) (FILE *f, const wchar_t *format, va_list ap);                                                                                   /* 3888 */
    int (* vswscanf) (const wchar_t *s, const wchar_t *format, va_list ap);                                                                          /* 3892 */
    int (* vwscanf) (const wchar_t *s, va_list ap);                                                                                                  /* 3896 */
    int (* wscanf) (const wchar_t *format, ...);                                                                                                     /* 3900 */
    wint_t (* fputwc) (wchar_t c, FILE *stream);                                                                                                     /* 3904 */
    wint_t (* putwc) (wchar_t c, FILE *stream);                                                                                                      /* 3908 */
    wint_t (* putwchar) (wchar_t c);                                                                                                                 /* 3912 */
    wint_t (* fputws) (const wchar_t *s, FILE *stream);                                                                                              /* 3916 */
    int (* fwprintf) (FILE *stream, const wchar_t *format, ...);                                                                                     /* 3920 */
    int (* swprintf) (wchar_t *restrict s, size_t l, const wchar_t *restrict fmt, ...);                                                              /* 3924 */
    int (* vfwprintf) (FILE *stream, const wchar_t *format, va_list arg);                                                                            /* 3928 */
    int (* vswprintf) (wchar_t *s, size_t maxlen, const wchar_t *format, va_list arg);                                                               /* 3932 */
    int (* vwprintf) (const wchar_t *format, va_list arg);                                                                                           /* 3936 */
    int (* wprintf) (const wchar_t *format, ...);                                                                                                    /* 3940 */
    size_t (* wcsftime) (wchar_t *s, size_t maxsize, const wchar_t *format, const struct tm *timeptr);                                               /* 3944 */
    size_t (* mbrlen) (const char *restrict s, size_t n, mbstate_t *restrict ps);                                                                    /* 3948 */
    size_t (* mbrtowc) (wchar_t *pwc, const char *src, size_t n, mbstate_t *ps);                                                                     /* 3952 */
    int (* mbsinit) (const mbstate_t *ps);                                                                                                           /* 3956 */
    size_t (* mbsrtowcs) (wchar_t *restrict dst, const char **restrict src, size_t len, mbstate_t *restrict ps);                                     /* 3960 */
    size_t (* wcrtomb) (char *restrict s, wchar_t wc, mbstate_t *restrict ps);                                                                       /* 3964 */
    int (* wcscoll) (const wchar_t *ws1, const wchar_t *ws2);                                                                                        /* 3968 */
    size_t (* wcscspn) (const wchar_t *s, const wchar_t *c);                                                                                         /* 3972 */
    wchar_t * (* wcsrchr) (const wchar_t *ws, wchar_t wc);                                                                                           /* 3976 */
    size_t (* wcsrtombs) (char *restrict dst, const wchar_t **restrict src, size_t len, mbstate_t *restrict ps);                                     /* 3980 */
    long long (* wcstoll) (const wchar_t *str, wchar_t **ptr, int base);                                                                             /* 3984 */
    unsigned long long (* wcstoull) (const wchar_t *str, wchar_t **ptr, int base);                                                                   /* 3988 */
    long double (* wcstold) (const wchar_t *nptr, wchar_t **endptr);                                                                                 /* 3992 */
    size_t (* mbsnrtowcs) (wchar_t *dst, const char **restrict src, size_t nmc, size_t len, mbstate_t *restrict ps);                                 /* 3996 */
    size_t (* wcsnrtombs) (char *restrict dst, const wchar_t **restrict src, size_t nwc, size_t len, mbstate_t *restrict ps);                        /* 4000 */
    wchar_t * (* wcsdup) (const wchar_t *src);                                                                                                       /* 4004 */
    size_t (* wcsnlen) (const wchar_t *src, size_t n);                                                                                               /* 4008 */
    wchar_t * (* wcpcpy) (wchar_t *dst, const wchar_t *src);                                                                                         /* 4012 */
    wchar_t * (* wcpncpy) (wchar_t *dst, const wchar_t *src, size_t len);                                                                            /* 4016 */
    int (* wcscasecmp) (const wchar_t *l, const wchar_t *r);                                                                                         /* 4020 */
    int (* wcscasecmp_l) (const wchar_t *l, const wchar_t *r, locale_t loc);                                                                         /* 4024 */
    int (* wcsncasecmp) (const wchar_t *l, const wchar_t *r, size_t n);                                                                              /* 4028 */
    int (* wcsncasecmp_l) (const wchar_t *l, const wchar_t *r, size_t n, locale_t loc);                                                              /* 4032 */
    int (* wcscoll_l) (const wchar_t *ws1, const wchar_t *ws2, locale_t loc);                                                                        /* 4036 */
    size_t (* wcsxfrm_l) (wchar_t *dest, const wchar_t *src, size_t len, locale_t loc);                                                              /* 4040 */

    /* wctype.h */
    int (* iswalnum) (wint_t c);                                                                                                                     /* 4044 */
    int (* iswalpha) (wint_t c);                                                                                                                     /* 4048 */
    int (* iswcntrl) (wint_t c);                                                                                                                     /* 4052 */
    int (* iswdigit) (wint_t c);                                                                                                                     /* 4056 */
    int (* iswxdigit) (wint_t c);                                                                                                                    /* 4060 */
    int (* iswgraph) (wint_t c);                                                                                                                     /* 4064 */
    int (* iswpunct) (wint_t c);                                                                                                                     /* 4068 */
    int (* iswprint) (wint_t c);                                                                                                                     /* 4072 */
    int (* iswlower) (wint_t c);                                                                                                                     /* 4076 */
    int (* iswupper) (wint_t c);                                                                                                                     /* 4080 */
    int (* iswspace) (wint_t c);                                                                                                                     /* 4084 */
    int (* iswblank) (wint_t c);                                                                                                                     /* 4088 */
    wint_t (* towlower) (wint_t c);                                                                                                                  /* 4092 */
    wint_t (* towupper) (wint_t c);                                                                                                                  /* 4096 */
    wctype_t (* wctype) (const char *property);                                                                                                      /* 4100 */
    int (* iswctype) (wint_t c, wctype_t desc);                                                                                                      /* 4104 */
    wctrans_t (* wctrans) (const char *property);                                                                                                    /* 4108 */
    wint_t (* towctrans) (wint_t c, wctrans_t desc);                                                                                                 /* 4112 */

    /* arpa/inet.h */
    in_addr_t (* inet_addr) (const char *cp);                                                                                                        /* 4116 */
    int (* inet_aton) (const char *cp, struct in_addr *pin);                                                                                         /* 4120 */
    in_addr_t (* inet_lnaof) (struct in_addr in);                                                                                                    /* 4124 */
    struct in_addr (* inet_makeaddr) (in_addr_t net, in_addr_t lna);                                                                                 /* 4128 */
    in_addr_t (* inet_netof) (struct in_addr in);                                                                                                    /* 4132 */
    in_addr_t (* inet_network) (const char *cp);                                                                                                     /* 4136 */
    char * (* inet_ntoa) (struct in_addr in);                                                                                                        /* 4140 */
    const char * (* inet_ntop) (int af, const void *src, char *dst, socklen_t cnt);                                                                  /* 4144 */
    int (* inet_pton) (int af, const char *src, void *dst);                                                                                          /* 4148 */

    /* arpa/nameser.h */
    unsigned (* ns_get16) (const unsigned char *);                                                                                                   /* 4152 */
    unsigned long (* ns_get32) (const unsigned char *);                                                                                              /* 4156 */
    void (* ns_put16) (unsigned, unsigned char *);                                                                                                   /* 4160 */
    void (* ns_put32) (unsigned long, unsigned char *);                                                                                              /* 4164 */
    int (* ns_initparse) (const unsigned char *, int, ns_msg *);                                                                                     /* 4168 */
    int (* ns_parserr) (ns_msg *, ns_sect, int, ns_rr *);                                                                                            /* 4172 */
    int (* ns_skiprr) (const unsigned char *, const unsigned char *, ns_sect, int);                                                                  /* 4176 */
    int (* ns_name_uncompress) (const unsigned char *, const unsigned char *, const unsigned char *, char *, size_t);                                /* 4180 */

    /* net/if.h */
    unsigned int (* if_nametoindex) (const char *ifname);                                                                                            /* 4184 */
    char * (* if_indextoname) (unsigned int ifindex, char *ifname);                                                                                  /* 4188 */

    /* netinet/in.h */
    int (* bindresvport) (int sd, struct sockaddr_in *sa);                                                                                           /* 4192 */
    int (* bindresvport6) (int sd, struct sockaddr_in6 *sa);                                                                                         /* 4196 */

    /* sys/byteswap.h */
    __CONST_FUNC uint16_t (* bswap16) (uint16_t);                                                                                                    /* 4200 */
    __CONST_FUNC uint32_t (* bswap24) (uint32_t);                                                                                                    /* 4204 */
    __CONST_FUNC uint32_t (* bswap32) (uint32_t);                                                                                                    /* 4208 */
    __CONST_FUNC uint64_t (* bswap64) (uint64_t);                                                                                                    /* 4212 */
    void * (* swab) (void *from,void *to,ssize_t nbytes);                                                                                            /* 4216 */
    void * (* swab24) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 24-bit words instead. */                              /* 4220 */
    void * (* swab32) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 32-bit words instead. */                              /* 4224 */
    void * (* swab64) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 64-bit words instead. */                              /* 4228 */

    /* sys/file.h */
    int (* flock) (int fd, int op);                                                                                                                  /* 4232 */

    /* sys/ioctl.h */
    int (* ioctl) (int fildes, int request, ... /* arg */);                                                                                          /* 4236 */

    /* sys/ipc.h */
    key_t (* ftok) (const char *path, int id);                                                                                                       /* 4240 */

    /* sys/mman.h */
    void * (* mmap) (void *addr, size_t len, int prot, int flags, int fd, off_t offset);                                                             /* 4244 */
    int (* munmap) (void *map, size_t length);                                                                                                       /* 4248 */
    int (* msync) (void *addr, size_t len, int flags);                                                                                               /* 4252 */

    /* sys/mount.h */
    int (* statfs) (const char *path, struct statfs *buf);                                                                                           /* 4256 */
    int (* fstatfs) (int fd, struct statfs *buf);                                                                                                    /* 4260 */

    /* sys/msg.h */
    int (* _msgctl) (int msqid, int cmd, struct msqid_ds *buf);                                                                                      /* 4264 */
    int (* _msgget) (key_t key, int msgflg);                                                                                                         /* 4268 */
    ssize_t (* _msgrcv) (int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);                                                              /* 4272 */
    int (* _msgsnd) (int msqid, const void *msgp, size_t msgsz, int msgflg);                                                                         /* 4276 */
    int (* _msgsnap) (int msqid, void *buf, size_t bufsz, long msgtyp);                                                                              /* 4280 */
    int (* _msgids) (int *buf, size_t nids, size_t  *pnids);                                                                                         /* 4284 */

    /* sys/resource.h */
    int (* getrlimit) (int resource, struct rlimit *rlp);                                                                                            /* 4288 */
    int (* setrlimit) (int resource, const struct rlimit *rlp);                                                                                      /* 4292 */
    int (* getrusage) (int who, struct rusage *usage);                                                                                               /* 4296 */

    /* sys/select.h */
    int (* select) (int nfds, fd_set *readfds,fd_set *writefds, fd_set *errorfds,struct timeval *timeout);                                           /* 4300 */
    int (* waitselect) (int num_fds,fd_set *read_fds,fd_set *write_fds,fd_set *except_fds,struct timeval *timeout,unsigned long * signal_mask);      /* 4304 */

    /* sys/sem.h */
    int (* _semctl) (int semid, int semnum, int cmd, union semun aun);                                                                               /* 4308 */
    int (* _semget) (key_t key, int nsems, int flags);                                                                                               /* 4312 */
    int (* _semop) (int semid, const struct sembuf *ops, int nops);                                                                                  /* 4316 */
    long int (* _semids) (long int *buf, uint32_t nids, long unsigned int *idcnt);                                                                   /* 4320 */
    int (* _semtimedop) (int semid, const struct sembuf *ops, int nops, struct timespec *to);                                                        /* 4324 */

    /* sys/shm.h */
    void * (* _shmat) (int shmid, const void *shmaddr, int shmflg);                                                                                  /* 4328 */
    int (* _shmctl) (int shmid, int cmd, struct shmid_ds *buf);                                                                                      /* 4332 */
    int (* _shmdt) (const void *shmaddr);                                                                                                            /* 4336 */
    int (* _shmget) (key_t key, size_t size, int shmflg);                                                                                            /* 4340 */
    int (* _shmids) (int *buf, size_t nids, size_t *pnids);                                                                                          /* 4344 */

    /* sys/socket.h */
    int (* accept) (int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);                                                                         /* 4348 */
    int (* bind) (int socket, const struct sockaddr *address, socklen_t address_len);                                                                /* 4352 */
    int (* connect) (int socket, const struct sockaddr *address, socklen_t address_len);                                                             /* 4356 */
    int (* getpeername) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 4360 */
    int (* getsockname) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 4364 */
    int (* getsockopt) (int socket, int level, int option_name, void *option_value, socklen_t *option_len);                                          /* 4368 */
    int (* listen) (int socket, int backlog);                                                                                                        /* 4372 */
    ssize_t (* recv) (int socket, void *buffer, size_t length, int flags);                                                                           /* 4376 */
    ssize_t (* recvfrom) (int socket, void *buffer, size_t length,int flags, struct sockaddr *address,socklen_t *address_len);                       /* 4380 */
    ssize_t (* recvmsg) (int socket, struct msghdr *message, int flags);                                                                             /* 4384 */
    ssize_t (* send) (int socket, const void *buffer, size_t length, int flags);                                                                     /* 4388 */
    ssize_t (* sendmsg) (int socket, const struct msghdr *message, int flags);                                                                       /* 4392 */
    ssize_t (* sendto) (int socket, const void *message, size_t length,int flags, const struct sockaddr *dest_addr,socklen_t dest_len);              /* 4396 */
    int (* setsockopt) (int socket, int level, int option_name, const void *option_value, socklen_t option_len);                                     /* 4400 */
    int (* shutdown) (int socket, int how);                                                                                                          /* 4404 */
    int (* socket) (int domain, int type, int protocol);                                                                                             /* 4408 */
    int (* socketpair) (int domain, int type, int protocol, int fds[2]);                                                                             /* 4412 */

    /* sys/stat.h */
    int (* stat) (const char * path_name, struct stat * buffer);                                                                                     /* 4416 */
    int (* fstat) (int file_descriptor, struct stat * buffer);                                                                                       /* 4420 */
    int (* lstat) (const char * path_name, struct stat * buffer);                                                                                    /* 4424 */
    int (* chmod) (const char * path_name, mode_t mode);                                                                                             /* 4428 */
    int (* fchmod) (int file_descriptor, mode_t mode);                                                                                               /* 4432 */
    int (* mkdir) (const char * path_name, mode_t mode);                                                                                             /* 4436 */
    int (* rmdir) (const char * path_name);                                                                                                          /* 4440 */
    mode_t (* umask) (mode_t new_mask);                                                                                                              /* 4444 */

    /* sys/statvfs.h */
    int (* statvfs) (const char *, struct statvfs *);                                                                                                /* 4448 */
    int (* fstatvfs) (int, struct statvfs *);                                                                                                        /* 4452 */

    /* sys/syslog.h */
    void (* closelog) (void);                                                                                                                        /* 4456 */
    void (* openlog) (const char *ident, int opt, int facility);                                                                                     /* 4460 */
    int (* setlogmask) (int maskpri);                                                                                                                /* 4464 */
    void (* syslog) (int priority, const char *message, ...);                                                                                        /* 4468 */
    void (* vsyslog) (int priority, const char *message, va_list args);                                                                              /* 4472 */

    /* sys/systeminfo.h */
    long (* sysinfo) (int cmd,char *buf,long buflen);                                                                                                /* 4476 */

    /* sys/time.h */
    int (* gettimeofday) (struct timeval *tp, struct timezone *tzp);                                                                                 /* 4480 */
    int (* settimeofday) (const struct timeval *, const struct timezone *);                                                                          /* 4484 */
    int (* utimes) (const char *path, const struct timeval *tvp);                                                                                    /* 4488 */
    int (* getitimer) (int which, struct itimerval *curr_value);                                                                                     /* 4492 */
    int (* setitimer) (int which, const struct itimerval *new_value, struct itimerval *old_value);                                                   /* 4496 */

    /* sys/timeb.h */
    int (* ftime) (struct timeb *);                                                                                                                  /* 4500 */

    /* sys/times.h */
    clock_t (* times) (struct tms *tp);                                                                                                              /* 4504 */

    /* sys/uio.h */
    ssize_t (* readv) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                   /* 4508 */
    ssize_t (* writev) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                  /* 4512 */

    /* sys/utsname.h */
    int (* uname) (struct utsname *);                                                                                                                /* 4516 */


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