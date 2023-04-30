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
    int  (* library_start)(char *argstr, int arglen, int (* start_main)(int, char **), void (*__CTOR_LIST__[])(void), void (*__DTOR_LIST__[])(void)); //80
    void (* __getClib2)(void);                                      //84
    void (* __translate_amiga_to_unix_path_name)(void);             //88
    void (* __translate_unix_to_amiga_path_name)(void);             //92
    void (* __restore_path_name)(void);                             //96
    void (* __getGlobalClib2)(void);                                //100
    void (* internal2)(void);                                       //104
    void (* internal3)(void);                                       //108
    void (* internal4)(void);                                       //112
    void (* internal5)(void);                                       //116

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
    int (* __isfinite_float) (float x);                                                                                                              /* 940 */
    int (* __isfinite_double) (double x);                                                                                                            /* 944 */
    int (* __isfinite_long_double) (long double x);                                                                                                  /* 948 */
    int (* __signbit_float) (float x);                                                                                                               /* 952 */
    int (* __signbit_double) (double x);                                                                                                             /* 956 */
    int (* __signbit_long_double) (long double x);                                                                                                   /* 960 */
    int (* __isnan) (double x);                                                                                                                      /* 964 */
    int (* __isnanf) (float x);                                                                                                                      /* 968 */
    int (* __isnanl) (long double x);                                                                                                                /* 972 */
    int (* __isinf) (double x);                                                                                                                      /* 976 */
    int (* __isinff) (float x);                                                                                                                      /* 980 */
    int (* __isinfl) (long double x);                                                                                                                /* 984 */
    int (* __isnormalf) (float x);                                                                                                                   /* 988 */
    int (* __isnormal) (double x);                                                                                                                   /* 992 */
    int (* __isnormall) (long double x);                                                                                                             /* 996 */
    float (* acosf) (float x);                                                                                                                       /* 1000 */
    float (* asinf) (float x);                                                                                                                       /* 1004 */
    float (* atanf) (float x);                                                                                                                       /* 1008 */
    float (* atan2f) (float y, float x);                                                                                                             /* 1012 */
    float (* ceilf) (float x);                                                                                                                       /* 1016 */
    float (* cosf) (float x);                                                                                                                        /* 1020 */
    float (* coshf) (float x);                                                                                                                       /* 1024 */
    float (* expf) (float x);                                                                                                                        /* 1028 */
    float (* fabsf) (float x);                                                                                                                       /* 1032 */
    float (* floorf) (float x);                                                                                                                      /* 1036 */
    float (* fmodf) (float x, float y);                                                                                                              /* 1040 */
    float (* frexpf) (float x, int *eptr);                                                                                                           /* 1044 */
    float (* ldexpf) (float x, int exp);                                                                                                             /* 1048 */
    float (* logf) (float x);                                                                                                                        /* 1052 */
    float (* log10f) (float x);                                                                                                                      /* 1056 */
    float (* modff) (float x, float *iptr);                                                                                                          /* 1060 */
    float (* powf) (float x, float y);                                                                                                               /* 1064 */
    float (* sinf) (float x);                                                                                                                        /* 1068 */
    float (* sinhf) (float x);                                                                                                                       /* 1072 */
    float (* sqrtf) (float x);                                                                                                                       /* 1076 */
    float (* tanf) (float x);                                                                                                                        /* 1080 */
    float (* tanhf) (float x);                                                                                                                       /* 1084 */
    float (* acoshf) (float x);                                                                                                                      /* 1088 */
    float (* asinhf) (float x);                                                                                                                      /* 1092 */
    float (* atanhf) (float x);                                                                                                                      /* 1096 */
    float (* cbrtf) (float x);                                                                                                                       /* 1100 */
    float (* copysignf) (float x, float y);                                                                                                          /* 1104 */
    float (* erfcf) (float x);                                                                                                                       /* 1108 */
    float (* erff) (float x);                                                                                                                        /* 1112 */
    float (* expm1f) (float x);                                                                                                                      /* 1116 */
    float (* fdimf) (float x, float y);                                                                                                              /* 1120 */
    float (* fmaf) (float x, float y, float z);                                                                                                      /* 1124 */
    float (* fmaxf) (float x, float y);                                                                                                              /* 1128 */
    float (* fminf) (float x, float y);                                                                                                              /* 1132 */
    float (* hypotf) (float x, float y);                                                                                                             /* 1136 */
    float (* lgammaf) (float x);                                                                                                                     /* 1140 */
    float (* log1pf) (float x);                                                                                                                      /* 1144 */
    float (* logbf) (float x);                                                                                                                       /* 1148 */
    long long (* llrintf) (float x);                                                                                                                 /* 1152 */
    long (* lrintf) (float x);                                                                                                                       /* 1156 */
    long (* lroundf) (float x);                                                                                                                      /* 1160 */
    long long (* llround) (double x);                                                                                                                /* 1164 */
    long long (* llroundf) (float x);                                                                                                                /* 1168 */
    float (* nanf) (const char *tagp);                                                                                                               /* 1172 */
    float (* nearbyintf) (float x);                                                                                                                  /* 1176 */
    float (* nextafterf) (float x, float y);                                                                                                         /* 1180 */
    float (* nexttowardf) (float x, long double y);                                                                                                  /* 1184 */
    float (* remainderf) (float x, float p);                                                                                                         /* 1188 */
    float (* remquof) (float x, float y, int *quo);                                                                                                  /* 1192 */
    float (* rintf) (float x);                                                                                                                       /* 1196 */
    float (* roundf) (float x);                                                                                                                      /* 1200 */
    float (* scalblnf) (float x, long n);                                                                                                            /* 1204 */
    float (* scalbnf) (float x, int n);                                                                                                              /* 1208 */
    float (* tgammaf) (float x);                                                                                                                     /* 1212 */
    float (* truncf) (float x);                                                                                                                      /* 1216 */
    int (* ilogbf) (float x);                                                                                                                        /* 1220 */
    int (* finite) (double x);                                                                                                                       /* 1224 */
    double (* acosh) (double x);                                                                                                                     /* 1228 */
    double (* asinh) (double x);                                                                                                                     /* 1232 */
    double (* atanh) (double x);                                                                                                                     /* 1236 */
    double (* cbrt) (double x);                                                                                                                      /* 1240 */
    double (* copysign) (double x, double y);                                                                                                        /* 1244 */
    double (* erf) (double x);                                                                                                                       /* 1248 */
    double (* erfc) (double x);                                                                                                                      /* 1252 */
    double (* expm1) (double x);                                                                                                                     /* 1256 */
    double (* fdim) (double x, double y);                                                                                                            /* 1260 */
    double (* fma) (double x, double y, double z);                                                                                                   /* 1264 */
    double (* fmax) (double x, double y);                                                                                                            /* 1268 */
    double (* fmin) (double x, double y);                                                                                                            /* 1272 */
    double (* hypot) (double x, double y);                                                                                                           /* 1276 */
    double (* lgamma) (double x);                                                                                                                    /* 1280 */
    double (* log1p) (double x);                                                                                                                     /* 1284 */
    double (* logb) (double x);                                                                                                                      /* 1288 */
    long long (* llrint) (double x);                                                                                                                 /* 1292 */
    long int (* lrint) (double x);                                                                                                                   /* 1296 */
    long int (* lround) (double x);                                                                                                                  /* 1300 */
    double (* nan) (const char *tagp);                                                                                                               /* 1304 */
    double (* nearbyint) (double x);                                                                                                                 /* 1308 */
    double (* nexttoward) (double x, long double y);                                                                                                 /* 1312 */
    double (* nextafter) (double x, double y);                                                                                                       /* 1316 */
    double (* remainder) (double x, double p);                                                                                                       /* 1320 */
    double (* remquo) (double x, double y, int *quo);                                                                                                /* 1324 */
    double (* rint) (double x);                                                                                                                      /* 1328 */
    double (* round) (double x);                                                                                                                     /* 1332 */
    double (* scalbln) (double x, long n);                                                                                                           /* 1336 */
    double (* scalbn) (double x, int n);                                                                                                             /* 1340 */
    double (* tgamma) (double x);                                                                                                                    /* 1344 */
    double (* trunc) (double x);                                                                                                                     /* 1348 */
    int (* ilogb) (double x);                                                                                                                        /* 1352 */
    float (* exp2f) (float x);                                                                                                                       /* 1356 */
    double (* exp2) (double x);                                                                                                                      /* 1360 */
    long double (* exp2l) (long double x);                                                                                                           /* 1364 */
    double (* log2) (double x);                                                                                                                      /* 1368 */
    float (* log2f) (float x);                                                                                                                       /* 1372 */
    long double (* log2l) (long double x);                                                                                                           /* 1376 */
    long double (* acosl) (long double x);                                                                                                           /* 1380 */
    long double (* asinl) (long double x);                                                                                                           /* 1384 */
    long double (* atanl) (long double x);                                                                                                           /* 1388 */
    long double (* atan2l) (long double y, long double x);                                                                                           /* 1392 */
    long double (* ceill) (long double x);                                                                                                           /* 1396 */
    long double (* cosl) (long double x);                                                                                                            /* 1400 */
    long double (* coshl) (long double x);                                                                                                           /* 1404 */
    long double (* expl) (long double x);                                                                                                            /* 1408 */
    long double (* fabsl) (long double x);                                                                                                           /* 1412 */
    long double (* floorl) (long double x);                                                                                                          /* 1416 */
    long double (* fmodl) (long double x, long double y);                                                                                            /* 1420 */
    long double (* frexpl) (long double x, int *nptr);                                                                                               /* 1424 */
    long double (* ldexpl) (long double x, int n);                                                                                                   /* 1428 */
    long double (* logl) (long double x);                                                                                                            /* 1432 */
    long double (* log10l) (long double x);                                                                                                          /* 1436 */
    long double (* modfl) (long double x, long double *iptr);                                                                                        /* 1440 */
    long double (* powl) (long double x, long double y);                                                                                             /* 1444 */
    long double (* sinl) (long double x);                                                                                                            /* 1448 */
    long double (* sinhl) (long double x);                                                                                                           /* 1452 */
    long double (* sqrtl) (long double x);                                                                                                           /* 1456 */
    long double (* tanl) (long double x);                                                                                                            /* 1460 */
    long double (* tanhl) (long double x);                                                                                                           /* 1464 */
    long double (* acoshl) (long double x);                                                                                                          /* 1468 */
    long double (* asinhl) (long double x);                                                                                                          /* 1472 */
    long double (* atanhl) (long double x);                                                                                                          /* 1476 */
    long double (* cbrtl) (long double x);                                                                                                           /* 1480 */
    long double (* copysignl) (long double x, long double y);                                                                                        /* 1484 */
    long double (* erfcl) (long double x);                                                                                                           /* 1488 */
    long double (* erfl) (long double x);                                                                                                            /* 1492 */
    long double (* expm1l) (long double x);                                                                                                          /* 1496 */
    long double (* fdiml) (long double x, long double y);                                                                                            /* 1500 */
    long double (* fmal) (long double x, long double y, long double z);                                                                              /* 1504 */
    long double (* fmaxl) (long double x, long double y);                                                                                            /* 1508 */
    long double (* fminl) (long double x, long double y);                                                                                            /* 1512 */
    long double (* hypotl) (long double x, long double y);                                                                                           /* 1516 */
    long double (* lgammal) (long double x);                                                                                                         /* 1520 */
    long double (* log1pl) (long double x);                                                                                                          /* 1524 */
    long double (* logbl) (long double x);                                                                                                           /* 1528 */
    long long int (* llrintl) (long double x);                                                                                                       /* 1532 */
    long int (* lrintl) (long double x);                                                                                                             /* 1536 */
    long long int (* llroundl) (long double x);                                                                                                      /* 1540 */
    long int (* lroundl) (long double x);                                                                                                            /* 1544 */
    long double (* nanl) (const char *tagp);                                                                                                         /* 1548 */
    long double (* nearbyintl) (long double x);                                                                                                      /* 1552 */
    long double (* nextafterl) (long double x, long double y);                                                                                       /* 1556 */
    long double (* nexttowardl) (long double x, long double y);                                                                                      /* 1560 */
    long double (* remainderl) (long double x, long double p);                                                                                       /* 1564 */
    long double (* remquol) (long double x, long double y, int *quo);                                                                                /* 1568 */
    long double (* rintl) (long double x);                                                                                                           /* 1572 */
    long double (* roundl) (long double x);                                                                                                          /* 1576 */
    long double (* scalblnl) (long double x, long int n);                                                                                            /* 1580 */
    long double (* scalbnl) (long double x, int n);                                                                                                  /* 1584 */
    long double (* tgammal) (long double x);                                                                                                         /* 1588 */
    long double (* truncl) (long double x);                                                                                                          /* 1592 */
    int (* ilogbl) (long double x);                                                                                                                  /* 1596 */

    /* netdb.h */
    struct hostent * (* gethostbyaddr) (const void *addr, socklen_t len, int type);                                                                  /* 1600 */
    struct hostent * (* gethostbyname) (const char *name);                                                                                           /* 1604 */
    struct netent * (* getnetbyaddr) (in_addr_t net, int type);                                                                                      /* 1608 */
    struct netent * (* getnetbyname) (const char *name);                                                                                             /* 1612 */
    struct protoent * (* getprotobyname) (const char *name);                                                                                         /* 1616 */
    struct protoent * (* getprotobynumber) (int proto);                                                                                              /* 1620 */
    struct servent * (* getservbyname) (const char *name, const char *proto);                                                                        /* 1624 */
    struct servent * (* getservbyport) (int port, const char *proto);                                                                                /* 1628 */
    void (* herror) (const char *);                                                                                                                  /* 1632 */
    const char * (* hstrerror) (int);                                                                                                                /* 1636 */
    int (* gethostbyname_r) (const char *, struct hostent *, char *, size_t, struct hostent **, int *);                                              /* 1640 */
    int (* gethostbyname2_r) (const char *, int, struct hostent *, char *, size_t, struct hostent **, int *);                                        /* 1644 */
    struct hostent * (* gethostbyname2) (const char *, int);                                                                                         /* 1648 */
    int (* gethostbyaddr_r) (const void *, socklen_t, int, struct hostent *, char *, size_t, struct hostent **, int *);                              /* 1652 */
    int (* getservbyport_r) (int, const char *, struct servent *, char *, size_t, struct servent **);                                                /* 1656 */
    int (* getservbyname_r) (const char *, const char *, struct servent *, char *, size_t, struct servent **);                                       /* 1660 */
    const char * (* gai_strerror) (int ecode);                                                                                                       /* 1664 */
    int (* getaddrinfo) (const char *name, const char *service, const struct addrinfo *req, struct addrinfo **pai);                                  /* 1668 */
    void (* freeaddrinfo) (struct addrinfo *ai);                                                                                                     /* 1672 */
    int (* getnameinfo) (const struct sockaddr *sa, socklen_t salen, char *node, socklen_t nodelen, char *service, socklen_t servicelen, int flags); /* 1676 */

    /* nl_types.h */
    nl_catd (* catopen) (const char* name, int flag);                                                                                                /* 1680 */
    char* (* catgets) (nl_catd catalog, int set_number, int msg_number, const char* msg);                                                            /* 1684 */
    int (* catclose) (nl_catd catalog);                                                                                                              /* 1688 */

    /* poll.h */
    int (* poll) (struct pollfd *fds, nfds_t nfds, int timeout);                                                                                     /* 1692 */

    /* pwd.h */
    void (* endpwent) (void);                                                                                                                        /* 1696 */
    struct passwd * (* getpwent) (void);                                                                                                             /* 1700 */
    struct passwd * (* getpwnam) (const char *name);                                                                                                 /* 1704 */
    int (* getpwnam_r) (const char *name, struct passwd *pw, char *buffer, size_t bufsize, struct passwd **result);                                  /* 1708 */
    struct passwd * (* getpwuid) (uid_t uid);                                                                                                        /* 1712 */
    int (* getpwuid_r) (uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);                                        /* 1716 */
    void (* setpwent) (void);                                                                                                                        /* 1720 */

    /* regex.h */
    int (* regcomp) (regex_t * preg, const char * regex, int cflags);                                                                                /* 1724 */
    int (* regexec) (const regex_t *, const char *, size_t, regmatch_t *, int);                                                                      /* 1728 */
    void (* regfree) (regex_t *preg);                                                                                                                /* 1732 */
    size_t (* regerror) (int e, const regex_t * preg, char * buf, size_t size);                                                                      /* 1736 */

    /* resolv.h */
    struct __res_state (*__res_state) (void);                                                                                                        /* 1740 */
    int (* res_init) (void);                                                                                                                         /* 1744 */
    int (* res_query) (const char *dname, int class, int type, unsigned char *answer, int anslen);                                                   /* 1748 */
    int (* res_querydomain) (const char *name, const char *domain,int class, int type, unsigned char *answer,int anslen);                            /* 1752 */
    int (* res_search) (const char *dname, int class, int type, unsigned char *answer, int anslen);                                                  /* 1756 */
    int (* res_mkquery) (int, const char *, int, int, const unsigned char *, int, const unsigned char *, unsigned char *, int);                      /* 1760 */
    int (* res_send) (const unsigned char *msg, int msglen, unsigned char *answer,int anslen);                                                       /* 1764 */
    int (* dn_comp) (unsigned char *exp_dn, unsigned char *comp_dn,int length, unsigned char **dnptrs, unsigned char **lastdnptr);                   /* 1768 */
    int (* dn_expand) (unsigned char *msg, unsigned char *eomorig,unsigned char *comp_dn, char *exp_dn,int length);                                  /* 1772 */
    int (* dn_skipname) (const unsigned char *, const unsigned char *);                                                                              /* 1776 */

    /* search.h */
    int (* hcreate) (size_t);                                                                                                                        /* 1780 */
    void (* hdestroy) (void);                                                                                                                        /* 1784 */
    ENTRY * (* hsearch) (ENTRY, ACTION);                                                                                                             /* 1788 */
    int (* hcreate_r) (size_t, struct hsearch_data *);                                                                                               /* 1792 */
    void (* hdestroy_r) (struct hsearch_data *);                                                                                                     /* 1796 */
    int (* hsearch_r) (ENTRY, ACTION, ENTRY **, struct hsearch_data *);                                                                              /* 1800 */
    void * (* lfind) (const void *key, const void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                     /* 1804 */
    void * (* lsearch) (const void *key, void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));                         /* 1808 */
    void * (* tdelete) (const void *vkey, void **vrootp, int (*compar)(const void *, const void *));                                                 /* 1812 */
    void (* tdestroy) (void *vrootp, void (*freefct)(void *));                                                                                       /* 1816 */
    void * (* tfind) (const void *, void **, int (*)(const void *, const void *));                                                                   /* 1820 */
    void * (* tsearch) (const void *, void **, int (*)(const void *, const void *));                                                                 /* 1824 */
    void (* twalk) (const void *root, void (*action)(const void *, VISIT, int));                                                                     /* 1828 */

    /* setjmp.h */
    int (* setjmp) (jmp_buf __env);                                                                                                                  /* 1832 */
    int (* __sigsetjmp) (struct __jmp_buf_tag __env[1], int __savemask);                                                                             /* 1836 */
    int (* _setjmp) (struct __jmp_buf_tag __env[1]);                                                                                                 /* 1840 */
    void (* longjmp) (jmp_buf __env, int __val) __attribute__ ((__noreturn__));                                                                      /* 1844 */
    void (* _longjmp) (struct __jmp_buf_tag __env[1], int __val) __attribute__ ((__noreturn__));                                                     /* 1848 */
    void (* siglongjmp) (sigjmp_buf __env, int __val) __attribute__ ((__noreturn__));                                                                /* 1852 */
    int (* __sigjmp_save) (jmp_buf __env, int __savemask);                                                                                           /* 1856 */

    /* signal.h */
    _sig_func_ptr (* signal) (int, _sig_func_ptr);;                                                                                                  /* 1860 */
    int (* raise) (int sig);                                                                                                                         /* 1864 */
    int (* sigmask) (int signum);                                                                                                                    /* 1868 */
    int (* sigblock) (int signal_mask);                                                                                                              /* 1872 */
    int (* sigsetmask) (int signal_mask);                                                                                                            /* 1876 */
    int (* sigprocmask) (int how, const sigset_t *set, sigset_t *oset);                                                                              /* 1880 */
    int (* sigismember) (const sigset_t *set, int sig);                                                                                              /* 1884 */
    int (* sigemptyset) (sigset_t *set);                                                                                                             /* 1888 */
    int (* sigfillset) (sigset_t *set);                                                                                                              /* 1892 */
    int (* sigdelset) (sigset_t *set, int sig);                                                                                                      /* 1896 */
    int (* sigaddset) (sigset_t *set, int sig);                                                                                                      /* 1900 */
    int (* kill) (pid_t pid, int signal_number);                                                                                                     /* 1904 */
    int (* sigaction) (int sig, const struct sigaction *act, struct sigaction *oact);                                                                /* 1908 */

    /* stdio.h */
    void (* perror) (const char *s);                                                                                                                 /* 1912 */
    FILE * (* fopen) (const char *filename, const char *mode);                                                                                       /* 1916 */
    int (* fclose) (FILE *stream);                                                                                                                   /* 1920 */
    int (* fflush) (FILE *stream);                                                                                                                   /* 1924 */
    FILE * (* freopen) (const char *filename, const char *mode, FILE *stream);                                                                       /* 1928 */
    int (* setvbuf) (FILE *stream, char *buf, int bufmode, size_t size);                                                                             /* 1932 */
    void (* setbuf) (FILE *stream, char *buf);                                                                                                       /* 1936 */
    int (* fseek) (FILE *stream, long int offset, int wherefrom);                                                                                    /* 1940 */
    long int (* ftell) (FILE *stream);                                                                                                               /* 1944 */
    void (* rewind) (FILE *stream);                                                                                                                  /* 1948 */
    int (* fgetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 1952 */
    int (* fsetpos) (FILE *stream, fpos_t *pos);                                                                                                     /* 1956 */
    int (* fgetc) (FILE *stream);                                                                                                                    /* 1960 */
    int (* getc) (FILE *stream);                                                                                                                     /* 1964 */
    int (* getchar) (void);                                                                                                                          /* 1968 */
    int (* ungetc) (int c, FILE *stream);                                                                                                            /* 1972 */
    int (* fputc) (int c, FILE *stream);                                                                                                             /* 1976 */
    int (* putc) (int c, FILE *stream);                                                                                                              /* 1980 */
    int (* putchar) (int c);                                                                                                                         /* 1984 */
    char * (* fgets) (char *s, int n, FILE *stream);                                                                                                 /* 1988 */
    char * (* gets) (char *s);                                                                                                                       /* 1992 */
    int (* fscanf) (FILE *stream, const char *format, ...);                                                                                          /* 1996 */
    int (* scanf) (const char *format, ...);                                                                                                         /* 2000 */
    int (* sscanf) (const char *s, const char *format, ...);                                                                                         /* 2004 */
    int (* fputs) (const char *s, FILE *stream);                                                                                                     /* 2008 */
    int (* puts) (const char *s);                                                                                                                    /* 2012 */
    int (* dprintf) (int fd, const char *format, ...);                                                                                               /* 2016 */
    int (* fprintf) (FILE *stream, const char *format, ...);                                                                                         /* 2020 */
    int (* printf) (const char *format, ...);                                                                                                        /* 2024 */
    int (* sprintf) (char *s, const char *format, ...);                                                                                              /* 2028 */
    int (* vdprintf) (int fd, const char *format, va_list arg);                                                                                      /* 2032 */
    int (* vfprintf) (FILE *stream, const char *format, va_list arg);                                                                                /* 2036 */
    int (* vprintf) (const char *format, va_list arg);                                                                                               /* 2040 */
    int (* vsprintf) (char *s, const char *format, va_list arg);                                                                                     /* 2044 */
    size_t (* fread) (void *ptr, size_t element_size, size_t count, FILE *stream);                                                                   /* 2048 */
    size_t (* fwrite) (const void *ptr, size_t element_size, size_t count, FILE *stream);                                                            /* 2052 */
    int (* feof) (FILE *stream);                                                                                                                     /* 2056 */
    int (* ferror) (FILE *stream);                                                                                                                   /* 2060 */
    void (* clearerr) (FILE *stream);                                                                                                                /* 2064 */
    int (* rename) (const char *oldname, const char *newname);                                                                                       /* 2068 */
    int (* remove) (const char *filename);                                                                                                           /* 2072 */
    FILE * (* tmpfile) (void);                                                                                                                       /* 2076 */
    char * (* tmpnam) (char *buf);                                                                                                                   /* 2080 */
    char * (* tempnam) (const char *dir, const char *pfx);                                                                                           /* 2084 */
    char * (* ctermid) (char *);                                                                                                                     /* 2088 */
    int (* __flush) (FILE *stream);                                                                                                                  /* 2092 */
    int (* __unlockfile) (FILE *stream, int c);                                                                                                      /* 2096 */
    int (* getc_unlocked) (FILE *stream);                                                                                                            /* 2100 */
    int (* getchar_unlocked) (void);                                                                                                                 /* 2104 */
    int (* putc_unlocked) (int c, FILE *stream);                                                                                                     /* 2108 */
    int (* putchar_unlocked) (int c);                                                                                                                /* 2112 */
    ssize_t (* getline) (char **lineptr, size_t *n, FILE *stream);                                                                                   /* 2116 */
    ssize_t (* getdelim) (char **lineptr, size_t *n, int delim, FILE *stream);                                                                       /* 2120 */
    FILE * (* fdopen) (int file_descriptor, const char *type);                                                                                       /* 2124 */
    int (* fileno) (FILE *file);                                                                                                                     /* 2128 */
    int (* asprintf) (char **ret, const char *format, ...);                                                                                          /* 2132 */
    int (* vsnprintf) (char *s, size_t size, const char *format, va_list arg);                                                                       /* 2136 */
    int (* pclose) (FILE *stream);                                                                                                                   /* 2140 */
    FILE * (* popen) (const char *command, const char *type);                                                                                        /* 2144 */
    int (* fseeko) (FILE *stream, off_t offset, int wherefrom);                                                                                      /* 2148 */
    off_t (* ftello) (FILE *stream);                                                                                                                 /* 2152 */
    void (* flockfile) (FILE *file);                                                                                                                 /* 2156 */
    void (* funlockfile) (FILE *file);                                                                                                               /* 2160 */
    int (* ftrylockfile) (FILE *file);                                                                                                               /* 2164 */
    int (* vasprintf) (char **ret, const char *format, va_list arg);                                                                                 /* 2168 */
    int (* vfscanf) (FILE *stream, const char *format, va_list arg);                                                                                 /* 2172 */
    int (* vsscanf) (const char *s, const char *format, va_list arg);                                                                                /* 2176 */
    int (* vscanf) (const char *format, va_list arg);                                                                                                /* 2180 */
    int (* snprintf) (char *s, size_t size, const char *format, ...);                                                                                /* 2184 */

    /* stdio_ext.h */
    void (* _flushlbf) (void);                                                                                                                       /* 2188 */
    int (* __fsetlocking) (FILE *, int);                                                                                                             /* 2192 */
    int (* __fwriting) (FILE *);                                                                                                                     /* 2196 */
    int (* __freading) (FILE *);                                                                                                                     /* 2200 */
    int (* __freadable) (FILE *);                                                                                                                    /* 2204 */
    int (* __fwritable) (FILE *);                                                                                                                    /* 2208 */
    int (* __flbf) (FILE *);                                                                                                                         /* 2212 */
    size_t (* __fbufsize) (FILE *);                                                                                                                  /* 2216 */
    size_t (* __fpending) (FILE *);                                                                                                                  /* 2220 */
    int (* __fpurge) (FILE *);                                                                                                                       /* 2224 */
    void (* fpurge) (FILE *);                                                                                                                        /* 2228 */
    void (* __fseterr) (FILE *);                                                                                                                     /* 2232 */
    size_t (* __freadahead) (FILE *);                                                                                                                /* 2236 */
    void (* __freadptrinc) (FILE *f, size_t inc);                                                                                                    /* 2240 */

    /* stdlib.h */
    int * (* __mb_cur_max) (void);                                                                                                                  /* 2244 */
    const char * (* __getprogname) (void);                                                                                                           /* 2248 */
    int (* mblen) (const char *s, size_t n);                                                                                                         /* 2252 */
    int (* mbtowc) (wchar_t *pwc, const char *s, size_t n);                                                                                          /* 2256 */
    int (* _mbtowc_r) (wchar_t *pwc, const char *s, size_t n, _mbstate_t *st);                                                                       /* 2260 */
    size_t (* _mbstowcs_r) (wchar_t *pwcs, const char *s, size_t n, mbstate_t *state);                                                               /* 2264 */
    int (* wctomb) (char *s, wchar_t wchar);                                                                                                         /* 2268 */
    int (* _wctomb_r) (char *s, wchar_t wchar, mbstate_t *state);                                                                                    /* 2272 */
    size_t (* mbstowcs) (wchar_t *ws, const char *s, size_t wn);                                                                                     /* 2276 */
    size_t (* wcstombs) (char *s, const wchar_t *pwcs, size_t n);                                                                                    /* 2280 */
    size_t (* _wcstombs_r) (char *s, const wchar_t *pwcs, size_t n, mbstate_t *state);                                                               /* 2284 */
    void * (* malloc) (size_t size);                                                                                                                 /* 2288 */
    void * (* calloc) (size_t num_elements, size_t element_size);                                                                                    /* 2292 */
    void (* free) (void *ptr);                                                                                                                       /* 2296 */
    void * (* realloc) (void *ptr, size_t size);                                                                                                     /* 2300 */
    void * (* valloc) (size_t size);                                                                                                                 /* 2304 */
    void * (* aligned_alloc) (size_t alignment, size_t size);                                                                                        /* 2308 */
    int (* posix_memalign) (void **memptr, size_t alignment, size_t size);                                                                           /* 2312 */
    int (* abs) (int x);                                                                                                                             /* 2316 */
    long (* labs) (long x);                                                                                                                          /* 2320 */
    div_t (* div) (int n, int d);                                                                                                                    /* 2324 */
    ldiv_t (* ldiv) (long n, long d);                                                                                                                /* 2328 */
    int (* rand) (void);                                                                                                                             /* 2332 */
    void (* srand) (unsigned int seed);                                                                                                              /* 2336 */
    long (* random) (void);                                                                                                                          /* 2340 */
    void (* srandom) (unsigned int seed);                                                                                                            /* 2344 */
    char * (* initstate) (unsigned int seed, char *state, size_t size);                                                                              /* 2348 */
    char * (* setstate) (const char *state);                                                                                                         /* 2352 */
    char * (* l64a) (long x0);                                                                                                                       /* 2356 */
    long (* a64l) (const char *s);                                                                                                                   /* 2360 */
    void (* setkey) (const char *key);                                                                                                               /* 2364 */
    int (* system) (const char *command);                                                                                                            /* 2368 */
    void (* exit) (int status);                                                                                                                      /* 2372 */
    void (* abort) (void);                                                                                                                           /* 2376 */
    int (* atexit) (void (*)(void));                                                                                                                 /* 2380 */
    char * (* getenv) (const char *name);                                                                                                            /* 2384 */
    void * (* bsearch) (const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value));           /* 2388 */
    void (* qsort) (void *base, size_t count, size_t size, int (*compare)(const void *element1, const void *element2));                              /* 2392 */
    double (* strtod) (const char *str, char **ptr);                                                                                                 /* 2396 */
    long (* strtol) (const char *str, char **ptr, int base);                                                                                         /* 2400 */
    unsigned long (* strtoul) (const char *str, char **ptr, int base);                                                                               /* 2404 */
    quad_t (* strtoq) (const char *nptr, char **endptr, register int base);                                                                          /* 2408 */
    double (* atof) (const char *str);                                                                                                               /* 2412 */
    int (* atoi) (const char *str);                                                                                                                  /* 2416 */
    long (* atol) (const char *str);                                                                                                                 /* 2420 */
    char * (* itoa) (int value, char *buffer, int base);                                                                                             /* 2424 */
    char * (* lltoa) (int64_t ll, char *buffer, int radix);                                                                                          /* 2428 */
    void (* _exit) (int status);                                                                                                                     /* 2432 */
    int (* rand_r) (unsigned int *seed);                                                                                                             /* 2436 */
    char * (* mktemp) (char *name_template);                                                                                                         /* 2440 */
    int (* mkstemp) (char *name_template);                                                                                                           /* 2444 */
    int (* unsetenv) (const char *name);                                                                                                             /* 2448 */
    int (* setenv) (const char *name, const char *value, int overwrite);                                                                             /* 2452 */
    int (* putenv) (const char *string);                                                                                                             /* 2456 */
    char * (* mkdtemp) (char *name_template);                                                                                                        /* 2460 */
    const char * (* getexecname) (void);                                                                                                             /* 2464 */
    long long (* strtoll) (const char *str, char **ptr, int base);                                                                                   /* 2468 */
    long double (* strtold) (const char *str, char **ptr);                                                                                           /* 2472 */
    unsigned long long (* strtoull) (const char *str, char **ptr, int base);                                                                         /* 2476 */
    long long (* atoll) (const char *str);                                                                                                           /* 2480 */
    long long (* llabs) (long long x);                                                                                                               /* 2484 */
    lldiv_t (* lldiv) (long long n, long long d);                                                                                                    /* 2488 */
    float (* strtof) (const char *str, char **ptr);                                                                                                  /* 2492 */
    float (* atoff) (const char *nptr);                                                                                                              /* 2496 */
    void (* _Exit) (int status);                                                                                                                     /* 2500 */
    char * (* ecvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2504 */
    char * (* fcvt) (double x, int n, int *dp, int *sign);                                                                                           /* 2508 */
    char * (* gcvt) (double x, int n, char *b);                                                                                                      /* 2512 */
    char * (* secure_getenv) (const char *name);                                                                                                     /* 2516 */
    void * (* reallocarray) (void *ptr, size_t m, size_t n);                                                                                         /* 2520 */
    void (* qsort_r) (void *, size_t, size_t, int (*)(const void *, const void *, void *), void *);                                                  /* 2524 */
    uint32_t (* arc4random) (void);                                                                                                                  /* 2528 */
    void (* arc4random_buf) (void *buf, size_t nbytes);                                                                                              /* 2532 */
    uint32_t (* arc4random_uniform) (uint32_t upper_bound);                                                                                          /* 2536 */
    void (* arc4random_stir) (void);                                                                                                                 /* 2540 */
    void (* arc4random_addrandom) (unsigned char *dat, int datlen);                                                                                  /* 2544 */

    /* string.h */
    char * (* strerror) (int error_number);                                                                                                          /* 2548 */
    char * (* strcat) (char *dest, const char *src);                                                                                                 /* 2552 */
    char * (* strncat) (char *dest, const char *src, size_t n);                                                                                      /* 2556 */
    int (* strcmp) (const char *s1, const char * s2);                                                                                                /* 2560 */
    int (* strncmp) (const char *s1, const char *s2, size_t n);                                                                                      /* 2564 */
    char * (* strcpy) (char *dest, const char *src);                                                                                                 /* 2568 */
    char * (* strncpy) (char *dest, const char *src, size_t n);                                                                                      /* 2572 */
    size_t (* strnlen) (const char *s, size_t maxlen);                                                                                               /* 2576 */
    size_t (* strlen) (const char *s);                                                                                                               /* 2580 */
    char * (* strchr) (const char *s, int c);                                                                                                        /* 2584 */
    char * (* strrchr) (const char *s, int c);                                                                                                       /* 2588 */
    size_t (* strspn) (const char *s, const char *set);                                                                                              /* 2592 */
    size_t (* strcspn) (const char *s, const char *set);                                                                                             /* 2596 */
    char * (* strpbrk) (const char *s, const char *set);                                                                                             /* 2600 */
    char * (* strtok) (char *str, const char *set);                                                                                                  /* 2604 */
    char * (* strstr) (const char *src, const char *sub);                                                                                            /* 2608 */
    char * (* strnstr) (const char *src, const char *sub, size_t len);                                                                               /* 2612 */
    char * (* strsep) (char ** src, const char *delim);                                                                                              /* 2616 */
    char * (* stpcpy) (char *dest, const char *src);                                                                                                 /* 2620 */
    char * (* stpncpy) (char *dest, const char *src, size_t len);                                                                                    /* 2624 */
    size_t (* stccpy) (char * dest, const char * src, size_t n);                                                                                     /* 2628 */
    int (* strcoll) (const char *s1, const char *s2);                                                                                                /* 2632 */
    size_t (* strxfrm) (char *dest, const char *src, size_t len);                                                                                    /* 2636 */
    void * (* memmove) (void *dest, const void * src, size_t len);                                                                                   /* 2640 */
    void * (* memchr) (const void * ptr, int val, size_t len);                                                                                       /* 2644 */
    int (* memcmp) (const void *ptr1, const void *ptr2, size_t len);                                                                                 /* 2648 */
    void * (* memcpy) (void *dest, const void *src, size_t len);                                                                                     /* 2652 */
    void * (* memset) (void *ptr, int val, size_t len);                                                                                              /* 2656 */
    int (* strerror_r) (int error,char * buffer,size_t buffer_size);                                                                                 /* 2660 */
    char * (* strdup) (const char *s);                                                                                                               /* 2664 */
    char * (* strndup) (const char *str, size_t max);                                                                                                /* 2668 */
    void (* bcopy) (const void *from,void *to,size_t len);                                                                                           /* 2672 */
    void (* bzero) (void *m,size_t len);                                                                                                             /* 2676 */
    void (* explicit_bzero) (void *m, size_t len);                                                                                                   /* 2680 */
    int (* bcmp) (const void *a,const void *b,size_t len);                                                                                           /* 2684 */
    size_t (* strlcpy) (char *dst, const char *src, size_t siz);                                                                                     /* 2688 */
    size_t (* strlcat) (char *dst, const char *src, size_t siz);                                                                                     /* 2692 */
    char * (* strtok_r) (char *str, const char *separator_set,char ** state_ptr);                                                                    /* 2696 */
    char * (* strsignal) (int);                                                                                                                      /* 2700 */
    void * (* memccpy) (void *dst0, const void *src0, int endchar0, size_t len0);                                                                    /* 2704 */
    char * (* index) (const char *s, int c);                                                                                                         /* 2708 */
    char * (* rindex) (const char *s, int c);                                                                                                        /* 2712 */
    int (* strverscmp) (const char *s1, const char *s2);                                                                                             /* 2716 */
    char * (* strchrnul) (const char *s, int c_in);                                                                                                  /* 2720 */
    char * (* strcasestr) (const char *haystack, const char *needle);                                                                                /* 2724 */
    void * (* memmem) (const void *h0, size_t k, const void *n0, size_t l);                                                                          /* 2728 */
    void * (* memrchr) (const void *s, int c, size_t n);                                                                                             /* 2732 */
    void * (* mempcpy) (void *dst0, const void *src0, size_t len0);                                                                                  /* 2736 */

    /* strings.h */
    int (* strcasecmp) (const char *s1, const char *s2);                                                                                             /* 2740 */
    int (* strncasecmp) (const char *s1, const char *s2, size_t len);                                                                                /* 2744 */
    int (* ffs) (int i);                                                                                                                             /* 2748 */
    int (* ffsl) (long i);                                                                                                                           /* 2752 */
    int (* ffsll) (long long i);                                                                                                                     /* 2756 */

    /* termcap.h */
    int (* tgetent) (char *buffer, const char *termtype);                                                                                            /* 2760 */
    int (* tgetnum) (const char *name);                                                                                                              /* 2764 */
    int (* tgetflag) (const char *name);                                                                                                             /* 2768 */
    char * (* tgetstr) (const char *name, char **area);                                                                                              /* 2772 */
    void (* tputs) (const char *str, int nlines, int (*outfun) (int));                                                                               /* 2776 */
    char * (* tparam) (const char *string, char *outstring, int leb, int arg0, int arg1, int arg2, int arg3);                                        /* 2780 */
    char * (* tgoto) (const char *cstring, int hpos, int vpos);                                                                                      /* 2784 */

    /* termios.h */
    int (* tcgetattr) (int file_descriptor, struct termios *tios_p);                                                                                 /* 2788 */
    int (* tcsetattr) (int file_descriptor, int action, struct termios *tios_p);                                                                     /* 2792 */
    int (* tcsendbreak) (int file_descriptor, int duration);                                                                                         /* 2796 */
    int (* tcdrain) (int file_descriptor);                                                                                                           /* 2800 */
    int (* tcflush) (int file_descriptor, int what);                                                                                                 /* 2804 */
    int (* tcflow) (int file_descriptor, int action);                                                                                                /* 2808 */
    int (* cfmakeraw) (struct termios *tios_p);                                                                                                      /* 2812 */
    speed_t (* cfgetispeed) (const struct termios *tios_p);                                                                                          /* 2816 */
    speed_t (* cfgetospeed) (const struct termios *tios_p);                                                                                          /* 2820 */
    int (* cfsetispeed) (struct termios *tios_p, speed_t input_speed);                                                                               /* 2824 */
    int (* cfsetospeed) (struct termios *tios_p, speed_t output_speed);                                                                              /* 2828 */

    /* time.h */
    clock_t (* clock) (void);                                                                                                                        /* 2832 */
    time_t (* time) (time_t *t);                                                                                                                     /* 2836 */
    char * (* asctime) (const struct tm *tm);                                                                                                        /* 2840 */
    char * (* ctime) (const time_t *t);                                                                                                              /* 2844 */
    struct tm * (* gmtime) (const time_t *t);                                                                                                        /* 2848 */
    struct tm * (* localtime) (const time_t *t);                                                                                                     /* 2852 */
    time_t (* mktime) (struct tm *tm);                                                                                                               /* 2856 */
    double (* difftime) (time_t t1, time_t t0);                                                                                                      /* 2860 */
    size_t (* strftime) (char *s, size_t maxsize, const char *format, const struct tm *tm);                                                          /* 2864 */
    char * (* strptime) (const char *buf, const char *fmt, struct tm *timeptr);                                                                      /* 2868 */
    char * (* asctime_r) (const struct tm *tm, char *buffer);                                                                                        /* 2872 */
    char * (* ctime_r) (const time_t *tptr, char *buffer);                                                                                           /* 2876 */
    struct tm * (* gmtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                   /* 2880 */
    struct tm * (* localtime_r) (const time_t *t, struct tm *tm_ptr);                                                                                /* 2884 */
    void (* tzset) (void);                                                                                                                           /* 2888 */
    int (* nanosleep) (const struct timespec *req, struct timespec *rem);                                                                            /* 2892 */
    int (* clock_gettime) (clockid_t clk_id, struct timespec *t);                                                                                    /* 2896 */
    int (* clock_settime) (clockid_t clk_id, const struct timespec *t);                                                                              /* 2900 */
    int (* clock_getres) (clockid_t clock_id, struct timespec *res);                                                                                 /* 2904 */
    int (* clock_nanosleep) (clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);                                /* 2908 */
    unsigned long long (* rdtsc) (void);                                                                                                             /* 2912 */
    int (* clock_gettime64) (clockid_t clock_id, struct timespec64 *tp);                                                                             /* 2916 */

    /* uchar.h */
    size_t (* c16rtomb) (char *__restrict, char16_t, mbstate_t *__restrict);                                                                         /* 2920 */
    size_t (* mbrtoc16) (char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 2924 */
    size_t (* c32rtomb) (char *__restrict, char32_t, mbstate_t *__restrict);                                                                         /* 2928 */
    size_t (* mbrtoc32) (char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);                                               /* 2932 */

    /* ulimit.h */
    long (* ulimit) (int cmd,long newlimit);                                                                                                         /* 2936 */

    /* unistd.h */
    int (* isatty) (int file_descriptor);                                                                                                            /* 2940 */
    int (* dup) (int file_descriptor);                                                                                                               /* 2944 */
    int (* dup2) (int file_descriptor1, int file_descriptor2);                                                                                       /* 2948 */
    off_t (* lseek) (int file_descriptor, off_t offset, int mode);                                                                                   /* 2952 */
    int (* access) (const char * path_name, int mode);                                                                                               /* 2956 */
    int (* chown) (const char * path_name, uid_t owner, gid_t group);                                                                                /* 2960 */
    int (* fchown) (int file_descriptor, uid_t owner, gid_t group);                                                                                  /* 2964 */
    int (* lchown) (const char * path_name, uid_t owner, gid_t group);                                                                               /* 2968 */
    int (* truncate) (const char * path_name, off_t length);                                                                                         /* 2972 */
    int (* ftruncate) (int file_descriptor, off_t length);                                                                                           /* 2976 */
    int (* getpagesize) (void);                                                                                                                      /* 2980 */
    int (* ftruncate64) (int file_descriptor, _off64_t length);                                                                                      /* 2984 */
    int (* truncate64) (const char *path_name, _off64_t length);                                                                                     /* 2988 */
    off64_t (* lseek64) (int fd, off64_t offset, int whence);                                                                                        /* 2992 */
    unsigned int (* alarm) (unsigned seconds);                                                                                                       /* 2996 */
    useconds_t (* ualarm) (useconds_t usecs, useconds_t interval);                                                                                   /* 3000 */
    int (* link) (const char * existing_path,const char * new_path);                                                                                 /* 3004 */
    int (* unlink) (const char * path_name);                                                                                                         /* 3008 */
    int (* symlink) (const char * actual_path, const char * symbolic_path);                                                                          /* 3012 */
    int (* readlink) (const char * path_name, char * buffer, int buffer_size);                                                                       /* 3016 */
    int (* chdir) (const char * path_name);                                                                                                          /* 3020 */
    int (* fchdir) (int file_descriptor);                                                                                                            /* 3024 */
    int (* lockf) (int file_descriptor, int function, off_t size);                                                                                   /* 3028 */
    unsigned int (* sleep) (unsigned int seconds);                                                                                                   /* 3032 */
    int (* usleep) (useconds_t microseconds);                                                                                                        /* 3036 */
    int (* pause) (void);                                                                                                                            /* 3040 */
    pid_t (* getpid) (void);                                                                                                                         /* 3044 */
    pid_t (* getppid) (void);                                                                                                                        /* 3048 */
    pid_t (* getpgrp) (void );                                                                                                                       /* 3052 */
    pid_t (* tcgetpgrp) (int fildes);                                                                                                                /* 3056 */
    int (* tcsetpgrp) (int filedes, pid_t pgrp);                                                                                                     /* 3060 */
    char * (* realpath) (const char *file_name, char *resolved_name);                                                                                /* 3064 */
    int (* fsync) (int file_descriptor);                                                                                                             /* 3068 */
    int (* fdatasync) (int file_descriptor);                                                                                                         /* 3072 */
    char * (* ttyname) (int);                                                                                                                        /* 3076 */
    int (* ttyname_r) (int file_descriptor,char *name,size_t buflen);                                                                                /* 3080 */
    int (* execl) (const char *path,const char *arg0,...);                                                                                           /* 3084 */
    int (* execle) (const char *path,const char *arg0,...);                                                                                          /* 3088 */
    int (* execlp) (const char *path,const char *arg0,...);                                                                                          /* 3092 */
    int (* execv) (const char *path,char * const argv[]);                                                                                            /* 3096 */
    int (* execve) (const char *path,char *const argv[],char *const envp[]);                                                                         /* 3100 */
    int (* execvp) (const char *command,char * const argv[]);                                                                                        /* 3104 */
    void (* encrypt) (char *block, int edflag);                                                                                                      /* 3108 */
    int (* spawnv) (int mode, const char *file, const char **argv);                                                                                  /* 3112 */
    int (* spawnvp) (int mode, const char *path, const char **argv);                                                                                 /* 3116 */
    long (* sysconf) (int name);                                                                                                                     /* 3120 */
    void (* enableUnixPaths) (void);                                                                                                                 /* 3124 */
    void (* disableUnixPaths) (void);                                                                                                                /* 3128 */
    void (* enableAltivec) (void);                                                                                                                   /* 3132 */
    void (* disableAltivec) (void);                                                                                                                  /* 3136 */
    void (* enableOptimizedFunctions) (void);                                                                                                        /* 3140 */
    void (* disableOptimizedFunctions) (void);                                                                                                       /* 3144 */
    char * (* getcwd) (char * buffer, size_t buffer_size);                                                                                           /* 3148 */
    char * (* getwd) (char *buf);                                                                                                                    /* 3152 */
    char * (* get_current_dir_name) (void);                                                                                                          /* 3156 */
    long (* gethostid) ();                                                                                                                           /* 3160 */
    int (* gethostname) (const char *name, size_t len);                                                                                              /* 3164 */
    int (* getdomainname) (char *name, size_t len);                                                                                                  /* 3168 */
    int (* setdomainname) (const char *name, size_t len);                                                                                            /* 3172 */
    char * (* getlogin) (void);                                                                                                                      /* 3176 */
    int (* getlogin_r) (char *buf, size_t bufsize);                                                                                                  /* 3180 */
    char * (* crypt) (const char *key, const char *salt);                                                                                            /* 3184 */
    gid_t (* getegid) (void);                                                                                                                        /* 3188 */
    uid_t (* geteuid) (void);                                                                                                                        /* 3192 */
    gid_t (* getgid) (void);                                                                                                                         /* 3196 */
    int (* getgroups) (int gidsetsize, gid_t grouplist[]);                                                                                           /* 3200 */
    char * (* getpass) (const char *prompt);                                                                                                         /* 3204 */
    uid_t (* getuid) (void);                                                                                                                         /* 3208 */
    int (* initgroups) (const char *name, gid_t basegid);                                                                                            /* 3212 */
    int (* setegid) (gid_t gid);                                                                                                                     /* 3216 */
    int (* seteuid) (uid_t uid);                                                                                                                     /* 3220 */
    int (* setgid) (gid_t gid);                                                                                                                      /* 3224 */
    int (* setgroups) (int ngroups, const gid_t *gidset);                                                                                            /* 3228 */
    int (* setregid) (gid_t rgid, gid_t egid);                                                                                                       /* 3232 */
    int (* setreuid) (uid_t ruid, uid_t euid);                                                                                                       /* 3236 */
    pid_t (* setsid) (void);                                                                                                                         /* 3240 */
    int (* setuid) (uid_t uid);                                                                                                                      /* 3244 */
    int (* setlogin) (const char *name);                                                                                                             /* 3248 */
    long (* pathconf) (const char *path,int name);                                                                                                   /* 3252 */
    long (* fpathconf) (int file_descriptor,int name);                                                                                               /* 3256 */
    int (* pipe) (int fd[2]);                                                                                                                        /* 3260 */
    int (* pipe2) (int fd[2], int flags);                                                                                                            /* 3264 */
    ssize_t (* pread) (int fd, void *buf, size_t nbytes, off_t offset);                                                                              /* 3268 */
    ssize_t (* pwrite) (int fd, const void *buf, size_t nbytes, off_t offset);                                                                       /* 3272 */
    ssize_t (* pread64) (int fd, void *buf, size_t nbytes, off64_t offset);                                                                          /* 3276 */
    ssize_t (* pwrite64) (int fd, const void *buf, size_t nbytes, off64_t offset);                                                                   /* 3280 */

    /* utime.h */
    int (* utime) (const char * path_name,const struct utimbuf * times);                                                                             /* 3284 */

    /* wchar.h */
    wint_t (* btowc) (int c);                                                                                                                        /* 3288 */
    int (* wctob) (wint_t c);                                                                                                                        /* 3292 */
    wchar_t * (* wcscat) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3296 */
    wchar_t * (* wcsncat) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3300 */
    int (* wcscmp) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3304 */
    int (* wcsncmp) (const wchar_t *s1, const wchar_t *s2, size_t n);                                                                                /* 3308 */
    wchar_t * (* wcscpy) (wchar_t *dest, const wchar_t *src);                                                                                        /* 3312 */
    wchar_t * (* wcsncpy) (wchar_t *dest, const wchar_t *src, size_t n);                                                                             /* 3316 */
    size_t (* wcslen) (const wchar_t *s);                                                                                                            /* 3320 */
    wchar_t * (* wcschr) (const wchar_t *s, wchar_t c);                                                                                              /* 3324 */
    size_t (* wcsspn) (const wchar_t *s, const wchar_t *set);                                                                                        /* 3328 */
    wchar_t * (* wcspbrk) (const wchar_t *s, const wchar_t *set);                                                                                    /* 3332 */
    wchar_t * (* wcstok) (wchar_t *str, const wchar_t *delim, wchar_t **ptr);                                                                        /* 3336 */
    wchar_t * (* wcsstr) (const wchar_t *big, const wchar_t *little);                                                                                /* 3340 */
    int (* wcswidth) (const wchar_t *pwcs, size_t n);                                                                                                /* 3344 */
    int (* wcwidth) (const wchar_t wc);                                                                                                              /* 3348 */
    double (* wcstod) (const wchar_t *str, wchar_t **ptr);                                                                                           /* 3352 */
    long (* wcstol) (const wchar_t *str, wchar_t **ptr, int base);                                                                                   /* 3356 */
    unsigned long (* wcstoul) (const wchar_t *str, wchar_t **ptr, int base);                                                                         /* 3360 */
    float (* wcstof) (const wchar_t *nptr, wchar_t **endptr);                                                                                        /* 3364 */
    int (* wscoll) (const wchar_t *s1, const wchar_t *s2);                                                                                           /* 3368 */
    size_t (* wcsxfrm) (wchar_t *dest, const wchar_t *src, size_t len);                                                                              /* 3372 */
    wchar_t * (* wmemchr) (const wchar_t *ptr, wchar_t val, size_t len);                                                                             /* 3376 */
    int (* wmemcmp) (const wchar_t *ptr1, const wchar_t *ptr2, size_t len);                                                                          /* 3380 */
    wchar_t * (* wmemcpy) (wchar_t *dest, const wchar_t *src, size_t len);                                                                           /* 3384 */
    wchar_t * (* wmemmove) (wchar_t *dest, const wchar_t *src, size_t len);                                                                          /* 3388 */
    wchar_t * (* wmemset) (wchar_t *ptr, int val, size_t len);                                                                                       /* 3392 */
    int (* fwide) (FILE *stream, int orient);                                                                                                        /* 3396 */
    wint_t (* fgetwc) (FILE *stream);                                                                                                                /* 3400 */
    wint_t (* getwc) (FILE *stream);                                                                                                                 /* 3404 */
    wint_t (* getwchar) (void);                                                                                                                      /* 3408 */
    wint_t (* ungetwc) (wint_t wc, FILE *fp);                                                                                                        /* 3412 */
    wchar_t * (* fgetws) (wchar_t *s, int n, FILE *stream);                                                                                          /* 3416 */
    int (* fwscanf) (FILE *stream, const wchar_t *format, ...);                                                                                      /* 3420 */
    int (* swscanf) (const wchar_t *s, const wchar_t *format, ...);                                                                                  /* 3424 */
    int (* vfwscanf) (FILE *f, const wchar_t *format, va_list ap);                                                                                   /* 3428 */
    int (* vswscanf) (const wchar_t *s, const wchar_t *format, va_list ap);                                                                          /* 3432 */
    int (* vwscanf) (const wchar_t *s, va_list ap);                                                                                                  /* 3436 */
    int (* wscanf) (const wchar_t *format, ...);                                                                                                     /* 3440 */
    wint_t (* fputwc) (wchar_t c, FILE *stream);                                                                                                     /* 3444 */
    wint_t (* putwc) (wchar_t c, FILE *stream);                                                                                                      /* 3448 */
    wint_t (* putwchar) (wchar_t c);                                                                                                                 /* 3452 */
    wint_t (* fputws) (const wchar_t *s, FILE *stream);                                                                                              /* 3456 */
    int (* fwprintf) (FILE *stream, const wchar_t *format, ...);                                                                                     /* 3460 */
    int (* swprintf) (wchar_t *restrict s, size_t l, const wchar_t *restrict fmt, ...);                                                              /* 3464 */
    int (* vfwprintf) (FILE *stream, const wchar_t *format, va_list arg);                                                                            /* 3468 */
    int (* vswprintf) (wchar_t *s, size_t maxlen, const wchar_t *format, va_list arg);                                                               /* 3472 */
    int (* vwprintf) (const wchar_t *format, va_list arg);                                                                                           /* 3476 */
    int (* wprintf) (const wchar_t *format, ...);                                                                                                    /* 3480 */
    size_t (* wcsftime) (wchar_t *s, size_t maxsize, const wchar_t *format, const struct tm *timeptr);                                               /* 3484 */
    size_t (* mbrlen) (const char *restrict s, size_t n, mbstate_t *restrict ps);                                                                    /* 3488 */
    size_t (* mbrtowc) (wchar_t *pwc, const char *src, size_t n, mbstate_t *ps);                                                                     /* 3492 */
    int (* mbsinit) (const mbstate_t *ps);                                                                                                           /* 3496 */
    size_t (* mbsrtowcs) (wchar_t *restrict dst, const char **restrict src, size_t len, mbstate_t *restrict ps);                                     /* 3500 */
    size_t (* wcrtomb) (char *restrict s, wchar_t wc, mbstate_t *restrict ps);                                                                       /* 3504 */
    int (* wcscoll) (const wchar_t *ws1, const wchar_t *ws2);                                                                                        /* 3508 */
    size_t (* wcscspn) (const wchar_t *s, const wchar_t *c);                                                                                         /* 3512 */
    wchar_t * (* wcsrchr) (const wchar_t *ws, wchar_t wc);                                                                                           /* 3516 */
    size_t (* wcsrtombs) (char *restrict dst, const wchar_t **restrict src, size_t len, mbstate_t *restrict ps);                                     /* 3520 */
    long long (* wcstoll) (const wchar_t *str, wchar_t **ptr, int base);                                                                             /* 3524 */
    unsigned long long (* wcstoull) (const wchar_t *str, wchar_t **ptr, int base);                                                                   /* 3528 */
    long double (* wcstold) (const wchar_t *nptr, wchar_t **endptr);                                                                                 /* 3532 */
    size_t (* mbsnrtowcs) (wchar_t *dst, const char **restrict src, size_t nmc, size_t len, mbstate_t *restrict ps);                                 /* 3536 */
    size_t (* wcsnrtombs) (char *restrict dst, const wchar_t **restrict src, size_t nwc, size_t len, mbstate_t *restrict ps);                        /* 3540 */
    wchar_t * (* wcsdup) (const wchar_t *src);                                                                                                       /* 3544 */
    size_t (* wcsnlen) (const wchar_t *src, size_t n);                                                                                               /* 3548 */
    wchar_t * (* wcpcpy) (wchar_t *dst, const wchar_t *src);                                                                                         /* 3552 */
    wchar_t * (* wcpncpy) (wchar_t *dst, const wchar_t *src, size_t len);                                                                            /* 3556 */
    int (* wcscasecmp) (const wchar_t *l, const wchar_t *r);                                                                                         /* 3560 */
    int (* wcscasecmp_l) (const wchar_t *l, const wchar_t *r, locale_t loc);                                                                         /* 3564 */
    int (* wcsncasecmp) (const wchar_t *l, const wchar_t *r, size_t n);                                                                              /* 3568 */
    int (* wcsncasecmp_l) (const wchar_t *l, const wchar_t *r, size_t n, locale_t loc);                                                              /* 3572 */
    int (* wcscoll_l) (const wchar_t *ws1, const wchar_t *ws2, locale_t loc);                                                                        /* 3576 */
    size_t (* wcsxfrm_l) (wchar_t *dest, const wchar_t *src, size_t len, locale_t loc);                                                              /* 3580 */

    /* wctype.h */
    int (* iswalnum) (wint_t c);                                                                                                                     /* 3584 */
    int (* iswalpha) (wint_t c);                                                                                                                     /* 3588 */
    int (* iswcntrl) (wint_t c);                                                                                                                     /* 3592 */
    int (* iswdigit) (wint_t c);                                                                                                                     /* 3596 */
    int (* iswxdigit) (wint_t c);                                                                                                                    /* 3600 */
    int (* iswgraph) (wint_t c);                                                                                                                     /* 3604 */
    int (* iswpunct) (wint_t c);                                                                                                                     /* 3608 */
    int (* iswprint) (wint_t c);                                                                                                                     /* 3612 */
    int (* iswlower) (wint_t c);                                                                                                                     /* 3616 */
    int (* iswupper) (wint_t c);                                                                                                                     /* 3620 */
    int (* iswspace) (wint_t c);                                                                                                                     /* 3624 */
    int (* iswblank) (wint_t c);                                                                                                                     /* 3628 */
    wint_t (* towlower) (wint_t c);                                                                                                                  /* 3632 */
    wint_t (* towupper) (wint_t c);                                                                                                                  /* 3636 */
    wctype_t (* wctype) (const char *property);                                                                                                      /* 3640 */
    int (* iswctype) (wint_t c, wctype_t desc);                                                                                                      /* 3644 */
    wctrans_t (* wctrans) (const char *property);                                                                                                    /* 3648 */
    wint_t (* towctrans) (wint_t c, wctrans_t desc);                                                                                                 /* 3652 */

    /* arpa/inet.h */
    in_addr_t (* inet_addr) (const char *cp);                                                                                                        /* 3656 */
    int (* inet_aton) (const char *cp, struct in_addr *pin);                                                                                         /* 3660 */
    in_addr_t (* inet_lnaof) (struct in_addr in);                                                                                                    /* 3664 */
    struct in_addr (* inet_makeaddr) (in_addr_t net, in_addr_t lna);                                                                                 /* 3668 */
    in_addr_t (* inet_netof) (struct in_addr in);                                                                                                    /* 3672 */
    in_addr_t (* inet_network) (const char *cp);                                                                                                     /* 3676 */
    char * (* inet_ntoa) (struct in_addr in);                                                                                                        /* 3680 */
    const char * (* inet_ntop) (int af, const void *src, char *dst, socklen_t cnt);                                                                  /* 3684 */
    int (* inet_pton) (int af, const char *src, void *dst);                                                                                          /* 3688 */

    /* arpa/nameser.h */
    unsigned (* ns_get16) (const unsigned char *);                                                                                                   /* 3692 */
    unsigned long (* ns_get32) (const unsigned char *);                                                                                              /* 3696 */
    void (* ns_put16) (unsigned, unsigned char *);                                                                                                   /* 3700 */
    void (* ns_put32) (unsigned long, unsigned char *);                                                                                              /* 3704 */
    int (* ns_initparse) (const unsigned char *, int, ns_msg *);                                                                                     /* 3708 */
    int (* ns_parserr) (ns_msg *, ns_sect, int, ns_rr *);                                                                                            /* 3712 */
    int (* ns_skiprr) (const unsigned char *, const unsigned char *, ns_sect, int);                                                                  /* 3716 */
    int (* ns_name_uncompress) (const unsigned char *, const unsigned char *, const unsigned char *, char *, size_t);                                /* 3720 */

    /* net/if.h */
    unsigned int (* if_nametoindex) (const char *ifname);                                                                                            /* 3724 */
    char * (* if_indextoname) (unsigned int ifindex, char *ifname);                                                                                  /* 3728 */

    /* netinet/in.h */
    int (* bindresvport) (int sd, struct sockaddr_in *sa);                                                                                           /* 3732 */
    int (* bindresvport6) (int sd, struct sockaddr_in6 *sa);                                                                                         /* 3736 */

    /* sys/byteswap.h */
    __CONST_FUNC uint16_t (* bswap16) (uint16_t);                                                                                                    /* 3740 */
    __CONST_FUNC uint32_t (* bswap24) (uint32_t);                                                                                                    /* 3744 */
    __CONST_FUNC uint32_t (* bswap32) (uint32_t);                                                                                                    /* 3748 */
    __CONST_FUNC uint64_t (* bswap64) (uint64_t);                                                                                                    /* 3752 */
    void * (* swab) (void *from,void *to,ssize_t nbytes);                                                                                            /* 3756 */
    void * (* swab24) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 24-bit words instead. */                              /* 3760 */
    void * (* swab32) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 32-bit words instead. */                              /* 3764 */
    void * (* swab64) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 64-bit words instead. */                              /* 3768 */

    /* sys/file.h */
    int (* flock) (int fd, int op);                                                                                                                  /* 3772 */

    /* sys/ioctl.h */
    int (* ioctl) (int fildes, int request, ... /* arg */);                                                                                          /* 3776 */

    /* sys/ipc.h */
    key_t (* ftok) (const char *path, int id);                                                                                                       /* 3780 */

    /* sys/mman.h */
    void * (* mmap) (void *addr, size_t len, int prot, int flags, int fd, off_t offset);                                                             /* 3784 */
    int (* munmap) (void *map, size_t length);                                                                                                       /* 3788 */
    int (* msync) (void *addr, size_t len, int flags);                                                                                               /* 3792 */

    /* sys/mount.h */
    int (* statfs) (const char *path, struct statfs *buf);                                                                                           /* 3796 */
    int (* fstatfs) (int fd, struct statfs *buf);                                                                                                    /* 3800 */

    /* sys/msg.h */
    int (* _msgctl) (int msqid, int cmd, struct msqid_ds *buf);                                                                                      /* 3804 */
    int (* _msgget) (key_t key, int msgflg);                                                                                                         /* 3808 */
    ssize_t (* _msgrcv) (int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);                                                              /* 3812 */
    int (* _msgsnd) (int msqid, const void *msgp, size_t msgsz, int msgflg);                                                                         /* 3816 */
    int (* _msgsnap) (int msqid, void *buf, size_t bufsz, long msgtyp);                                                                              /* 3820 */
    int (* _msgids) (int *buf, size_t nids, size_t  *pnids);                                                                                         /* 3824 */

    /* sys/resource.h */
    int (* getrlimit) (int resource, struct rlimit *rlp);                                                                                            /* 3828 */
    int (* setrlimit) (int resource, const struct rlimit *rlp);                                                                                      /* 3832 */
    int (* getrusage) (int who, struct rusage *usage);                                                                                               /* 3836 */

    /* sys/select.h */
    int (* select) (int nfds, fd_set *readfds,fd_set *writefds, fd_set *errorfds,struct timeval *timeout);                                           /* 3840 */
    int (* waitselect) (int num_fds,fd_set *read_fds,fd_set *write_fds,fd_set *except_fds,struct timeval *timeout,unsigned long * signal_mask);      /* 3844 */

    /* sys/sem.h */
    int (* _semctl) (int semid, int semnum, int cmd, union semun aun);                                                                               /* 3848 */
    int (* _semget) (key_t key, int nsems, int flags);                                                                                               /* 3852 */
    int (* _semop) (int semid, const struct sembuf *ops, int nops);                                                                                  /* 3856 */
    long int (* _semids) (long int *buf, uint32_t nids, long unsigned int *idcnt);                                                                   /* 3860 */
    int (* _semtimedop) (int semid, const struct sembuf *ops, int nops, struct timespec *to);                                                        /* 3864 */

    /* sys/shm.h */
    void * (* _shmat) (int shmid, const void *shmaddr, int shmflg);                                                                                  /* 3868 */
    int (* _shmctl) (int shmid, int cmd, struct shmid_ds *buf);                                                                                      /* 3872 */
    int (* _shmdt) (const void *shmaddr);                                                                                                            /* 3876 */
    int (* _shmget) (key_t key, size_t size, int shmflg);                                                                                            /* 3880 */
    int (* _shmids) (int *buf, size_t nids, size_t *pnids);                                                                                          /* 3884 */

    /* sys/socket.h */
    int (* accept) (int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);                                                                         /* 3888 */
    int (* bind) (int socket, const struct sockaddr *address, socklen_t address_len);                                                                /* 3892 */
    int (* connect) (int socket, const struct sockaddr *address, socklen_t address_len);                                                             /* 3896 */
    int (* getpeername) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 3900 */
    int (* getsockname) (int socket, struct sockaddr *address, socklen_t *address_len);                                                              /* 3904 */
    int (* getsockopt) (int socket, int level, int option_name, void *option_value, socklen_t *option_len);                                          /* 3908 */
    int (* listen) (int socket, int backlog);                                                                                                        /* 3912 */
    ssize_t (* recv) (int socket, void *buffer, size_t length, int flags);                                                                           /* 3916 */
    ssize_t (* recvfrom) (int socket, void *buffer, size_t length,int flags, struct sockaddr *address,socklen_t *address_len);                       /* 3920 */
    ssize_t (* recvmsg) (int socket, struct msghdr *message, int flags);                                                                             /* 3924 */
    ssize_t (* send) (int socket, const void *buffer, size_t length, int flags);                                                                     /* 3928 */
    ssize_t (* sendmsg) (int socket, const struct msghdr *message, int flags);                                                                       /* 3932 */
    ssize_t (* sendto) (int socket, const void *message, size_t length,int flags, const struct sockaddr *dest_addr,socklen_t dest_len);              /* 3936 */
    int (* setsockopt) (int socket, int level, int option_name, const void *option_value, socklen_t option_len);                                     /* 3940 */
    int (* shutdown) (int socket, int how);                                                                                                          /* 3944 */
    int (* socket) (int domain, int type, int protocol);                                                                                             /* 3948 */
    int (* socketpair) (int domain, int type, int protocol, int fds[2]);                                                                             /* 3952 */

    /* sys/stat.h */
    int (* stat) (const char * path_name, struct stat * buffer);                                                                                     /* 3956 */
    int (* fstat) (int file_descriptor, struct stat * buffer);                                                                                       /* 3960 */
    int (* lstat) (const char * path_name, struct stat * buffer);                                                                                    /* 3964 */
    int (* chmod) (const char * path_name, mode_t mode);                                                                                             /* 3968 */
    int (* fchmod) (int file_descriptor, mode_t mode);                                                                                               /* 3972 */
    int (* mkdir) (const char * path_name, mode_t mode);                                                                                             /* 3976 */
    int (* rmdir) (const char * path_name);                                                                                                          /* 3980 */
    mode_t (* umask) (mode_t new_mask);                                                                                                              /* 3984 */

    /* sys/statvfs.h */
    int (* statvfs) (const char *, struct statvfs *);                                                                                                /* 3988 */
    int (* fstatvfs) (int, struct statvfs *);                                                                                                        /* 3992 */

    /* sys/syslog.h */
    void (* closelog) (void);                                                                                                                        /* 3996 */
    void (* openlog) (const char *ident, int opt, int facility);                                                                                     /* 4000 */
    int (* setlogmask) (int maskpri);                                                                                                                /* 4004 */
    void (* syslog) (int priority, const char *message, ...);                                                                                        /* 4008 */
    void (* vsyslog) (int priority, const char *message, va_list args);                                                                              /* 4012 */

    /* sys/systeminfo.h */
    long (* sysinfo) (int cmd,char *buf,long buflen);                                                                                                /* 4016 */

    /* sys/time.h */
    int (* gettimeofday) (struct timeval *tp, struct timezone *tzp);                                                                                 /* 4020 */
    int (* settimeofday) (const struct timeval *, const struct timezone *);                                                                          /* 4024 */
    int (* utimes) (const char *path, const struct timeval *tvp);                                                                                    /* 4028 */
    int (* getitimer) (int which, struct itimerval *curr_value);                                                                                     /* 4032 */
    int (* setitimer) (int which, const struct itimerval *new_value, struct itimerval *old_value);                                                   /* 4036 */

    /* sys/timeb.h */
    int (* ftime) (struct timeb *);                                                                                                                  /* 4040 */

    /* sys/times.h */
    clock_t (* times) (struct tms *tp);                                                                                                              /* 4044 */

    /* sys/uio.h */
    ssize_t (* readv) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                   /* 4048 */
    ssize_t (* writev) (int file_descriptor,const struct iovec *iov,int vec_count);                                                                  /* 4052 */

    /* sys/utsname.h */
    int (* uname) (struct utsname *);                                                                                                                /* 4056 */

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