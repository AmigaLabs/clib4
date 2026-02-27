# POSIX Threads for Clib4 based on AROS/MOS version (thanks to joakim ports)

This library implements a subset of the POSIX Threads standard on top of the native Amiga APIs (SignalSemaphores, signals, processes, etc.). 

## Limitations

Due to underlying API limitations detached threads are not supported. 

## License

The library is availabe under the zlib license.

## Websites
[http://bszili.morphos.me](http://bszili.morphos.me)
[https://github.com/jaokim/aros-stuff](https://github.com/jaokim/aros-stuff)

## Acknowledgements

This library is not directly based on any existing one, but it was inspired by the following projects:

* [AROS' thread.library](http://aros.sourceforge.net/documentation/developers/autodocs/thread.php) by Rob Norris
* [Amiga SDL](http://aminet.net/package/dev/misc/SDL-Amiga) by Gabriele Greco
* [Pthreads-w32](https://sourceware.org/pthreads-win32) by Ross Johnson
* [winpthreads](http://locklessinc.com/articles/pthreads_on_windows) by Lockless Inc.

## Changes
MOS/Aros #ifdefs has been removed to avoid readability problems