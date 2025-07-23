#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cassert>
#include <memory>
#include <vector>
#include <malloc.h>
// Check if a pointer is aligned
bool is_aligned(void* ptr, size_t alignment) {
    return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
}

// Simple memalign test
void test_memalign() {
    const size_t test_alignments[] = {8, 16, 32, 64, 128, 256, 512, 4096};
    const size_t allocation_size = 100;

    printf("=== memalign() alignment test ===\n");
    for (size_t alignment : test_alignments) {
        void* ptr = memalign(alignment, allocation_size);
        printf("Allocated %zu bytes aligned to %zu -> %p\n", allocation_size, alignment, ptr);
        assert(ptr != nullptr);
        assert(is_aligned(ptr, alignment));
        free(ptr);  // This must not crash or leak!
    }
    printf("memalign() tests passed.\n\n");
}

// Simulate loop deleter behavior with aligned allocations
void test_loop_deleter_behavior() {
    printf("=== C++ loop deleter / smart pointer test ===\n");

    // Test using smart pointers with custom deleters
    for (size_t i = 0; i < 100; ++i) {
        void* raw = memalign(64, sizeof(int) * 10); // aligned alloc
        assert(raw != nullptr);
        assert(is_aligned(raw, 64));

        // free(raw);

        // Simulate a loop deleter: wrap in unique_ptr with a custom deleter
        std::unique_ptr<int[], decltype(&free)> uptr(static_cast<int*>(raw), &free);
        for (int j = 0; j < 10; ++j)
            uptr[j] = j;

        // Automatic cleanup here via free()
    }

    // Stress test: multiple vectors of aligned memory managed by smart pointers
    std::vector<std::unique_ptr<void, decltype(&free)>> blocks;
    for (int i = 0; i < 1000; ++i) {
        void* ptr = memalign(128, 128);
        assert(ptr != nullptr);
        blocks.emplace_back(ptr, &free);
    }

    printf("Loop deleter tests passed.\n\n");
}

// // Entry point
// int main() {
//     test_memalign();
//     test_loop_deleter_behavior();

//     printf("All tests finished without crashes.\n");
//     return 0;
// }
