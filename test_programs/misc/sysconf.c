#include <unistd.h> /* UNIX std stf    POSIX */
#include <stdio.h>  /* I/O lib         C89   */

int main(int argc, char *argv[])
{
  printf("_SC_OPEN_MAX ....... The maximum number of open files per user id .................... %d\n", (int)sysconf(_SC_OPEN_MAX));
  printf("_SC_CLK_TCK  ....... The frequency of the statistics clock in ticks per second ....... %d\n", (int)sysconf(_SC_CLK_TCK));
  printf("_SC_PAGESIZE ....... The memory page size of the system expressed in bytes ........... %d\n", (int)sysconf(_SC_PAGESIZE));
  printf("_SC_ARG_MAX  ....... The maximum length of the arguments to the exec family functions  %d\n", (int)sysconf(_SC_ARG_MAX));
  printf("_SC_HOST_NAME_MAX... Max length of a hostname, not including the terminating null byte %d\n", (int)sysconf(_SC_HOST_NAME_MAX));
  printf("_SC_NPROCESSORS_CONF The number of processors configured ............................. %d\n", (int)sysconf(_SC_NPROCESSORS_CONF));
  return 0;
} /* end func main */