# Clib4 - C runtime library for AmigaOS4

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

## What is this?

This is a fork of <a href="https://github.com/adtools/clib2">official clib2</a> present in adtools.
The point of this library is to make it Amiga OS4 only to maintain it easily and add all missing clib2 features that are
hard to add also on classic amigas.
Classic amigas has also ixemul that is the most complete, <a href="POSIX.md">POSIX</a> compliant, library we have. It is
useless share code that most probably no one will use.
Te goal is to try to make it <a href="POSIX.md">POSIX</a> compliant and fix also all C++ problems we have with newer
compilers.
All warnings (except really few one) produced by GCC 10 and above are now gone.  
All **deprecated** OS4 functions are replaced by modern OS4 one

For the original README follow this <a href="https://github.com/adtools/clib2">link</a>

## ARM port (Raspberry Pi 2, little endian)

clib4 also builds for ARMv7 little endian, the target of the Raspberry Pi 2 port of the
AROS4 kernel (`TARGETARCH=arm`). The PowerPC build is unchanged: the ARM code is gated on
`__arm__` or on `__BYTE_ORDER__`, and the PowerPC objects are checked byte for byte against a
baseline after every change.

### What is different on ARM

- **Static library only.** There is no `clib4.library` and no shared object on ARM. `libc.a`
  and `libm.a` are the real C library, not stubs, and a program is linked as an ET_REL command
  that the kernel loads with `elf.library`. The start code is `library/arch/arm/crtbegin.c`:
  it opens dos and utility, creates the per-process context (`library/stdlib/static_init.c`,
  which copies what `libOpen()`/`libClose()` do for the shared library) and calls `_main()`.
- **Byte order.** `BYTE_ORDER` comes from `__BYTE_ORDER__`. Network order is produced with
  `htonl()`/`ntohl()`, which swap on ARM and are the identity on PowerPC. Code that read a
  double's words or searched strings a word at a time was fixed for little endian, and a
  write-enabled `open()` of an existing file uses `MODE_READWRITE` (`MODE_OLDFILE` is read-only on the
  FAT handler).
- **Floating point.** Soft float (`-mfloat-abi=soft`). On ARM `long double` is the 64-bit
  `double`, so the `long double` functions that read a 128-bit layout (`asinl`, `acosl`,
  `atanl`, `atan2l`, `copysignl`, `roundevenl`, `__isnormall`) forward to the `double` ones
  (`library/arch/arm/ldbl_double.c`). The PowerPC routines for them are left out of the ARM
  archive.
- **Left out on ARM for now:** `ucontext` (`makecontext`/`swapcontext`), `profile` (`gprof`),
  the `cpu/*` assembly variants (AltiVec, SPE, 4xx) and the PowerPC `setjmp`/`bswap`/`getsp`
  assembly, replaced by C or AAPCS code in `library/arch/arm/`. The SysV IPC family is built.
- **OS4 libraries not yet on ARM.** `timezone.library`, `usergroup.library`, `diskfont.library`
  and `bsdsocket.library` are closed OS4 libraries and are not in the ARM image. The static
  start goes on without them: time functions fall back to UTC or to the locale offset, and
  the socket paths that need `bsdsocket.library` are not usable (not tested on ARM).

### Building

You need the arm-amigaos cross toolchain (gcc 11, binutils 2.23 with the `arm-amigaos`
target) and the superproject's `src/sdk` built for ARM. The ARM SDK include path must come
first: `exec/exectags.h` packs `struct Node`, and the kernel's `ExecBase` layout depends on
it. `GNUmakefile.os4` does this itself (`SDKROOT`).

```bash
make -k -f GNUmakefile.os4 TARGETARCH=arm arm-libc -j8
```

This leaves `build/arm/lib/libc.a`, `libm.a`, `crtbegin.o` and `crtend.o`. Do not use the
`all` target for ARM: it runs `gitver`, which rewrites `library/c.lib_rev.h`.

A program is compiled and linked like this (the paths are the ones of a superproject checkout):

```bash
arm-amigaos-gcc -O2 -march=armv7-a -marm -mfloat-abi=soft -mno-unaligned-access \
    -nostdinc -I<clib4>/library/include -I<superproject>/src/sdk/include -c hello.c -o hello.o
arm-amigaos-ld -r -o Hello <clib4>/build/arm/lib/crtbegin.o hello.o \
    <clib4>/build/arm/lib/crtend.o --start-group <clib4>/build/arm/lib/libc.a \
    <clib4>/build/arm/lib/libm.a "$(arm-amigaos-gcc -print-libgcc-file-name)" --end-group
```

