#include <memory>
#include <vector>
#include <iostream>
#include <cassert>

void test_custom_unique_ptr_in_vector() {
    std::cout << "=== unique_ptr with custom deleter in vector test ===\n";

    std::vector<std::unique_ptr<void, decltype(&free)>> blocks;
    for (int i = 0; i < 1000; ++i) {
        void* p = memalign(256, 256);
        assert(p != nullptr);
        assert(reinterpret_cast<uintptr_t>(p) % 256 == 0);
        blocks.emplace_back(p, &free);
    }

    std::cout << "Custom unique_ptr in vector test passed.\n\n";
}
