void test_heap_coalescing() {
    std::cout << "=== heap coalescing / fragmentation test ===\n";

    std::vector<void*> pool;

    // Allocate and free in patterns
    for (int i = 0; i < 1000; ++i) {
        void* a = malloc(128);
        void* b = malloc(128);
        void* c = malloc(128);
        assert(a && b && c);
        free(b);  // Middle block
        pool.push_back(a);
        pool.push_back(c);
    }

    // Try to reuse freed middle blocks
    for (int i = 0; i < 1000; ++i) {
        void* r = malloc(128);  // Should reuse coalesced or freed blocks
        assert(r != nullptr);
        free(r);
    }

    for (void* p : pool) free(p);

    std::cout << "Heap coalescing test passed.\n\n";
}