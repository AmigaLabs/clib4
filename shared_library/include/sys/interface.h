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
    int  (* library_start)(char *argstr, int arglen, struct Library **_DOSBase, struct DOSIFace **_IDOS, int (* start_main)(int, char **, char **), void (*__CTOR_LIST__[])(void), void (*__DTOR_LIST__[])(void)); //80
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
    void (* aio_init) (const struct aioinit *init);
    int (* aio_read) (struct aiocb *aiocbp);
    int (* aio_write) (struct aiocb *aiocbp);
    int (* lio_listio) (int mode, struct aiocb *const list[restrict], int nent, struct sigevent * sig);
    int (* aio_error) (const struct aiocb *aiocbp);
    ssize_t (* aio_return) (struct aiocb *aiocbp);
    int (* aio_cancel) (int fildes, struct aiocb *aiocbp);
    int (* aio_suspend) (const struct aiocb *const list[], int nent, const struct timespec *timeout);
    int (* aio_fsync) (int operation, struct aiocb *aiocbp);
    int (* aio_read64) (struct aiocb64 *aiocbp);
    int (* aio_write64) (struct aiocb64 *aiocbp);

    /* argz.h */
    error_t (* argz_create) (char *const argv[], char **argz, size_t *argz_len);
    error_t (* argz_create_sep) (const char *string, int sep, char **argz, size_t *argz_len);
    size_t (* argz_count) (const char *argz, size_t argz_len);
    void (* argz_extract) (char *argz, size_t argz_len, char **argv);
    void (* argz_stringify) (char *argz, size_t argz_len, int sep);
    error_t (* argz_add) (char **argz, size_t *argz_len, const char *str);
    error_t (* argz_add_sep) (char **argz, size_t *argz_len, const char *str, int sep);
    error_t (* argz_append) (char **argz, size_t *argz_len, const char *buf, size_t buf_len);
    error_t (* argz_delete) (char **argz, size_t *argz_len, char *entry);
    error_t (* argz_insert) (char **argz, size_t *argz_len, char *before, const char *entry);
    char * (* argz_next) (char *argz, size_t argz_len, const char *entry);
    error_t (* argz_replace) (char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count);

    /* assert.h */
    void (* __assertion_failure) (const char *file_name, int line_number, const char *expression);

    /* complex.h */
    double complex (* cacos) (double complex z);
    float complex (* cacosf) (float complex z);
    long double complex (* cacosl) (long double complex z);
    double complex (* casin) (double complex z);
    float complex (* casinf) (float complex z);
    long double complex (* casinl) (long double complex z);
    double complex (* catan) (double complex z);
    float complex (* catanf) (float complex z);
    long double complex (* catanl) (long double complex z);
    double complex (* ccos) (double complex z);
    float complex (* ccosf) (float complex z);
    long double complex (* ccosl) (long double complex z);
    double complex (* csin) (double complex z);
    float complex (* csinf) (float complex z);
    long double complex (* csinl) (long double complex z);
    double complex (* ctan) (double complex z);
    float complex (* ctanf) (float complex z);
    long double complex (* ctanl) (long double complex z);
    double complex (* cacosh) (double complex z);
    float complex (* cacoshf) (float complex z);
    long double complex (* cacoshl) (long double complex z);
    double complex (* casinh) (double complex z);
    float complex (* casinhf) (float complex z);
    long double complex (* casinhl) (long double complex z);
    double complex (* catanh) (double complex z);
    float complex (* catanhf) (float complex z);
    long double complex (* catanhl) (long double complex z);
    double complex (* ccosh) (double complex z);
    float complex (* ccoshf) (float complex z);
    long double complex (* ccoshl) (long double complex z);
    double complex (* csinh) (double complex z);
    float complex (* csinhf) (float complex z);
    long double complex (* csinhl) (long double complex z);
    double complex (* ctanh) (double complex z);
    float complex (* ctanhf) (float complex z);
    long double complex (* ctanhl) (long double complex z);
    double complex (* cexp) (double complex z);
    float complex (* cexpf) (float complex z);
    long double complex (* cexpl) (long double complex z);
    double complex (* clog) (double complex z);
    float complex (* clogf) (float complex z);
    long double complex (* clogl) (long double complex z);
    double (* cabs) (double complex z);
    float (* cabsf) (float complex z);
    long double (* cabsl) (long double complex z);
    double complex (* cpow) (double complex z, double complex c);
    float complex (* cpowf) (float complex z, float complex c);
    long double complex (* cpowl) (long double complex z, long double complex c);
    double complex (* csqrt) (double complex z);
    float complex (* csqrtf) (float complex z);
    long double complex (* csqrtl) (long double complex z);
    double complex (* carg) (double complex z);
    float complex (* cargf) (float complex z);
    long double complex (* cargl) (long double complex z);
    double complex (* conj) (double complex z);
    float complex (* conjf) (float complex z);
    long double complex (* conjl) (long double complex z);
    double complex (* cproj) (double complex z);
    float complex (* cprojf) (float complex z);
    long double complex (* cprojl) (long double complex z);
    double (* cimag) (double complex z);
    float (* cimagf) (float complex z);
    long double (* cimagl) (long double complex z);
    double (* creal) (double complex z);
    float (* crealf) (float complex z);
    long double (* creall) (long double complex z);

    /* crypt.h */
    char * (* crypt_r) (const char *__phrase, const char *__salt, struct crypt_data * __restrict __data);

    /* ctype.h */
    int (* isalnum) (int c);
    int (* isalpha) (int c);
    int (* iscntrl) (int c);
    int (* isdigit) (int c);
    int (* isgraph) (int c);
    int (* islower) (int c);
    int (* isprint) (int c);
    int (* ispunct) (int c);
    int (* isspace) (int c);
    int (* isupper) (int c);
    int (* isxdigit) (int c);
    int (* tolower) (int c);
    int (* toupper) (int c);
    int (* isblank) (int c);
    int (* isascii) (int c);
    int (* toascii) (int c);

    /* dirent.h */
    DIR * (* opendir) (const char * path_name);
    struct dirent * (* readdir) (DIR * directory_pointer);
    void (* rewinddir) (DIR * directory_pointer);
    int (* closedir) (DIR * directory_pointer);
    DIR * (* fdopendir) (int);
    int (* alphasort) (const struct dirent **a, const struct dirent **b);
    int (* dirfd) (DIR *dirp);
    int (* scandir) (const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));
    int (* readdir_r) (DIR *dir, struct dirent *buf, struct dirent **result);
    int (* readdir64_r) (DIR *dir, struct dirent *buf, struct dirent **result);

    /* dlfcn.h */
    int (* dlclose) (void *__handle);
    const char * (* dlerror) (void);
    void * (* dlopen) (const char *__path_name, int __mode);
    void * (* dlsym) (void *__handle, const char *__symbol_name);

    /* envz.h */
    char * (* envz_entry) (const char *envz, size_t envz_len, const char *name);
    char * (* envz_get) (const char *envz, size_t envz_len, const char *name);
    error_t (* envz_add) (char **envz, size_t *envz_len, const char *name, const char *value);
    error_t (* envz_merge) (char **envz, size_t *envz_len, const char *envz2, size_t envz2_len, int override);
    void (* envz_remove) (char **envz, size_t *envz_len, const char *name);
    void (* envz_strip) (char **envz, size_t *envz_len);

    /* err.h */
    void (* warn) (const char *, ...);
    void (* vwarn) (const char *, va_list);
    void (* warnx) (const char *, ...);
    void (* vwarnx) (const char *, va_list);
    void (* err) (int, const char *, ...);
    void (* verr) (int, const char *, va_list);
    void (* errx) (int, const char *, ...);
    void (* verrx) (int, const char *, va_list);

    /* fcntl.h */
    int (* open) (const char *path_name, int open_flag, ... /* mode_t mode */ );
    int (* openat) (int fd, const char *filename, int flags, ...);
    int (* creat) (const char * path_name, mode_t mode);
    int (* close) (int file_descriptor);
    ssize_t (* read) (int file_descriptor, void * buffer, size_t num_bytes);
    ssize_t (* write) (int file_descriptor, const void * buffer, size_t num_bytes);
    int (* fcntl) (int file_descriptor, int cmd, ... /* int arg */ );

    /* fnmatch.h */
    int (* fnmatch) (const char *pattern, const char *string, int flags);

    /* ftw.h */
    int (* ftw) (const char *path,int (*func)(const char *,const struct stat *,int),int depth);
    int (* nftw) (const char *path,int (*func)(const char *,const struct stat *,int,struct FTW *),int depth,int flags);

    /* getopt.h */
    int (* getopt_long) (int argc, char* const *argv, const char *optstring, const struct option *longopts, int *longindex);
    int (* getopt_long_only) (int argc, char* const *argv, const char *optstring, const struct option *longopts, int *longindex);

    /* glob.h */
    int (* glob) (const char *pattern, int flags, int (*errfunc) (const char *epath, int eerrno), glob_t *pglob);
    void (* globfree) (glob_t *pglob);

    /* grp.h */
    void (* endgrent) (void);
    struct group * (* getgrent) (void);
    struct group * (* getgrgid) (gid_t gid);
    struct group * (* getgrnam) (const char *name);
    void (* setgrent) (void);

    /* iconv.h */
    iconv_t (* iconv_open) (const char *to, const char *from);
    size_t (* iconv) (iconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
    int (* iconv_close) (iconv_t cd);

    /* ifaddrs.h */
    int (* getifaddrs) (struct ifaddrs **ifap);
    void (* freeifaddrs) (struct ifaddrs *ifa);

    /* inttypes.h */
    intmax_t (* imaxabs) (intmax_t x);
    imaxdiv_t (* imaxdiv) (intmax_t n,intmax_t d);
    intmax_t (* strtoimax) (const char *str, char **ptr, int base);
    uintmax_t (* strtoumax) (const char *str, char **ptr, int base);
    intmax_t (* wcstoimax) (const wchar_t *str, wchar_t **ptr, int base);
    uintmax_t (* wcstoumax) (const wchar_t *str, wchar_t **ptr, int base);

    /* langinfo.h */
    char * (* nl_langinfo) (nl_item item);

    /* libgen.h */
    char * (* basename) (const char *path);
    char * (* dirname) (const char *path);

    /* locale.h */
    char * (* setlocale) (int category, const char *locale);
    struct lconv * (* localeconv) (void);

    /* malloc.h */
    void * (* memalign) (size_t alignment, size_t size);

    /* math.h */
    double (* acos) (double x);
    double (* asin) (double x);
    double (* atan) (double x);
    double (* atan2) (double y, double x);
    double (* ceil) (double x);
    double (* cos) (double x);
    double (* cosh) (double x);
    void (* sincos) (double x, double *s, double *c);
    void (* sincosf) (float x, float *s, float *c);
    void (* sincosl) (double x, double *s, double *c);
    double (* exp10) (double x);
    float (* exp10f) (float x);
    long double (* exp10l) (long double x);
    double (* exp) (double x);
    double (* fabs) (double x);
    double (* floor) (double x);
    double (* fmod) (double x, double y);
    double (* frexp) (double x, int *nptr);
    double (* ldexp) (double x, int n);
    double (* log) (double x);
    double (* log10) (double x);
    double (* modf) (double x, double *nptr);
    double (* pow) (double x, double y);
    double (* pow10) (double x);
    float (* pow10f) (float x);
    long double (* pow10l) (long double x);
    double (* sin) (double x);
    double (* sinh) (double x);
    double (* sqrt) (double x);
    double (* tan) (double x);
    double (* tanh) (double x);
    double (* j0) (double x);
    double (* y0) (double x);
    float (* j0f) (float x);
    float (* y0f) (float x);
    double (* j1) (double x);
    double (* y1) (double x);
    float (* j1f) (float x);
    float (* y1f) (float x);
    double (* jn) (int n, double x);
    double (* yn) (int n, double x);
    float (* jnf) (int n, float x);
    float (* ynf) (int n, float x);
    int (* __fpclassify_float) (float x);
    int (* __fpclassify_double) (double x);
    int (* __fpclassify_long_double) (long double x);
    int (* __isfinite_float) (float x);
    int (* __isfinite_double) (double x);
    int (* __isfinite_long_double) (long double x);
    int (* __signbit_float) (float x);
    int (* __signbit_double) (double x);
    int (* __signbit_long_double) (long double x);
    int (* __isnan) (double x);
    int (* __isnanf) (float x);
    int (* __isnanl) (long double x);
    int (* __isinf) (double x);
    int (* __isinff) (float x);
    int (* __isinfl) (long double x);
    int (* __isnormalf) (float x);
    int (* __isnormal) (double x);
    int (* __isnormall) (long double x);
    float (* acosf) (float x);
    float (* asinf) (float x);
    float (* atanf) (float x);
    float (* atan2f) (float y, float x);
    float (* ceilf) (float x);
    float (* cosf) (float x);
    float (* coshf) (float x);
    float (* expf) (float x);
    float (* fabsf) (float x);
    float (* floorf) (float x);
    float (* fmodf) (float x, float y);
    float (* frexpf) (float x, int *eptr);
    float (* ldexpf) (float x, int exp);
    float (* logf) (float x);
    float (* log10f) (float x);
    float (* modff) (float x, float *iptr);
    float (* powf) (float x, float y);
    float (* sinf) (float x);
    float (* sinhf) (float x);
    float (* sqrtf) (float x);
    float (* tanf) (float x);
    float (* tanhf) (float x);
    float (* acoshf) (float x);
    float (* asinhf) (float x);
    float (* atanhf) (float x);
    float (* cbrtf) (float x);
    float (* copysignf) (float x, float y);
    float (* erfcf) (float x);
    float (* erff) (float x);
    float (* expm1f) (float x);
    float (* fdimf) (float x, float y);
    float (* fmaf) (float x, float y, float z);
    float (* fmaxf) (float x, float y);
    float (* fminf) (float x, float y);
    float (* hypotf) (float x, float y);
    float (* lgammaf) (float x);
    float (* log1pf) (float x);
    float (* logbf) (float x);
    long long (* llrintf) (float x);
    long (* lrintf) (float x);
    long (* lroundf) (float x);
    long long (* llround) (double x);
    long long (* llroundf) (float x);
    float (* nanf) (const char *tagp);
    float (* nearbyintf) (float x);
    float (* nextafterf) (float x, float y);
    float (* nexttowardf) (float x, long double y);
    float (* remainderf) (float x, float p);
    float (* remquof) (float x, float y, int *quo);
    float (* rintf) (float x);
    float (* roundf) (float x);
    float (* scalblnf) (float x, long n);
    float (* scalbnf) (float x, int n);
    float (* tgammaf) (float x);
    float (* truncf) (float x);
    int (* ilogbf) (float x);
    int (* finite) (double x);
    double (* acosh) (double x);
    double (* asinh) (double x);
    double (* atanh) (double x);
    double (* cbrt) (double x);
    double (* copysign) (double x, double y);
    double (* erf) (double x);
    double (* erfc) (double x);
    double (* expm1) (double x);
    double (* fdim) (double x, double y);
    double (* fma) (double x, double y, double z);
    double (* fmax) (double x, double y);
    double (* fmin) (double x, double y);
    double (* hypot) (double x, double y);
    double (* lgamma) (double x);
    double (* log1p) (double x);
    double (* logb) (double x);
    long long (* llrint) (double x);
    long int (* lrint) (double x);
    long int (* lround) (double x);
    double (* nan) (const char *tagp);
    double (* nearbyint) (double x);
    double (* nexttoward) (double x, long double y);
    double (* nextafter) (double x, double y);
    double (* remainder) (double x, double p);
    double (* remquo) (double x, double y, int *quo);
    double (* rint) (double x);
    double (* round) (double x);
    double (* scalbln) (double x, long n);
    double (* scalbn) (double x, int n);
    double (* tgamma) (double x);
    double (* trunc) (double x);
    int (* ilogb) (double x);
    float (* exp2f) (float x);
    double (* exp2) (double x);
    long double (* exp2l) (long double x);
    double (* log2) (double x);
    float (* log2f) (float x);
    long double (* log2l) (long double x);
    long double (* acosl) (long double x);
    long double (* asinl) (long double x);
    long double (* atanl) (long double x);
    long double (* atan2l) (long double y, long double x);
    long double (* ceill) (long double x);
    long double (* cosl) (long double x);
    long double (* coshl) (long double x);
    long double (* expl) (long double x);
    long double (* fabsl) (long double x);
    long double (* floorl) (long double x);
    long double (* fmodl) (long double x, long double y);
    long double (* frexpl) (long double x, int *nptr);
    long double (* ldexpl) (long double x, int n);
    long double (* logl) (long double x);
    long double (* log10l) (long double x);
    long double (* modfl) (long double x, long double *iptr);
    long double (* powl) (long double x, long double y);
    long double (* sinl) (long double x);
    long double (* sinhl) (long double x);
    long double (* sqrtl) (long double x);
    long double (* tanl) (long double x);
    long double (* tanhl) (long double x);
    long double (* acoshl) (long double x);
    long double (* asinhl) (long double x);
    long double (* atanhl) (long double x);
    long double (* cbrtl) (long double x);
    long double (* copysignl) (long double x, long double y);
    long double (* erfcl) (long double x);
    long double (* erfl) (long double x);
    long double (* expm1l) (long double x);
    long double (* fdiml) (long double x, long double y);
    long double (* fmal) (long double x, long double y, long double z);
    long double (* fmaxl) (long double x, long double y);
    long double (* fminl) (long double x, long double y);
    long double (* hypotl) (long double x, long double y);
    long double (* lgammal) (long double x);
    long double (* log1pl) (long double x);
    long double (* logbl) (long double x);
    long long int (* llrintl) (long double x);
    long int (* lrintl) (long double x);
    long long int (* llroundl) (long double x);
    long int (* lroundl) (long double x);
    long double (* nanl) (const char *tagp);
    long double (* nearbyintl) (long double x);
    long double (* nextafterl) (long double x, long double y);
    long double (* nexttowardl) (long double x, long double y);
    long double (* remainderl) (long double x, long double p);
    long double (* remquol) (long double x, long double y, int *quo);
    long double (* rintl) (long double x);
    long double (* roundl) (long double x);
    long double (* scalblnl) (long double x, long int n);
    long double (* scalbnl) (long double x, int n);
    long double (* tgammal) (long double x);
    long double (* truncl) (long double x);
    int (* ilogbl) (long double x);

    /* netdb.h */
    struct hostent * (* gethostbyaddr) (const void *addr, socklen_t len, int type);
    struct hostent * (* gethostbyname) (const char *name);
    struct netent * (* getnetbyaddr) (in_addr_t net, int type);
    struct netent * (* getnetbyname) (const char *name);
    struct protoent * (* getprotobyname) (const char *name);
    struct protoent * (* getprotobynumber) (int proto);
    struct servent * (* getservbyname) (const char *name, const char *proto);
    struct servent * (* getservbyport) (int port, const char *proto);
    void (* herror) (const char *);
    const char * (* hstrerror) (int);
    int (* gethostbyname_r) (const char *, struct hostent *, char *, size_t, struct hostent **, int *);
    int (* gethostbyname2_r) (const char *, int, struct hostent *, char *, size_t, struct hostent **, int *);
    struct hostent * (* gethostbyname2) (const char *, int);
    int (* gethostbyaddr_r) (const void *, socklen_t, int, struct hostent *, char *, size_t, struct hostent **, int *);
    int (* getservbyport_r) (int, const char *, struct servent *, char *, size_t, struct servent **);
    int (* getservbyname_r) (const char *, const char *, struct servent *, char *, size_t, struct servent **);
    const char * (* gai_strerror) (int ecode);
    int (* getaddrinfo) (const char *name, const char *service, const struct addrinfo *req, struct addrinfo **pai);
    void (* freeaddrinfo) (struct addrinfo *ai);
    int (* getnameinfo) (const struct sockaddr *sa, socklen_t salen, char *node, socklen_t nodelen, char *service, socklen_t servicelen, int flags);

    /* nl_types.h */
    nl_catd (* catopen) (const char* name, int flag);
    char* (* catgets) (nl_catd catalog, int set_number, int msg_number, const char* msg);
    int (* catclose) (nl_catd catalog);

    /* poll.h */
    int (* poll) (struct pollfd *fds, nfds_t nfds, int timeout);

    /* pthread.h */
    int (* pthread_attr_init) (pthread_attr_t *attr);
    int (* pthread_attr_destroy) (pthread_attr_t *attr);
    int (* pthread_attr_getdetachstate) (const pthread_attr_t *attr, int *detachstate);
    int (* pthread_attr_setdetachstate) (pthread_attr_t *attr, int detachstate);
    int (* pthread_attr_getstack) (const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
    int (* pthread_attr_setstack) (pthread_attr_t *attr, void *stackaddr, size_t stacksize);
    int (* pthread_attr_getstacksize) (const pthread_attr_t *attr, size_t *stacksize);
    int (* pthread_attr_setstacksize) (pthread_attr_t *attr, size_t stacksize);
    int (* pthread_attr_getschedparam) (const pthread_attr_t *attr, struct sched_param *param);
    int (* pthread_attr_setschedparam) (pthread_attr_t *attr, const struct sched_param *param);
    int (* pthread_attr_getschedpolicy) (const pthread_attr_t *attr, int *policy);
    int (* pthread_attr_setschedpolicy) (pthread_attr_t *attr, int policy);
    int (* pthread_attr_getinheritsched) (const pthread_attr_t *attr, int *inheritsched);
    int (* pthread_attr_setinheritsched) (pthread_attr_t *attr, int inheritsched);
    int (* pthread_attr_getscope) (const pthread_attr_t *attr, int *contentionscope);
    int (* pthread_attr_setscope) (pthread_attr_t *attr, int contentionscope);
    int (* pthread_attr_setguardsize) (pthread_attr_t *attr, size_t guardsize);
    int (* pthread_attr_getguardsize) (const pthread_attr_t *attr, size_t *guardsize);
    int (* pthread_create) (pthread_t *thread, const pthread_attr_t *attr, void *(*start)(void *), void *arg);
    int (* pthread_detach) (pthread_t thread);
    int (* pthread_equal) (pthread_t t1, pthread_t t2);
    void (* pthread_exit) (void *value_ptr);
    int (* pthread_join) (pthread_t thread, void **value_ptr);
    pthread_t (* pthread_self) (void);
    int (* pthread_cancel) (pthread_t thread);
    int (* pthread_setcancelstate) (int state, int *oldstate);
    int (* pthread_setcanceltype) (int type, int *oldtype);
    void (* pthread_testcancel) (void);
    int (* pthread_once) (pthread_once_t *once_control, void (*init_routine)(void));
    int (* pthread_setschedprio) (pthread_t thread, int prio);
    int (* pthread_setschedparam) (pthread_t thread, int policy, const struct sched_param *param);
    int (* pthread_getschedparam) (pthread_t thread, int *policy, struct sched_param *param);
    int (* pthread_setconcurrency) (int level);
    int (* pthread_getconcurrency) (void);
    int (* pthread_key_create) (pthread_key_t *key, void (*destructor)(void *));
    int (* pthread_key_delete) (pthread_key_t key);
    int (* pthread_setspecific) (pthread_key_t key, const void *value);
    void * (* pthread_getspecific) (pthread_key_t key);
    int (* pthread_mutexattr_init) (pthread_mutexattr_t *attr);
    int (* pthread_mutexattr_destroy) (pthread_mutexattr_t *attr);
    int (* pthread_mutexattr_getpshared) (const pthread_mutexattr_t *attr, int *pshared);
    int (* pthread_mutexattr_setpshared) (pthread_mutexattr_t *attr, int pshared);
    int (* pthread_mutexattr_gettype) (pthread_mutexattr_t *attr, int *kind);
    int (* pthread_mutexattr_settype) (pthread_mutexattr_t *attr, int kind);
    int (* pthread_mutex_init) (pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
    int (* pthread_mutex_destroy) (pthread_mutex_t *mutex);
    int (* pthread_mutex_lock) (pthread_mutex_t *mutex);
    int (* pthread_mutex_timedlock) (pthread_mutex_t *mutex, const struct timespec *abstime);
    int (* pthread_mutex_trylock) (pthread_mutex_t *mutex);
    int (* pthread_mutex_unlock) (pthread_mutex_t *mutex);
    int (* pthread_condattr_init) (pthread_condattr_t *attr);
    int (* pthread_condattr_destroy) (pthread_condattr_t *attr);
    int (* pthread_condattr_getpshared) (const pthread_condattr_t *attr, int *pshared);
    int (* pthread_condattr_setpshared) (pthread_condattr_t *attr, int pshared);
    int (* pthread_cond_init) (pthread_cond_t *cond, const pthread_condattr_t *attr);
    int (* pthread_cond_destroy) (pthread_cond_t *cond);
    int (* pthread_cond_wait) (pthread_cond_t *cond, pthread_mutex_t *mutex);
    int (* pthread_cond_timedwait) (pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
    int (* pthread_cond_signal) (pthread_cond_t *cond);
    int (* pthread_cond_broadcast) (pthread_cond_t *cond);
    int (* pthread_barrierattr_init) (pthread_barrierattr_t *attr);
    int (* pthread_barrierattr_destroy) (pthread_barrierattr_t *attr);
    int (* pthread_barrierattr_getpshared) (const pthread_barrierattr_t *attr, int *pshared);
    int (* pthread_barrierattr_setpshared) (pthread_barrierattr_t *attr, int pshared);
    int (* pthread_barrier_init) (pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count);
    int (* pthread_barrier_destroy) (pthread_barrier_t *barrier);
    int (* pthread_barrier_wait) (pthread_barrier_t *barrier);
    int (* pthread_rwlockattr_init) (pthread_rwlockattr_t *attr);
    int (* pthread_rwlockattr_destroy) (pthread_rwlockattr_t *attr);
    int (* pthread_rwlockattr_getpshared) (const pthread_rwlockattr_t *attr, int *pshared);
    int (* pthread_rwlockattr_setpshared) (pthread_rwlockattr_t *attr, int pshared);
    int (* pthread_rwlock_init) (pthread_rwlock_t *lock, const pthread_rwlockattr_t *attr);
    int (* pthread_rwlock_destroy) (pthread_rwlock_t *lock);
    int (* pthread_rwlock_tryrdlock) (pthread_rwlock_t *lock);
    int (* pthread_rwlock_trywrlock) (pthread_rwlock_t *lock);
    int (* pthread_rwlock_rdlock) (pthread_rwlock_t *lock);
    int (* pthread_rwlock_timedrdlock) (pthread_rwlock_t *lock, const struct timespec *abstime);
    int (* pthread_rwlock_wrlock) (pthread_rwlock_t *lock);
    int (* pthread_rwlock_timedwrlock) (pthread_rwlock_t *lock, const struct timespec *abstime);
    int (* pthread_rwlock_unlock) (pthread_rwlock_t *lock);
    int (* pthread_spin_init) (pthread_spinlock_t *lock, int pshared);
    int (* pthread_spin_destroy) (pthread_spinlock_t *lock);
    int (* pthread_spin_lock) (pthread_spinlock_t *lock);
    int (* pthread_spin_trylock) (pthread_spinlock_t *lock);
    int (* pthread_spin_unlock) (pthread_spinlock_t *lock);
    int (* pthread_setname_np) (pthread_t thread, const char *name);
    int (* pthread_getname_np) (pthread_t thread, char *name, size_t len);
    int (* pthread_cond_timedwait_relative_np) (pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *reltime);
    int (* pthread_getattr_np) (pthread_t thread, pthread_attr_t *attr);
    void (* pthread_cleanup_push) (void (*routine)(void *), void *arg);
    void (* pthread_cleanup_pop) (int execute);
    int (* pthread_kill) (pthread_t thread, int sig);

    /* pwd.h */
    void (* endpwent) (void);
    struct passwd * (* getpwent) (void);
    struct passwd * (* getpwnam) (const char *name);
    int (* getpwnam_r) (const char *name, struct passwd *pw, char *buffer, size_t bufsize, struct passwd **result);
    struct passwd * (* getpwuid) (uid_t uid);
    int (* getpwuid_r) (uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize, struct passwd **result);
    void (* setpwent) (void);

    /* regex.h */
    int (* regcomp) (regex_t * preg, const char * regex, int cflags);
    int (* regexec) (const regex_t *, const char *, size_t, regmatch_t *, int);
    void (* regfree) (regex_t *preg);
    size_t (* regerror) (int e, const regex_t * preg, char * buf, size_t size);

    /* resolv.h */
    int (* res_init) (void);
    int (* res_query) (const char *dname, int class, int type, unsigned char *answer, int anslen);
    int (* res_querydomain) (const char *name, const char *domain,int class, int type, unsigned char *answer,int anslen);
    int (* res_search) (const char *dname, int class, int type, unsigned char *answer, int anslen);
    int (* res_mkquery) (int, const char *, int, int, const unsigned char *, int, const unsigned char *, unsigned char *, int);
    int (* res_send) (const unsigned char *msg, int msglen, unsigned char *answer,int anslen);
    int (* dn_comp) (unsigned char *exp_dn, unsigned char *comp_dn,int length, unsigned char **dnptrs, unsigned char **lastdnptr);
    int (* dn_expand) (unsigned char *msg, unsigned char *eomorig,unsigned char *comp_dn, char *exp_dn,int length);
    int (* dn_skipname) (const unsigned char *, const unsigned char *);

    /* sched.h */
    int (* sched_get_priority_max) (int);
    int (* sched_get_priority_min) (int);
    int (* sched_yield) (void);

    /* search.h */
    int (* hcreate) (size_t);
    void (* hdestroy) (void);
    ENTRY * (* hsearch) (ENTRY, ACTION);
    int (* hcreate_r) (size_t, struct hsearch_data *);
    void (* hdestroy_r) (struct hsearch_data *);
    int (* hsearch_r) (ENTRY, ACTION, ENTRY **, struct hsearch_data *);
    void * (* lfind) (const void *key, const void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));
    void * (* lsearch) (const void *key, void *base, size_t *nelp, size_t width, int (*compar)(const void *, const void *));
    void * (* tdelete) (const void *vkey, void **vrootp, int (*compar)(const void *, const void *));
    void (* tdestroy) (void *vrootp, void (*freefct)(void *));
    void * (* tfind) (const void *, void **, int (*)(const void *, const void *));
    void * (* tsearch) (const void *, void **, int (*)(const void *, const void *));
    void (* twalk) (const void *root, void (*action)(const void *, VISIT, int));

    /* semaphore.h */
    int (* sem_init) (sem_t *sem, int pshared, unsigned int value);
    int (* sem_destroy) (sem_t *sem);
    int (* sem_trywait) (sem_t *sem);
    int (* sem_wait) (sem_t *sem);
    int (* sem_timedwait) (sem_t *sem, const struct timespec *abstime);
    int (* sem_post) (sem_t *sem);
    sem_t * (* sem_open) (const char *name, int oflag, mode_t mode, unsigned int value);
    int (* sem_close) (sem_t *sem);
    int (* sem_unlink) (const char *name);
    int (* sem_getvalue) (sem_t *sem, int *sval);

    /* setjmp.h */
    int (* setjmp) (jmp_buf __env);
    int (* __sigsetjmp) (struct __jmp_buf_tag __env[1], int __savemask);
    int (* _setjmp) (struct __jmp_buf_tag __env[1]);
    void (* longjmp) (jmp_buf __env, int __val) __attribute__ ((__noreturn__));
    void (* _longjmp) (struct __jmp_buf_tag __env[1], int __val) __attribute__ ((__noreturn__));
    void (* siglongjmp) (sigjmp_buf __env, int __val) __attribute__ ((__noreturn__));
    int (* __sigjmp_save) (jmp_buf __env, int __savemask);

    /* signal.h */
    _sig_func_ptr (* signal) (int, _sig_func_ptr);
    int (* raise) (int sig);
    int (* sigmask) (int signum);
    int (* sigblock) (int signal_mask);
    int (* sigsetmask) (int signal_mask);
    int (* sigprocmask) (int how, const sigset_t *set, sigset_t *oset);
    int (* sigismember) (const sigset_t *set, int sig);
    int (* sigemptyset) (sigset_t *set);
    int (* sigfillset) (sigset_t *set);
    int (* sigdelset) (sigset_t *set, int sig);
    int (* sigaddset) (sigset_t *set, int sig);
    int (* kill) (pid_t pid, int signal_number);
    int (* sigaction) (int sig, const struct sigaction *act, struct sigaction *oact);

    /* stdio.h */
    void (* perror) (const char *s);
    FILE * (* fopen) (const char *filename, const char *mode);
    int (* fclose) (FILE *stream);
    int (* fflush) (FILE *stream);
    FILE * (* freopen) (const char *filename, const char *mode, FILE *stream);
    int (* setvbuf) (FILE *stream, char *buf, int bufmode, size_t size);
    void (* setbuf) (FILE *stream, char *buf);
    int (* fseek) (FILE *stream, long int offset, int wherefrom);
    long int (* ftell) (FILE *stream);
    void (* rewind) (FILE *stream);
    int (* fgetpos) (FILE *stream, fpos_t *pos);
    int (* fsetpos) (FILE *stream, fpos_t *pos);
    int (* fgetc) (FILE *stream);
    int (* getc) (FILE *stream);
    int (* getchar) (void);
    int (* ungetc) (int c, FILE *stream);
    int (* fputc) (int c, FILE *stream);
    int (* putc) (int c, FILE *stream);
    int (* putchar) (int c);
    char * (* fgets) (char *s, int n, FILE *stream);
    char * (* gets) (char *s);
    int (* fscanf) (FILE *stream, const char *format, ...);
    int (* scanf) (const char *format, ...);
    int (* sscanf) (const char *s, const char *format, ...);
    int (* fputs) (const char *s, FILE *stream);
    int (* puts) (const char *s);
    int (* dprintf) (int fd, const char *format, ...);
    int (* fprintf) (FILE *stream, const char *format, ...);
    int (* printf) (const char *format, ...);
    int (* sprintf) (char *s, const char *format, ...);
    int (* vdprintf) (int fd, const char *format, va_list arg);
    int (* vfprintf) (FILE *stream, const char *format, va_list arg);
    int (* vprintf) (const char *format, va_list arg);
    int (* vsprintf) (char *s, const char *format, va_list arg);
    size_t (* fread) (void *ptr, size_t element_size, size_t count, FILE *stream);
    size_t (* fwrite) (const void *ptr, size_t element_size, size_t count, FILE *stream);
    int (* feof) (FILE *stream);
    int (* ferror) (FILE *stream);
    void (* clearerr) (FILE *stream);
    int (* rename) (const char *oldname, const char *newname);
    int (* remove) (const char *filename);
    FILE * (* tmpfile) (void);
    char * (* tmpnam) (char *buf);
    char * (* tempnam) (const char *dir, const char *pfx);
    char * (* ctermid) (char *);
    int (* __flush) (FILE *stream);
    int (* __unlockfile) (FILE *stream, int c);
    int (* getc_unlocked) (FILE *stream);
    int (* getchar_unlocked) (void);
    int (* putc_unlocked) (int c, FILE *stream);
    int (* putchar_unlocked) (int c);
    ssize_t (* getline) (char **lineptr, size_t *n, FILE *stream);
    ssize_t (* getdelim) (char **lineptr, size_t *n, int delim, FILE *stream);
    FILE * (* fdopen) (int file_descriptor, const char *type);
    int (* fileno) (FILE *file);
    int (* asprintf) (char **ret, const char *format, ...);
    int (* vsnprintf) (char *s, size_t size, const char *format, va_list arg);
    int (* pclose) (FILE *stream);
    FILE * (* popen) (const char *command, const char *type);
    int (* fseeko) (FILE *stream, off_t offset, int wherefrom);
    off_t (* ftello) (FILE *stream);
    void (* flockfile) (FILE *file);
    void (* funlockfile) (FILE *file);
    int (* ftrylockfile) (FILE *file);
    int (* vasprintf) (char **ret, const char *format, va_list arg);
    int (* vfscanf) (FILE *stream, const char *format, va_list arg);
    int (* vsscanf) (const char *s, const char *format, va_list arg);
    int (* vscanf) (const char *format, va_list arg);
    int (* snprintf) (char *s, size_t size, const char *format, ...);

    /* stdio_ext.h */
    void (* _flushlbf) (void);
    int (* __fsetlocking) (FILE *, int);
    int (* __fwriting) (FILE *);
    int (* __freading) (FILE *);
    int (* __freadable) (FILE *);
    int (* __fwritable) (FILE *);
    int (* __flbf) (FILE *);
    size_t (* __fbufsize) (FILE *);
    size_t (* __fpending) (FILE *);
    int (* __fpurge) (FILE *);
    void (* fpurge) (FILE *);
    void (* __fseterr) (FILE *);
    size_t (* __freadahead) (FILE *);
    void (* __freadptrinc) (FILE *f, size_t inc);

    /* stdlib.h */
    int * (* __mb_cur_max) (void);
    int (* mblen) (const char *s, size_t n);
    int (* mbtowc) (wchar_t *pwc, const char *s, size_t n);
    int (* _mbtowc_r) (wchar_t *pwc, const char *s, size_t n, _mbstate_t *st);
    size_t (* _mbstowcs_r) (wchar_t *pwcs, const char *s, size_t n, mbstate_t *state);
    int (* wctomb) (char *s, wchar_t wchar);
    int (* _wctomb_r) (char *s, wchar_t wchar, mbstate_t *state);
    size_t (* mbstowcs) (wchar_t *ws, const char *s, size_t wn);
    size_t (* wcstombs) (char *s, const wchar_t *pwcs, size_t n);
    size_t (* _wcstombs_r) (char *s, const wchar_t *pwcs, size_t n, mbstate_t *state);
    void * (* malloc) (size_t size);
    void * (* calloc) (size_t num_elements, size_t element_size);
    void (* free) (void *ptr);
    void * (* realloc) (void *ptr, size_t size);
    void * (* valloc) (size_t size);
    void * (* aligned_alloc) (size_t alignment, size_t size);
    int (* posix_memalign) (void **memptr, size_t alignment, size_t size);
    int (* abs) (int x);
    long (* labs) (long x);
    div_t (* div) (int n, int d);
    ldiv_t (* ldiv) (long n, long d);
    int (* rand) (void);
    void (* srand) (unsigned int seed);
    long (* random) (void);
    void (* srandom) (unsigned int seed);
    char * (* initstate) (unsigned int seed, char *state, size_t size);
    char * (* setstate) (const char *state);
    char * (* l64a) (long x0);
    long (* a64l) (const char *s);
    void (* setkey) (const char *key);
    int (* system) (const char *command);
    void (* exit) (int status);
    void (* abort) (void);
    int (* atexit) (void (*)(void));
    char * (* getenv) (const char *name);
    void * (* bsearch) (const void *key, const void *base, size_t count, size_t size, int (*compare)(const void *key, const void *value));
    void (* qsort) (void *base, size_t count, size_t size, int (*compare)(const void *element1, const void *element2));
    double (* strtod) (const char *str, char **ptr);
    long (* strtol) (const char *str, char **ptr, int base);
    unsigned long (* strtoul) (const char *str, char **ptr, int base);
    quad_t (* strtoq) (const char *nptr, char **endptr, register int base);
    double (* atof) (const char *str);
    int (* atoi) (const char *str);
    long (* atol) (const char *str);
    char * (* itoa) (int value, char *buffer, int base);
    char * (* lltoa) (int64_t ll, char *buffer, int radix);
    void (* _exit) (int status);
    int (* rand_r) (unsigned int *seed);
    char * (* mktemp) (char *name_template);
    int (* mkstemp) (char *name_template);
    int (* unsetenv) (const char *name);
    int (* setenv) (const char *name, const char *value, int overwrite);
    int (* putenv) (const char *string);
    char * (* mkdtemp) (char *name_template);
    const char * (* getexecname) (void);
    long long (* strtoll) (const char *str, char **ptr, int base);
    long double (* strtold) (const char *str, char **ptr);
    unsigned long long (* strtoull) (const char *str, char **ptr, int base);
    long long (* atoll) (const char *str);
    long long (* llabs) (long long x);
    lldiv_t (* lldiv) (long long n, long long d);
    float (* strtof) (const char *str, char **ptr);
    float (* atoff) (const char *nptr);
    void (* _Exit) (int status);
    char * (* ecvt) (double x, int n, int *dp, int *sign);
    char * (* fcvt) (double x, int n, int *dp, int *sign);
    char * (* gcvt) (double x, int n, char *b);
    char * (* secure_getenv) (const char *name);
    void * (* reallocarray) (void *ptr, size_t m, size_t n);
    void (* qsort_r) (void *, size_t, size_t, int (*)(const void *, const void *, void *), void *);
    uint32_t (* arc4random) (void);
    void (* arc4random_buf) (void *buf, size_t nbytes);
    uint32_t (* arc4random_uniform) (uint32_t upper_bound);
    void (* arc4random_stir) (void);
    void (* arc4random_addrandom) (unsigned char *dat, int datlen);

    /* string.h */
    char * (* strerror) (int error_number);
    char * (* strcat) (char *dest, const char *src);
    char * (* strncat) (char *dest, const char *src, size_t n);
    int (* strcmp) (const char *s1, const char * s2);
    int (* strncmp) (const char *s1, const char *s2, size_t n);
    char * (* strcpy) (char *dest, const char *src);
    char * (* strncpy) (char *dest, const char *src, size_t n);
    size_t (* strnlen) (const char *s, size_t maxlen);
    size_t (* strlen) (const char *s);
    char * (* strchr) (const char *s, int c);
    char * (* strrchr) (const char *s, int c);
    size_t (* strspn) (const char *s, const char *set);
    size_t (* strcspn) (const char *s, const char *set);
    char * (* strpbrk) (const char *s, const char *set);
    char * (* strtok) (char *str, const char *set);
    char * (* strstr) (const char *src, const char *sub);
    char * (* strnstr) (const char *src, const char *sub, size_t len);
    char * (* strsep) (char ** src, const char *delim);
    char * (* stpcpy) (char *dest, const char *src);
    char * (* stpncpy) (char *dest, const char *src, size_t len);
    size_t (* stccpy) (char * dest, const char * src, size_t n);
    int (* strcoll) (const char *s1, const char *s2);
    size_t (* strxfrm) (char *dest, const char *src, size_t len);
    void * (* memmove) (void *dest, const void * src, size_t len);
    void * (* memchr) (const void * ptr, int val, size_t len);
    int (* memcmp) (const void *ptr1, const void *ptr2, size_t len);
    void * (* memcpy) (void *dest, const void *src, size_t len);
    void * (* memset) (void *ptr, int val, size_t len);
    int (* strerror_r) (int error,char * buffer,size_t buffer_size);
    char * (* strdup) (const char *s);
    char * (* strndup) (const char *str, size_t max);
    void (* bcopy) (const void *from,void *to,size_t len);
    void (* bzero) (void *m,size_t len);
    void (* explicit_bzero) (void *m, size_t len);
    int (* bcmp) (const void *a,const void *b,size_t len);
    size_t (* strlcpy) (char *dst, const char *src, size_t siz);
    size_t (* strlcat) (char *dst, const char *src, size_t siz);
    char * (* strtok_r) (char *str, const char *separator_set,char ** state_ptr);
    char * (* strsignal) (int);
    void * (* memccpy) (void *dst0, const void *src0, int endchar0, size_t len0);
    char * (* index) (const char *s, int c);
    char * (* rindex) (const char *s, int c);
    int (* strverscmp) (const char *s1, const char *s2);
    char * (* strchrnul) (const char *s, int c_in);
    char * (* strcasestr) (const char *haystack, const char *needle);
    void * (* memmem) (const void *h0, size_t k, const void *n0, size_t l);
    void * (* memrchr) (const void *s, int c, size_t n);
    void * (* mempcpy) (void *dst0, const void *src0, size_t len0);

    /* strings.h */
    int (* strcasecmp) (const char *s1, const char *s2);
    int (* strncasecmp) (const char *s1, const char *s2, size_t len);
    int (* ffs) (int i);
    int (* ffsl) (long i);
    int (* ffsll) (long long i);

    /* termcap.h */
    int (* tgetent) (char *buffer, const char *termtype);
    int (* tgetnum) (const char *name);
    int (* tgetflag) (const char *name);
    char * (* tgetstr) (const char *name, char **area);
    void (* tputs) (const char *str, int nlines, int (*outfun) (int));
    char * (* tparam) (const char *string, char *outstring, int leb, int arg0, int arg1, int arg2, int arg3);
    char * (* tgoto) (const char *cstring, int hpos, int vpos);

    /* termios.h */
    int (* tcgetattr) (int file_descriptor, struct termios *tios_p);
    int (* tcsetattr) (int file_descriptor, int action, struct termios *tios_p);
    int (* tcsendbreak) (int file_descriptor, int duration);
    int (* tcdrain) (int file_descriptor);
    int (* tcflush) (int file_descriptor, int what);
    int (* tcflow) (int file_descriptor, int action);
    int (* cfmakeraw) (struct termios *tios_p);
    speed_t (* cfgetispeed) (const struct termios *tios_p);
    speed_t (* cfgetospeed) (const struct termios *tios_p);
    int (* cfsetispeed) (struct termios *tios_p, speed_t input_speed);
    int (* cfsetospeed) (struct termios *tios_p, speed_t output_speed);

    /* time.h */
    clock_t (* clock) (void);
    time_t (* time) (time_t *t);
    char * (* asctime) (const struct tm *tm);
    char * (* ctime) (const time_t *t);
    struct tm * (* gmtime) (const time_t *t);
    struct tm * (* localtime) (const time_t *t);
    time_t (* mktime) (struct tm *tm);
    double (* difftime) (time_t t1, time_t t0);
    size_t (* strftime) (char *s, size_t maxsize, const char *format, const struct tm *tm);
    char * (* strptime) (const char *buf, const char *fmt, struct tm *timeptr);
    char * (* asctime_r) (const struct tm *tm, char *buffer);
    char * (* ctime_r) (const time_t *tptr, char *buffer);
    struct tm * (* gmtime_r) (const time_t *t, struct tm *tm_ptr);
    struct tm * (* localtime_r) (const time_t *t, struct tm *tm_ptr);
    void (* tzset) (void);
    int (* nanosleep) (const struct timespec *req, struct timespec *rem);
    int (* clock_gettime) (clockid_t clk_id, struct timespec *t);
    int (* clock_settime) (clockid_t clk_id, const struct timespec *t);
    int (* clock_getres) (clockid_t clock_id, struct timespec *res);
    int (* clock_nanosleep) (clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);
    unsigned long long (* rdtsc) (void);
    int (* clock_gettime64) (clockid_t clock_id, struct timespec64 *tp);

    /* uchar.h */
    size_t (* c16rtomb) (char *__restrict, char16_t, mbstate_t *__restrict);
    size_t (* mbrtoc16) (char16_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);
    size_t (* c32rtomb) (char *__restrict, char32_t, mbstate_t *__restrict);
    size_t (* mbrtoc32) (char32_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);

    /* ulimit.h */
    long (* ulimit) (int cmd,long newlimit);

    /* unistd.h */
    int (* isatty) (int file_descriptor);
    int (* dup) (int file_descriptor);
    int (* dup2) (int file_descriptor1, int file_descriptor2);
    off_t (* lseek) (int file_descriptor, off_t offset, int mode);
    int (* access) (const char * path_name, int mode);
    int (* chown) (const char * path_name, uid_t owner, gid_t group);
    int (* fchown) (int file_descriptor, uid_t owner, gid_t group);
    int (* lchown) (const char * path_name, uid_t owner, gid_t group);
    int (* truncate) (const char * path_name, off_t length);
    int (* ftruncate) (int file_descriptor, off_t length);
    int (* getpagesize) (void);
    int (* ftruncate64) (int file_descriptor, _off64_t length);
    int (* truncate64) (const char *path_name, _off64_t length);
    off64_t (* lseek64) (int fd, off64_t offset, int whence);
    unsigned int (* alarm) (unsigned seconds);
    useconds_t (* ualarm) (useconds_t usecs, useconds_t interval);
    int (* link) (const char * existing_path,const char * new_path);
    int (* unlink) (const char * path_name);
    int (* symlink) (const char * actual_path, const char * symbolic_path);
    int (* readlink) (const char * path_name, char * buffer, int buffer_size);
    int (* chdir) (const char * path_name);
    int (* fchdir) (int file_descriptor);
    int (* lockf) (int file_descriptor, int function, off_t size);
    unsigned int (* sleep) (unsigned int seconds);
    int (* usleep) (useconds_t microseconds);
    int (* pause) (void);
    int (* getopt) (int argc, char * const argv[], const char *opts);
    pid_t (* getpid) (void);
    pid_t (* getppid) (void);
    pid_t (* getpgrp) (void );
    pid_t (* tcgetpgrp) (int fildes);
    int (* tcsetpgrp) (int filedes, pid_t pgrp);
    char * (* realpath) (const char *file_name, char *resolved_name);
    int (* fsync) (int file_descriptor);
    int (* fdatasync) (int file_descriptor);
    char * (* ttyname) (int);
    int (* ttyname_r) (int file_descriptor,char *name,size_t buflen);
    int (* execl) (const char *path,const char *arg0,...);
    int (* execle) (const char *path,const char *arg0,...);
    int (* execlp) (const char *path,const char *arg0,...);
    int (* execv) (const char *path,char * const argv[]);
    int (* execve) (const char *path,char *const argv[],char *const envp[]);
    int (* execvp) (const char *command,char * const argv[]);
    void (* encrypt) (char *block, int edflag);
    int (* spawnv) (int mode, const char *file, const char **argv);
    int (* spawnvp) (int mode, const char *path, const char **argv);
    int (* profil) (unsigned short *buffer, size_t bufSize, size_t offset, unsigned int scale);
    long (* sysconf) (int name);
    void (* enableUnixPaths) (void);
    void (* disableUnixPaths) (void);
    void (* enableAltivec) (void);
    void (* disableAltivec) (void);
    void (* enableOptimizedFunctions) (void);
    void (* disableOptimizedFunctions) (void);
    char * (* getcwd) (char * buffer, size_t buffer_size);
    char * (* getwd) (char *buf);
    char * (* get_current_dir_name) (void);
    long (* gethostid) ();
    int (* gethostname) (const char *name, size_t len);
    int (* getdomainname) (char *name, size_t len);
    int (* setdomainname) (const char *name, size_t len);
    char * (* getlogin) (void);
    int (* getlogin_r) (char *buf, size_t bufsize);
    char * (* crypt) (const char *key, const char *salt);
    gid_t (* getegid) (void);
    uid_t (* geteuid) (void);
    gid_t (* getgid) (void);
    int (* getgroups) (int gidsetsize, gid_t grouplist[]);
    char * (* getpass) (const char *prompt);
    uid_t (* getuid) (void);
    int (* initgroups) (const char *name, gid_t basegid);
    int (* setegid) (gid_t gid);
    int (* seteuid) (uid_t uid);
    int (* setgid) (gid_t gid);
    int (* setgroups) (int ngroups, const gid_t *gidset);
    int (* setregid) (gid_t rgid, gid_t egid);
    int (* setreuid) (uid_t ruid, uid_t euid);
    pid_t (* setsid) (void);
    int (* setuid) (uid_t uid);
    int (* setlogin) (const char *name);
    long (* pathconf) (const char *path,int name);
    long (* fpathconf) (int file_descriptor,int name);
    int (* pipe) (int fd[2]);
    int (* pipe2) (int fd[2], int flags);
    ssize_t (* pread) (int fd, void *buf, size_t nbytes, off_t offset);
    ssize_t (* pwrite) (int fd, const void *buf, size_t nbytes, off_t offset);
    ssize_t (* pread64) (int fd, void *buf, size_t nbytes, off64_t offset);
    ssize_t (* pwrite64) (int fd, const void *buf, size_t nbytes, off64_t offset);

    /* utime.h */
    int (* utime) (const char * path_name,const struct utimbuf * times);

    /* wchar.h */
    wint_t (* btowc) (int c);
    int (* wctob) (wint_t c);
    wchar_t * (* wcscat) (wchar_t *dest, const wchar_t *src);
    wchar_t * (* wcsncat) (wchar_t *dest, const wchar_t *src, size_t n);
    int (* wcscmp) (const wchar_t *s1, const wchar_t *s2);
    int (* wcsncmp) (const wchar_t *s1, const wchar_t *s2, size_t n);
    wchar_t * (* wcscpy) (wchar_t *dest, const wchar_t *src);
    wchar_t * (* wcsncpy) (wchar_t *dest, const wchar_t *src, size_t n);
    size_t (* wcslen) (const wchar_t *s);
    wchar_t * (* wcschr) (const wchar_t *s, wchar_t c);
    size_t (* wcsspn) (const wchar_t *s, const wchar_t *set);
    wchar_t * (* wcspbrk) (const wchar_t *s, const wchar_t *set);
    wchar_t * (* wcstok) (wchar_t *str, const wchar_t *delim, wchar_t **ptr);
    wchar_t * (* wcsstr) (const wchar_t *big, const wchar_t *little);
    int (* wcswidth) (const wchar_t *pwcs, size_t n);
    int (* wcwidth) (const wchar_t wc);
    double (* wcstod) (const wchar_t *str, wchar_t **ptr);
    long (* wcstol) (const wchar_t *str, wchar_t **ptr, int base);
    unsigned long (* wcstoul) (const wchar_t *str, wchar_t **ptr, int base);
    float (* wcstof) (const wchar_t *nptr, wchar_t **endptr);
    int (* wscoll) (const wchar_t *s1, const wchar_t *s2);
    size_t (* wcsxfrm) (wchar_t *dest, const wchar_t *src, size_t len);
    wchar_t * (* wmemchr) (const wchar_t *ptr, wchar_t val, size_t len);
    int (* wmemcmp) (const wchar_t *ptr1, const wchar_t *ptr2, size_t len);
    wchar_t * (* wmemcpy) (wchar_t *dest, const wchar_t *src, size_t len);
    wchar_t * (* wmemmove) (wchar_t *dest, const wchar_t *src, size_t len);
    wchar_t * (* wmemset) (wchar_t *ptr, int val, size_t len);
    int (* fwide) (FILE *stream, int orient);
    wint_t (* fgetwc) (FILE *stream);
    wint_t (* getwc) (FILE *stream);
    wint_t (* getwchar) (void);
    wint_t (* ungetwc) (wint_t wc, FILE *fp);
    wchar_t * (* fgetws) (wchar_t *s, int n, FILE *stream);
    int (* fwscanf) (FILE *stream, const wchar_t *format, ...);
    int (* swscanf) (const wchar_t *s, const wchar_t *format, ...);
    int (* vfwscanf) (FILE *f, const wchar_t *format, va_list ap);
    int (* vswscanf) (const wchar_t *s, const wchar_t *format, va_list ap);
    int (* vwscanf) (const wchar_t *s, va_list ap);
    int (* wscanf) (const wchar_t *format, ...);
    wint_t (* fputwc) (wchar_t c, FILE *stream);
    wint_t (* putwc) (wchar_t c, FILE *stream);
    wint_t (* putwchar) (wchar_t c);
    wint_t (* fputws) (const wchar_t *s, FILE *stream);
    int (* fwprintf) (FILE *stream, const wchar_t *format, ...);
    int (* swprintf) (wchar_t *restrict s, size_t l, const wchar_t *restrict fmt, ...);
    int (* vfwprintf) (FILE *stream, const wchar_t *format, va_list arg);
    int (* vswprintf) (wchar_t *s, size_t maxlen, const wchar_t *format, va_list arg);
    int (* vwprintf) (const wchar_t *format, va_list arg);
    int (* wprintf) (const wchar_t *format, ...);
    size_t (* wcsftime) (wchar_t *s, size_t maxsize, const wchar_t *format, const struct tm *timeptr);
    size_t (* mbrlen) (const char *restrict s, size_t n, mbstate_t *restrict ps);
    size_t (* mbrtowc) (wchar_t *pwc, const char *src, size_t n, mbstate_t *ps);
    int (* mbsinit) (const mbstate_t *ps);
    size_t (* mbsrtowcs) (wchar_t *restrict dst, const char **restrict src, size_t len, mbstate_t *restrict ps);
    size_t (* wcrtomb) (char *restrict s, wchar_t wc, mbstate_t *restrict ps);
    int (* wcscoll) (const wchar_t *ws1, const wchar_t *ws2);
    size_t (* wcscspn) (const wchar_t *s, const wchar_t *c);
    wchar_t * (* wcsrchr) (const wchar_t *ws, wchar_t wc);
    size_t (* wcsrtombs) (char *restrict dst, const wchar_t **restrict src, size_t len, mbstate_t *restrict ps);
    long long (* wcstoll) (const wchar_t *str, wchar_t **ptr, int base);
    unsigned long long (* wcstoull) (const wchar_t *str, wchar_t **ptr, int base);
    long double (* wcstold) (const wchar_t *nptr, wchar_t **endptr);
    size_t (* mbsnrtowcs) (wchar_t *dst, const char **restrict src, size_t nmc, size_t len, mbstate_t *restrict ps);
    size_t (* wcsnrtombs) (char *restrict dst, const wchar_t **restrict src, size_t nwc, size_t len, mbstate_t *restrict ps);
    wchar_t * (* wcsdup) (const wchar_t *src);
    size_t (* wcsnlen) (const wchar_t *src, size_t n);
    wchar_t * (* wcpcpy) (wchar_t *dst, const wchar_t *src);
    wchar_t * (* wcpncpy) (wchar_t *dst, const wchar_t *src, size_t len);
    int (* wcscasecmp) (const wchar_t *l, const wchar_t *r);
    int (* wcscasecmp_l) (const wchar_t *l, const wchar_t *r, locale_t loc);
    int (* wcsncasecmp) (const wchar_t *l, const wchar_t *r, size_t n);
    int (* wcsncasecmp_l) (const wchar_t *l, const wchar_t *r, size_t n, locale_t loc);
    int (* wcscoll_l) (const wchar_t *ws1, const wchar_t *ws2, locale_t loc);
    size_t (* wcsxfrm_l) (wchar_t *dest, const wchar_t *src, size_t len, locale_t loc);

    /* wctype.h */
    int (* iswalnum) (wint_t c);
    int (* iswalpha) (wint_t c);
    int (* iswcntrl) (wint_t c);
    int (* iswdigit) (wint_t c);
    int (* iswxdigit) (wint_t c);
    int (* iswgraph) (wint_t c);
    int (* iswpunct) (wint_t c);
    int (* iswprint) (wint_t c);
    int (* iswlower) (wint_t c);
    int (* iswupper) (wint_t c);
    int (* iswspace) (wint_t c);
    int (* iswblank) (wint_t c);
    wint_t (* towlower) (wint_t c);
    wint_t (* towupper) (wint_t c);
    wctype_t (* wctype) (const char *property);
    int (* iswctype) (wint_t c, wctype_t desc);
    wctrans_t (* wctrans) (const char *property);
    wint_t (* towctrans) (wint_t c, wctrans_t desc);

    /* arpa/inet.h */
    in_addr_t (* inet_addr) (const char *cp);
    int (* inet_aton) (const char *cp, struct in_addr *pin);
    in_addr_t (* inet_lnaof) (struct in_addr in);
    struct in_addr (* inet_makeaddr) (in_addr_t net, in_addr_t lna);
    in_addr_t (* inet_netof) (struct in_addr in);
    in_addr_t (* inet_network) (const char *cp);
    char * (* inet_ntoa) (struct in_addr in);
    const char * (* inet_ntop) (int af, const void *src, char *dst, socklen_t cnt);
    int (* inet_pton) (int af, const char *src, void *dst);

    /* arpa/nameser.h */
    unsigned (* ns_get16) (const unsigned char *);
    unsigned long (* ns_get32) (const unsigned char *);
    void (* ns_put16) (unsigned, unsigned char *);
    void (* ns_put32) (unsigned long, unsigned char *);
    int (* ns_initparse) (const unsigned char *, int, ns_msg *);
    int (* ns_parserr) (ns_msg *, ns_sect, int, ns_rr *);
    int (* ns_skiprr) (const unsigned char *, const unsigned char *, ns_sect, int);
    int (* ns_name_uncompress) (const unsigned char *, const unsigned char *, const unsigned char *, char *, size_t);

    /* net/if.h */
    unsigned int (* if_nametoindex) (const char *ifname);
    char * (* if_indextoname) (unsigned int ifindex, char *ifname);

    /* netinet/in.h */
    int (* bindresvport) (int sd, struct sockaddr_in *sa);
    int (* bindresvport6) (int sd, struct sockaddr_in6 *sa);

    /* sys/byteswap.h */
    __CONST_FUNC uint16_t (* bswap16) (uint16_t);
    __CONST_FUNC uint32_t (* bswap24) (uint32_t);
    __CONST_FUNC uint32_t (* bswap32) (uint32_t);
    __CONST_FUNC uint64_t (* bswap64) (uint64_t);
    void * (* swab) (void *from,void *to,ssize_t nbytes);
    void * (* swab24) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 24-bit words instead. */
    void * (* swab32) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 32-bit words instead. */
    void * (* swab64) (void *from,void *to,ssize_t nbytes);	/* Same as swab(), but operates on 64-bit words instead. */

    /* sys/file.h */
    int (* flock) (int fd, int op);

    /* sys/ioctl.h */
    int (* ioctl) (int fildes, int request, ... /* arg */);

    /* sys/ipc.h */
    key_t (* ftok) (const char *path, int id);

    /* sys/mman.h */
    void * (* mmap) (void *addr, size_t len, int prot, int flags, int fd, off_t offset);
    int (* munmap) (void *map, size_t length);
    int (* msync) (void *addr, size_t len, int flags);

    /* sys/mount.h */
    int (* statfs) (const char *path, struct statfs *buf);
    int (* fstatfs) (int fd, struct statfs *buf);

    /* sys/msg.h */
    int (* _msgctl) (int msqid, int cmd, struct msqid_ds *buf);
    int (* _msgget) (key_t key, int msgflg);
    ssize_t (* _msgrcv) (int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
    int (* _msgsnd) (int msqid, const void *msgp, size_t msgsz, int msgflg);
    int (* _msgsnap) (int msqid, void *buf, size_t bufsz, long msgtyp);
    int (* _msgids) (int *buf, size_t nids, size_t  *pnids);

    /* sys/resource.h */
    int (* getrlimit) (int resource, struct rlimit *rlp);
    int (* setrlimit) (int resource, const struct rlimit *rlp);
    int (* getrusage) (int who, struct rusage *usage);

    /* sys/select.h */
    int (* select) (int nfds, fd_set *readfds,fd_set *writefds, fd_set *errorfds,struct timeval *timeout);
    int (* waitselect) (int num_fds,fd_set *read_fds,fd_set *write_fds,fd_set *except_fds,struct timeval *timeout,unsigned long * signal_mask);

    /* sys/sem.h */
    int (* _semctl) (int semid, int semnum, int cmd, union semun aun);
    int (* _semget) (key_t key, int nsems, int flags);
    int (* _semop) (int semid, const struct sembuf *ops, int nops);
    long int (* _semids) (long int *buf, uint32_t nids, long unsigned int *idcnt);
    int (* _semtimedop) (int semid, const struct sembuf *ops, int nops, struct timespec *to);

    /* sys/shm.h */
    void * (* _shmat) (int shmid, const void *shmaddr, int shmflg);
    int (* _shmctl) (int shmid, int cmd, struct shmid_ds *buf);
    int (* _shmdt) (const void *shmaddr);
    int (* _shmget) (key_t key, size_t size, int shmflg);
    int (* _shmids) (int *buf, size_t nids, size_t *pnids);

    /* sys/socket.h */
    int (* accept) (int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);
    int (* bind) (int socket, const struct sockaddr *address, socklen_t address_len);
    int (* connect) (int socket, const struct sockaddr *address, socklen_t address_len);
    int (* getpeername) (int socket, struct sockaddr *address, socklen_t *address_len);
    int (* getsockname) (int socket, struct sockaddr *address, socklen_t *address_len);
    int (* getsockopt) (int socket, int level, int option_name, void *option_value, socklen_t *option_len);
    int (* listen) (int socket, int backlog);
    ssize_t (* recv) (int socket, void *buffer, size_t length, int flags);
    ssize_t (* recvfrom) (int socket, void *buffer, size_t length,int flags, struct sockaddr *address,socklen_t *address_len);
    ssize_t (* recvmsg) (int socket, struct msghdr *message, int flags);
    ssize_t (* send) (int socket, const void *buffer, size_t length, int flags);
    ssize_t (* sendmsg) (int socket, const struct msghdr *message, int flags);
    ssize_t (* sendto) (int socket, const void *message, size_t length,int flags, const struct sockaddr *dest_addr,socklen_t dest_len);
    int (* setsockopt) (int socket, int level, int option_name, const void *option_value, socklen_t option_len);
    int (* shutdown) (int socket, int how);
    int (* socket) (int domain, int type, int protocol);
    int (* socketpair) (int domain, int type, int protocol, int fds[2]);

    /* sys/stat.h */
    int (* stat) (const char * path_name, struct stat * buffer);
    int (* fstat) (int file_descriptor, struct stat * buffer);
    int (* lstat) (const char * path_name, struct stat * buffer);
    int (* chmod) (const char * path_name, mode_t mode);
    int (* fchmod) (int file_descriptor, mode_t mode);
    int (* mkdir) (const char * path_name, mode_t mode);
    int (* rmdir) (const char * path_name);
    mode_t (* umask) (mode_t new_mask);

    /* sys/statvfs.h */
    int (* statvfs) (const char *, struct statvfs *);
    int (* fstatvfs) (int, struct statvfs *);

    /* sys/syslog.h */
    void (* closelog) (void);
    void (* openlog) (const char *ident, int opt, int facility);
    int (* setlogmask) (int maskpri);
    void (* syslog) (int priority, const char *message, ...);
    void (* vsyslog) (int priority, const char *message, va_list args);

    /* sys/systeminfo.h */
    long (* sysinfo) (int cmd,char *buf,long buflen);

    /* sys/time.h */
    int (* gettimeofday) (struct timeval *tp, struct timezone *tzp);
    int (* settimeofday) (const struct timeval *, const struct timezone *);
    int (* utimes) (const char *path, const struct timeval *tvp);
    int (* getitimer) (int which, struct itimerval *curr_value);
    int (* setitimer) (int which, const struct itimerval *new_value, struct itimerval *old_value);

    /* sys/timeb.h */
    int (* ftime) (struct timeb *);

    /* sys/times.h */
    clock_t (* times) (struct tms *tp);

    /* sys/uio.h */
    ssize_t (* readv) (int file_descriptor,const struct iovec *iov,int vec_count);
    ssize_t (* writev) (int file_descriptor,const struct iovec *iov,int vec_count);

    /* sys/utsname.h */
    int (* uname) (struct utsname *);
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