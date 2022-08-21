/*
 * $Id: ioctl.h,v 1.5 2006-06-21 15:53:55 clib2devs Exp $
*/

#ifndef _SYS_IOTCL_H
#define _SYS_IOTCL_H

#ifndef _SYS_SELECT_H
#include <sys/select.h>
#endif /* _SYS_SELECT_H */

#ifndef _SYS_FILIO_H
#include <sys/filio.h>
#endif /* _SYS_FILIO_H */

#include <sys/sockio.h>

#include <features.h>

struct winsize {
    unsigned short  ws_row;         /* rows, in characters */
    unsigned short  ws_col;         /* columns, in characters */
    unsigned short  ws_xpixel;      /* horizontal size, pixels - not used */
    unsigned short  ws_ypixel;      /* vertical size, pixels - not used */
};

#define TIOCGWINSZ  0x5413 /* Get terminal size */
/* Not supported by Console Device yet */
#define TIOCSWINSZ  0x5414 /* Set terminal size */

__BEGIN_DECLS

extern int ioctl(int fildes, int request, ... /* arg */);

__END_DECLS

#endif /* _SYS_IOTCL_H */
