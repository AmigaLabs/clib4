# C POSIX library header files

| Header file              | Description                                                                           | Status                            |
|--------------------------|---------------------------------------------------------------------------------------|-----------------------------------|
| <aio.h>                  | Asynchronous input and output                                                         | Implemented with some limitations |
| <arpa/inet.h>            | Functions for manipulating numeric IP addresses (part of Berkley sockets)             | Complete                          |
| <assert.h>               | Verify assumptions                                                                    | Complete                          |
| <complex.h>              | Complex Arithmetic                                                                    | Complete                          |
| <cpio.h>                 | Magic numbers for the cpio archive format                                             | Complete                          |
| <ctype.h>                | Character types                                                                       | Complete                          |
| <dirent.h>               | Allows the opening and listing of directories                                         | Complete                          |
| <dlfcn.h>                | Dynamic linking                                                                       | Complete                          |
| <errno.h>                | Retrieving Error Number                                                               | Complete                          |
| <fcntl.h>                | File opening, locking and other operations                                            | Complete                          |
| <fenv.h>                 | Floating-Point Environment (FPE)                                                      | Complete                          |
| <float.h>                | Floating-point types                                                                  | Complete                          |
| <fnmatch.h>              | Filename matching                                                                     | Complete                          |
| <ftw.h>                  | File tree traversal                                                                   | Complete                          |
| <glob.h>                 | Pathname "globbing" (pattern-matching)                                                | Complete                          |
| <grp.h>                  | User group information and control                                                    | Present with missing functions    |
| <iconv.h>                | Codeset conversion facility                                                           | Complete                          |
| <inttypes.h>             | Fixed sized integer types                                                             | Complete                          |
| <iso646.h>               | Alternative spellings                                                                 | Complete                          |
| <langinfo.h>             | Language information constants – builds on C localization functions                   | Complete                          |
| <libgen.h>               | Pathname manipulation                                                                 | Complete                          |
| <limits.h>               | Implementation-defined constants                                                      | Complete                          |
| <math.h>                 | Mathematical declarations                                                             | Complete                          |
| <monetary.h>             | String formatting of monetary units                                                   | Not present                       |
| <mqueue.h>               | Message queue                                                                         | Not present                       |
| <ndbm.h>                 | NDBM database operations                                                              | Complete                          |
| <net/if.h>               | Listing of local network interfaces                                                   | Complete                          |
| <netdb.h>                | Translating protocol and host names into numeric addresses (part of Berkeley sockets  | Complete                          |
| <netinet/in.h>           | Defines Internet protocol and address family (part of Berkley sockets)                | Complete                          |
| <netinet/tcp.h>          | Issue                                                                                 | Complete                          |
| <nl_types.h>             | Localization message catalog functions                                                | Complete                          |
| <poll.h>                 | Asynchronous file descriptor multiplexing                                             | Present with missing functions    |
| <pthread.h>              | Defines an API for creating and manipulating POSIX threads                            | Complete                          |
| <pwd.h>                  | Passwd (user information) access and control                                          | Present with missing functions    |
| <regex.h>                | Regular expression matching                                                           | Complete                          |
| <sched.h>                | Execution scheduling                                                                  | Present with missing functions    |
| <search.h>               | Search tables                                                                         | Complete                          |
| <semaphore.h>            | POSIX semaphores                                                                      | Complete                          |
| <setjmp.h>               | Stack environment declarations                                                        | Complete                          |
| <signal.h>               | Signals                                                                               | Complete                          |
| <spawn.h>                | Process spawning                                                                      | Not present                       |
| <stdarg.h>               | Handle Variable Argument List                                                         | Complete                          |
| <stdbool.h>              | Boolean type and values                                                               | Complete                          |
| <stddef.h>               | Standard type definitions                                                             | Complete                          |
| <stdint.h>               | Integer types                                                                         | Complete                          |
| <stdio.h>                | Standard buffered input/output                                                        | Complete                          |
| <stdlib.h>               | Standard library definitions                                                          | Complete                          |
| <string.h>               | Several String Operations                                                             | Complete                          |
| <strings.h>              | Case-insensitive string comparisons                                                   | Complete                          |
| <stropts.h>              | Stream manipulation, including ioctl                                                  | Not present                       |
| <sys/ipc.h>              | Inter-process Communication (IPC)                                                     | Complete                          |
| <sys/mman.h>             | Memory management, including POSIX shared memory  and memory mapped files             | Present with missing functions    |
| <sys/msg.h>              | POSIX messages queues                                                                 | Complete                          |
| <sys/resource.h>         | Resource usage, priorities, and limiting                                              | Present with missing functions    |
| <sys/select.h>           | Synchronous I/O multiplexing                                                          | Complete                          |
| <sys/sem.h>              | XSI (SysV style)semaphores                                                            | Complete                          |
| <sys/shm.h>              | XSI (SysV style) shared memory                                                        | Complete                          |
| <sys/socket.h>           | Main Berkeley sockets header                                                          | Complete                          |
| <sys/stat.h>             | File information                                                                      | Complete                          |
| <sys/statvfs.h>          | File System information                                                               | Complete                          |
| <sys/time.h>             | Time and date functions and structures                                                | Complete                          |
| <sys/times.h>            | File access and modification times                                                    | Complete                          |
| <sys/types.h>            | Various data types used elsewhere                                                     | Complete                          |
| <sys/uio.h>              | Vectored I/O operations                                                               | Complete                          |
| <sys/un.h>               | Unix domain sockets                                                                   | Complete                          |
| <sys/utsname.h>          | Operating system information, including uname                                         | Complete                          |
| <sys/wait.h>             | Status of terminated child processes                                                  | Present with missing functions    |
| <syslog.h>               | System error logging                                                                  | Complete                          |
| <tar.h>                  | Magic numbers for the tar archive format                                              | Complete                          |
| <termios.h>              | Allows terminal I/O interfaces                                                        | Complete                          |
| <tgmath.h>               | Type-Generic Macros                                                                   | Complete                          |
| <time.h>                 | Type-Generic Macros                                                                   | Complete                          |
| <trace.h>                | Tracing of runtime behavior (DEPRECATED)                                              | Not present                       |
| <ulimit.h>               | Resource limiting (DEPRECATED in favor of <sys/resource.h>)                           | Present with missing functions    |
| <unistd.h>               | Various essential POSIX functions and constants                                       | Complete                          |
| <utime.h>                | access and modification times                                                         | Complete                          |
| <utmpx.h>                | User accounting database functions                                                    | Not present                       |
| <wchar.h>                | Wide-Character Handling                                                               | Complete                          |
| <wctype.h>               | Wide-Character Classification and Mapping Utilities                                   | Complete                          |
| <wordexp.h>              | Word-expansion like the shell would perform                                           | Not present                       |
