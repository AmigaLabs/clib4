#include <new>      // For ::operator new
#include <iostream>
#include <cassert>

void test_aligned_new_delete() {
    std::cout << "=== aligned new/delete test ===\n";

    // Allocate over-aligned type (aligned to 64 bytes)
    struct alignas(64) Aligned64 {
        int data[16];
    };

    Aligned64* obj = new Aligned64;
    assert(reinterpret_cast<uintptr_t>(obj) % 64 == 0);
    delete obj;

    // Array version
    Aligned64* array = new Aligned64[10];
    assert(reinterpret_cast<uintptr_t>(array) % 64 == 0);
    delete[] array;

    std::cout << "Aligned new/delete test passed.\n\n";
}
