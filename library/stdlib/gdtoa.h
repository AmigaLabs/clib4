#ifndef GDTOA_H_INCLUDED
#define GDTOA_H_INCLUDED

enum {    /* return values from strtodg */
    STRTOG_Zero = 0,
    STRTOG_Normal = 1,
    STRTOG_Denormal = 2,
    STRTOG_Infinite = 3,
    STRTOG_NaN = 4,
    STRTOG_NaNbits = 5,
    STRTOG_NoNumber = 6,
    STRTOG_Retmask = 7,

    /* The following may be or-ed into one of the above values. */

    STRTOG_Neg = 0x08,
    STRTOG_Inexlo = 0x10,
    STRTOG_Inexhi = 0x20,
    STRTOG_Inexact = 0x30,
    STRTOG_Underflow = 0x40,
    STRTOG_Overflow = 0x80
};

typedef struct
FPI {
    int nbits;
    int emin;
    int emax;
    int rounding;
    int sudden_underflow;
} FPI;

enum {    /* FPI.rounding values: same as FLT_ROUNDS */
    FPI_Round_zero = 0,
    FPI_Round_near = 1,
    FPI_Round_up = 2,
    FPI_Round_down = 3
};

#endif /* GDTOA_H_INCLUDED */
