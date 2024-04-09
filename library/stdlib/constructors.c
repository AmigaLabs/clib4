/*
 * $Id: stdlib_constructors.c,v 1.0 2024-04-09 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void call_constructors(void) {
    SHOWMSG("Calling _stdlib_program_name_init");
    _stdlib_program_name_init();
    SHOWMSG("Calling _stdlib_memory_init");
    _stdlib_memory_init();
    SHOWMSG("_stdio_init _stdlib_memory_init");
    _stdio_init();
    SHOWMSG("Calling _stdio_file_init");
    _stdio_file_init();
    SHOWMSG("Calling _socket_init");
    _socket_init();
    SHOWMSG("Calling _math_init");
    _math_init();
    SHOWMSG("Calling _arg_init");
    _arg_init();
    SHOWMSG("Calling _usergroup_init");
    _usergroup_init();
    SHOWMSG("Calling _locale_init");
    _locale_init();
    SHOWMSG("Calling _unistd_init");
    _unistd_init();
    SHOWMSG("Calling _dirent_init");
    _dirent_init();
    SHOWMSG("Calling _timer_init");
    _timer_init();
    SHOWMSG("Calling _timezone_init");
    _timezone_init();
    SHOWMSG("Calling _clock_init");
    _clock_init();
    SHOWMSG("Init functions called");
}

void call_destructors(void) {
    SHOWMSG("Calling _dcngettext_exit");
    _dcngettext_exit();
    SHOWMSG("Calling _usergroup_exit");
    _usergroup_exit();
    SHOWMSG("Calling _wildcard_expand_exit");
    _wildcard_expand_exit();
    SHOWMSG("Calling _timer_exit");
    _timer_exit();
    SHOWMSG("Calling _timezone_exit");
    _timezone_exit();
    SHOWMSG("Calling _setenv_exit");
    _setenv_exit();
    SHOWMSG("Calling _chdir_exit");
    _chdir_exit();
    SHOWMSG("Calling _dirent_exit");
    _dirent_exit();
    SHOWMSG("Calling _unistd_exit");
    _unistd_exit();
    SHOWMSG("Calling _locale_exit");
    _locale_exit();
    SHOWMSG("Calling _arg_exit");
    _arg_exit();
    SHOWMSG("Calling _socket_exit");
    _socket_exit();
    SHOWMSG("Calling _stdio_file_exit");
    _stdio_file_exit();
    SHOWMSG("Calling _stdio_exit");
    _stdio_exit();
    SHOWMSG("Calling _stdlib_memory_exit");
    _stdlib_memory_exit();
    SHOWMSG("Calling _stdlib_program_name_exit");
    _stdlib_program_name_exit();
    SHOWMSG("Exit functions called");
}