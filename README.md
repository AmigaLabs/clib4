# clib2 – An ISO 'C' (1994) compliant runtime library for AmigaOS

[![Build Status](https://travis-ci.org/afxgroup/clib2.svg?branch=master)](https://travis-ci.org/afxgroup/clib2)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)


## What is this?

This is a for of <a href="https://github.com/adtools/clib2">official clib2</a> present in adtool.
The point of this library is to make it Amiga OS4 only to maintain it easily and add all missing clib2 features that are hard to add also on classic amigas.
Classic amigas has also ixemul that is the most complete, posix compliant, library we have. Is useless share code that most probably no one will use.
I'm trying (yeah.. trying) to make it posix compliant and fix also all C++ problems we have with newer compilers.

For the original readme follow this <a href="https://github.com/adtools/clib2">link</a>

## Limitations and caveats

The added code is most of the time tested. I've also added some test programs to test the added featrues. Of course it can contain errors and bugs. If you find an issue please report it


### Libraries

The plain `libc.a` now contains also `libnet.a` and `libunix.a`. Is useless to link against those libraries when everything can be added in `libc.a`

### SYSV functions

This library version conatin shm* and msg* functions. It needs SYSV IPC library. You can find an outdated version on OS4 Depot. If you don't install it those functions will not work and will return to you an **ENOSYS** error.

### Complex numbers

Complex numbers has been added to libm (thanks to sodero port) and i've added a test to see if the complex numbers are working correctly.

A lot of other functions has been added trying to make OS4 ports easier.

### Shared objects

Shared objects at moment doesn't work.
using dlopen/dlsym will cause a crash. So don't use it

## Legal status

Because this library is in part based upon free software it would be uncourteous not to make it free software itself. The BSD license would probably be appropriate here.

The PowerPC math library is based in part on work by Sun Microsystems:

<pre>
====================================================
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.

Developed at SunPro, a Sun Microsystems, Inc. business.
Permission to use, copy, modify, and distribute this
software is freely granted, provided that this notice
is preserved.
====================================================
</pre>
