#ifndef _SYS_INTERFACE_H_
#define _SYS_INTERFACE_H_

#include <proto/dos.h>

#undef __BSD_VISIBLE
#define __BSD_VISIBLE 1
#include <arpa/inet.h>
#include <aio.h>
#include <argz.h>
#include <assert.h>
#include <complex.h>
#include <crypt.h>
#include <ctype.h>
#include <db.h>
#include <dirent.h>
#include <dlfcn.h>
#include <envz.h>
#include <err.h>
#include <execinfo.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <fts.h>
#include <ftw.h>
#include <getopt.h>
#include <glob.h>
#include <grp.h>
#include <iconv.h>
#include <ieeefp.h>
#include <ifaddrs.h>
#include <langinfo.h>
#include <libgen.h>
#include <libintl.h>
#include <locale.h>
#include <malloc.h>
#include <math.h>
#include <ndbm.h>
#include <net/ethernet.h>
#include <netdb.h>
#include <nl_types.h>
#include <poll.h>
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
#include <netinet/ether.h>
#include <sys/byteswap.h>
#include <sys/file.h>
#include <sys/iconvnls.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/msg.h>
#include <sys/random.h>
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
#include <sys/wait.h>

struct NameTranslationInfo;

#ifdef __GNUC__
#define ___VALIST __gnuc_va_list
#else
#define ___VALIST va_list
#endif

#define MIN_OS_VERSION 52

#include <stdarg.h>

struct Clib4IFace {
    struct InterfaceData Data;                                              //-4/56
    unsigned long APICALL (*Obtain)(struct Clib4IFace *Self);               //60
    unsigned long APICALL (*Release)(struct Clib4IFace *Self);              //64
    void APICALL (*Expunge)(struct Clib4IFace *Self);                       //68
    struct Clib4IFace *APICALL (*Clone)(struct Clib4IFace *Self);           //72
    /* internal */
    int  (* library_start)(char *argstr, int arglen, int (* start_main)(int, char **), void (*__CTOR_LIST__[])(void), void (*__DTOR_LIST__[])(void)); //76
    struct _clib4 * (* __getClib4)(void);                                   //80
    void (* internal1)(void);                                               //84
    void (* __translate_amiga_to_unix_path_name)(void);                     //88
    void (* __translate_unix_to_amiga_path_name)(void);                     //92
    void (* internal2)(void);                                               //96
    void (* __print_termination_message)(const char *termination_message);  //100
    void (* __get_default_file)(int file_descriptor, long *file_ptr);       //104
    void (* unixPathsEnabled)(void);                                               //108
    void (* internal5)(void);                                               //112

    /* argz.h */
    error_t (* argz_create) (char *const argv[], char **argz, size_t *argz_len);                                                                     /* 116 */
    error_t (* argz_create_sep) (const char *string, int sep, char **argz, size_t *argz_len);                                                        /* 120 */
    size_t (* argz_count) (const char *argz, size_t argz_len);                                                                                       /* 124 */
    void (* argz_extract) (char *argz, size_t argz_len, char **argv);                                                                                /* 128 */
    void (* argz_stringify) (char *argz, size_t argz_len, int sep);                                                                                  /* 132 */
    error_t (* argz_add) (char **argz, size_t *argz_len, const char *str);                                                                           /* 136 */
    error_t (* argz_add_sep) (char **argz, size_t *argz_len, const char *str, int sep);                                                              /* 140 */
    error_t (* argz_append) (char **argz, size_t *argz_len, const char *buf, size_t buf_len);                                                        /* 144 */
    error_t (* argz_delete) (char **argz, size_t *argz_len, char *entry);                                                                            /* 148 */
    error_t (* argz_insert) (char **argz, size_t *argz_len, char *before, const char *entry);                                                        /* 152 */
    char * (* argz_next) (char *argz, size_t argz_len, const char *entry);                                                                           /* 156 */
    error_t (* argz_replace) (char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count);                            /* 160 */

    /* assert.h */
    void (* __assertion_failure) (const char *file_name, int line_number, const char *expression);                                                   /* 164 */

    /* complex.h */
    double complex (* cacos) (double complex z);                                                                                                     /* 168 */
    float complex (* cacosf) (float complex z);                                                                                                      /* 172 */
    long double complex (* cacosl) (long double complex z);                                                                                          /* 176 */
    double complex (* casin) (double complex z);                                                                                                     /* 180 */
    float complex (* casinf) (float complex z);                                                                                                      /* 184 */
    long double complex (* casinl) (long double complex z);                                                                                          /* 188 */
    double complex (* catan) (double complex z);                                                                                                     /* 192 */
    float complex (* catanf) (float complex z);                                                                                                      /* 196 */
    long double complex (* catanl) (long double complex z);                                                                                          /* 200 */
    double complex (* ccos) (double complex z);                                                                                                      /* 204 */
    float complex (* ccosf) (float complex z);                                                                                                       /* 208 */
    long double complex (* ccosl) (long double complex z);                                                                                           /* 212 */
    double complex (* csin) (double complex z);                                                                                                      /* 216 */
    float complex (* csinf) (float complex z);                                                                                                       /* 220 */
    long double complex (* csinl) (long double complex z);                                                                                           /* 224 */
    double complex (* ctan) (double complex z);                                                                                                      /* 228 */
    float complex (* ctanf) (float complex z);                                                                                                       /* 232 */
    long double complex (* ctanl) (long double complex z);                                                                                           /* 236 */
    double complex (* cacosh) (double complex z);                                                                                                    /* 240 */
    float complex (* cacoshf) (float complex z);                                                                                                     /* 244 */
    long double complex (* cacoshl) (long double complex z);                                                                                         /* 248 */
    double complex (* casinh) (double complex z);                                                                                                    /* 252 */
    float complex (* casinhf) (float complex z);                                                                                                     /* 256 */
    long double complex (* casinhl) (long double complex z);                                                                                         /* 260 */
    double complex (* catanh) (double complex z);                                                                                                    /* 264 */
    float complex (* catanhf) (float complex z);                                                                                                     /* 268 */
    long double complex (* catanhl) (long double complex z);                                                                                         /* 272 */
    double complex (* ccosh) (double complex z);                                                                                                     /* 276 */
    float complex (* ccoshf) (float complex z);                                                                                                      /* 280 */
    long double complex (* ccoshl) (long double complex z);                                                                                          /* 284 */
    double complex (* csinh) (double complex z);                                                                                                     /* 288 */
    float complex (* csinhf) (float complex z);                                                                                                      /* 292 */
    long double complex (* csinhl) (long double complex z);                                                                                          /* 296 */
    double complex (* ctanh) (double complex z);                                                                                                     /* 300 */
    float complex (* ctanhf) (float complex z);                                                                                                      /* 304 */
    long double complex (* ctanhl) (long double complex z);                                                                                          /* 308 */
    double complex (* cexp) (double complex z);                                                                                                      /* 312 */
    float complex (* cexpf) (float complex z);                                                                                                       /* 316 */
    long double complex (* cexpl) (long double complex z);                                                                                           /* 320 */
    double complex (* clog) (double complex z);                                                                                                      /* 324 */
    float complex (* clogf) (float complex z);                                                                                                       /* 328 */
    long double complex (* clogl) (long double complex z);                                                                                           /* 332 */
    double (* cabs) (double complex z);                                                                                                              /* 336 */
    float (* cabsf) (float complex z);                                                                                                               /* 340 */
    long double (* cabsl) (long double complex z);                                                                                                   /* 344 */
    double complex (* cpow) (double complex z, double complex c);                                                                                    /* 348 */
    float complex (* cpowf) (float complex z, float complex c);                                                                                      /* 352 */
    long double complex (* cpowl) (long double complex z, long double complex c);                                                                    /* 356 */
    double complex (* csqrt) (double complex z);                                                                                                     /* 360 */
    float complex (* csqrtf) (float complex z);                                                                                                      /* 364 */
    long double complex (* csqrtl) (long double complex z);                                                                                          /* 368 */
    double complex (* carg) (double complex z);                                                                                                      /* 372 */
    float complex (* cargf) (float complex z);                                                                                                       /* 376 */
    long double complex (* cargl) (long double complex z);                                                                                           /* 380 */
    double complex (* conj) (double complex z);                                                                                                      /* 384 */
    float complex (* conjf) (float complex z);                                                                                                       /* 388 */
    long double complex (* conjl) (long double complex z);                                                                                           /* 392 */
    double complex (* cproj) (double complex z);                                                                                                     /* 396 */
    float complex (* cprojf) (float complex z);                                                                                                      /* 400 */
    long double complex (* cprojl) (long double complex z);                                                                                          /* 404 */
    double (* cimag) (double complex z);                                                                                                             /* 408 */
    float (* cimagf) (float complex z);                                                                                                              /* 412 */
    long double (* cimagl) (long double complex z);                                                                                                  /* 416 */
    double (* creal) (double complex z);                                                                                                             /* 420 */
    float (* crealf) (float complex z);                                                                                                              /* 424 */
    long double (* creall) (long double complex z);                                                                                                  /* 428 */

    /* crypt.h */
    char * (* crypt_r) (const char *__phrase, const char *__salt, struct crypt_data * __restrict __data);                                            /* 432 */

    /* ctype.h */
    int (* isalnum) (int c);                                                                                                                         /* 436 */
    int (* isalpha) (int c);                                                                                                                         /* 440 */
    int (* iscntrl) (int c);                                                                                                                         /* 444 */
    int (* isdigit) (int c);                                                                                                                         /* 448 */
    int (* isgraph) (int c);                                                                                                                         /* 452 */
    int (* islower) (int c);                                                                                                                         /* 456 */
    int (* isprint) (int c);                                                                                                                         /* 460 */
    int (* ispunct) (int c);                                                                                                                         /* 464 */
    int (* isspace) (int c);                                                                                                                         /* 468 */
    int (* isupper) (int c);                                                                                                                         /* 472 */
    int (* isxdigit) (int c);                                                                                                                        /* 476 */
    int (* tolower) (int c);                                                                                                                         /* 480 */
    int (* toupper) (int c);                                                                                                                         /* 484 */
    int (* isblank) (int c);                                                                                                                         /* 488 */
    int (* isascii) (int c);                                                                                                                         /* 492 */
    int (* toascii) (int c);                                                                                                                         /* 496 */

