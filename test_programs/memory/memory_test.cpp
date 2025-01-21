#include <iostream>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>

#include <unistd.h>

#include <functional>

constexpr uint32_t size = 85000000;
constexpr uint32_t mb = 85;

constexpr int workers_number{8};

struct timed_scope {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;

    timed_scope(const std::string &name) : start(std::chrono::high_resolution_clock::now()), name(name) {}

    ~timed_scope() {
        auto took = std::chrono::duration_cast<std::chrono::microseconds>
                (std::chrono::high_resolution_clock::now() - start).count();
        std::cout << name << " -- took: " << took / 1000.0 << " ms [" << size / (took) << "MB/s]" << std::endl;
    }

};


void init_buffers(void *&buf1, void *&buf2) {

    buf1 = malloc(size);

    if (!buf1) {
        std::cout << "Error in malloc" << std::endl;
        exit(1);
    }

    buf2 = malloc(size);

    if (!buf2) {
        std::cout << "Error in malloc" << std::endl;
        exit(1);
    }


}

void free_buffers(void *buf1, void *buf2) {

    free(buf1);
    buf1 = nullptr;

    free(buf2);
    buf2 = nullptr;

}

void single_thread_memcpy() {

    void *dst, *src;
    init_buffers(dst, src);

    {
        timed_scope _("Single thread memcpy ");
        memcpy(dst, src, size);
    }

    auto res = memcmp(dst, src, size);

    if (res) {
        std::cout << "Error in compare" << std::endl;
        exit(1);
    }

    free_buffers(dst, src);
}


template<typename Functor>
void multithread_copy_helper(Functor function, const std::string &name) {

    void *dst, *src;
    init_buffers(dst, src);

    size_t per_worker_size{size / workers_number};

    std::vector <std::thread> workers;
    workers.reserve(workers_number);

    {
        timed_scope _("Multithread " + name + " thread memcpy");

        for (auto i = 0; i < workers_number; ++i) {
            workers.emplace_back(
                    [i, function, dst, src, per_worker_size]() {
                        function(
                                ((uint8_t *) dst) + i * per_worker_size,
                                ((uint8_t *) src) + i * per_worker_size,
                                per_worker_size
                        );
                    }
            );
        }

        for (auto &thread: workers)
            if (thread.joinable())
                thread.join();

    }

    auto res = std::memcmp(dst, src, size);
    if (res) {
        std::cout << "Error in compare" << std::endl;
        exit(1);
    }

    free_buffers(dst, src);
}

int main() {
    std::cout << "Memcpy test -- copying " << mb << "MB without optimized functions" << std::endl;

    single_thread_memcpy();
    multithread_copy_helper(memcpy, "memcpy");
}