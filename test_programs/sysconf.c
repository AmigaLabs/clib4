#include <unistd.h>             /* UNIX std stf    POSIX */
#include <stdio.h>              /* I/O lib         C89   */

/**********************************************************************************************************************************/
int main(int argc, char *argv[]) {

  printf("_SC_OPEN_MAX ..... The maximum number of open files per user id .............. %d\n", (int)sysconf(_SC_OPEN_MAX));
  printf("_SC_CLK_TCK ...... The frequency of the statistics clock in ticks per second . %d\n", (int)sysconf(_SC_CLK_TCK));
  printf("_SC_PAGESIZE ..... The memory page size of the system expressed in bytes ..... %d\n", (int)sysconf(_SC_PAGESIZE));

  return 0;
} /* end func main */