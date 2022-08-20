#include <unistd.h>

int main(int argc, const char *argv[]) {

    spawnv(_P_WAIT, argv[1], &argv[1]);

    return 0;
}