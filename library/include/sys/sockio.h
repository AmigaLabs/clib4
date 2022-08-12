#ifndef _SYS_SOCKIO_H
#define _SYS_SOCKIO_H

#include <features.h>

#include <sys/ioccom.h>
#include <net/if.h>

__BEGIN_DECLS

#ifdef __GNUC__
#ifdef __PPC__
#pragma pack(2)
#endif
#elif defined(__VBCC__)
#pragma amiga-align
#endif

/* Socket ioctl's. */
#define    SIOCSHIWAT       _IOW('s',  0, __LONG)        /* set high watermark */
#define    SIOCGHIWAT       _IOR('s',  1, __LONG)        /* get high watermark */
#define    SIOCSLOWAT       _IOW('s',  2, __LONG)        /* set low watermark */
#define    SIOCGLOWAT       _IOR('s',  3, __LONG)        /* get low watermark */
#define    SIOCATMARK       _IOR('s',  7, __LONG)        /* at oob mark? */
#define    SIOCSPGRP        _IOW('s',  8, __LONG)        /* set process group */
#define    SIOCGPGRP        _IOR('s',  9, __LONG)        /* get process group */

#define    SIOCADDRT        _IOW('r', 10, struct ortentry)    /* add route */
#define    SIOCDELRT        _IOW('r', 11, struct ortentry)    /* delete route */

#define    SIOCSIFADDR      _IOW('i', 12, struct ifreq)    /* set ifnet address */
#define    OSIOCGIFADDR     _IOWR('i', 13, struct ifreq)    /* get ifnet address */
#define    SIOCGIFADDR      _IOWR('i', 33, struct ifreq)    /* get ifnet address */
#define    SIOCSIFDSTADDR   _IOW('i', 14, struct ifreq)    /* set p-p address */
#define    OSIOCGIFDSTADDR  _IOWR('i', 15, struct ifreq)    /* get p-p address */
#define    SIOCGIFDSTADDR   _IOWR('i', 34, struct ifreq)    /* get p-p address */
#define    SIOCSIFFLAGS     _IOW('i', 16, struct ifreq)    /* set ifnet flags */
#define    SIOCGIFFLAGS     _IOWR('i', 17, struct ifreq)    /* get ifnet flags */
#define    OSIOCGIFBRDADDR  _IOWR('i', 18, struct ifreq)    /* get broadcast addr */
#define    SIOCGIFBRDADDR   _IOWR('i', 35, struct ifreq)    /* get broadcast addr */
#define    SIOCSIFBRDADDR   _IOW('i', 19, struct ifreq)    /* set broadcast addr */
#define    OSIOCGIFCONF     _IOWR('i', 20, struct ifconf)    /* get ifnet list */
#define    SIOCGIFCONF      _IOWR('i', 36, struct ifconf)    /* get ifnet list */
#define    OSIOCGIFNETMASK  _IOWR('i', 21, struct ifreq)    /* get net addr mask */
#define    SIOCGIFNETMASK   _IOWR('i', 37, struct ifreq)    /* get net addr mask */
#define    SIOCSIFNETMASK   _IOW('i', 22, struct ifreq)    /* set net addr mask */
#define    SIOCGIFMETRIC    _IOWR('i', 23, struct ifreq)    /* get IF metric */
#define    SIOCSIFMETRIC    _IOW('i', 24, struct ifreq)    /* set IF metric */
#define    SIOCDIFADDR      _IOW('i', 25, struct ifreq)    /* delete IF addr */
#define    SIOCAIFADDR      _IOW('i', 26, struct ifaliasreq)/* add/chg IF alias */

#define    SIOCADDMULTI     _IOW('i', 49, struct ifreq)    /* add m'cast addr */
#define    SIOCDELMULTI     _IOW('i', 50, struct ifreq)    /* del m'cast addr */

#ifdef __GNUC__
#ifdef __PPC__
#pragma pack()
#endif
#elif defined(__VBCC__)
#pragma default-align
#endif

        __END_DECLS

#endif /* !_SYS_SOCKIO_H_ */