### Endianness

On a big-endian host (PowerPC) and on the little-endian ARM host the same program prints
different words. `test_programs/endian/endian1.c` prints the value of a `uint32_t` whose bytes
are `11 22 33 44` in memory: on ARM it gives `x.u32 = 0x44332211`, `htole32` leaves it
unchanged and `htobe32` gives `0x11223344`. The PowerPC output is the reverse for the two
conversions.

The `tests/` suite covers `strlen` on ordinary strings and `fread` round trips, and `atof` on simple
values. The cases that found the little-endian bugs were checked with separate probe programs during the
port (`strlen` at every alignment, the bignum path of `strtod` such as `1e300`, `htonl`/`htons`, and
`inet_aton`); those probes are not in the repository yet.

### Test status on ARM

The `tests/` suite runs on ARM as separate programs: `test_runner` calls `system()` and is not
used there. Each program is linked as above and started from the serial shell of the kernel
(raspi2b, QEMU). The scratch-file tests (`test_stdio`, `test_mmap`, `test_shm`) take their
directory from `TEST_TMP_PREFIX`, which defaults to `/tmp/` (stdio) or `T:` (mmap, shm); the
ARM image has no `T:`, so it is built with `-DTEST_TMP_PREFIX='"SYS:"'`.

Last run (2026-10-09), all passing:

| Suite | Assertions |
|-------|------------|
| string.h | 78 |
| stdlib.h | 59 |
| stdio.h | 54 |
| math.h | 91 |
| time.h | 40 |
| mmap / mprotect | 53 |
| mlock | 64 |
| shm / SysV / mmap | 99 |

Known open: `strtod("2.2250738585072011e-308")` returns the smallest normal double instead of
the largest subnormal (the rounding at the subnormal boundary). The ARM test image is a 1 MB
FAT volume, so the suite runs in several boots.

## Limitations and caveats

The new functions and code, most of the time are tested and some tests has been added to be sure that added features and
functions are working correctly.
Usually all new functions has been tested against linux.  
If you find any issue please <a href="https://github.com/afxgroup/clib2/issues">report it</a>.

### wordexp() and command substitution

`wordexp()` is fully supported, but *command substitution* (the `` `...` `` and `$(...)` constructs) requires a POSIX `sh` shell to be installed on the system (for example in `C:`), because clib4 runs the command through `sh -c` using `spawnvpe()` instead of `fork()`+`execve()`. The stock AmigaOS 4 shell is **not** a POSIX shell and cannot be used for this purpose.
All other expansions (word splitting, quoting, tilde, parameter and arithmetic expansion, globbing) work without `sh`. If `sh` is not installed, command substitution will fail and `wordexp()` will return an error.
Note that the AmigaOS 4 `sh` port is not 100% POSIX compliant, so very complex shell constructs may behave differently than on Linux.

## Shared library

`clib4` now is an AmigaOS4 shared library. This means that you have to install `clib4.library` in LIBS: folder.  
Thanks to shared library now the exe files will be smaller since libc.a/so are just stubs to main library files.

### Libraries

Clib4 now has also a working shared version called `libc.so`. And of course also all other libs. Don't use newlib .so
files when linking because **it will not work**

If you want to use `dlopen`/`dlsym` with shared objects:

**YOU MUST** compile your Clib4 version of library you want to use!  
**DON'T TRY TO USE** newlib shared objects because it will crash badly!  
**DON'T USE** static libraries otherwise you could have undefined symbols!

The plain `libc.a` now contains also `libnet.a`, `libunix.a`. Socket support and floating point support are always
enabled.  
Soft float version is not available (at moment).

Clib4 is always thread safe

If you want to use the shared version of library remember to pack your OS4 software with all clib4 shared objects
otherwise the elf loader will try to load objects from SOBJS: and it will load newlib one.  
**DON'T overwrite SOBJS: files with your Clib4 files** otherwise OS4 most probably will not load them, and you could
have problems running existent software!

### New memory allocator

Clib4 now use `wmem` allocator from WireShark that is faster than previous one, and it seems more robust and with a cleaner
and portable code.  
You can choose at runtime (for test purpose) which kind of allocators want to use setting `CLIB4_MEMORY_ALLOCATOR` env variable.  
At moment you can use:

