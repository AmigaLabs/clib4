/*
 * $Id: stdio.h,v 1.24 2010-10-20 13:12:59 clib4devs Exp $
*/

#ifndef _STDIO_H
#define _STDIO_H

#include <features.h>

#include <stddef.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/types.h>
#include <wchar.h>

__BEGIN_DECLS

/* 'End of file' indicator returned by, for example, fgetc() */
#define EOF (-1)

/****************************************************************************/

/*
 * Maximum number of files that can be open at a time. This number does not
 * correspond to a real limitation for this 'C' runtime library and is
 * included solely for ISO 'C' (1994) compliance.
 */
#define FOPEN_MAX 64

/****************************************************************************/

/* Maximum size of a file/path name. */
#define FILENAME_MAX 256

/****************************************************************************/

/* Default size for all standard I/O file buffers. */
#define BUFSIZ 8192 * 10

/* File buffering modes for use with setvbuf() */

#define _IOFBF 0 /* Full buffering (flush when buffer is full) */
#define _IOLBF 1 /* Line buffering (flush when buffer is full or when  a line feed character is written) */
#define _IONBF 2 /* Perform no buffering at all */

/* Positioning modes for use with fseek() */

#define SEEK_SET 0 /* New position is absolute */
#define SEEK_CUR 1 /* New position is relative to current file position */
#define SEEK_END 2 /* New position is relative to end of file */

/****************************************************************************/

/* Used by fgetpos() and fsetpos() */
typedef int64_t fpos_t;

struct __sFILE
{
	unsigned long _flags;	  /* See below for some of the public flag bits defined; this is by no means a complete list, though! */
	unsigned char *buffer;	  /* Points to the first byte of the buffer; this could be NULL! */
	off_t size;			      /* How many bytes will fit into the buffer; this could be 0! */
    off_t position;		      /* Current buffer position, which is usually a number between 0 and size-1 */
    off_t num_read_bytes;     /* How many bytes can be read from the buffer; this can be 0! */
    off_t num_write_bytes;    /* How many bytes have been copied to the buffer which have not been written back yet; this can be 0! */

	_mbstate_t _mbstate; 	  /* for wide char stdio functions. */
	int _flags2;		 	  /* for future use */

	ssize_t (*_read)(void *cookie, char *buf, int n);
	ssize_t (*_write)(void *cookie, const char *buf, int n);
	fpos_t (*_seek)(void *cookie, fpos_t offset, int whence);
	int (*_close)(void *cookie);
    fpos_t (*_seek64)(void *cookie, fpos_t offset, int whence);

	/* Private iob fields follow...
	 * DON'T TRY TO ACCESS iob fields from FILE pointer!
	 */
};

typedef struct __sFILE FILE;

/****************************************************************************/

/* Field and flag definitions for the getc/putc macros below. */
#define __FILE_BUFFER_MASK 3	 /* Masks off the buffering mode */
#define __FILE_EOF (1 << 2)		 /* EOF reached */
#define __FILE_READABLE (1 << 3) /* File is readable */
#define __FILE_WRITABLE (1 << 4) /* File is writable */
#define __FILE_IN_USE (1 << 5)	 /* File is in use */
#define __FILE_ERROR (1 << 6)	 /* Error detected */

/****************************************************************************/

#define __SORD (1 << 10) /* true => stream orientation (byte/wide) decided */
#define __SL64 (1 << 11) /* is 64-bit offset large file */
#define __SERR (1 << 12) /* found error */

/* _flags2 flags */
#define __SNLK 0x0001 /* stdio functions do not lock streams themselves */
#define __SWID 0x2000 /* true => stream orientation wide, false => byte, only valid if __SORD in _flags is true */

/****************************************************************************/

/*
 * Maximum file name buffer size for use with tmpfile() and tmpnam();
 * note that the maximum file name length is shorter by one character
 */
#define P_tmpdir "T:"
#define L_tmpnam 10

/* Maximum number of unique file names tmpnam() can generate */
#define TMP_MAX 0x40000

/****************************************************************************/

