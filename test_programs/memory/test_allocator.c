#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

// Magic number safe for all endianness
#define ALIGNED_BLOCK_MAGIC 0xABCD1234DCBA5678ULL

// Header structure with size optimized for 32/64-bit
typedef struct {
#if INTPTR_MAX == INT32_MAX
    uint32_t padding;  // Maintain 8-byte alignment on 32-bit
#endif
    void* original_ptr;
    uint64_t magic;
} AlignedHeader;

void* _aligned_alloc(size_t size, size_t alignment) {
    // Validate alignment
    if (alignment == 0 || (alignment & (alignment - 1)))
        return NULL;

    // Architecture-specific minimum alignment
    const size_t min_alignment = sizeof(void*) * 2;  // 8 on 32-bit, 16 on 64-bit
    if (alignment < min_alignment)
        alignment = min_alignment;

    const size_t header_size = sizeof(AlignedHeader);
    const size_t padding = alignment - 1;

    // Prevent overflow
    if (size > SIZE_MAX - (header_size + padding))
        return NULL;

    void* original_ptr = malloc(size + header_size + padding);
    if (!original_ptr)
        return NULL;

    // Calculate aligned address
    uintptr_t base = (uintptr_t)original_ptr;
    uintptr_t aligned_addr = (base + header_size + alignment - 1) & ~(alignment - 1);

    // Store metadata
    AlignedHeader* header = (AlignedHeader*)(aligned_addr - header_size);
    header->magic = ALIGNED_BLOCK_MAGIC;
    header->original_ptr = original_ptr;

    return (void*)aligned_addr;
}

void unified_free(void* ptr) {
    if (!ptr) return;

    AlignedHeader* header = (AlignedHeader*)((uintptr_t)ptr - sizeof(AlignedHeader));

    // Validate with endian-independent integer comparison
    if (header->magic == ALIGNED_BLOCK_MAGIC) {
        free(header->original_ptr);
    } else {
        free(ptr);
    }
}
// =====================
// VALIDATION TESTS
// =====================

// ... (_aligned_alloc and unified_free implementations here) ...

void run_architecture_tests() {
    printf("\n===== Architecture-Specific Tests =====\n");
    printf("System Info: %d-bit, %s endian\n\n",
           (int)sizeof(void*)*8,
           *(uint16_t*)"AB" == 0x4241 ? "Little" : "Big");

    // Test pointer size awareness
    void* ptrs[5];
    size_t alignments[] = {8, 16, 32};

    for (int i = 0; i < 3; i++) {
        ptrs[i] = _aligned_alloc(100, alignments[i]);
        assert(ptrs[i] != NULL);
        assert(((uintptr_t)ptrs[i] & (alignments[i] - 1)) == 0);

        AlignedHeader* hdr = (AlignedHeader*)((uintptr_t)ptrs[i] - sizeof(AlignedHeader));
        assert(hdr->magic == ALIGNED_BLOCK_MAGIC);

        unified_free(ptrs[i]);
    }

    // Test header size validation
    printf("Header size: %zu bytes\n", sizeof(AlignedHeader));
    #if INTPTR_MAX == INT32_MAX
        assert(sizeof(AlignedHeader) == 12 || sizeof(AlignedHeader) == 16);
    #else
        assert(sizeof(AlignedHeader) == 16);
    #endif

    printf("All architecture tests passed!\n");
}

void benchmark_arch_performance() {
    printf("\n===== Architecture Performance Benchmark =====\n");
    const int iterations = 1000000;
    const size_t test_size = 64;
    const size_t alignments[] = {8, 64, 256};

    clock_t start, end;
    double base_time, aligned_time;

    // Baseline malloc/free
    start = clock();
    for (int i = 0; i < iterations; i++) {
        void* p = malloc(test_size);
        free(p);
    }
    end = clock();
    base_time = (double)(end - start) / CLOCKS_PER_SEC;

    // Aligned allocator
    for (size_t a = 0; a < sizeof(alignments)/sizeof(alignments[0]); a++) {
        start = clock();
        for (int i = 0; i < iterations; i++) {
            void* p = _aligned_alloc(test_size, alignments[a]);
            unified_free(p);
        }
        end = clock();
        aligned_time = (double)(end - start) / CLOCKS_PER_SEC;

        printf("Alignment %3zu: %.2f ns/alloc (%.1fx slower than malloc)\n",
               alignments[a],
               (aligned_time - base_time)*1e9/iterations,
               aligned_time / base_time);
    }

    // memalign allocator
    for (size_t a = 0; a < sizeof(alignments)/sizeof(alignments[0]); a++) {
        start = clock();
        for (int i = 0; i < iterations; i++) {
            void* p = memalign(alignments[a], test_size);
            free(p);
        }
        end = clock();
        aligned_time = (double)(end - start) / CLOCKS_PER_SEC;

        printf("Memalign alignment %3zu: %.2f ns/alloc (%.1fx slower than malloc)\n",
               alignments[a],
               (aligned_time - base_time)*1e9/iterations,
               aligned_time / base_time);
    }
}

int main() {
    printf("===== Starting Cross-Architecture Validation =====\n");
    run_architecture_tests();

    printf("\n===== Running Performance Comparison =====\n");
    benchmark_arch_performance();

    printf("\nAll tests completed successfully!\n");
    return 0;
}