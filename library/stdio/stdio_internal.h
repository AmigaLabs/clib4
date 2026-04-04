/*
 * $Id: stdio_internal.h,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Internal header for the newlib-inspired stdio rewrite.
 * Contains internal types, macros and declarations for the new I/O layer.
 *
 * This file is included by stdio_headers.h and should NOT be included
 * directly by any file outside of the stdio directory.
 */

#ifndef _STDIO_INTERNAL_H
#define _STDIO_INTERNAL_H

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _DOS_H
#include <dos.h>
#endif

/****************************************************************************/

/* Forward declarations */
struct iob;
struct fd;
struct _clib4;

/****************************************************************************/
/*
 * Small buffer struct used for the ungetc pushback buffer (_ub)
 * and the fgetline scratch buffer (_lb).
 */
struct __sbuf {
    unsigned char *_base;
    int _size;
};

/****************************************************************************/
/*
 * Glue structure: a linked-list node holding an array of FILE (struct iob)
 * pointers. The first node (__sglue) holds the 3 static stdin/stdout/stderr
 * entries; additional blocks are allocated on demand by __sfp().
 *
 * This replaces the old flat __iob[] / __num_iob array in _clib4.
 */
struct _glue {
    struct _glue *next;     /* next block in chain */
    int niobs;              /* number of iob pointers in this block */
    struct iob **iobs;      /* array of struct iob pointers */
};

/****************************************************************************/
/* Extended internal flag bits (added to iob_Flags beyond what's public).
 *
 * IMPORTANT: bits 0-12 are already used by IOBF_* flags in stdio_headers.h.
 * We start new flags at bit 13+.
 */
#define IOBF_MALLOC_BUF     (1<<13)    /* Buffer was malloc'd by __smakebuf; free on close */
#define IOBF_APP            (1<<14)    /* Append mode (fopen "a") */
#define IOBF_STR            (1<<15)    /* String stream (sprintf/sscanf) */
#define IOBF_OPT            (1<<16)    /* fseek optimization allowed (can seek within buffer) */
#define IOBF_NOPT           (1<<17)    /* fseek optimization NOT allowed */
#define IOBF_OFF            (1<<18)    /* _offset field is valid */
#define IOBF_UNGETC         (1<<19)    /* Ungetc buffer is active */

/****************************************************************************/
/*
 * CHECK_INIT — ensures stdio is initialized before first use.
 * Must be called at the start of every public stdio function.
 */
#define CHECK_INIT(clib4) \
    do { \
        if (!(clib4)->__stdio_initialized) \
            __sinit(clib4); \
    } while (0)

/****************************************************************************/
/*
 * Ungetc buffer macros.
 */
#define HASUB(fp)   ((fp)->_ub._base != NULL)
#define FREEUB(clib4, fp) \
    do { \
        if ((fp)->_ub._base != (fp)->_ubuf) \
            free((fp)->_ub._base); \
        (fp)->_ub._base = NULL; \
        (fp)->_ub._size = 0; \
    } while (0)

/*
 * Line buffer macros (for fgetline scratch buffer).
 */
#define HASLB(fp)   ((fp)->_lb._base != NULL)
#define FREELB(clib4, fp) \
    do { \
        free((fp)->_lb._base); \
        (fp)->_lb._base = NULL; \
        (fp)->_lb._size = 0; \
    } while (0)

/****************************************************************************/
/*
 * cantread / cantwrite — quick checks adapted to clib4's flag model.
 *
 * cantread: true if the stream is not open for reading.
 * cantwrite: true if the stream is not open for writing.
 *
 * These return an error (set errno, set error flag) and can be used
 * in early-return style.
 */
#define cantread(clib4, fp) \
    (!FLAG_IS_SET(((struct iob *)(fp))->iob_Flags, IOBF_READ))

#define cantwrite(clib4, fp) \
    (!FLAG_IS_SET(((struct iob *)(fp))->iob_Flags, IOBF_WRITE))

/****************************************************************************/
/*
 * Macros to access clib4's buffer fields in a newlib-style way.
 *
 * In newlib:
 *   _p  = current pointer into buffer
 *   _r  = bytes remaining for reading
 *   _w  = bytes remaining for writing (space left)
 *
 * In clib4 (ABI-fixed public fields):
 *   buffer         = base pointer
 *   position       = current read offset from base
 *   num_read_bytes = total readable bytes in buffer
 *   num_write_bytes = bytes written but not yet flushed
 *   size           = total buffer capacity
 *
 * Equivalences:
 *   _p  <=> buffer + position         (current read pointer)
 *   _r  <=> num_read_bytes - position (remaining readable bytes)
 *   _w  <=> size - num_write_bytes    (remaining writable space)
 */
#define READABLE_BYTES(fp)  ((fp)->iob_BufferReadBytes - (fp)->iob_BufferPosition)
#define WRITABLE_SPACE(fp)  ((fp)->iob_BufferSize - (fp)->iob_BufferWriteBytes)
#define READ_PTR(fp)        ((fp)->iob_Buffer + (fp)->iob_BufferPosition)
#define WRITE_PTR(fp)       ((fp)->iob_Buffer + (fp)->iob_BufferWriteBytes)

/****************************************************************************/
/*
 * New internal function declarations.
 *
 * These implement the core newlib-inspired stdio engine.
 */

/* findfp.c — stream allocation via glue list */
extern struct iob *__sfp(struct _clib4 *__clib4);
extern void __sinit(struct _clib4 *__clib4);

/* smakebuf.c — lazy buffer allocation */
extern void __smakebuf(struct _clib4 *__clib4, struct iob *fp);

/* srefill.c — refill read buffer */
extern int __srefill(struct _clib4 *__clib4, struct iob *fp);

/* swbuf.c — write buffer overflow (single byte) */
extern int __swbuf(struct _clib4 *__clib4, int c, struct iob *fp);

/* swsetup.c — prepare stream for writing */
extern int __swsetup(struct _clib4 *__clib4, struct iob *fp);

/* fwalk.c — iterate all open streams */
extern int _fwalk(struct _clib4 *__clib4, int (*func)(struct _clib4 *, struct iob *));
extern int _fwalk_sglue(struct _clib4 *__clib4, int (*func)(struct _clib4 *, struct iob *), struct _glue *g);

/* stdio_rw.c — bridge functions: FILE function pointers → fd_Action hooks */
extern ssize_t __sread(void *cookie, char *buf, int n);
extern ssize_t __swrite(void *cookie, const char *buf, int n);
extern fpos_t __sseek(void *cookie, fpos_t offset, int whence);
extern int __sclose(void *cookie);

/* sflush.c (in flushiobwritebuffer.c) — unified flush */
extern int __sflush(struct _clib4 *__clib4, struct iob *fp);

/* sflags.c — convert mode string to flags */
extern int __sflags(const char *mode, int *oflags);

/****************************************************************************/

#endif /* _STDIO_INTERNAL_H */
