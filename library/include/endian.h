#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <features.h>

/* Swap bytes in 16 bit value.  */
#ifdef __GNUC__
# define __bswap_16(x) \
    (__extension__							      \
     ({ unsigned short int __bsx = (x);					      \
        ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8)); }))
#else
static __inline unsigned short int
__bswap_16 (unsigned short int __bsx)
{
    return ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8));
}
#endif

/* Swap bytes in 32 bit value.  */
#ifdef __GNUC__
# define __bswap_32(x) \
    (__extension__							      \
     ({ unsigned int __bsx = (x);					      \
        ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >>  8) |    \
	 (((__bsx) & 0x0000ff00) <<  8) | (((__bsx) & 0x000000ff) << 24)); }))
#else
static __inline unsigned int
__bswap_32 (unsigned int __bsx)
{
    return ((((__bsx) & 0xff000000) >> 24) | (((__bsx) & 0x00ff0000) >>  8) |
            (((__bsx) & 0x0000ff00) <<  8) | (((__bsx) & 0x000000ff) << 24));
}
#endif

#if defined __GNUC__ && __GNUC__ >= 2
/* Swap bytes in 64 bit value.  */
# define __bswap_constant_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)				      \
      | (((x) & 0x00ff000000000000ull) >> 40)				      \
      | (((x) & 0x0000ff0000000000ull) >> 24)				      \
      | (((x) & 0x000000ff00000000ull) >> 8)				      \
      | (((x) & 0x00000000ff000000ull) << 8)				      \
      | (((x) & 0x0000000000ff0000ull) << 24)				      \
      | (((x) & 0x000000000000ff00ull) << 40)				      \
      | (((x) & 0x00000000000000ffull) << 56))

# define __bswap_64(x) \
     (__extension__							      \
      ({ union { __extension__ unsigned long long int __ll;		      \
		 unsigned int __l[2]; } __w, __r;			      \
         if (__builtin_constant_p (x))					      \
	   __r.__ll = __bswap_constant_64 (x);				      \
	 else								      \
	   {								      \
	     __w.__ll = (x);						      \
	     __r.__l[0] = __bswap_32 (__w.__l[1]);			      \
	     __r.__l[1] = __bswap_32 (__w.__l[0]);			      \
	   }								      \
	 __r.__ll; }))
#endif

/* Endianness: we assume a big endian memory layout. */
#define LITTLE_ENDIAN	1234
#define BIG_ENDIAN		4321
#define BYTE_ORDER		BIG_ENDIAN

#ifndef __ORDER_LITTLE_ENDIAN__
#define __ORDER_LITTLE_ENDIAN__  LITTLE_ENDIAN
#endif

#ifndef __ORDER_BIG_ENDIAN__
#define __ORDER_BIG_ENDIAN__     BIG_ENDIAN
#endif

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN             BIG_ENDIAN
#endif

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN          BIG_ENDIAN
#endif


#ifndef __BYTE_ORDER__
#define __BYTE_ORDER__           BYTE_ORDER
#endif

#ifndef __BYTE_ORDER__
#define __BYTE_ORDER             BYTE_ORDER
#endif

#ifndef __FLOAT_WORD_ORDER__
#define __FLOAT_WORD_ORDER__     __BYTE_ORDER__
#endif

#if BYTE_ORDER == LITTLE_ENDIAN

#define htobe16(x) __bswap_16(x)
#define htobe32(x) __bswap_32(x)
#define htobe64(x) __bswap_64(x)

#define be16toh(x) __bswap_16(x)
#define be32toh(x) __bswap_32(x)
#define be64toh(x) __bswap_64(x)

#define htole16(x) (x)
#define htole32(x) (x)
#define htole64(x) (x)

#define le16toh(x) (x)
#define le32toh(x) (x)
#define le64toh(x) (x)

#endif /*BYTE_ORDER == LITTLE_ENDIAN*/

#if BYTE_ORDER == BIG_ENDIAN

#define htobe16(x) (x)
#define htobe32(x) (x)
#define htobe64(x) (x)

#define be16toh(x) (x)
#define be32toh(x) (x)
#define be64toh(x) (x)

#define htole16(x) __bswap_16(x)
#define htole32(x) __bswap_32(x)
#define htole64(x) __bswap_64(x)

#define le16toh(x) __bswap_16(x)
#define le32toh(x) __bswap_32(x)
#define le64toh(x) __bswap_64(x)

#endif /*BYTE_ORDER == BIG_ENDIAN*/

#endif