#include <stdio.h>

int
main(void)  {
	int first, second, num;

	num = sscanf("6", "%d %d", &first, &second);

	printf("%d %d\n", num, first);

	return(0);
}
