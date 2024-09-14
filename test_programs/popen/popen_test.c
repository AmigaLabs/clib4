#include <stdio.h>
#include <unistd.h>
int main() {
	enableUnixPaths();
    FILE *proc;
	int i;
	for(i = 0; i < 100; i++)
//    if (proc = popen("list", "r")) {
    if (proc = popen("/SDK/gcc/bin/gcc -mcrt=clib4 -Wl,--verbose xc++ -o NIL:", "r")) {
	usleep(100);
       while (!feof(proc)) {
            char buff[10 * 1024];
            size_t read_in = fread(buff, 1, sizeof(buff), proc);
            if (!read_in)
                break;
            buff[read_in+1] = '\0';
            printf("%s\n", buff);
        }
        int ec = pclose(proc);

	printf("return code : %d\n", ec);
    }

    return 0;
}