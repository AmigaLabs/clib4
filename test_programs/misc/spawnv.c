#include <unistd.h>

int main(int argc, char *argv[]) {

     spawnv( _P_WAIT, argv[1], &argv[1] );

    return 0;
}