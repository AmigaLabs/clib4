/*
 * $Id: termios.h,v 1.6 2021-02-08 19:08:14 clib4devs Exp $
*/

#ifndef _TERMIOS_H
#define _TERMIOS_H

#include <features.h>

__BEGIN_DECLS

/*
 * Almost-but-not-quite termios.
 */

typedef unsigned int tcflag_t;
typedef unsigned char cc_t;
typedef unsigned int speed_t;

#define NCCS 16 /* Number of Control Characters = Length of the c_cc array.  */

/* All the control characters in c_cc[] in a struct termios (below) */
enum
{
    VINTR = 0,  /* ^C (break)      		*/
    VQUIT,	    /* ^\ (quit)       		*/
    VERASE,	    /* ^H (eraase)     		*/
    VKILL,	    /* ^X (erase-line) 		*/
    VEOF,		/* ^D (end-of-file)		*/
    VMIN,		/* Minimum number of characters to read. Interacts with VTIME.  */
    VEOL,		/* NUL 					*/
    VTIME,	    /* Timeout in 1/10:ths of a second. Interacts with VMIN. */
    VSTART,	    /* ^Q (continue) 		*/
    VSTOP,	    /* ^S (stop) 			*/
    VSUSP,	    /* ^Z (suspend) 		*/
    VWERASE,	/* ^W (word-erase)	 	*/
};

struct termios
{
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
	/* Private */
	speed_t c_ispeed;
	speed_t c_ospeed;
	int type;
	unsigned int flags;
};

/* c_iflag */

#define IGNBRK  0x00001	/* Ignore VINTR */
#define BRKINT  0x00002	/* Unimpl. */
#define IGNPAR  0x00004	/* Unimpl. */
#define IMAXBEL 0x00008 /* Unimpl. */
#define INPCK   0x00010	/* Unimpl. */
#define ISTRIP  0x00020	/* 7-bit data (strip high bit) */
#define INLCR   0x00040	/* Map NL->CR */
#define IGNCR   0x00080	/* Map CR->nothing   */
#define ICRNL   0x00100	/* Map CR->NL */
#define IXON    0x00400	/* Unimpl. Enable XON/XOFF for output. */
#define IXOFF   0x01000	/* Unimpl. Enable XON/XOFF for input. */
#define IUCLC   0x04000	/* Unimpl. */
#define IXANY   0x08000	/* Unimpl. */
#define PARMRK  0x10000	/* Unimpl. */
#define IUTF8   0x20000	/* Unimpl. */

/* c_oflag */

#define OPOST   (1 << 0L)	/* Enable output processing. */
#define ONLCR   (1 << 1L)	/* Map NL->CR+NL */
#define OCRNL   (1 << 2L)	/* Map CR->NL */
#define ONOCR   (1 << 3L)	/* Map CR->nothing, but only in column 0. */
#define ONLRET  (1 << 4L)   /* Map CR->nothing */

/* c_cflag */

#define CSIZE   (0x07)	    /* Bit-width mask. */
#define CS5     (0x01)	    /* 5-bits */
#define CS6     (0x02)	    /* 6-bits */
#define CS7     (0x03)	    /* 7-bits */
#define CS8     (0x04)	    /* 8-bits */
#define CSTOPB  (1 << 3L)   /* Use 2 stop bits. */
#define CREAD   (1 << 4L)	/* Enable reading/receiving. */
#define PARENB  (1 << 5L)   /* Enable parity generation/checking. */
#define PARODD  (1 << 6L)   /* Parity is odd. */
#define HUPCL   (1 << 7L)	/* Hangup on close (when the device is closed). */
#define CLOCAL  (1 << 8L)   /* Ignore modem control lines (i.e. a null-modem) */

/* c_lflag */

#define ISIG    0x0001
#define ICANON  0x0002
#define ECHO    0x0004
#define ECHOE   0x0008
#define ECHOK   0x0010
#define ECHONL  0x0020
#define NOFLSH  0x0040
#define TOSTOP  0x0080
#define IEXTEN  0x0100
#define FLUSHO  0x0200
#define ECHOKE  0x0400
#define ECHOCTL 0x0800

/* Speeds  */

enum
{
	B0 = 0, /* 0 baud => disconnect. */
	B50 = 50,
	B75 = 75,
	B110 = 110,
	B134 = 134,
	B150 = 150,
	B200 = 200,
	B300 = 300,
	B600 = 600,
	B1200 = 1200,
	B1800 = 1800,
	B2400 = 2400,
	B4800 = 4800,
	B9600 = 9600,
	B19200 = 19200,
	B31250 = 31250,
	B38400 = 38400,
	B57600 = 57600,
	B115200 = 115200,
	B230400 = 230400,
	B460800 = 460800,
	B576000 = 576000,
	B1152000 = 1152000
};

#define EXTA 1 /* Tell applications we have speeds > 19200 baud. */
#define EXTB 2 /* Tell applications we have speeds > 38400 baud. */
/* Action types for tcsetattr() */

enum
{
	TCSANOW,
	TCSADRAIN,
	TCSAFLUSH
};

/* Action types for tcflush() */

enum
{
	TCIFLUSH,
	TCOFLUSH,
	TCIOFLUSH
};

/* Action types for tcflow() */

enum
{
	TCOOFF,
	TCOON,
	TCIOFF,
	TCION
};

#ifdef __USE_MISC
# define CIBAUD   002003600000      /* input baud rate (not used) */
# define CMSPAR   010000000000      /* mark or space (stick) parity */
# define CRTSCTS  020000000000      /* flow control */
#endif

/* Prototypes. */

extern int tcgetattr(int file_descriptor, struct termios *tios_p);
extern int tcsetattr(int file_descriptor, int action, struct termios *tios_p);
extern int tcsendbreak(int file_descriptor, int duration);
extern int tcdrain(int file_descriptor);
extern int tcflush(int file_descriptor, int what);
extern int tcflow(int file_descriptor, int action);
extern int cfmakeraw(struct termios *tios_p);
extern speed_t cfgetispeed(const struct termios *tios_p);
extern speed_t cfgetospeed(const struct termios *tios_p);
extern int cfsetispeed(struct termios *tios_p, speed_t input_speed);
extern int cfsetospeed(struct termios *tios_p, speed_t output_speed);

__END_DECLS

#endif /* _TERMIOS_H */
