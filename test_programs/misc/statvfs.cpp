// Get Available Filesystem Space on Linux
// A tutorial on this can be found at
// http://www.systutorials.com/136585/how-to-get-available-filesystem-space-on-linux-a-c-function-and-a-cpp-example/

// header for statvfs
#include <sys/statvfs.h>

// C++ I/O header
#include <iostream>

int64_t GetAvailableSpace(const char* path)
{
    struct statvfs stat;

    if (statvfs(path, &stat) != 0) {
        // error happens, just quits here
        return -1;
    }

    // the available size is f_bsize * f_bavail
    return ((int64_t)stat.f_bsize) * stat.f_bavail;
}

int main(int argc, char* argv[])
{
    // check arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " path" << std::endl;
        return -1;
    }

    std::cout << "Available space under `" << argv[1] << "`: ";

    int64_t space = GetAvailableSpace(argv[1]);

    if ( space < 0) {
        // error happens, just quits here
        std::cout << std::endl;
        std::cerr << "ERROR: failed to get available space: " << errno << std::endl;
        return -2;
    }

    // the available size in bytes
    std::cout << space << " bytes." << std::endl;

    return 0;
}