/* The three standard I/O streams */
extern FILE *__stdin();
extern FILE *__stdout();
extern FILE *__stderr();
#define stdin  (__stdin())
#define stdout ( __stdout())
#define stderr (__stderr())

/****************************************************************************/

extern void perror(const char *s);

#ifdef __USE_LARGEFILE64
extern FILE *fdopen64(int, const char *);
extern FILE *fopen64(const char *filename, const char *mode);
extern FILE *freopen64(const char *filename, const char *type, FILE *stream);
extern _off64_t ftello64(FILE *stream);
extern _off64_t fseeko64(FILE *stream, _off64_t, int);
extern int fgetpos64(FILE *stream, _fpos64_t *pos);
extern int fsetpos64(FILE *stream, _fpos64_t *pos);
extern FILE *tmpfile64(void);
#endif

/****************************************************************************/

extern FILE *fopen(const char *filename, const char *mode);
extern int fclose(FILE *stream);
extern int fflush(FILE *stream);
extern FILE *freopen(const char *filename, const char *mode, FILE *stream);
extern int setvbuf(FILE *stream, char *buf, int bufmode, size_t size);
extern void setbuf(FILE *stream, char *buf);

/****************************************************************************/

extern int fseek(FILE *stream, long int offset, int wherefrom);
extern long int ftell(FILE *stream);
extern void rewind(FILE *stream);
extern int fgetpos(FILE *stream, fpos_t *pos);
extern int fsetpos(FILE *stream, fpos_t *pos);

/****************************************************************************/

extern int fgetc(FILE *stream);
extern int getc(FILE *stream);
extern int getchar(void);
extern int ungetc(int c, FILE *stream);

/****************************************************************************/

extern int fputc(int c, FILE *stream);
extern int putc(int c, FILE *stream);
extern int putchar(int c);

/****************************************************************************/

extern char *fgets(char *s, int n, FILE *stream);
extern char *gets(char *s);

/****************************************************************************/

extern int fscanf(FILE *stream, const char *format, ...);
extern int scanf(const char *format, ...);
extern int sscanf(const char *s, const char *format, ...);

/****************************************************************************/

extern int fputs(const char *s, FILE *stream);
extern int puts(const char *s);

/****************************************************************************/

extern int dprintf(int fd, const char *format, ...);
extern int fprintf(FILE *stream, const char *format, ...);
extern int printf(const char *format, ...);
extern int sprintf(char *s, const char *format, ...);

/****************************************************************************/

extern int vdprintf(int fd, const char *format, va_list arg);
extern int vfprintf(FILE *stream, const char *format, va_list arg);
extern int vprintf(const char *format, va_list arg);
extern int vsprintf(char *s, const char *format, va_list arg);

/****************************************************************************/

extern size_t fread(void *ptr, size_t element_size, size_t count, FILE *stream);
extern size_t fwrite(const void *ptr, size_t element_size, size_t count, FILE *stream);

/****************************************************************************/

extern int feof(FILE *stream);
extern int ferror(FILE *stream);
extern void clearerr(FILE *stream);

/****************************************************************************/

extern int rename(const char *oldname, const char *newname);
extern int remove(const char *filename);

/****************************************************************************/

extern FILE *tmpfile(void);
extern char *tmpnam(char *buf);
extern char *tempnam(const char *dir, const char *pfx);

/****************************************************************************/

/*
 * fgetc() implemented as a "simple" macro; note that fgetc() does much more than
 * can be conveniently expressed as a macro!
 */
#define __getc_unlocked(f)                                                                                             \
	(((((FILE *)(f))->_flags & (__FILE_IN_USE | __FILE_READABLE | __FILE_EOF)) == (__FILE_IN_USE | __FILE_READABLE) && \
	  (((FILE *)(f))->_flags & __FILE_BUFFER_MASK) != _IONBF &&                                                        \
	  ((FILE *)(f))->position < ((FILE *)(f))->num_read_bytes)                                                         \
		 ? ((FILE *)(f))->buffer[((FILE *)(f))->position++]                                                            \
		 : fgetc(f))

/****************************************************************************/

/*
 * fputc() implemented as a "simple" macro; note that fputc() does much more than
 * can be conveniently expressed as a macro!
 */
