void test_realloc_behavior() {
    std::cout << "=== realloc() stress test ===\n";

    const int N = 10000;
    std::vector<void*> blocks;

    // Initial allocations
    for (int i = 0; i < N; ++i) {
        void* ptr = malloc(64);
        assert(ptr != nullptr);
        blocks.push_back(ptr);
    }

    // Reallocate with increasing size
    for (int i = 0; i < N; ++i) {
        void* ptr = realloc(blocks[i], 64 + (i % 32) * 16);
        assert(ptr != nullptr);
        blocks[i] = ptr;
    }

    // Shrinking
    for (int i = 0; i < N; ++i) {
        void* ptr = realloc(blocks[i], 32);
        assert(ptr != nullptr);
        blocks[i] = ptr;
    }

    // Free all
    for (void* ptr : blocks) {
        free(ptr);
    }

    std::cout << "realloc() test passed.\n\n";
}