| Value | Allocator                 |  
|-------|---------------------------|
| 1     | WMEM_ALLOCATOR_SIMPLE     |
| 2     | WMEM_ALLOCATOR_BLOCK      |
| 3     | WMEM_ALLOCATOR_STRICT     |
| 4     | WMEM_ALLOCATOR_BLOCK_FAST |

The default one is `WMEM_ALLOCATOR_BLOCK`. `WMEM_ALLOCATOR_SIMPLE` can crash on some situation. Please refer to `wmem/wmem_core.h` for all details. 

To enable memory debug set `MEMORY_DEBUG` to 1 in `wmem_core.h` or pass it in GNUMakefile when compiling

### Optimized AMCC functions

Some functions like (memchr, memcmp, memcpy and some other) are optimized for SAM440 and SAM460.
Feel free to add other CPU versions.

### Altivec

`libmotovec` has been added to clib4 and enable some altivec functions. Altivec optimizations are not enabled at program
start except if you recompile clib4 with *ENABLE_ALTIVEC_AT_START*.  
However two new methods (**enableAltivec** and **disableAltivec**) are present and you can use them to enable or disable
optimizations at runtime.  
Keep in mind that clib4 is not compiled with altivec optimizations for all files.

### SPE

`libcfsl_e500` (thanks to wayback machine..) has been added to clib4 providing some optimized functions. To compile
*e500* functions pass SPE=true at makefile.  
However to compile these functions you need a compiler that supports SPE ABI. The latest gcc version that supports SPE
is gcc8.

### Shared objects

Shared objects **are working** also with clib4 (there is an example under test_programs/dlopen folder).
However, shared objects needs the beta elf.library not yet released to public

### Large file support

Large files are now supported and tested (A working version of p7zip has been tested with a 8GB file without any
problem).

### SYSV functions

Clib4 now contains **shm***, **msg*** and **sem*** functions.
<a href="http://www.os4depot.net/share/development/library/misc/sysvipc.lha">SYSV IPC</a> library is now fully
integrated into clib4 library and doesn't need any external resource

### Unix path support

`libunix` doesn't exists anymore but you can enable/disable unix support at any time. By default, unix support is
disabled. If you want to enable it at runtime you can create a `.unix` file inside the exe directory or use **enableUnixPaths()** function in your software.  
You can also disable it at runtime using **disableUnixPaths()**. However, is not recommended to enable and disable it at
runtime because you could have problems with internal structures.

### wchar / wctype

All **wctype** functions should be working correctly now. We need a valid test suite.  
All **wchar** functions aree now implemented and tested but maybe something is not working correctly.
There are no valid tests except a little few so if you find any issue
please <a href="https://github.com/afxgroup/clib2/issues">report it</a>.

### Locale

Locales relies always to OS4 locales but now you can set the encoding so wide functions will work correctly.
Accepted encodings are:

`C-UTF-8`
`C-EUCJP`
`C-SJIS`
`C-JIS`
`ISO-8859-1`

### Math library

