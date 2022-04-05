#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <chrono>

#define TimePoint std::chrono::time_point<std::chrono::high_resolution_clock>
#define Duration std::chrono::duration<double>
#define getTime() std::chrono::high_resolution_clock::now()

#define GB *(1<<30)
#define MB *(1<<20)
#define KB *(1<<10)
#define  B *(1<<00)

#define INITIAL_MEMORY (64 MB)                    //How much memory to start with
#define ALLOCATE_MEMORY (32 MB)                    //How much memory to add/subtract
#define BYTE_STEP (4 KB)                        //How many bytes to allocate/deallocate per iteration
#define ITERATIONS ALLOCATE_MEMORY/BYTE_STEP    //Times to repeat allocation

inline void measure(const char *name, void *(*func)(int *, void *)) {
    TimePoint start, end;
    int size = INITIAL_MEMORY;    //16MB of memory
    void *memory = malloc(size);
    start = getTime();
    for (int i = 0; i < ITERATIONS; i++) {
        printf("%d ", i);
        memory = func(&size, memory);
    }
    end = getTime();
    Duration time = end - start;
    int ms = (int) (time.count() * 1000);
    printf("%s took ", name);
    if (ms / 1000 != 0) {
        printf("%d ", ms / 1000);
    }
    printf("%d ms\n", ms % 1000);
}

//free->malloc (data is lost)
inline void *testFreeMallocIncrease(int *size, void *memory) {
    free(memory);
    *size += BYTE_STEP;
    return malloc(*size);
}

inline void *testFreeMallocDecrease(int *size, void *memory) {
    free(memory);
    *size -= BYTE_STEP;
    return malloc(*size);
}

//malloc->memcpy->free (data is conserved)
inline void *testMallocCopyFreeIncrease(int *size, void *memory) {
    int oldSize = *size;
    *size += BYTE_STEP;
    void *newMemory = malloc(*size);
    memcpy(newMemory, memory, oldSize);
    free(memory);
    return newMemory;
}

inline void *testMallocCopyFreeDecrease(int *size, void *memory) {
    *size -= BYTE_STEP;
    void *newMemory = malloc(*size);
    memcpy(newMemory, memory, *size);
    free(memory);
    return newMemory;
}

//realloc (data is conserved)
inline void *testReallocIncrease(int *size, void *memory) {
    *size += BYTE_STEP;
    return realloc(memory, *size);
}

inline void *testReallocDecrease(int *size, void *memory) {
    *size -= BYTE_STEP;
    return realloc(memory, *size);
}

int main(int argc, char *argv[]) {
    measure("Increasing Lossful Malloc", testFreeMallocIncrease);
    measure("Decreasing Lossful Malloc", testFreeMallocDecrease);
    //measure("Increasing Malloc",testMallocCopyFreeIncrease);
    //measure("Decreasing Malloc",testMallocCopyFreeDecrease);
    measure("Increasing Realloc", testReallocIncrease);
    measure("Decreasing Realloc", testReallocDecrease);
    return 0;
}