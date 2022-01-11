/* strtold example */
#include <stdio.h>  /* printf, NULL */
#include <stdlib.h> /* strtold */

int main()
{
    char szOrbits[] = "90613.305 365.24";
    char *pEnd;
    char *str = " 111.11 22.22 0X1.BC70A3D70A3D7P+6  1.18973e+4932";
    char *str_end;
/*    
    printf("%f\n", strtod(str,&str_end));
    printf("str_end --> [%c]\n", *str_end);
    printf("%f\n", strtod(str_end,&str_end));
    printf("%g\n", strtod(str_end,&str_end));
    printf("%f\n", strtod(str_end, NULL));
*/  
    long double f1, f2;
    f1 = strtold(szOrbits, &pEnd);
    f2 = strtold(pEnd, NULL);
    printf("Pluto takes %.2Lf years to complete an orbit.\n", f1 / f2);
    return 0;
}