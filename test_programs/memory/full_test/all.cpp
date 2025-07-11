#include "alignment_loop_deleters.cpp"
#include "alignment_stress.cpp"
#include "new_delete.cpp"
#include "stl_containers.cpp"
#include "random_free.cpp"
#include "realloc.cpp"
#include "multithread.cpp"
#include "heap_coalescing.cpp"

int main() {
    // test_memalign();
    // test_loop_deleter_behavior();
    // test_vector_growth_alignment();
    // test_aligned_new_delete();
    // test_custom_unique_ptr_in_vector();
    // test_random_alloc_free_patterns();

    // test_realloc_behavior();
    test_multithreaded_allocations();
    // test_heap_coalescing();

    std::cout << "\n All allocator tests completed successfully.\n";
    return 0;
}