    /* dirent.h */
    DIR * (* opendir) (const char * path_name);                                                                                                      /* 500 */
    struct dirent * (* readdir) (DIR * directory_pointer);                                                                                           /* 504 */
    void (* rewinddir) (DIR * directory_pointer);                                                                                                    /* 508 */
    int (* closedir) (DIR * directory_pointer);                                                                                                      /* 512 */
    DIR * (* fdopendir) (int);                                                                                                                       /* 516 */
    int (* alphasort) (const struct dirent **a, const struct dirent **b);                                                                            /* 520 */
    int (* dirfd) (DIR *dirp);                                                                                                                       /* 524 */
    int (* scandir) (const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));      /* 528 */
    int (* readdir_r) (DIR *dir, struct dirent *buf, struct dirent **result);                                                                        /* 532 */
    int (* readdir64_r) (DIR *dir, struct dirent *buf, struct dirent **result);                                                                      /* 536 */

    /* dlfcn.h */
    int (* dlclose) (void *__handle);                                                                                                                /* 540 */
    const char * (* dlerror) (void);                                                                                                                 /* 544 */
    void * (* dlopen) (const char *__path_name, int __mode);                                                                                         /* 548 */
    void * (* dlsym) (void *__handle, const char *__symbol_name);                                                                                    /* 552 */

    /* envz.h */
    char * (* envz_entry) (const char *envz, size_t envz_len, const char *name);                                                                     /* 556 */
    char * (* envz_get) (const char *envz, size_t envz_len, const char *name);                                                                       /* 560 */
    error_t (* envz_add) (char **envz, size_t *envz_len, const char *name, const char *value);                                                       /* 564 */
    error_t (* envz_merge) (char **envz, size_t *envz_len, const char *envz2, size_t envz2_len, int override);                                       /* 568 */
    void (* envz_remove) (char **envz, size_t *envz_len, const char *name);                                                                          /* 572 */
    void (* envz_strip) (char **envz, size_t *envz_len);                                                                                             /* 576 */

    /* err.h */
    void (* warn) (const char *, ...);                                                                                                               /* 580 */
    void (* vwarn) (const char *, va_list);                                                                                                          /* 584 */
    void (* warnx) (const char *, ...);                                                                                                              /* 588 */
    void (* vwarnx) (const char *, va_list);                                                                                                         /* 592 */
    void (* err) (int, const char *, ...);                                                                                                           /* 596 */
    void (* verr) (int, const char *, va_list);                                                                                                      /* 600 */
    void (* errx) (int, const char *, ...);                                                                                                          /* 604 */
    void (* verrx) (int, const char *, va_list);                                                                                                     /* 608 */

    /* errno.h */
    int * (* __errno) (void);                                                                                                                        /* 612 */
    int * (* __h_errno) (void);                                                                                                                      /* 616 */

    /* fcntl.h */
    int (* open) (const char *path_name, int open_flag, ... /* mode_t mode */ );                                                                     /* 620 */
    int (* openat) (int fd, const char *filename, int flags, ...);                                                                                   /* 624 */
    int (* creat) (const char * path_name, mode_t mode);                                                                                             /* 628 */
    int (* close) (int file_descriptor);                                                                                                             /* 632 */
    ssize_t (* read) (int file_descriptor, void * buffer, size_t num_bytes);                                                                         /* 636 */
    ssize_t (* write) (int file_descriptor, const void * buffer, size_t num_bytes);                                                                  /* 640 */
    int (* fcntl) (int file_descriptor, int cmd, ... /* int arg */ );                                                                                /* 644 */

    /* fnmatch.h */
    int (* fnmatch) (const char *pattern, const char *string, int flags);                                                                            /* 648 */

    /* ftw.h */
    int (* ftw) (const char *path,int (*func)(const char *,const struct stat *,int),int depth);                                                      /* 652 */
    int (* nftw) (const char *path,int (*func)(const char *,const struct stat *,int,struct FTW *),int depth,int flags);                              /* 656 */

    /* glob.h */
    int (* glob) (const char *pattern, int flags, int (*errfunc) (const char *epath, int eerrno), glob_t *pglob);                                    /* 660 */
    void (* globfree) (glob_t *pglob);                                                                                                               /* 664 */

    /* grp.h */
    void (* endgrent) (void);                                                                                                                        /* 668 */
    struct group * (* getgrent) (void);                                                                                                              /* 672 */
    struct group * (* getgrgid) (gid_t gid);                                                                                                         /* 676 */
    struct group * (* getgrnam) (const char *name);                                                                                                  /* 680 */
    void (* setgrent) (void);                                                                                                                        /* 684 */

    /* iconv.h */
    iconv_t (* iconv_open) (const char *to, const char *from);                                                                                       /* 688 */
    size_t (* iconv) (iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);                                           /* 692 */
    int (* iconv_close) (iconv_t cd);                                                                                                                /* 696 */

    /* ifaddrs.h */
    int (* getifaddrs) (struct ifaddrs **ifap);                                                                                                      /* 700 */
    void (* freeifaddrs) (struct ifaddrs *ifa);                                                                                                      /* 704 */

    /* inttypes.h */
    intmax_t (* imaxabs) (intmax_t x);                                                                                                               /* 708 */
    imaxdiv_t (* imaxdiv) (intmax_t n,intmax_t d);                                                                                                   /* 712 */
    intmax_t (* strtoimax) (const char *str, char **ptr, int base);                                                                                  /* 716 */
    uintmax_t (* strtoumax) (const char *str, char **ptr, int base);                                                                                 /* 720 */
    intmax_t (* wcstoimax) (const wchar_t *str, wchar_t **ptr, int base);                                                                            /* 724 */
    uintmax_t (* wcstoumax) (const wchar_t *str, wchar_t **ptr, int base);                                                                           /* 728 */

    /* langinfo.h */
    char * (* nl_langinfo) (nl_item item);                                                                                                           /* 732 */

    /* libgen.h */
    char * (* basename) (const char *path);                                                                                                          /* 736 */
    char * (* dirname) (const char *path);                                                                                                           /* 740 */

    /* locale.h */
    char * (* setlocale) (int category, const char *locale);                                                                                         /* 744 */
    struct lconv * (* localeconv) (void);                                                                                                            /* 748 */

    /* malloc.h */
    void * (* memalign) (size_t alignment, size_t size);                                                                                             /* 752 */

    /* math.h */
    double (* acos) (double x);                                                                                                                      /* 756 */
    double (* asin) (double x);                                                                                                                      /* 760 */
    double (* atan) (double x);                                                                                                                      /* 764 */
    double (* atan2) (double y, double x);                                                                                                           /* 768 */
    double (* ceil) (double x);                                                                                                                      /* 772 */
    double (* cos) (double x);                                                                                                                       /* 776 */
    double (* cosh) (double x);                                                                                                                      /* 780 */
    void (* sincos) (double x, double *s, double *c);                                                                                                /* 784 */
    void (* sincosf) (float x, float *s, float *c);                                                                                                  /* 788 */
    void (* sincosl) (double x, double *s, double *c);                                                                                               /* 792 */
    double (* exp10) (double x);                                                                                                                     /* 796 */
    float (* exp10f) (float x);                                                                                                                      /* 800 */
    long double (* exp10l) (long double x);                                                                                                          /* 804 */
    double (* exp) (double x);                                                                                                                       /* 808 */
    double (* fabs) (double x);                                                                                                                      /* 812 */
    double (* floor) (double x);                                                                                                                     /* 816 */
    double (* fmod) (double x, double y);                                                                                                            /* 820 */
    double (* frexp) (double x, int *nptr);                                                                                                          /* 824 */
    double (* ldexp) (double x, int n);                                                                                                              /* 828 */
    double (* log) (double x);                                                                                                                       /* 832 */
    double (* log10) (double x);                                                                                                                     /* 836 */
    double (* modf) (double x, double *nptr);                                                                                                        /* 840 */
    double (* pow) (double x, double y);                                                                                                             /* 844 */
    double (* pow10) (double x);                                                                                                                     /* 848 */
    float (* pow10f) (float x);                                                                                                                      /* 852 */
    long double (* pow10l) (long double x);                                                                                                          /* 856 */
    double (* sin) (double x);                                                                                                                       /* 860 */
    double (* sinh) (double x);                                                                                                                      /* 864 */
    double (* sqrt) (double x);                                                                                                                      /* 868 */
    double (* tan) (double x);                                                                                                                       /* 872 */
    double (* tanh) (double x);                                                                                                                      /* 876 */
    double (* j0) (double x);                                                                                                                        /* 880 */
    double (* y0) (double x);                                                                                                                        /* 884 */
    float (* j0f) (float x);                                                                                                                         /* 888 */
    float (* y0f) (float x);                                                                                                                         /* 892 */
    double (* j1) (double x);                                                                                                                        /* 896 */
    double (* y1) (double x);                                                                                                                        /* 900 */
    float (* j1f) (float x);                                                                                                                         /* 904 */
    float (* y1f) (float x);                                                                                                                         /* 908 */
    double (* jn) (int n, double x);                                                                                                                 /* 912 */
    double (* yn) (int n, double x);                                                                                                                 /* 916 */
    float (* jnf) (int n, float x);                                                                                                                  /* 920 */
    float (* ynf) (int n, float x);                                                                                                                  /* 924 */
    int (* __fpclassify_float) (float x);                                                                                                            /* 928 */
    int (* __fpclassify_double) (double x);                                                                                                          /* 932 */
    int (* __fpclassify_long_double) (long double x);                                                                                                /* 936 */
    int (* __signbit_float) (float x);                                                                                                               /* 940 */
    int (* __signbit_double) (double x);                                                                                                             /* 944 */
    int (* __signbit_long_double) (long double x);                                                                                                   /* 948 */
    int * (* __signgam) (void);                                                                                                                      /* 952 */
    float (* acosf) (float x);                                                                                                                       /* 956 */
    float (* asinf) (float x);                                                                                                                       /* 960 */
    float (* atanf) (float x);                                                                                                                       /* 964 */
    float (* atan2f) (float y, float x);                                                                                                             /* 968 */
    float (* ceilf) (float x);                                                                                                                       /* 972 */
    float (* cosf) (float x);                                                                                                                        /* 976 */
    float (* coshf) (float x);                                                                                                                       /* 980 */
    float (* expf) (float x);                                                                                                                        /* 984 */
    float (* fabsf) (float x);                                                                                                                       /* 988 */
    float (* floorf) (float x);                                                                                                                      /* 992 */
    float (* fmodf) (float x, float y);                                                                                                              /* 996 */
    float (* frexpf) (float x, int *eptr);                                                                                                           /* 1000 */
    float (* ldexpf) (float x, int exp);                                                                                                             /* 1004 */
    float (* logf) (float x);                                                                                                                        /* 1008 */
    float (* log10f) (float x);                                                                                                                      /* 1012 */
    float (* modff) (float x, float *iptr);                                                                                                          /* 1016 */
    float (* powf) (float x, float y);                                                                                                               /* 1020 */
    float (* sinf) (float x);                                                                                                                        /* 1024 */
    float (* sinhf) (float x);                                                                                                                       /* 1028 */
    float (* sqrtf) (float x);                                                                                                                       /* 1032 */
    float (* tanf) (float x);                                                                                                                        /* 1036 */
    float (* tanhf) (float x);                                                                                                                       /* 1040 */
    float (* acoshf) (float x);                                                                                                                      /* 1044 */
    float (* asinhf) (float x);                                                                                                                      /* 1048 */
    float (* atanhf) (float x);                                                                                                                      /* 1052 */
    float (* cbrtf) (float x);                                                                                                                       /* 1056 */
    float (* copysignf) (float x, float y);                                                                                                          /* 1060 */
    float (* erfcf) (float x);                                                                                                                       /* 1064 */
    float (* erff) (float x);                                                                                                                        /* 1068 */
    float (* expm1f) (float x);                                                                                                                      /* 1072 */
    float (* fdimf) (float x, float y);                                                                                                              /* 1076 */
    float (* fmaf) (float x, float y, float z);                                                                                                      /* 1080 */
    float (* fmaxf) (float x, float y);                                                                                                              /* 1084 */
    float (* fminf) (float x, float y);                                                                                                              /* 1088 */
    float (* hypotf) (float x, float y);                                                                                                             /* 1092 */
    float (* lgammaf) (float x);                                                                                                                     /* 1096 */
    float (* log1pf) (float x);                                                                                                                      /* 1100 */
    float (* logbf) (float x);                                                                                                                       /* 1104 */
    long long (* llrintf) (float x);                                                                                                                 /* 1108 */
    long (* lrintf) (float x);                                                                                                                       /* 1112 */
    long (* lroundf) (float x);                                                                                                                      /* 1116 */
    long long (* llround) (double x);                                                                                                                /* 1120 */
    long long (* llroundf) (float x);                                                                                                                /* 1124 */
    float (* nanf) (const char *tagp);                                                                                                               /* 1128 */
    float (* nearbyintf) (float x);                                                                                                                  /* 1132 */
    float (* nextafterf) (float x, float y);                                                                                                         /* 1136 */
    float (* nexttowardf) (float x, long double y);                                                                                                  /* 1140 */
    float (* remainderf) (float x, float p);                                                                                                         /* 1144 */
    float (* remquof) (float x, float y, int *quo);                                                                                                  /* 1148 */
    float (* rintf) (float x);                                                                                                                       /* 1152 */
    float (* roundf) (float x);                                                                                                                      /* 1156 */
    float (* scalblnf) (float x, long n);                                                                                                            /* 1160 */
    float (* scalbnf) (float x, int n);                                                                                                              /* 1164 */
    float (* tgammaf) (float x);                                                                                                                     /* 1168 */
    float (* truncf) (float x);                                                                                                                      /* 1172 */
    int (* ilogbf) (float x);                                                                                                                        /* 1176 */
    int (* finite) (double x);                                                                                                                       /* 1180 */
    double (* acosh) (double x);                                                                                                                     /* 1184 */
    double (* asinh) (double x);                                                                                                                     /* 1188 */
    double (* atanh) (double x);                                                                                                                     /* 1192 */
    double (* cbrt) (double x);                                                                                                                      /* 1196 */
    double (* copysign) (double x, double y);                                                                                                        /* 1200 */
    double (* erf) (double x);                                                                                                                       /* 1204 */
    double (* erfc) (double x);                                                                                                                      /* 1208 */
    double (* expm1) (double x);                                                                                                                     /* 1212 */
    double (* fdim) (double x, double y);                                                                                                            /* 1216 */
    double (* fma) (double x, double y, double z);                                                                                                   /* 1220 */
    double (* fmax) (double x, double y);                                                                                                            /* 1224 */
    double (* fmin) (double x, double y);                                                                                                            /* 1228 */
    double (* hypot) (double x, double y);                                                                                                           /* 1232 */
    double (* lgamma) (double x);                                                                                                                    /* 1236 */
    double (* log1p) (double x);                                                                                                                     /* 1240 */
    double (* logb) (double x);                                                                                                                      /* 1244 */
    long long (* llrint) (double x);                                                                                                                 /* 1248 */
    long int (* lrint) (double x);                                                                                                                   /* 1252 */
    long int (* lround) (double x);                                                                                                                  /* 1256 */
    double (* nan) (const char *tagp);                                                                                                               /* 1260 */
    double (* nearbyint) (double x);                                                                                                                 /* 1264 */
    double (* nexttoward) (double x, long double y);                                                                                                 /* 1268 */
    double (* nextafter) (double x, double y);                                                                                                       /* 1272 */
    double (* remainder) (double x, double p);                                                                                                       /* 1276 */
    double (* remquo) (double x, double y, int *quo);                                                                                                /* 1280 */
    double (* rint) (double x);                                                                                                                      /* 1284 */
    double (* round) (double x);                                                                                                                     /* 1288 */
    double (* scalbln) (double x, long n);                                                                                                           /* 1292 */
    double (* scalbn) (double x, int n);                                                                                                             /* 1296 */
    double (* tgamma) (double x);                                                                                                                    /* 1300 */
    double (* trunc) (double x);                                                                                                                     /* 1304 */
    int (* ilogb) (double x);                                                                                                                        /* 1308 */
    float (* exp2f) (float x);                                                                                                                       /* 1312 */
    double (* exp2) (double x);                                                                                                                      /* 1316 */
    long double (* exp2l) (long double x);                                                                                                           /* 1320 */
    double (* log2) (double x);                                                                                                                      /* 1324 */
    float (* log2f) (float x);                                                                                                                       /* 1328 */
    long double (* log2l) (long double x);                                                                                                           /* 1332 */
    long double (* acosl) (long double x);                                                                                                           /* 1336 */
    long double (* asinl) (long double x);                                                                                                           /* 1340 */
    long double (* atanl) (long double x);                                                                                                           /* 1344 */
    long double (* atan2l) (long double y, long double x);                                                                                           /* 1348 */
    long double (* ceill) (long double x);                                                                                                           /* 1352 */
    long double (* cosl) (long double x);                                                                                                            /* 1356 */
    long double (* coshl) (long double x);                                                                                                           /* 1360 */
    long double (* expl) (long double x);                                                                                                            /* 1364 */
    long double (* fabsl) (long double x);                                                                                                           /* 1368 */
    long double (* floorl) (long double x);                                                                                                          /* 1372 */
    long double (* fmodl) (long double x, long double y);                                                                                            /* 1376 */
    long double (* frexpl) (long double x, int *nptr);                                                                                               /* 1380 */
    long double (* ldexpl) (long double x, int n);                                                                                                   /* 1384 */
    long double (* logl) (long double x);                                                                                                            /* 1388 */
    long double (* log10l) (long double x);                                                                                                          /* 1392 */
    long double (* modfl) (long double x, long double *iptr);                                                                                        /* 1396 */
    long double (* powl) (long double x, long double y);                                                                                             /* 1400 */
    long double (* sinl) (long double x);                                                                                                            /* 1404 */
    long double (* sinhl) (long double x);                                                                                                           /* 1408 */
    long double (* sqrtl) (long double x);                                                                                                           /* 1412 */
    long double (* tanl) (long double x);                                                                                                            /* 1416 */
    long double (* tanhl) (long double x);                                                                                                           /* 1420 */
    long double (* acoshl) (long double x);                                                                                                          /* 1424 */
    long double (* asinhl) (long double x);                                                                                                          /* 1428 */
    long double (* atanhl) (long double x);                                                                                                          /* 1432 */
    long double (* cbrtl) (long double x);                                                                                                           /* 1436 */
    long double (* copysignl) (long double x, long double y);                                                                                        /* 1440 */
    long double (* erfcl) (long double x);                                                                                                           /* 1444 */
    long double (* erfl) (long double x);                                                                                                            /* 1448 */
    long double (* expm1l) (long double x);                                                                                                          /* 1452 */
    long double (* fdiml) (long double x, long double y);                                                                                            /* 1456 */
    long double (* fmal) (long double x, long double y, long double z);                                                                              /* 1460 */
    long double (* fmaxl) (long double x, long double y);                                                                                            /* 1464 */
    long double (* fminl) (long double x, long double y);                                                                                            /* 1468 */
    long double (* hypotl) (long double x, long double y);                                                                                           /* 1472 */
    long double (* lgammal) (long double x);                                                                                                         /* 1476 */
    long double (* log1pl) (long double x);                                                                                                          /* 1480 */
    long double (* logbl) (long double x);                                                                                                           /* 1484 */
    long long int (* llrintl) (long double x);                                                                                                       /* 1488 */
    long int (* lrintl) (long double x);                                                                                                             /* 1492 */
    long long int (* llroundl) (long double x);                                                                                                      /* 1496 */
    long int (* lroundl) (long double x);                                                                                                            /* 1500 */
    long double (* nanl) (const char *tagp);                                                                                                         /* 1504 */
    long double (* nearbyintl) (long double x);                                                                                                      /* 1508 */
    long double (* nextafterl) (long double x, long double y);                                                                                       /* 1512 */
    long double (* nexttowardl) (long double x, long double y);                                                                                      /* 1516 */
    long double (* remainderl) (long double x, long double p);                                                                                       /* 1520 */
    long double (* remquol) (long double x, long double y, int *quo);                                                                                /* 1524 */
    long double (* rintl) (long double x);                                                                                                           /* 1528 */
    long double (* roundl) (long double x);                                                                                                          /* 1532 */
    long double (* scalblnl) (long double x, long int n);                                                                                            /* 1536 */
    long double (* scalbnl) (long double x, int n);                                                                                                  /* 1540 */
    long double (* tgammal) (long double x);                                                                                                         /* 1544 */
    long double (* truncl) (long double x);                                                                                                          /* 1548 */
    int (* ilogbl) (long double x);                                                                                                                  /* 1552 */

    /* netdb.h */
    struct hostent * (* gethostbyaddr) (const void *addr, socklen_t len, int type);                                                                  /* 1556 */
    struct hostent * (* gethostbyname) (const char *name);                                                                                           /* 1560 */
    struct netent * (* getnetbyaddr) (in_addr_t net, int type);                                                                                      /* 1564 */
    struct netent * (* getnetbyname) (const char *name);                                                                                             /* 1568 */
    struct protoent * (* getprotobyname) (const char *name);                                                                                         /* 1572 */
    struct protoent * (* getprotobynumber) (int proto);                                                                                              /* 1576 */
    struct servent * (* getservbyname) (const char *name, const char *proto);                                                                        /* 1580 */
    struct servent * (* getservbyport) (int port, const char *proto);                                                                                /* 1584 */
    void (* herror) (const char *);                                                                                                                  /* 1588 */
    const char * (* hstrerror) (int);                                                                                                                /* 1592 */
    int (* gethostbyname_r) (const char *, struct hostent *, char *, size_t, struct hostent **, int *);                                              /* 1596 */
    int (* gethostbyname2_r) (const char *, int, struct hostent *, char *, size_t, struct hostent **, int *);                                        /* 1600 */
    struct hostent * (* gethostbyname2) (const char *, int);                                                                                         /* 1604 */
    int (* gethostbyaddr_r) (const void *, socklen_t, int, struct hostent *, char *, size_t, struct hostent **, int *);                              /* 1608 */
    int (* getservbyport_r) (int, const char *, struct servent *, char *, size_t, struct servent **);                                                /* 1612 */
    int (* getservbyname_r) (const char *, const char *, struct servent *, char *, size_t, struct servent **);                                       /* 1616 */
    const char * (* gai_strerror) (int ecode);                                                                                                       /* 1620 */
    int (* getaddrinfo) (const char *name, const char *service, const struct addrinfo *req, struct addrinfo **pai);                                  /* 1624 */
    void (* freeaddrinfo) (struct addrinfo *ai);                                                                                                     /* 1628 */
    int (* getnameinfo) (const struct sockaddr *sa, socklen_t salen, char *node, socklen_t nodelen, char *service, socklen_t servicelen, int flags); /* 1632 */

    /* nl_types.h */
    nl_catd (* catopen) (const char* name, int flag);                                                                                                /* 1636 */
    char* (* catgets) (nl_catd catalog, int set_number, int msg_number, const char* msg);                                                            /* 1640 */
    int (* catclose) (nl_catd catalog);                                                                                                              /* 1644 */

    /* poll.h */
    int (* poll) (struct pollfd *fds, nfds_t nfds, int timeout);                                                                                     /* 1648 */

    /* pwd.h */
    void (* endpwent) (void);                                                                                                                        /* 1652 */
    struct passwd * (* getpwent) (void);                                                                                                             /* 1656 */
    struct passwd * (* getpwnam) (const char *name);                                                                                                 /* 1660 */
    int (* getpwnam_r) (const char *name, struct passwd *pw, char *buffer, size_t bufsize, struct passwd **result);                                  /* 1664 */
    struct passwd * (* getpwuid) (uid_t uid);                                                                                                        /* 1668 */
    int (* getpwuid_r) (uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);                                        /* 1672 */
    void (* setpwent) (void);                                                                                                                        /* 1676 */

    /* regex.h */
    int (* regcomp) (regex_t * preg, const char * regex, int cflags);                                                                                /* 1680 */
    int (* regexec) (const regex_t *, const char *, size_t, regmatch_t *, int);                                                                      /* 1684 */
    void (* regfree) (regex_t *preg);                                                                                                                /* 1688 */
    size_t (* regerror) (int e, const regex_t * preg, char * buf, size_t size);                                                                      /* 1692 */

    /* resolv.h */
    struct __res_state * (* __res_state) (void);                                                                                     /* 1696 */
    int (* res_init) (void);                                                                                                                         /* 1700 */
    int (* res_query) (const char *dname, int klass, int type, unsigned char *answer, int anslen);                                                   /* 1704 */
    int (* res_querydomain) (const char *name, const char *domain,int klass, int type, unsigned char *answer,int anslen);                            /* 1708 */
    int (* res_search) (const char *dname, int klass, int type, unsigned char *answer, int anslen);                                                  /* 1712 */
    int (* res_mkquery) (int, const char *, int, int, const unsigned char *, int, const unsigned char *, unsigned char *, int);                      /* 1716 */
    int (* res_send) (const unsigned char *msg, int msglen, unsigned char *answer,int anslen);                                                       /* 1720 */
    int (* dn_comp) (unsigned char *exp_dn, unsigned char *comp_dn,int length, unsigned char **dnptrs, unsigned char **lastdnptr);                   /* 1724 */
    int (* dn_expand) (unsigned char *msg, unsigned char *eomorig,unsigned char *comp_dn, char *exp_dn,int length);                                  /* 1728 */
    int (* dn_skipname) (const unsigned char *, const unsigned char *);                                                                              /* 1732 */

    /* search.h */
    int (* hcreate) (size_t);                                                                                                                        /* 1736 */
    void (* hdestroy) (void);                                                                                                                        /* 1740 */
    ENTRY * (* hsearch) (ENTRY, ACTION);                                                                                                             /* 1744 */
    int (* hcreate_r) (size_t, struct hsearch_data *);                                                                                               /* 1748 */
    void (* hdestroy_r) (struct hsearch_data *);                                                                                                     /* 1752 */
    int (* hsearch_r) (ENTRY, ACTION, ENTRY **, struct hsearch_data *);                                                                              /* 1756 */
    void * (* lfind) (const void *key, const void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                     /* 1760 */
    void * (* lsearch) (const void *key, void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                         /* 1764 */
    void * (* tdelete) (const void *vkey, void **vrootp, int (*compar)(const void *, const void *));                                                 /* 1768 */
    void (* tdestroy) (void *vrootp, void (*freefct)(void *));                                                                                       /* 1772 */
    void * (* tfind) (const void *, void **, int (*)(const void *, const void *));                                                                   /* 1776 */
    void * (* tsearch) (const void *, void **, int (*)(const void *, const void *));                                                                 /* 1780 */
    void (* twalk) (const void *root, void (*action)(const void *, VISIT, int));                                                                     /* 1784 */

    /* setjmp.h */
    int (* setjmp) (jmp_buf __env);                                                                                                                  /* 1788 */
    int (* __sigsetjmp) (struct __jmp_buf_tag __env[1], int __savemask);                                                                             /* 1792 */
    int (* _setjmp) (struct __jmp_buf_tag __env[1]);                                                                                                 /* 1796 */
    void (* longjmp) (jmp_buf __env, int __val);                                                                                                     /* 1800 */
    void (* _longjmp) (struct __jmp_buf_tag __env[1], int __val);                                                                                    /* 1804 */
    void (* siglongjmp) (sigjmp_buf __env, int __val);                                                                                               /* 1808 */
    int (* __sigjmp_save) (jmp_buf __env, int __savemask);                                                                                           /* 1812 */

    /* signal.h */
    _sig_func_ptr (* signal) (int, _sig_func_ptr);;                                                                                                  /* 1816 */
    int (* raise) (int sig);                                                                                                                         /* 1820 */
    int (* sigmask) (int signum);                                                                                                                    /* 1824 */
    int (* sigblock) (int signal_mask);                                                                                                              /* 1828 */
    int (* sigsetmask) (int signal_mask);                                                                                                            /* 1832 */
    int (* sigprocmask) (int how, const sigset_t *set, sigset_t *oset);                                                                              /* 1836 */
    int (* sigismember) (const sigset_t *set, int sig);                                                                                              /* 1840 */
    int (* sigemptyset) (sigset_t *set);                                                                                                             /* 1844 */
    int (* sigfillset) (sigset_t *set);                                                                                                              /* 1848 */
    int (* sigdelset) (sigset_t *set, int sig);                                                                                                      /* 1852 */
    int (* sigaddset) (sigset_t *set, int sig);                                                                                                      /* 1856 */
    int (* kill) (pid_t pid, int signal_number);                                                                                                     /* 1860 */
    int (* sigaction) (int sig, const struct sigaction *act, struct sigaction *oact);                                                                /* 1864 */

    /* stdio.h */
    void (* perror) (const char *s);                                                                                                                 /* 1868 */
    FILE * (* fdopen64) (int, const char *);                                                                                                         /* 1872 */
    FILE * (* fopen64) (const char *filename, const char *mode);                                                                                     /* 1876 */
    FILE * (* freopen64) (const char *filename, const char *type, FILE *stream);                                                                     /* 1880 */
    _off64_t (* ftello64) (FILE *stream);                                                                                                            /* 1884 */
    _off64_t (* fseeko64) (FILE *stream, _off64_t, int);                                                                                             /* 1888 */
    int (* fgetpos64) (FILE *stream, _fpos64_t *pos);                                                                                                /* 1892 */
    int (* fsetpos64) (FILE *stream, _fpos64_t *pos);                                                                                                /* 1896 */
    FILE * (* tmpfile64) (void);                                                                                                                     /* 1900 */
    FILE * (* fopen) (const char *filename, const char *mode);                                                                                       /* 1904 */
    int (* fclose) (FILE *stream);                                                                                                                   /* 1908 */
    int (* fflush) (FILE *stream);                                                                                                                   /* 1912 */
    FILE * (* freopen) (const char *filename, const char *mode, FILE *stream);                                                                       /* 1916 */
    int (* setvbuf) (FILE *stream, char *buf, int bufmode, size_t size);                                                                             /* 1920 */
    void (* setbuf) (FILE *stream, char *buf);                                                                                                       /* 1924 */
    int (* fseek) (FILE *stream, long int offset, int wherefrom);                                                                                    /* 1928 */
    long int (* ftell) (FILE *stream);                                                                                                               /* 1932 */
    void (* rewind) (FILE *stream);                                                                                                                  /* 1936 */
    int (* fgetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 1940 */
    int (* fsetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 1944 */
    int (* fgetc) (FILE *stream);                                                                                                                    /* 1948 */
    int (* getc) (FILE *stream);                                                                                                                     /* 1952 */
    int (* getchar) (void);                                                                                                                          /* 1956 */
    int (* ungetc) (int c, FILE *stream);                                                                                                            /* 1960 */
    int (* fputc) (int c, FILE *stream);                                                                                                             /* 1964 */
    int (* putc) (int c, FILE *stream);                                                                                                              /* 1968 */
    int (* putchar) (int c);                                                                                                                         /* 1972 */
    char * (* fgets) (char *s, int n, FILE *stream);                                                                                                 /* 1976 */
    char * (* gets) (char *s);                                                                                                                       /* 1980 */
    int (* fscanf) (FILE *stream, const char *format, ...);                                                                                          /* 1984 */
    int (* scanf) (const char *format, ...);                                                                                                         /* 1988 */
    int (* sscanf) (const char *s, const char *format, ...);                                                                                         /* 1992 */
    int (* fputs) (const char *s, FILE *stream);                                                                                                     /* 1996 */
    int (* puts) (const char *s);                                                                                                                    /* 2000 */
    int (* dprintf) (int fd, const char *format, ...);                                                                                               /* 2004 */
    int (* fprintf) (FILE *stream, const char *format, ...);                                                                                         /* 2008 */
    int (* printf) (const char *format, ...);                                                                                                        /* 2012 */
    int (* sprintf) (char *s, const char *format, ...);                                                                                              /* 2016 */
    int (* vdprintf) (int fd, const char *format, va_list arg);                                                                                      /* 2020 */
    int (* vfprintf) (FILE *stream, const char *format, va_list arg);                                                                                /* 2024 */
    int (* vprintf) (const char *format, va_list arg);                                                                                               /* 2028 */
    int (* vsprintf) (char *s, const char *format, va_list arg);                                                                                     /* 2032 */
    size_t (* fread) (void *ptr, size_t element_size, size_t count, FILE *stream);                                                                   /* 2036 */
    size_t (* fwrite) (const void *ptr, size_t element_size, size_t count, FILE *stream);                                                            /* 2040 */
    int (* feof) (FILE *stream);                                                                                                                     /* 2044 */
    int (* ferror) (FILE *stream);                                                                                                                   /* 2048 */
    void (* clearerr) (FILE *stream);                                                                                                                /* 2052 */
    int (* rename) (const char *oldname, const char *newname);                                                                                       /* 2056 */
    int (* remove) (const char *filename);                                                                                                           /* 2060 */
    FILE * (* tmpfile) (void);                                                                                                                       /* 2064 */
    char * (* tmpnam) (char *buf);                                                                                                                   /* 2068 */
    char * (* tempnam) (const char *dir, const char *pfx);                                                                                           /* 2072 */
    char * (* ctermid) (char *);                                                                                                                     /* 2076 */
    int (* __flush) (FILE *stream);                                                                                                                  /* 2080 */
    int (* __unlockfile) (FILE *stream, int c);                                                                                                      /* 2084 */
    int (* getc_unlocked) (FILE *stream);                                                                                                            /* 2088 */
    int (* getchar_unlocked) (void);                                                                                                                 /* 2092 */
    int (* putc_unlocked) (int c, FILE *stream);                                                                                                     /* 2096 */
    int (* putchar_unlocked) (int c);                                                                                                                /* 2100 */
    ssize_t (* getline) (char **lineptr, size_t *n, FILE *stream);                                                                                   /* 2104 */
    ssize_t (* getdelim) (char **lineptr, size_t *n, int delim, FILE *stream);                                                                       /* 2108 */
    FILE * (* fdopen) (int file_descriptor, const char *type);                                                                                       /* 2112 */
    int (* fileno) (FILE *file);                                                                                                                     /* 2116 */
    int (* asprintf) (char **ret, const char *format, ...);                                                                                          /* 2120 */
    int (* vsnprintf) (char *s, size_t size, const char *format, va_list arg);                                                                       /* 2124 */
    int (* pclose) (FILE *stream);                                                                                                                   /* 2128 */
    FILE * (* popen) (const char *command, const char *type);                                                                                        /* 2132 */
    int (* fseeko) (FILE *stream, off_t offset, int wherefrom);                                                                                      /* 2136 */
    off_t (* ftello) (FILE *stream);                                                                                                                 /* 2140 */
    void (* flockfile) (FILE *file);                                                                                                                 /* 2144 */
    void (* funlockfile) (FILE *file);                                                                                                               /* 2148 */
    int (* ftrylockfile) (FILE *file);                                                                                                               /* 2152 */
    int (* vasprintf) (char **ret, const char *format, va_list arg);                                                                                 /* 2156 */
    int (* vfscanf) (FILE *stream, const char *format, va_list arg);                                                                                 /* 2160 */
    int (* vsscanf) (const char *s, const char *format, va_list arg);                                                                                /* 2164 */
    int (* vscanf) (const char *format, va_list arg);                                                                                                /* 2168 */
    int (* snprintf) (char *s, size_t size, const char *format, ...);                                                                                /* 2172 */

    /* stdio_ext.h */
    void (* _flushlbf) (void);                                                                                                                       /* 2176 */
    int (* __fsetlocking) (FILE *, int);                                                                                                             /* 2180 */
    int (* __fwriting) (FILE *);                                                                                                                     /* 2184 */
    int (* __freading) (FILE *);                                                                                                                     /* 2188 */
    int (* __freadable) (FILE *);                                                                                                                    /* 2192 */
    int (* __fwritable) (FILE *);                                                                                                                    /* 2196 */
    int (* __flbf) (FILE *);                                                                                                                         /* 2200 */
    size_t (* __fbufsize) (FILE *);                                                                                                                  /* 2204 */
    size_t (* __fpending) (FILE *);                                                                                                                  /* 2208 */
    int (* __fpurge) (FILE *);                                                                                                                       /* 2212 */
    int (* fpurge) (FILE *);                                                                                                                        /* 2216 */
    void (* __fseterr) (FILE *);                                                                                                                     /* 2220 */
    size_t (* __freadahead) (FILE *);                                                                                                                /* 2224 */
    void (* __freadptrinc) (FILE *f, size_t inc);                                                                                                    /* 2228 */

    /* stdlib.h */
    int * (* __mb_cur_max) (void);                                                                                                                  /* 2232 */
    const char * (* __getprogname) (void);                                                                                                           /* 2236 */
    int (* mblen) (const char *s, size_t n);                                                                                                         /* 2240 */
    int (* mbtowc) (wchar_t *pwc, const char *s, size_t n);                                                                                          /* 2244 */
    int (* _mbtowc_r) (wchar_t *pwc, const char *s, size_t n, _mbstate_t *st);                                                                       /* 2248 */
    size_t (* _mbstowcs_r) (wchar_t *pwcs, const char *s, size_t n, mbstate_t *state);                                                               /* 2252 */
    int (* wctomb) (char *s, wchar_t wchar);                                                                                                         /* 2256 */
    int (* _wctomb_r) (char *s, wchar_t wchar, mbstate_t *state);                                                                                    /* 2260 */
    size_t (* mbstowcs) (wchar_t *ws, const char *s, size_t wn);                                                                                     /* 2264 */
    size_t (* wcstombs) (char *s, const wchar_t *pwcs, size_t n);                                                                                    /* 2268 */
    size_t (* _wcstombs_r) (char *s, const wchar_t *pwcs, size_t n, mbstate_t *state);                                                               /* 2272 */
    void * (* malloc) (size_t size);                                                                                                                 /* 2276 */
    void * (* calloc) (size_t num_elements, size_t element_size);                                                                                    /* 2280 */
    void (* free) (void *ptr);                                                                                                                       /* 2284 */
    void * (* realloc) (void *ptr, size_t size);                                                                                                     /* 2288 */
    void * (* valloc) (size_t size);                                                                                                                 /* 2292 */
    void * (* aligned_alloc) (size_t alignment, size_t size);                                                                                        /* 2296 */
    int (* posix_memalign) (void **memptr, size_t alignment, size_t size);                                                                           /* 2300 */
    int (* abs) (int x);                                                                                                                             /* 2304 */
    long (* labs) (long x);                                                                                                                          /* 2308 */
    div_t (* div) (int n, int d);                                                                                                                    /* 2312 */
    ldiv_t (* ldiv) (long n, long d);                                                                                                                /* 2316 */
    int (* rand) (void);                                                                                                                             /* 2320 */
    void (* srand) (unsigned int seed);                                                                                                              /* 2324 */
    long (* random) (void);                                                                                                                          /* 2328 */
    void (* srandom) (unsigned int seed);                                                                                                            /* 2332 */
    char * (* initstate) (unsigned int seed, char *state, size_t size);                                                                              /* 2336 */
    char * (* setstate) (const char *state);                                                                                                         /* 2340 */
    char * (* l64a) (long x0);                                                                                                                       /* 2344 */
    long (* a64l) (const char *s);                                                                                                                   /* 2348 */
    void (* setkey) (const char *key);                                                                                                               /* 2352 */
    double (* drand48) (void);                                                                                                                       /* 2356 */
    double (* erand48) (unsigned short subi[3]);                                                                                                     /* 2360 */
    long int (* lrand48) (void);                                                                                                                     /* 2364 */
    long int (* nrand48) (unsigned short subi[3]);                                                                                                   /* 2368 */
    long (* mrand48) (void);                                                                                                                         /* 2372 */
    long (* jrand48) (unsigned short subi[3]);                                                                                                       /* 2376 */
    void (* srand48) (long);                                                                                                                         /* 2380 */
    unsigned short * (* seed48) (unsigned short subi[3]);                                                                                            /* 2384 */
    void (* lcong48) (unsigned short subi[7]);                                                                                                       /* 2388 */
    int (* system) (const char *command);                                                                                                            /* 2392 */
    void (* exit) (int status);                                                                                                                      /* 2396 */
    void (* abort) (void);                                                                                                                           /* 2400 */
    int (* atexit) (void (*)(void));                                                                                                                 /* 2404 */
    char * (* getenv) (const char *name);                                                                                                            /* 2408 */
    void * (* bsearch) (const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value));           /* 2412 */
    void (* qsort) (void *base, size_t count, size_t size, int (*compare)(const void *element1, const void *element2));                              /* 2416 */
    double (* strtod) (const char *str, char **ptr);                                                                                                 /* 2420 */
    long (* strtol) (const char *str, char **ptr, int base);                                                                                         /* 2424 */
    unsigned long (* strtoul) (const char *str, char **ptr, int base);                                                                               /* 2428 */
    quad_t (* strtoq) (const char *nptr, char **endptr, register int base);                                                                          /* 2432 */
    double (* atof) (const char *str);                                                                                                               /* 2436 */
    int (* atoi) (const char *str);                                                                                                                  /* 2440 */
    long (* atol) (const char *str);                                                                                                                 /* 2444 */
    char * (* itoa) (int value, char *buffer, int base);                                                                                             /* 2448 */
    char * (* lltoa) (int64_t ll, char *buffer, int radix);                                                                                          /* 2452 */
    void (* _exit) (int status);                                                                                                                     /* 2456 */
    int (* rand_r) (unsigned int *seed);                                                                                                             /* 2460 */
    char * (* mktemp) (char *name_template);                                                                                                         /* 2464 */
    int (* mkstemp) (char *name_template);                                                                                                           /* 2468 */
    int (* unsetenv) (const char *name);                                                                                                             /* 2472 */
    int (* setenv) (const char *name, const char *value, int overwrite);                                                                             /* 2476 */
    int (* putenv) (const char *string);                                                                                                             /* 2480 */
    char * (* mkdtemp) (char *name_template);                                                                                                        /* 2484 */
    const char * (* getexecname) (void);                                                                                                             /* 2488 */
    long long (* strtoll) (const char *str, char **ptr, int base);                                                                                   /* 2492 */
    long double (* strtold) (const char *str, char **ptr);                                                                                           /* 2496 */
    unsigned long long (* strtoull) (const char *str, char **ptr, int base);                                                                         /* 2500 */
    long long (* atoll) (const char *str);                                                                                                           /* 2504 */
    long long (* llabs) (long long x);                                                                                                               /* 2508 */
    lldiv_t (* lldiv) (long long n, long long d);                                                                                                    /* 2512 */
    float (* strtof) (const char *str, char **ptr);                                                                                                  /* 2516 */
    float (* atoff) (const char *nptr);                                                                                                              /* 2520 */
    void (* _Exit) (int status);                                                                                                                     /* 2524 */
    char * (* ecvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2528 */
    char * (* fcvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2532 */
    char * (* gcvt) (double x, int n, char *b);                                                                                                      /* 2536 */
    char * (* secure_getenv) (const char *name);                                                                                                     /* 2540 */
    void * (* reallocarray) (void *ptr, size_t m, size_t n);                                                                                         /* 2544 */
    void (* qsort_r) (void *, size_t, size_t, int (*)(const void *, const void *, void *), void *);                                                  /* 2548 */
    uint32_t (* arc4random) (void);                                                                                                                  /* 2552 */
    void (* arc4random_buf) (void *buf, size_t nbytes);                                                                                              /* 2556 */
    uint32_t (* arc4random_uniform) (uint32_t upper_bound);                                                                                          /* 2560 */
    void (* arc4random_stir) (void);                                                                                                                 /* 2564 */
    void (* arc4random_addrandom) (unsigned char *dat, int datlen);                                                                                  /* 2568 */

    /* string.h */
    char * (* strerror) (int error_number);                                                                                                          /* 2572 */
    char * (* strcat) (char *dest, const char *src);                                                                                                 /* 2576 */
    char * (* strncat) (char *dest, const char *src, size_t n);                                                                                      /* 2580 */
    int (* strcmp) (const char *s1, const char * s2);                                                                                                /* 2584 */
    int (* strncmp) (const char *s1, const char *s2, size_t n);                                                                                      /* 2588 */
    char * (* strcpy) (char *dest, const char *src);                                                                                                 /* 2592 */
    char * (* strncpy) (char *dest, const char *src, size_t n);                                                                                      /* 2596 */
    size_t (* strnlen) (const char *s, size_t maxlen);                                                                                               /* 2600 */
    size_t (* strlen) (const char *s);                                                                                                               /* 2604 */
    char * (* strchr) (const char *s, int c);                                                                                                        /* 2608 */
    char * (* strrchr) (const char *s, int c);                                                                                                       /* 2612 */
    size_t (* strspn) (const char *s, const char *set);                                                                                              /* 2616 */
    size_t (* strcspn) (const char *s, const char *set);                                                                                             /* 2620 */
    char * (* strpbrk) (const char *s, const char *set);                                                                                             /* 2624 */
    char * (* strtok) (char *str, const char *set);                                                                                                  /* 2628 */
    char * (* strstr) (const char *src, const char *sub);                                                                                            /* 2632 */
    char * (* strnstr) (const char *src, const char *sub, size_t len);                                                                               /* 2636 */
    char * (* strsep) (char ** src, const char *delim);                                                                                              /* 2640 */
    char * (* stpcpy) (char *dest, const char *src);                                                                                                 /* 2644 */
    char * (* stpncpy) (char *dest, const char *src, size_t len);                                                                                    /* 2648 */
    size_t (* stccpy) (char * dest, const char * src, size_t n);                                                                                     /* 2652 */
    int (* strcoll) (const char *s1, const char *s2);                                                                                                /* 2656 */
    size_t (* strxfrm) (char *dest, const char *src, size_t len);                                                                                    /* 2660 */
    void * (* memmove) (void *dest, const void * src, size_t len);                                                                                   /* 2664 */
    void * (* memchr) (const void * ptr, int val, size_t len);                                                                                       /* 2668 */
    int (* memcmp) (const void *ptr1, const void *ptr2, size_t len);                                                                                 /* 2672 */
    void * (* memcpy) (void *dest, const void *src, size_t len);                                                                                     /* 2676 */
    void * (* memset) (void *ptr, int val, size_t len);                                                                                              /* 2680 */
    int (* strerror_r) (int error,char * buffer,size_t buffer_size);                                                                                 /* 2684 */
    char * (* strdup) (const char *s);                                                                                                               /* 2688 */
    char * (* strndup) (const char *str, size_t max);                                                                                                /* 2692 */
    void (* bcopy) (const void *from,void *to,size_t len);                                                                                           /* 2696 */
    void (* bzero) (void *m,size_t len);                                                                                                             /* 2700 */
    void (* explicit_bzero) (void *m, size_t len);                                                                                                   /* 2704 */
    int (* bcmp) (const void *a,const void *b,size_t len);                                                                                           /* 2708 */
    size_t (* strlcpy) (char *dst, const char *src, size_t siz);                                                                                     /* 2712 */
    size_t (* strlcat) (char *dst, const char *src, size_t siz);                                                                                     /* 2716 */
    char * (* strtok_r) (char *str, const char *separator_set,char ** state_ptr);                                                                    /* 2720 */
    char * (* strsignal) (int);                                                                                                                      /* 2724 */
    void * (* memccpy) (void *dst0, const void *src0, int endchar0, size_t len0);                                                                    /* 2728 */
    char * (* index) (const char *s, int c);                                                                                                         /* 2732 */
    char * (* rindex) (const char *s, int c);                                                                                                        /* 2736 */
    int (* strverscmp) (const char *s1, const char *s2);                                                                                             /* 2740 */
    char * (* strchrnul) (const char *s, int c_in);                                                                                                  /* 2744 */
    char * (* strcasestr) (const char *haystack, const char *needle);                                                                                /* 2748 */
    void * (* memmem) (const void *h0, size_t k, const void *n0, size_t l);                                                                          /* 2752 */
    void * (* memrchr) (const void *s, int c, size_t n);                                                                                             /* 2756 */
    void * (* mempcpy) (void *dst0, const void *src0, size_t len0);                                                                                  /* 2760 */

    /* strings.h */
    int (* strcasecmp) (const char *s1, const char *s2);                                                                                             /* 2764 */
    int (* strncasecmp) (const char *s1, const char *s2, size_t len);                                                                                /* 2768 */
    int (* ffs) (int i);                                                                                                                             /* 2772 */
    int (* ffsl) (long i);                                                                                                                           /* 2776 */
    int (* ffsll) (long long i);                                                                                                                     /* 2780 */

    /* termcap.h */
    int (* tgetent) (char *buffer, const char *termtype);                                                                                            /* 2784 */
    int (* tgetnum) (const char *name);                                                                                                              /* 2788 */
    int (* tgetflag) (const char *name);                                                                                                             /* 2792 */
    char * (* tgetstr) (const char *name, char **area);                                                                                              /* 2796 */
    void (* tputs) (const char *str, int nlines, int (*outfun) (int));                                                                               /* 2800 */
    char * (* tparam) (const char *string, char *outstring, int leb, int arg0, int arg1, int arg2, int arg3);                                        /* 2804 */
    char * (* tgoto) (const char *cstring, int hpos, int vpos);                                                                                      /* 2808 */

    /* termios.h */
    int (* tcgetattr) (int file_descriptor, struct termios *tios_p);                                                                                 /* 2812 */
    int (* tcsetattr) (int file_descriptor, int action, struct termios *tios_p);                                                                     /* 2816 */
    int (* tcsendbreak) (int file_descriptor, int duration);                                                                                         /* 2820 */
    int (* tcdrain) (int file_descriptor);                                                                                                           /* 2824 */
    int (* tcflush) (int file_descriptor, int what);                                                                                                 /* 2828 */
    int (* tcflow) (int file_descriptor, int action);                                                                                                /* 2832 */
    int (* cfmakeraw) (struct termios *tios_p);                                                                                                      /* 2836 */
    speed_t (* cfgetispeed) (const struct termios *tios_p);                                                                                          /* 2840 */
    speed_t (* cfgetospeed) (const struct termios *tios_p);                                                                                          /* 2844 */
    int (* cfsetispeed) (struct termios *tios_p, speed_t input_speed);                                                                               /* 2848 */
    int (* cfsetospeed) (struct termios *tios_p, speed_t output_speed);                                                                              /* 2852 */

    /* time.h */
    clock_t (* clock) (void);                                                                                                                        /* 2856 */
    time_t (* time) (time_t *t);                                                                                                                     /* 2860 */
    char * (* asctime) (const struct tm *tm);                                                                                                        /* 2864 */
    char * (* ctime) (const time_t *t);                                                                                                              /* 2868 */
    struct tm * (* gmtime) (const time_t *t);                                                                                                        /* 2872 */
    struct tm * (* localtime) (const time_t *t);                                                                                                     /* 2876 */
    time_t (* mktime) (struct tm *tm);                                                                                                               /* 2880 */
    double (* difftime) (time_t t1, time_t t0);                                                                                                      /* 2884 */
    size_t (* strftime) (char *s, size_t maxsize, const char *format, const struct tm *tm);                                                          /* 2888 */
    char * (* strptime) (const char *buf, const char *fmt, struct tm *timeptr);                                                                      /* 2892 */
    char * (* asctime_r) (const struct tm *tm, char *buffer);                                                                                        /* 2896 */
    char * (* ctime_r) (const time_t *tptr, char *buffer);                                                                                           /* 2900 */
    struct tm * (* gmtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                   /* 2904 */
    struct tm * (* localtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                /* 2908 */
    void (* tzset) (void);                                                                                                                           /* 2912 */
    int (* nanosleep) (const struct timespec *req, struct timespec *rem);                                                                            /* 2916 */
    int (* clock_gettime) (clockid_t clk_id, struct timespec *t);                                                                                    /* 2920 */
    int (* clock_settime) (clockid_t clk_id, const struct timespec *t);                                                                              /* 2924 */
    int (* clock_getres) (clockid_t clock_id, struct timespec *res);                                                                                 /* 2928 */
    int (* clock_nanosleep) (clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);                                /* 2932 */
    unsigned long long (* rdtsc) (void);                                                                                                             /* 2936 */
    int (* clock_gettime64) (clockid_t clock_id, struct timespec64 *tp);                                                                             /* 2940 */

    /* uchar.h */
    size_t (* c16rtomb) (char *__restrict, char16_t, mbstate_t *__restrict);                                                                         /* 2944 */
    size_t (* mbrtoc16) (char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 2948 */
    size_t (* c32rtomb) (char *__restrict, char32_t, mbstate_t *__restrict);                                                                         /* 2952 */
    size_t (* mbrtoc32) (char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 2956 */

    /* ulimit.h */
    long (* ulimit) (int cmd,long newlimit);                                                                                                         /* 2960 */

    /* unistd.h */
    char *** (* __environ) (void);                                                                                                                   /* 2964 */
    int (* isatty) (int file_descriptor);                                                                                                            /* 2968 */
    int (* dup) (int file_descriptor);                                                                                                               /* 2972 */
    int (* dup2) (int file_descriptor1, int file_descriptor2);                                                                                       /* 2976 */
    off_t (* lseek) (int file_descriptor, off_t offset, int mode);                                                                                   /* 2980 */
    int (* access) (const char * path_name, int mode);                                                                                               /* 2984 */
    int (* chown) (const char * path_name, uid_t owner, gid_t group);                                                                                /* 2988 */
    int (* fchown) (int file_descriptor, uid_t owner, gid_t group);                                                                                  /* 2992 */
    int (* lchown) (const char * path_name, uid_t owner, gid_t group);                                                                               /* 2996 */
    int (* truncate) (const char * path_name, off_t length);                                                                                         /* 3000 */
    int (* ftruncate) (int file_descriptor, off_t length);                                                                                           /* 3004 */
    int (* getpagesize) (void);                                                                                                                      /* 3008 */
    int (* ftruncate64) (int file_descriptor, _off64_t length);                                                                                      /* 3012 */
    int (* truncate64) (const char *path_name, _off64_t length);                                                                                     /* 3016 */
    off64_t (* lseek64) (int fd, off64_t offset, int whence);                                                                                        /* 3020 */
    unsigned int (* alarm) (unsigned seconds);                                                                                                       /* 3024 */
    useconds_t (* ualarm) (useconds_t usecs, useconds_t interval);                                                                                   /* 3028 */
    int (* link) (const char * existing_path,const char * new_path);                                                                                 /* 3032 */
    int (* unlink) (const char * path_name);                                                                                                         /* 3036 */
    int (* symlink) (const char * actual_path, const char * symbolic_path);                                                                          /* 3040 */
    int (* readlink) (const char * path_name, char * buffer, int buffer_size);                                                                       /* 3044 */
    int (* chdir) (const char * path_name);                                                                                                          /* 3048 */
    int (* fchdir) (int file_descriptor);                                                                                                            /* 3052 */
    int (* lockf) (int file_descriptor, int function, off_t size);                                                                                   /* 3056 */
    unsigned int (* sleep) (unsigned int seconds);                                                                                                   /* 3060 */
    int (* usleep) (useconds_t microseconds);                                                                                                        /* 3064 */
    int (* pause) (void);                                                                                                                            /* 3068 */
    pid_t (* getpid) (void);                                                                                                                         /* 3072 */
    pid_t (* getppid) (void);                                                                                                                        /* 3076 */
    pid_t (* getpgrp) (void );                                                                                                                       /* 3080 */
    pid_t (* tcgetpgrp) (int fildes);                                                                                                                /* 3084 */
    int (* tcsetpgrp) (int filedes, pid_t pgrp);                                                                                                     /* 3088 */
    char * (* realpath) (const char *file_name, char *resolved_name);                                                                                /* 3092 */
    int (* fsync) (int file_descriptor);                                                                                                             /* 3096 */
    int (* fdatasync) (int file_descriptor);                                                                                                         /* 3100 */
    char * (* ttyname) (int);                                                                                                                        /* 3104 */
    int (* ttyname_r) (int file_descriptor,char *name,size_t buflen);                                                                                /* 3108 */
    int (* execl) (const char *path,const char *arg0,...);                                                                                           /* 3112 */
    int (* execle) (const char *path,const char *arg0,...);                                                                                          /* 3116 */
    int (* execlp) (const char *path,const char *arg0,...);                                                                                          /* 3120 */
    int (* execv) (const char *path,char * const argv[]);                                                                                            /* 3124 */
    int (* execve) (const char *path,char *const argv[],char *const envp[]);                                                                         /* 3128 */
    int (* execvp) (const char *command,char * const argv[]);                                                                                        /* 3132 */
    void (* encrypt) (char *block, int edflag);                                                                                                      /* 3136 */
    int (* spawnv) (int mode, const char *file, const char **argv);                                                                                  /* 3140 */
    int (* spawnvp) (int mode, const char *path, const char **argv);                                                                                 /* 3144 */
    long (* sysconf) (int name);                                                                                                                     /* 3148 */
    void (* enableUnixPaths) (void);                                                                                                                 /* 3152 */
    void (* disableUnixPaths) (void);                                                                                                                /* 3156 */
    void (* enableAltivec) (void);                                                                                                                   /* 3160 */
    void (* disableAltivec) (void);                                                                                                                  /* 3164 */
    void (* enableOptimizedFunctions) (void);                                                                                                        /* 3168 */
    void (* disableOptimizedFunctions) (void);                                                                                                       /* 3172 */
    char * (* getcwd) (char * buffer, size_t buffer_size);                                                                                           /* 3176 */
    char * (* getwd) (char *buf);                                                                                                                    /* 3180 */
    char * (* get_current_dir_name) (void);                                                                                                          /* 3184 */
    long (* gethostid) ();                                                                                                                           /* 3188 */
    int (* gethostname) (const char *name, size_t len);                                                                                              /* 3192 */
    int (* getdomainname) (char *name, size_t len);                                                                                                  /* 3196 */
    int (* setdomainname) (const char *name, size_t len);                                                                                            /* 3200 */
    char * (* getlogin) (void);                                                                                                                      /* 3204 */
    int (* getlogin_r) (char *buf, size_t bufsize);                                                                                                  /* 3208 */
    char * (* crypt) (const char *key, const char *salt);                                                                                            /* 3212 */
    gid_t (* getegid) (void);                                                                                                                        /* 3216 */
    uid_t (* geteuid) (void);                                                                                                                        /* 3220 */
    gid_t (* getgid) (void);                                                                                                                         /* 3224 */
    int (* getgroups) (int gidsetsize, gid_t grouplist[]);                                                                                           /* 3228 */
    char * (* getpass) (const char *prompt);                                                                                                         /* 3232 */
    uid_t (* getuid) (void);                                                                                                                         /* 3236 */
    int (* initgroups) (const char *name, gid_t basegid);                                                                                            /* 3240 */
    int (* setegid) (gid_t gid);                                                                                                                     /* 3244 */
    int (* seteuid) (uid_t uid);                                                                                                                     /* 3248 */
    int (* setgid) (gid_t gid);                                                                                                                      /* 3252 */
    int (* setgroups) (int ngroups, const gid_t *gidset);                                                                                            /* 3256 */
    int (* setregid) (gid_t rgid, gid_t egid);                                                                                                       /* 3260 */
    int (* setreuid) (uid_t ruid, uid_t euid);                                                                                                       /* 3264 */
    pid_t (* setsid) (void);                                                                                                                         /* 3268 */
    int (* setuid) (uid_t uid);                                                                                                                      /* 3272 */
    int (* setlogin) (const char *name);                                                                                                             /* 3276 */
    long (* pathconf) (const char *path,int name);                                                                                                   /* 3280 */
    long (* fpathconf) (int file_descriptor,int name);                                                                                               /* 3284 */
    int (* pipe) (int fd[2]);                                                                                                                        /* 3288 */
    int (* pipe2) (int fd[2], int flags);                                                                                                            /* 3292 */
    ssize_t (* pread) (int fd, void *buf, size_t nbytes, off_t offset);                                                                              /* 3296 */
    ssize_t (* pwrite) (int fd, const void *buf, size_t nbytes, off_t offset);                                                                       /* 3300 */
    ssize_t (* pread64) (int fd, void *buf, size_t nbytes, off64_t offset);                                                                          /* 3304 */
    ssize_t (* pwrite64) (int fd, const void *buf, size_t nbytes, off64_t offset);                                                                   /* 3308 */

    /* utime.h */
    int (* utime) (const char * path_name,const struct utimbuf * times);                                                                             /* 3312 */

    /* wchar.h */
    wint_t (* btowc) (int c);                                                                                                                        /* 3316 */
    int (* wctob) (wint_t c);                                                                                                                        /* 3320 */
    wchar_t * (* wcscat) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3324 */
    wchar_t * (* wcsncat) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3328 */
    int (* wcscmp) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3332 */
    int (* wcsncmp) (const wchar_t *s1, const wchar_t *s2, size_t n);                                                                                /* 3336 */
    wchar_t * (* wcscpy) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3340 */
    wchar_t * (* wcsncpy) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3344 */
    size_t (* wcslen) (const wchar_t *s);                                                                                                            /* 3348 */
    wchar_t * (* wcschr) (const wchar_t *s, wchar_t c);                                                                                              /* 3352 */
    size_t (* wcsspn) (const wchar_t *s, const wchar_t *set);                                                                                        /* 3356 */
    wchar_t * (* wcspbrk) (const wchar_t *s, const wchar_t *set);                                                                                    /* 3360 */
    wchar_t * (* wcstok) (wchar_t *str, const wchar_t *delim, wchar_t **ptr);                                                                        /* 3364 */
    wchar_t * (* wcsstr) (const wchar_t *big, const wchar_t *little);                                                                                /* 3368 */
    int (* wcswidth) (const wchar_t *pwcs, size_t n);                                                                                                /* 3372 */
    int (* wcwidth) (const wchar_t wc);                                                                                                              /* 3376 */
    double (* wcstod) (const wchar_t *str, wchar_t **ptr);                                                                                           /* 3380 */
    long (* wcstol) (const wchar_t *str, wchar_t **ptr, int base);                                                                                   /* 3384 */
    unsigned long (* wcstoul) (const wchar_t *str, wchar_t **ptr, int base);                                                                         /* 3388 */
    float (* wcstof) (const wchar_t *nptr, wchar_t **endptr);                                                                                        /* 3392 */
    int (* wscoll) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3396 */
    size_t (* wcsxfrm) (wchar_t *dest, const wchar_t *src, size_t len);                                                                              /* 3400 */
    wchar_t * (* wmemchr) (const wchar_t *ptr, wchar_t val, size_t len);                                                                             /* 3404 */
    int (* wmemcmp) (const wchar_t *ptr1, const wchar_t *ptr2, size_t len);                                                                          /* 3408 */
    wchar_t * (* wmemcpy) (wchar_t *dest, const wchar_t *src, size_t len);                                                                           /* 3412 */
    wchar_t * (* wmemmove) (wchar_t *dest, const wchar_t *src, size_t len);                                                                          /* 3416 */
    wchar_t * (* wmemset) (wchar_t *ptr, int val, size_t len);                                                                                       /* 3420 */
    int (* fwide) (FILE *stream, int orient);                                                                                                        /* 3424 */
    wint_t (* fgetwc) (FILE *stream);                                                                                                                /* 3428 */
    wint_t (* getwc) (FILE *stream);                                                                                                                 /* 3432 */
    wint_t (* getwchar) (void);                                                                                                                      /* 3436 */
    wint_t (* ungetwc) (wint_t wc, FILE *fp);                                                                                                        /* 3440 */
    wchar_t * (* fgetws) (wchar_t *s, int n, FILE *stream);                                                                                          /* 3444 */
    int (* fwscanf) (FILE *stream, const wchar_t *format, ...);                                                                                      /* 3448 */
    int (* swscanf) (const wchar_t *s, const wchar_t *format, ...);                                                                                  /* 3452 */
    int (* vfwscanf) (FILE *f, const wchar_t *format, va_list ap);                                                                                   /* 3456 */
    int (* vswscanf) (const wchar_t *s, const wchar_t *format, va_list ap);                                                                          /* 3460 */
    int (* vwscanf) (const wchar_t *s, va_list ap);                                                                                                  /* 3464 */
    int (* wscanf) (const wchar_t *format, ...);                                                                                                     /* 3468 */
    wint_t (* fputwc) (wchar_t c, FILE *stream);                                                                                                     /* 3472 */
    wint_t (* putwc) (wchar_t c, FILE *stream);                                                                                                      /* 3476 */
    wint_t (* putwchar) (wchar_t c);                                                                                                                 /* 3480 */
    int (* fputws) (const wchar_t *s, FILE *stream);                                                                                                 /* 3484 */
    int (* fwprintf) (FILE *stream, const wchar_t *format, ...);                                                                                     /* 3488 */
    int (* swprintf) (wchar_t *s, size_t l, const wchar_t *fmt, ...);                                                                                /* 3492 */
    int (* vfwprintf) (FILE *stream, const wchar_t *format, va_list arg);                                                                            /* 3496 */
    int (* vswprintf) (wchar_t *s, size_t maxlen, const wchar_t *format, va_list arg);                                                               /* 3500 */
    int (* vwprintf) (const wchar_t *format, va_list arg);                                                                                           /* 3504 */
    int (* wprintf) (const wchar_t *format, ...);                                                                                                    /* 3508 */
    size_t (* wcsftime) (wchar_t *s, size_t maxsize, const wchar_t *format, const struct tm *timeptr);                                               /* 3512 */
    size_t (* mbrlen) (const char *s, size_t n, mbstate_t *ps);                                                                                      /* 3516 */
    size_t (* mbrtowc) (wchar_t *pwc, const char *src, size_t n, mbstate_t *ps);                                                                     /* 3520 */
    int (* mbsinit) (const mbstate_t *ps);                                                                                                           /* 3524 */
    size_t (* mbsrtowcs) (wchar_t *dst, const char **src, size_t len, mbstate_t *ps);                                                                /* 3528 */
    size_t (* wcrtomb) (char *s, wchar_t wc, mbstate_t *ps);                                                                                         /* 3532 */
    int (* wcscoll) (const wchar_t *ws1, const wchar_t *ws2);                                                                                        /* 3536 */
    size_t (* wcscspn) (const wchar_t *s, const wchar_t *c);                                                                                         /* 3540 */
    wchar_t * (* wcsrchr) (const wchar_t *ws, wchar_t wc);                                                                                           /* 3544 */
    size_t (* wcsrtombs) (char *dst, const wchar_t **src, size_t len, mbstate_t *ps);                                                                /* 3548 */
    long long (* wcstoll) (const wchar_t *str, wchar_t **ptr, int base);                                                                             /* 3552 */
    unsigned long long (* wcstoull) (const wchar_t *str, wchar_t **ptr, int base);                                                                   /* 3556 */
    long double (* wcstold) (const wchar_t *nptr, wchar_t **endptr);                                                                                 /* 3560 */
    size_t (* mbsnrtowcs) (wchar_t *dst, const char **src, size_t nmc, size_t len, mbstate_t *ps);                                                   /* 3564 */
    size_t (* wcsnrtombs) (char *dst, const wchar_t **src, size_t nwc, size_t len, mbstate_t *ps);                                                   /* 3568 */
    wchar_t * (* wcsdup) (const wchar_t *src);                                                                                                       /* 3572 */
    size_t (* wcsnlen) (const wchar_t *src, size_t n);                                                                                               /* 3576 */
    wchar_t * (* wcpcpy) (wchar_t *dst, const wchar_t *src);                                                                                         /* 3580 */
    wchar_t * (* wcpncpy) (wchar_t *dst, const wchar_t *src, size_t len);                                                                            /* 3584 */
    int (* wcscasecmp) (const wchar_t *l, const wchar_t *r);                                                                                         /* 3588 */
    int (* wcscasecmp_l) (const wchar_t *l, const wchar_t *r, locale_t loc);                                                                         /* 3592 */
    int (* wcsncasecmp) (const wchar_t *l, const wchar_t *r, size_t n);                                                                              /* 3596 */
    int (* wcsncasecmp_l) (const wchar_t *l, const wchar_t *r, size_t n, locale_t loc);                                                              /* 3600 */
    int (* wcscoll_l) (const wchar_t *ws1, const wchar_t *ws2, locale_t loc);                                                                        /* 3604 */
    size_t (* wcsxfrm_l) (wchar_t *dest, const wchar_t *src, size_t len, locale_t loc);                                                              /* 3608 */

    /* wctype.h */
    int (* iswalnum) (wint_t c);                                                                                                                     /* 3612 */
    int (* iswalpha) (wint_t c);                                                                                                                     /* 3616 */
    int (* iswcntrl) (wint_t c);                                                                                                                     /* 3620 */
    int (* iswdigit) (wint_t c);                                                                                                                     /* 3624 */
    int (* iswxdigit) (wint_t c);                                                                                                                    /* 3628 */
    int (* iswgraph) (wint_t c);                                                                                                                     /* 3632 */
    int (* iswpunct) (wint_t c);                                                                                                                     /* 3636 */
    int (* iswprint) (wint_t c);                                                                                                                     /* 3640 */
    int (* iswlower) (wint_t c);                                                                                                                     /* 3644 */
    int (* iswupper) (wint_t c);                                                                                                                     /* 3648 */
    int (* iswspace) (wint_t c);                                                                                                                     /* 3652 */
    int (* iswblank) (wint_t c);                                                                                                                     /* 3656 */
    wint_t (* towlower) (wint_t c);                                                                                                                  /* 3660 */
    wint_t (* towupper) (wint_t c);                                                                                                                  /* 3664 */
    wctype_t (* wctype) (const char *property);                                                                                                      /* 3668 */
    int (* iswctype) (wint_t c, wctype_t desc);                                                                                                      /* 3672 */
    wctrans_t (* wctrans) (const char *property);                                                                                                    /* 3676 */
    wint_t (* towctrans) (wint_t c, wctrans_t desc);                                                                                                 /* 3680 */

    /* arpa/inet.h */
    in_addr_t (* inet_addr) (const char *cp);                                                                                                        /* 3684 */
    int (* inet_aton) (const char *cp, struct in_addr *pin);                                                                                         /* 3688 */
    in_addr_t (* inet_lnaof) (struct in_addr in);                                                                                                    /* 3692 */
    struct in_addr (* inet_makeaddr) (in_addr_t net, in_addr_t lna);                                                                                 /* 3696 */
    in_addr_t (* inet_netof) (struct in_addr in);                                                                                                    /* 3700 */
    in_addr_t (* inet_network) (const char *cp);                                                                                                     /* 3704 */
    char * (* inet_ntoa) (struct in_addr in);                                                                                                        /* 3708 */
    const char * (* inet_ntop) (int af, const void *src, char *dst, socklen_t cnt);                                                                  /* 3712 */
    int (* inet_pton) (int af, const char *src, void *dst);                                                                                          /* 3716 */

    /* arpa/nameser.h */
    unsigned (* ns_get16) (const unsigned char *);                                                                                                   /* 3720 */
    unsigned long (* ns_get32) (const unsigned char *);                                                                                              /* 3724 */
    void (* ns_put16) (unsigned, unsigned char *);                                                                                                   /* 3728 */
    void (* ns_put32) (unsigned long, unsigned char *);                                                                                              /* 3732 */
    int (* ns_initparse) (const unsigned char *, int, ns_msg *);                                                                                     /* 3736 */
    int (* ns_parserr) (ns_msg *, ns_sect, int, ns_rr *);                                                                                            /* 3740 */
    int (* ns_skiprr) (const unsigned char *, const unsigned char *, ns_sect, int);                                                                  /* 3744 */
    int (* ns_name_uncompress) (const unsigned char *, const unsigned char *, const unsigned char *, char *, size_t);                                /* 3748 */

    /* net/if.h */
    unsigned int (* if_nametoindex) (const char *ifname);                                                                                            /* 3752 */
    char * (* if_indextoname) (unsigned int ifindex, char *ifname);                                                                                  /* 3756 */

    /* netinet/in.h */
    int (* bindresvport) (int sd, struct sockaddr_in *sa);                                                                                           /* 3760 */
    int (* bindresvport6) (int sd, struct sockaddr_in6 *sa);                                                                                         /* 3764 */

    /* sys/byteswap.h */
    __CONST_FUNC uint16_t (* bswap16) (uint16_t);                                                                                                    /* 3768 */
    __CONST_FUNC uint32_t (* bswap24) (uint32_t);                                                                                                    /* 3772 */
    __CONST_FUNC uint32_t (* bswap32) (uint32_t);                                                                                                    /* 3776 */
    __CONST_FUNC uint64_t (* bswap64) (uint64_t);                                                                                                    /* 3780 */
    void * (* swab) (void *from,void *to,ssize_t nbytes);                                                                                            /* 3784 */
    void * (* swab24) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 24-bit words instead. */                              /* 3788 */
    void * (* swab32) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 32-bit words instead. */                              /* 3792 */
    void * (* swab64) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 64-bit words instead. */                              /* 3796 */

    /* sys/file.h */
    int (* flock) (int fd, int op);                                                                                                                  /* 3800 */

    /* sys/ioctl.h */
    int (* ioctl) (int fildes, int request, ... /* arg */);                                                                                          /* 3804 */

    /* sys/ipc.h */
    key_t (* ftok) (const char *path, int id);                                                                                                       /* 3808 */

    /* sys/mman.h */
    void * (* mmap) (void *addr, size_t len, int prot, int flags, int fd, off_t offset);                                                             /* 3812 */
    int (* munmap) (void *map, size_t length);                                                                                                       /* 3816 */
    int (* msync) (void *addr, size_t len, int flags);                                                                                               /* 3820 */

    /* sys/mount.h */
    int (* statfs) (const char *path, struct statfs *buf);                                                                                           /* 3824 */
    int (* fstatfs) (int fd, struct statfs *buf);                                                                                                    /* 3828 */

    /* sys/msg.h */
    int (* _msgctl) (int msqid, int cmd, struct msqid_ds *buf);                                                                                      /* 3832 */
    int (* _msgget) (key_t key, int msgflg);                                                                                                         /* 3836 */
    ssize_t (* _msgrcv) (int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);                                                              /* 3840 */
    int (* _msgsnd) (int msqid, const void *msgp, size_t msgsz, int msgflg);                                                                         /* 3844 */
    int (* _msgsnap) (int msqid, void *buf, size_t bufsz, long msgtyp);                                                                              /* 3848 */
    int (* _msgids) (int *buf, size_t nids, size_t  *pnids);                                                                                         /* 3852 */

    /* sys/resource.h */
    int (* getrlimit) (int resource, struct rlimit *rlp);                                                                                            /* 3856 */
    int (* setrlimit) (int resource, const struct rlimit *rlp);                                                                                      /* 3860 */
    int (* getrusage) (int who, struct rusage *usage);                                                                                               /* 3864 */

    /* sys/select.h */
    int (* select) (int nfds, fd_set *readfds,fd_set *writefds, fd_set *errorfds,struct timeval *timeout);                                           /* 3868 */
    int (* waitselect) (int num_fds,fd_set *read_fds,fd_set *write_fds,fd_set *except_fds,struct timeval *timeout,unsigned long * signal_mask);      /* 3872 */

    /* sys/sem.h */
    int (* _semctl) (int semid, int semnum, int cmd, union semun aun);                                                                               /* 3876 */
    int (* _semget) (key_t key, int nsems, int flags);                                                                                               /* 3880 */
    int (* _semop) (int semid, const struct sembuf *ops, int nops);                                                                                  /* 3884 */
    long int (* _semids) (long int *buf, uint32_t nids, long unsigned int *idcnt);                                                                   /* 3888 */
    int (* _semtimedop) (int semid, const struct sembuf *ops, int nops, struct timespec *to);                                                        /* 3892 */

    /* sys/shm.h */
    void * (* _shmat) (int shmid, const void *shmaddr, int shmflg);                                                                                  /* 3896 */
    int (* _shmctl) (int shmid, int cmd, struct shmid_ds *buf);                                                                                      /* 3900 */
    int (* _shmdt) (const void *shmaddr);                                                                                                            /* 3904 */
    int (* _shmget) (key_t key, size_t size, int shmflg);                                                                                            /* 3908 */
    int (* _shmids) (int *buf, size_t nids, size_t *pnids);                                                                                          /* 3912 */

    /* sys/socket.h */
    int (* accept) (int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);                                                                         /* 3916 */
    int (* bind) (int socket, const struct sockaddr *address, socklen_t address_len);                                                                /* 3920 */
    int (* connect) (int socket, const struct sockaddr *address, socklen_t address_len);                                                             /* 3924 */
    int (* getpeername) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 3928 */
    int (* getsockname) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 3932 */
    int (* getsockopt) (int socket, int level, int option_name, void *option_value, socklen_t *option_len);                                          /* 3936 */
    int (* listen) (int socket, int backlog);                                                                                                        /* 3940 */
    ssize_t (* recv) (int socket, void *buffer, size_t length, int flags);                                                                           /* 3944 */
    ssize_t (* recvfrom) (int socket, void *buffer, size_t length,int flags, struct sockaddr *address,socklen_t *address_len);                       /* 3948 */
    ssize_t (* recvmsg) (int socket, struct msghdr *message, int flags);                                                                             /* 3952 */
    ssize_t (* send) (int socket, const void *buffer, size_t length, int flags);                                                                     /* 3956 */
    ssize_t (* sendmsg) (int socket, const struct msghdr *message, int flags);                                                                       /* 3960 */
    ssize_t (* sendto) (int socket, const void *message, size_t length,int flags, const struct sockaddr *dest_addr,socklen_t dest_len);              /* 3964 */
    int (* setsockopt) (int socket, int level, int option_name, const void *option_value, socklen_t option_len);                                     /* 3968 */
    int (* shutdown) (int socket, int how);                                                                                                          /* 3972 */
    int (* socket) (int domain, int type, int protocol);                                                                                             /* 3976 */
    int (* socketpair) (int domain, int type, int protocol, int fds[2]);                                                                             /* 3980 */

    /* sys/stat.h */
    int (* stat) (const char * path_name, struct stat * buffer);                                                                                     /* 3984 */
    int (* fstat) (int file_descriptor, struct stat * buffer);                                                                                       /* 3988 */
    int (* lstat) (const char * path_name, struct stat * buffer);                                                                                    /* 3992 */
    int (* chmod) (const char * path_name, mode_t mode);                                                                                             /* 3996 */
    int (* fchmod) (int file_descriptor, mode_t mode);                                                                                               /* 4000 */
    int (* mkdir) (const char * path_name, mode_t mode);                                                                                             /* 4004 */
    int (* rmdir) (const char * path_name);                                                                                                          /* 4008 */
    mode_t (* umask) (mode_t new_mask);                                                                                                              /* 4012 */

    /* sys/statvfs.h */
    int (* statvfs) (const char *, struct statvfs *);                                                                                                /* 4016 */
    int (* fstatvfs) (int, struct statvfs *);                                                                                                        /* 4020 */

    /* sys/syslog.h */
    void (* closelog) (void);                                                                                                                        /* 4024 */
    void (* openlog) (const char *ident, int opt, int facility);                                                                                     /* 4028 */
    int (* setlogmask) (int maskpri);                                                                                                                /* 4032 */
    void (* syslog) (int priority, const char *message, ...);                                                                                        /* 4036 */
    void (* vsyslog) (int priority, const char *message, va_list args);                                                                              /* 4040 */

    /* sys/systeminfo.h */
    long (* sysinfo) (int cmd,char *buf,long buflen);                                                                                                /* 4044 */

    /* sys/time.h */
    int (* gettimeofday) (struct timeval *tp, struct timezone *tzp);                                                                                 /* 4048 */
    int (* settimeofday) (const struct timeval *, const struct timezone *);                                                                          /* 4052 */
    int (* utimes) (const char *path, const struct timeval *tvp);                                                                                    /* 4056 */
    int (* getitimer) (int which, struct itimerval *curr_value);                                                                                     /* 4060 */
    int (* setitimer) (int which, const struct itimerval *new_value, struct itimerval *old_value);                                                   /* 4064 */

    /* sys/timeb.h */
    int (* ftime) (struct timeb *);                                                                                                                  /* 4068 */

    /* sys/times.h */
    clock_t (* times) (struct tms *tp);                                                                                                              /* 4072 */

    /* sys/uio.h */
    ssize_t (* readv) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                   /* 4076 */
    ssize_t (* writev) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                  /* 4080 */

    /* sys/utsname.h */
    int (* uname) (struct utsname *);                                                                                                                /* 4084 */

    /* END OF CLIB4 VERSION 1.0   */
    /* New function will go below */

    int (* futimens) (int fd, const struct timespec times[2]);                                                                                       /* 4098 */
    int (* utimensat) (int fd, const char *path, const struct timespec times[2], int flags);                                                         /* 4092 */
    int (* __isnan) (double d);                                                                                                                      /* 4096 */
    int (* __isnanf) (float f);                                                                                                                      /* 4100 */
    int (* __isnanl) (long double d);                                                                                                                /* 4104 */
    int (* __isinf) (double d);                                                                                                                      /* 4108 */
    int (* __isinff) (float f);                                                                                                                      /* 4112 */
    int (* __isinfl) (long double d);                                                                                                                /* 4116 */
    int (* __isnormal) (double d);                                                                                                                   /* 4120 */
    int (* __isnormalf) (float f);                                                                                                                   /* 4124 */
    int (* __isnormall) (long double d);                                                                                                             /* 4128 */
    int (* __isfinite_double) (double f);                                                                                                            /* 4132 */
    int (* __isfinite_float) (float d);                                                                                                              /* 4136 */
    int (* __isfinite_long_double) (long double d);                                                                                                  /* 4140 */
    u_quad_t (* strtouq) (const char *nptr, char **endptr, int base);                                                                                /* 4144 */

    char * (* gettext) (const char *);                                                                                                               /* 4148 */
    char * (* dgettext) (const char *, const char *);                                                                                                /* 4152 */
    char * (* dcgettext) (const char *, const char *, int);                                                                                          /* 4156 */
    char * (* ngettext) (const char *, const char *, unsigned long);                                                                                 /* 4162 */
    char * (* dngettext) (const char *, const char *, const char *, unsigned long);                                                                  /* 4164 */
    char * (* dcngettext) (const char *, const char *, const char *, unsigned long, int);                                                            /* 4168 */
    char * (* textdomain) (const char *);                                                                                                            /* 4172 */
    char * (* bindtextdomain) (const char *, const char *);                                                                                          /* 4176 */
    char * (* bind_textdomain_codeset) (const char *, const char *);                                                                                 /* 4180 */

    char * (* ether_ntoa) (const struct ether_addr *);                                                                                               /* 4184 */
    struct ether_addr * (* ether_aton) (const char *);                                                                                               /* 4188 */
    char * (* ether_ntoa_r) (const struct ether_addr *, char *);                                                                                     /* 4192 */
    struct ether_addr * (* ether_aton_r) (const char *, struct ether_addr *);                                                                        /* 4196 */
    int  (* ether_line) (const char *, struct ether_addr *, char *);                                                                                 /* 4200 */
    int  (* ether_ntohost) (char *, const struct ether_addr *);                                                                                      /* 4204 */
    int  (* ether_hostton) (const char *, struct ether_addr *);                                                                                      /* 4208 */

    int  (* getrandom) (void *buffer, size_t length, unsigned int flags);                                                                            /* 4212 */
    int  (* getentropy) (void *, size_t);                                                                                                            /* 4216 */

    time_t (* timegm) (struct tm *tm);                                                                                                               /* 4220 */
    int    (* stime) (const time_t *t);                                                                                                              /* 4224 */

    int	  (* dbm_clearerr) (DBM *);                                                                                                                  /* 4228 */
    void  (* dbm_close) (DBM *);                                                                                                                     /* 4232 */
    int	  (* dbm_delete) (DBM *, datum);                                                                                                             /* 4236 */
    int	  (* dbm_error) (DBM *);                                                                                                                     /* 4240 */
    datum (* dbm_fetch) (DBM *, datum);                                                                                                              /* 4244 */
    datum (* dbm_firstkey) (DBM *);                                                                                                                  /* 4248 */
    datum (* dbm_nextkey) (DBM *);                                                                                                                   /* 4252 */
    DBM	* (* dbm_open) (const char *, int, int);                                                                                                     /* 4256 */
    int	  (* dbm_store) (DBM *, datum, datum, int);                                                                                                  /* 4260 */
    int   (* dbm_dirfno) (DBM *);                                                                                                                    /* 4264 */
    DB *  (* dbopen) (const char *, int, int, DBTYPE, const void *);                                                                                 /* 4268 */

    int   (* mkostemp) (char *name, int flags);                                                                                                      /* 4272 */
    int   (* mkostemps) (char *name, int len, int flags);                                                                                            /* 4276 */

    float (* strtof_l) (const char *__restrict s00, char **__restrict se, locale_t loc);                                                             /* 4280 */
    double (* strtod_l) (const char *__restrict s00, char **__restrict se, locale_t loc);                                                            /* 4284 */

    fp_rnd_t    (* fpgetround) (void);                                                                                                               /* 4288 */
    fp_rnd_t    (* fpsetround) (fp_rnd_t);                                                                                                           /* 4292 */
    fp_except_t (* fpgetmask) (void);                                                                                                                /* 4296 */
    fp_except_t (* fpsetmask) (fp_except_t);                                                                                                         /* 4300 */
    fp_except_t (* fpgetsticky) (void);                                                                                                              /* 4304 */

    FTSENT * (* fts_children) (FTS *, int);                                                                                                          /* 4308 */
    int	     (* fts_close) (FTS *);                                                                                                                  /* 4312 */
    FTS	   * (* fts_open) (char * const *, int, int (*)(const FTSENT **, const FTSENT **));                                                          /* 4316 */
    FTSENT * (* fts_read) (FTS *);                                                                                                                   /* 4320 */
    int	     (* fts_set) (FTS *, FTSENT *, int);                                                                                                     /* 4324 */

    FILE * (* __stdin) (void);                                                                                                                       /* 4328 */
    FILE * (* __stdout) (void);                                                                                                                      /* 4332 */
    FILE * (* __stderr) (void);                                                                                                                      /* 4336 */

    int (* __cxa_atexit) (void (*func) (void *), void *arg, void *d);                                                                                /* 4340 */
    void (* __cxa_finalize) (void *d);                                                                                                               /* 4344 */

    int (* __get_timezone) (void);                                                                                                                   /* 4348 */
    int (*__get_daylight) (void);                                                                                                                    /* 4352 */
    char ** (* __get_tzname) (void);                                                                                                                 /* 4356 */

    pid_t (* wait) (int *status);                                                                                                                    /* 4360 */
    pid_t (* waitpid) (pid_t pid, int *status, int options);                                                                                         /* 4364 */
    int (* spawnvpe) (const char *file, const char **argv, char **deltaenv, const char *dir, int fhin, int fhout, int fherr);                        /* 4368 */

    int (* waitpoll) (struct pollfd *fds, nfds_t nfds, int timeout, uint32 *signals);                                                                /* 4372 */

    char (* __get_tc_pc) (void);                                                                                                                     /* 4376 */
    short (* __get_tc_ospeed) (void);                                                                                                                /* 4380 */
    char * (* __get_tc_up) (void);                                                                                                                   /* 4384 */
    char * (* __get_tc_bc) (void);                                                                                                                   /* 4388 */

    int ( *spawnvpe_callback_UNUSED) (const char *file, const char **argv, char **deltaenv, const char *dir, int fhin, int fhout, int fherr, void (*entry_fp)(void *), void* entry_data, void (*final_fp)(int, void *), void* final_data);     /* 4392 */

    int (* sigsuspend) (const sigset_t *mask);		                                                                              					 /* 4396 */
    int (* spawnve) (int mode, const char *path, const char **argv, char * const envp[]);                                                            /* 4400 */
    int (* sigpause) (int sig_or_mask, int is_sig);                                                                                                  /* 4404 */
    int (* sigwait) (const sigset_t *set, int *sig);                                                                                                 /* 4408 */
	int (* sigwaitinfo) (const sigset_t *set, siginfo_t *info);																						 /* 4412 */
	int (* sigtimedwait) (const sigset_t *set, siginfo_t *info, const struct timespec *timeout);													 /* 4416 */

    int (* getvfsstat) (struct statvfs *buf, size_t bufsize, int flags);                                                                             /* 4420 */
	int (* backtrace) (void **buffer, int max_frames);																						 	 	 /* 4424 */
	char ** (* backtrace_symbols) (void *const *buffer, int size);																					 /* 4428 */
	void (* backtrace_symbols_fd) (void *const *buffer, int size, int fd);    																		 /* 4432 */
	int (* dladdr) (const void *addr, Dl_info *info);																								 /* 4436 */

    int (* getpriority) (int which, unsigned int who);																								 /* 4440 */
    int (* setpriority) (int which, unsigned int who, int priority);																				 /* 4444 */

};

#ifdef __PIC__
#define Clib4Call2(function, offset)     \
   asm(".section	\".text\"        \n\
	    .align 8                     \n\
	    .globl " #function "         \n\
	    .type	" #function ", @function \n\
" #function ":                       \n\
	    li  %r12," #offset "         \n\
	    b __Clib4Call@plt            \n\
	    .size	" #function ", .-" #function)
#elif !defined(__PIC__)
#define Clib4Call2(function, offset)     \
   asm(".section	\".text\"        \n\
	    .align 8                     \n\
	    .globl " #function "         \n\
	    .type	" #function ", @function \n\
" #function ":                       \n\
	    li  %r12," #offset "         \n\
	    b __Clib4Call                \n\
	    .size	" #function ", .-" #function)
#endif

#define Clib4Call(x...) Clib4Call2(x)

#endif
