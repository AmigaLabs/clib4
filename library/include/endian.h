#ifndef __ENDIAN_H__
#define __ENDIAN_H__

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

#ifndef __BYTE_ORDER__
#define __BYTE_ORDER__           BYTE_ORDER
#endif

#ifndef __FLOAT_WORD_ORDER__
#define __FLOAT_WORD_ORDER__     __BYTE_ORDER__
#endif

#endif