#define __putc_unlocked(c, f)                                                                             \
	(((((FILE *)(f))->_flags & (__FILE_IN_USE | __FILE_WRITABLE)) == (__FILE_IN_USE | __FILE_WRITABLE) && \
	  (((FILE *)(f))->_flags & __FILE_BUFFER_MASK) != _IONBF &&                                           \
	  (((FILE *)(f))->num_write_bytes < ((FILE *)(f))->size))                                             \
		 ? (((FILE *)(f))->buffer[((FILE *)(f))->num_write_bytes] = (c),                                  \
			(((((FILE *)(f))->_flags & __FILE_BUFFER_MASK) == _IOLBF &&                                   \
			  ((FILE *)(f))->buffer[((FILE *)(f))->num_write_bytes] == '\n')                              \
			 ? ((FILE *)(f))->num_write_bytes++,                                                          \
			 __flush(f) : ((FILE *)(f))->buffer[((FILE *)(f))->num_write_bytes++]))                       \
		 : fputc((c), (f)))

/****************************************************************************/

#define getc(f) (flockfile(f), __unlockfile((f), __getc_unlocked(f)))
#define putc(c, f) (flockfile(f), __unlockfile((f), __putc_unlocked((c), (f))))

/****************************************************************************/

#define getchar() getc(stdin)
#define putchar(c) putc((c), stdout)

/****************************************************************************/

/*
 * If requested, reimplement some of the file I/O routines as macros.
 */

#ifdef __C_MACROS__

/****************************************************************************/

#define clearerr(file) ((void)(flockfile(file), (file)->_flags &= ~(__FILE_EOF | __FILE_ERROR), funlockfile(file)))
#define feof(file) (flockfile(file), __unlockfile((file), ((file)->_flags & __FILE_EOF) != 0))
#define ferror(file) (flockfile(file), __unlockfile((file), ((file)->_flags & __FILE_ERROR) != 0))

/****************************************************************************/

#endif /* __C_MACROS__ */

#define MAXPATHLEN PATH_MAX

/****************************************************************************/

#define L_ctermid 32

/****************************************************************************/

extern char *ctermid(char *);

/****************************************************************************/

/*
 * A special buffer flush routine which returns the last character written
 * in case of success and EOF in case of failure. This is used by the
 * __putc_unlocked() macro defined above.
 */
extern int __flush(FILE *stream);

/****************************************************************************/

/*
 * A special function which returns the input character. This is used by
 * the __getc_unlocked() macro defined above.
 */
extern int __unlockfile(FILE *stream, int c);

/****************************************************************************/

extern int getc_unlocked(FILE *stream);
extern int getchar_unlocked(void);
extern int putc_unlocked(int c, FILE *stream);
extern int putchar_unlocked(int c);

extern ssize_t getline(char **lineptr, size_t *n, FILE *stream);
extern ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream);

/****************************************************************************/

#define getc_unlocked(f) __getc_unlocked(f)
#define putc_unlocked(c, f) __putc_unlocked((c), (f))

#define getchar_unlocked() __getc_unlocked(stdin)
#define putchar_unlocked(c) __putc_unlocked((c), stdout)

/****************************************************************************/

extern FILE *fdopen(int file_descriptor, const char *type);
extern int fileno(FILE *file);
extern int asprintf(char **ret, const char *format, ...);
extern int vsnprintf(char *s, size_t size, const char *format, va_list arg);
extern int pclose(FILE *stream);
extern FILE *popen(const char *command, const char *type);
extern int fseeko(FILE *stream, off_t offset, int wherefrom);
extern off_t ftello(FILE *stream);

extern void flockfile(FILE *file);
extern void funlockfile(FILE *file);
extern int ftrylockfile(FILE *file);

extern int vasprintf(char **ret, const char *format, va_list arg);

extern int vfscanf(FILE *stream, const char *format, va_list arg);
extern int vsscanf(const char *s, const char *format, va_list arg);
extern int vscanf(const char *format, va_list arg);
extern int snprintf(char *s, size_t size, const char *format, ...);

__END_DECLS

#endif /* _STDIO_H */
