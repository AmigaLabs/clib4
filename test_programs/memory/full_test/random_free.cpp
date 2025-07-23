#include <cstdlib>
#include <vector>
#include <random>
#include <ctime>
#include <cassert>
#include <iostream>

static inline bool
power_of_two(size_t alignment) {
    return (alignment != 0) && ((alignment & (alignment - 1)) == 0);
}

void test_random_alloc_free_patterns() {
    std::cout << "=== random alloc/free pattern test ===\n";

    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> size_dist(8, 1024);
    std::uniform_int_distribution<size_t> align_dist(8, 512);

    std::vector<void*> pool;

    for (int i = 0; i < 10000; ++i) {
        size_t sz = size_dist(rng);
        size_t al = align_dist(rng);
        if(!power_of_two(al)) continue;

        void* ptr = memalign(al, sz);
        assert(ptr != nullptr);
        if(reinterpret_cast<uintptr_t>(ptr) % al != 0) {
            std::cout << "assertion failed with ptr = " << ptr << ", sz = " << sz << ", al = " << al << "\n";
        }
        assert(reinterpret_cast<uintptr_t>(ptr) % al == 0);
        pool.push_back(ptr);

        if (pool.size() > 1000 && rng() % 2 == 0) {
            size_t idx = rng() % pool.size();
            free(pool[idx]);
            pool.erase(pool.begin() + idx);
        }
    }

    for (void* ptr : pool) {
        free(ptr);
    }

    std::cout << "Random allocation/free test passed.\n\n";
}
