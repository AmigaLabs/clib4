# clib2 – A C runtime library for AmigaOS4

[![Build Status](https://travis-ci.org/afxgroup/clib2.svg?branch=master)](https://travis-ci.org/afxgroup/clib2)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)


## What is this?

This is a fork of <a href="https://github.com/adtools/clib2">official clib2</a> present in adtools.
The point of this library is to make it Amiga OS4 only to maintain it easily and add all missing clib2 features that are hard to add also on classic amigas.
Classic amigas has also ixemul that is the most complete, posix compliant, library we have. Is useless share code that most probably no one will use.
I'm trying (yeah.. trying) to make it posix compliant and fix also all C++ problems we have with newer compilers.
All warnings produced by GCC 10 are now gone. 
All **deprecated** OS4 functions are replaced by modern one (except for StackSwap i don't find the replacement)

For the original readme follow this <a href="https://github.com/adtools/clib2">link</a>


## Limitations and caveats

The added code is most of the time tested. I've also added some test programs to test the added featrues. Of course it can contain errors and bugs. If you find an issue please report it


### Libraries

Clib2 now has also a working shared version called `libc.so`. And of course also all other libs. Don't use newlib .so files when linking because **it will not work**
If you want to use `dlopen`/`dlsym` with shared objects:

**YOU HAVE** to compile your Clib2 version! 

**DON'T TRY TO USE** newlib shared objects because it will crash everything!

**DON'T USE** static libraries otherwise you could have undefined symbols!

The plain `libc.a` now contains also `libnet.a`, `libunix.a`. `libm.a` is just a stub because GCC is searching it. But ll code now is in `libc.a`. 
Socket support and floating point support is always enabled
Soft float version is no longer available.

If you want to use the shared version remember to pack your OS4 software with all CLib2 shared objects othwerwise the elf loader will try to load objects from SOBJS: and it will load newlib one.
Don't overwrite SOBJS: files with your Clib2 files othwerise OS4 most probably will not load or you could have problems running software!

### Shared objects

Shared objects **are working** also with clib2 (there is an example under test_programs/dlopen folder).
using dlopen/dlsym will not crash anymore however due a bug on `libgcc.so` you have to use the static version. Don't use the flag -static-libgcc because it isn't working too. Just remove (or move somewhere) libgcc.so so the linker will use the static one.
If you are compiling linking with C++ it is better to remove `libstdc++.so` too and add -lgcc_eh othweriswe .so is linked against it will not work.


### Large file support

Large files are now supported and tested (i've ported p7zip and tested it with a 8GB file without any problem). To use it you have to add `#define _LARGEFILE64_SOURCE` at top of your file before `<stdio.h>` 


### SYSV functions

This library version conatin shm* and msg* functions. It needs SYSV IPC library. You can find an outdated version on OS4 Depot. If you don't install it those functions will not work and will return to you an **ENOSYS** error.


### Complex numbers

Complex numbers has been added to libm (thanks to sodero port) and i've added a test to see if the complex numbers are working correctly.

A lot of other functions has been added trying to make OS4 ports easier.


### Unix support

`libunix` doesn't exists anymore but you can enable/disable unix support at any time. By default unix support it is disabled. If you want to enable it at runtime you can create a `.unix` file inside the exe directory or use **enableUnixPaths()** function in your software.
You can also disable it at runtime using **disableUnixPaths()**. However is not reccomanded to enable and disable it at runtime because you can have problems with internal structures.


### Wctype

All **wctype** functions should be working correctly now. We need a valid test suite


### Wchar

Some **wchar** functions are now implemented. There are no valid tests except little few so use at your own risk..

### TODO

There is a memory leak at clib2 end needs to be tracked down

## Legal status

Because this library is in part based upon free software it would be uncourteous not to make it free software itself. The BSD license would probably be appropriate here.

The PowerPC math library is based in part on work by Sun Microsystems:

<pre>
========================================================================
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.

Developed at SunPro, a Sun Microsystems, Inc. business.
Permission to use, copy, modify, and distribute this
software is freely granted, provided that this notice
is preserved.
========================================================================
</pre>

Most of (actual) wchar functions are based in part from newlib code.

<pre>
========================================================================
Copyright (c) 1990 The Regents of the University of California.
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
and other materials related to such distribution and use 
acknowledge that the software was developed
by the University of California, Berkeley.  The name of the
University may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
========================================================================
</pre>