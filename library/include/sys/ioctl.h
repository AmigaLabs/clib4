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

#include <features.h>

__BEGIN_DECLS

extern int ioctl(int fildes, int request, ... /* arg */);

__END_DECLS

#endif /* _SYS_IOTCL_H */
