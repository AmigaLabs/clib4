# clib4 on ARM (Raspberry Pi 2, little endian)

clib4 also builds for ARMv7 little endian, the target of the Raspberry Pi 2 port of the
AROS4 kernel (`TARGETARCH=arm`). The PowerPC build is unchanged: every ARM change is gated on
`__arm__`, `__BYTE_ORDER__` or `TARGETARCH=arm`, and the PowerPC objects are checked byte for
byte against a baseline after each change.

## Shape of the port

The ARM port has the same three parts as the PowerPC one:

- **`clib4.library`**, the C library itself, is a relocatable object (ET_REL) built with
  `arm-amigaos-ld -r`. It is installed in `LIBS:` and opened by `ramlib`, the way the other
  ARM libraries of the tree (asl, icon, locale, iffparse, diskfont) are.
- **`libc.a` and `libm.a`** hold the stubs only. Each stub loads the `IClib4` interface
  pointer and jumps through the vector at the function's offset, so every program uses the one
  clib4 instance. The stub is three instructions (`ldr ip, =IClib4; ldr ip, [ip]; ldr pc, [ip, #off]`);
  a vector at 4096 bytes or more takes one more `add`. The offsets are the PowerPC ones: a
  compile-time check confirms that all 1107 agree with the ARM structure layout.
- **`crtbegin.o`** opens dos, utility and clib4.library and calls `library_start()`, as the
  PowerPC `crtbegin` does. It reserves no register: on AAPCS r13 is `sp`, and the PowerPC
  build binds it as a global.

### What is different from PowerPC

- **Byte order.** `BYTE_ORDER` comes from `__BYTE_ORDER__`. Network order is produced with
  `htonl()`/`ntohl()`, which swap on ARM and are the identity on PowerPC. Code that read a
  double's words or searched strings a word at a time was fixed for little endian. A
  write-enabled `open()` of an existing file uses `MODE_READWRITE`, because `MODE_OLDFILE` is a
  read-only share on the FAT handler.
- **Floating point.** Soft float (`-mfloat-abi=soft`). On ARM `long double` is the 64-bit
  `double`, so the functions that read a 128-bit layout (`asinl`, `acosl`, `atanl`, `atan2l`,
  `copysignl`, `roundevenl`, `__isnormall`) forward to the `double` ones
  (`library/arch/arm/ldbl_double.c`). The PowerPC double-double support (`shared_library/math.c`)
  is not linked on ARM.
- **ARM replacements for the PowerPC assembly** in `library/arch/arm/`: `setjmp` and friends
  (AAPCS), `bswap`, `swab`, `getsp`, `backtrace` (no back chain), the VFP `fenv`, and
  `ucontext`.
- **`ucontext` is not ported.** `getcontext`, `setcontext`, `swapcontext` and `makecontext`
  exist and return `ENOSYS`, so a program that calls them gets an error rather than a missing
  symbol. The ARM register switch is future work.
- **Left out on ARM for now:** `profile` (gprof) and the `cpu/*` variants (AltiVec, SPE, 4xx).
- **No `_start` in the library.** The PowerPC library defines a `_start` that returns
  `RETURN_FAIL`. dos.library reads an object that defines `_start` as a program, so on ARM the
  symbol is left out and `clib4.library` loads as a resident library.
- **The library is non-PIC, and complete.** The ARM loader resolves no GOT, PLT or COPY
  relocations, and a non-weak undefined symbol fails the load. So the library is linked
  with `ld -r` against libgcc, and `arm-amigaos-nm -u build/arm/clib4.library.debug` must print
  nothing.
- **Optimised for size (`-Os`).** The 1 MB FAT volume the ARM test image uses has to hold the
  library next to the test programs.
- **Missing OS4 libraries.** The test image has no `timezone.library`, `usergroup.library`,
  `diskfont.library` or `bsdsocket.library`. The library starts without them: time functions
  use the UTC or locale offset, and the socket paths that need `bsdsocket.library` are not
  usable on ARM yet.

