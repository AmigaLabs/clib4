#include <thread>

#include <proto/exec.h>

void thread_alloc_free_worker(int id) {
    for (int i = 0; i < 10000; ++i) {
        size_t size = 32 + (i % 128);
        void* ptr = malloc(size);
        assert(ptr != nullptr);
        memset(ptr, id, size);  // Write to memory
        free(ptr);
    }
}

void test_multithreaded_allocations() {
    std::cout << "=== multithreaded malloc/free test ===\n";

    std::vector<std::thread> threads;

    for (int i = 0; i < 8; ++i)
        threads.emplace_back(thread_alloc_free_worker, i);

    for (auto& t : threads)
        t.join();

    std::cout << "Multithreaded allocation test passed.\n\n";
}