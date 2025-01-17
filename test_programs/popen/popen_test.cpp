#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
int main(int argc, char **argv) {
	if(argc < 2) return 0;

	enableUnixPaths();

	std::string result;

	FILE *proc;
	proc = popen(argv[1], "r");
	// proc = popen("/qt6-amiga/libexec/rcc", "r"); // -list -name textures textures.qrc", "r");
	// proc = popen("/SDK/gcc/bin/gcc -mcrt=clib4 -Wl,--verbose xc++ -o NIL:", "r");

	if (!proc) {
		printf("Unable to open piped process.\n");
		return 0;
	}

    char buff[1024];
	while(fgets(buff, sizeof(buff), proc) != NULL)
		result += buff;

    int ec = pclose(proc);

	std::cout << "output: [" << result << "]" << std::endl;

	printf("return code : %d\n", ec);

	return 0;
}