## Building

You need the arm-amigaos cross toolchain (gcc 11, binutils 2.23 with the `arm-amigaos`
target) and the superproject's `src/sdk`. The ARM SDK include path comes first: its
`exec/exectags.h` packs `struct Node`, and the kernel's `ExecBase` layout depends on it.
`GNUmakefile.os4` sets this itself (`SDKROOT`).

```bash
make -k -f GNUmakefile.os4 TARGETARCH=arm arm-clib4 arm-libc -j8
```

This leaves `build/arm/clib4.library` (the library, debug sections stripped) and
`build/arm/lib/libc.a`, `libm.a`, `crtbegin.o` and `crtend.o`. The full library with its
symbols is `build/arm/clib4.library.debug`. Do not use the `all` target for ARM: it runs
`gitver`, which rewrites `library/c.lib_rev.h`.

Install the library where `LIBS:` points (`SYS:Libs` on the test image):

```bash
cp build/arm/clib4.library <LIBS>/clib4.library
```

A program is compiled and linked as an ET_REL command, against the stubs:

```bash
arm-amigaos-gcc -O2 -march=armv7-a -marm -mfloat-abi=soft -mno-unaligned-access \
    -nostdinc -I<clib4>/library/include -I<superproject>/src/sdk/include -c hello.c -o hello.o
arm-amigaos-ld -r -o Hello <clib4>/build/arm/lib/crtbegin.o hello.o \
    <clib4>/build/arm/lib/crtend.o --start-group <clib4>/build/arm/lib/libc.a \
    <clib4>/build/arm/lib/libm.a "$(arm-amigaos-gcc -print-libgcc-file-name)" --end-group
```

## Endianness

On a big-endian host (PowerPC) and on the little-endian ARM host the same program prints
different words. `test_programs/endian/endian1.c` prints the value of a `uint32_t` whose bytes
are `11 22 33 44` in memory: on ARM it gives `x.u32 = 0x44332211`, `htole32` leaves it
unchanged and `htobe32` gives `0x11223344`. The PowerPC output is the reverse for the two
conversions.

The `tests/` suite covers `strlen` on ordinary strings, `fread` round trips and `atof` on
simple values. The cases that found the little-endian bugs were checked with separate probe
programs during the port (`strlen` at every alignment, the bignum path of `strtod` such as
`1e300`, `htonl`/`htons`, and `inet_aton`); those probes are not in the repository yet.

## Test status on ARM

The `tests/` suite runs on ARM as separate programs: `test_runner` calls `system()` and is
not used there. Each program is linked as above and started from the serial shell of the
kernel (raspi2b, QEMU), with `clib4.library` in `SYS:Libs`. The scratch-file tests
(`test_stdio`, `test_mmap`, `test_shm`) take their directory from `TEST_TMP_PREFIX`, which
defaults to `/tmp/` (stdio) or `T:` (mmap, shm); the ARM image has no `T:`, so it is built with
`-DTEST_TMP_PREFIX='"SYS:"'`.

Last run (2026-10-10), all passing. Each suite was started from the serial shell with
`clib4.library` in `SYS:Libs`, one suite per boot:

| Suite | Result |
|-------|--------|
| string.h | 78 / 78 |
| stdlib.h | 59 / 59 |
| stdio.h | 54 / 54 |
| math.h | 91 / 91 |
| time.h | 40 / 40 |
| mmap / mprotect | 53 / 53 |
| mlock | 64 / 64 |
| shm / SysV | 99 / 99 |

`test_programs/endian/endian1` prints the expected words (`0x44332211`, `0x11223344` for
`htobe32`). One boot of the time suite stalled in the kernel's own self-test before the shell
came up; the second boot ran it to completion, so the figure above is from that run.

Known open: `strtod("2.2250738585072011e-308")` returns the smallest normal double instead of
the largest subnormal (the rounding at the subnormal boundary). The ARM test image is a 1 MB
FAT volume, so the suite runs in several boots.