Math library has been replaced with openlibm (https://github.com/JuliaMath/openlibm).   
This version is faster and more compatible and contains also fenv support that wasn't implemented in clib4.

### Complex numbers

Complex numbers has been added to libm and tests has been added to test that everything is working correctly.  
A lot of other functions has been added trying to make OS4 ports easier.

### libauto

Clib4 now contain also libauto with almost all OS4 components. We'll try to keep them updated.

### libpthread

Clib4 now contain a native pthread implementation with some functions are not present in the pthread.library.  
All mutex* function now use OS4 Mutexes instead of Semaphores

### librt

aio* functions are present, and they are in librt (like on linux). Pthreads are needed to use all aio* functions

### libresolv

Added resolv library to use dns functions. A lot of socket functions that was using bsdsocket.library now use this
library.
And thanks to this library more functions have been added to socket library

### libcrypt

Added crypt library that implements crypt and crypt_r

* ##### About salt

The characters that can be specified are [a-zA-Z0-9. /]. Lowercase alphabets, uppercase alphabets, numbers,'.' And'/'.

By adjusting the format of the salt passed to the crypt function, you can ** specify the hash algorithm **. Make salt
like $ (id) $ (salt string) $. id is a number to specify the hash algorithm. (The hash algorithm will be described
later) (By the way, the $ mark at the end of the salt string can be omitted.)

* ##### Salt id and hash algorithm

Correspondence table between id and hash algorithm

| ID | Hash algorithm |  
|----|----------------|
| 1  | MD5            |
| 2a | Blowfish       |
| 5  | SHA-256        |
| 6  | SHA-512        |

If not specified, it will be DES. DES is very vulnerable and is not recommended because it uses only 2 characters for
Salt and only recognizes passwords for up to 8 characters.

| Hash algorithm | Number of characters in the hashed string |  
|----------------|-------------------------------------------|
| MD5            | 22 Characters                             |
| SHA-256        | 43 Characters                             |
| SHA-512        | 86 Characters                             |

Number of characters in salt DES: Fixed to 2 characters (only the first 2 characters of the salt string are seen, the
characters after that are ignored) MD5: Up to 8 characters (see only the first 8 characters of the salt string, the
characters after that are ignored)

About the security of the hash algorithm DES<MD5<SHA-256<SHA-512 And, the larger the id number, the higher the safety.

Use `-lcrypt` option when linking.

### UNIX sockets (AF_UNIX)

clib4 now supports emulated UNIX sockets. Since roadshow and OS4 doesn't support them natively, UNIX sockets are
emulated via
native sockets. The user doesn't need to change anything in the linux code. Everything will be transparent.

### Debug

To use `debug` functions you have to explicitly pass `DEBUG=true` to GNUMakefile.os4 and
debug functions will be enabled
If you want to use `gstabs` to non debug version of library you can use the flag `STABS=-gstabs` to GNUMakefile.

### Misc

Clib4 now supports opening directories with open() and opendir() functions. There are also new other functions that are
used with fd int files.
Check `fcntl.h` for details

### Known problems

Don't call `exit()` function in an `alarm()` handler otherwise your program will be stuck at exit.  
All *crt* files needs to be compiled with -fno-aggressive-loop-optimizations! Otherwise you will have problems during
constructors/destructors executions

### TODO

- ~~There are some memory leaks at clib4 end needs to be tracked down~~
- Try to use Microsoft <a href="https://github.com/microsoft/mimalloc">`mimalloc`</a> as memory allocator that should be
  faster when there are multiple cores.
- ~~Add a test suite~~
- Try to use some functions/headers from https://github.com/attractivechaos/klib to improve speed
- Use a good locale implementation

### Test Suite

A comprehensive test suite for clib4 is available in the `tests/` directory with 318 tests covering:
- String functions (78 tests)
- Standard library functions (55 tests)
- Standard I/O functions (54 tests)
- Math functions (91 tests)
- Time functions (40 tests)

To build and run the tests:
```bash
cd tests
make run
```

For more information, see [tests/README.md](tests/README.md).

### Libraries repository

A repository with pre packaged libraries is present and ready to use for debian like systems.
You can find all informations to the following <a href="https://github.com/AmigaLabs/clib4/wiki/Clib4-apt-packages-repository">wiki page</a>.

A list of packages can be found into public <a href="https://clib4pkg.amigasoft.net/ubuntu/dists/focal/main/binary-amd64/Packages">Packages</a> file.

## Legal status

Because this library is in part based upon free software it would be uncourteous not to make it free software itself.
The BSD license would probably be appropriate here.

BSD 3-Clause License

Copyright (c) 2016, Olaf Barthel
Copyright (c) 2021, Clib4Developers
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### SYSV

Thanks to Peter Bengtsson for SYSV implementation

### Math library

> The PowerPC math library is based in part on work by Sun Microsystems:
>
>Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
>
>Developed at SunPro, a Sun Microsystems, Inc. business.
> Permission to use, copy, modify, and distribute this
> software is freely granted, provided that this notice
> is preserved.

> Most of (actual) wchar functions are based in part from newlib code.
> Copyright (c) 1990 The Regents of the University of California.
> All rights reserved.
>
>Redistribution and use in source and binary forms are permitted
> provided that the above copyright notice and this paragraph are
> duplicated in all such forms and that any documentation,
> and other materials related to such distribution and use
> acknowledge that the software was developed
> by the University of California, Berkeley. The name of the
> University may not be used to endorse or promote products derived
> from this software without specific prior written permission.
> THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
> IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
> WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

> iconv code license
> Copyright (c) 2003-2004, Artem B. Bityuckiy
>
>Redistribution and use in source and binary forms, with or without
> modification, are permitted provided that the following conditions
> are met:
>1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
>2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
>
>THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
> ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
> IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
> ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
> FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
> DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
> OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
> HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
> LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
> OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
> SUCH DAMAGE.
