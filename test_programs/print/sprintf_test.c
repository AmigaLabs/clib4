#include <string.h>
#include <stdio.h>

static char buf[256] = {0};

void addsomething(void);

void addsomething(void)
{
  char *p = &buf[strlen(buf)];
  sprintf(p, "yeah");
}

int main(void)
{
  buf[0] = '\0';

  addsomething();
  printf("1: [%s]\n", buf);

  sprintf(buf, "");

  addsomething();
  printf("2: [%s]\n", buf);

  return (0);
}
