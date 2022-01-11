/* isnan example */
#include <stdio.h>      /* printf */
#include <math.h>       /* isnan, sqrt */

int main()
{
  printf ("isnan(0.0)       : %d\n",isnan(0.0));
  printf ("isnan(1.0/0.0)   : %d\n",isnan(1.0/0.0));
  printf ("isnan(-1.0/0.0)  : %d\n",isnan(-1.0/0.0));
  printf ("isnan(sqrt(-1.0)): %d\n",isnan(sqrt(-1.0)));
  return 0;
}