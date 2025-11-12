/*
 * Main test runner for clib4 test suite
 * Executes all test modules and reports overall results
 */

#include <stdio.h>
#include <stdlib.h>

/* ANSI color codes */
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_RESET   "\033[0m"

/* Test module information */
typedef struct {
    const char *name;
    const char *executable;
} TestModule;

/* List of all test modules */
static TestModule test_modules[] = {
    {"String Functions", "./test_string"},
    {"Standard Library", "./test_stdlib"},
    {"Standard I/O", "./test_stdio"},
    {"Math Functions", "./test_math"},
    {"Time Functions", "./test_time"},
    {NULL, NULL}
};

/* Run a single test module */
static int run_test_module(const TestModule *module) {
    int status;

    printf(COLOR_BLUE "\n========================================\n");
    printf("Running: %s\n", module->name);
    printf("========================================\n" COLOR_RESET);
    
    status = system(module->executable);
    
    if (status < 0) {
        printf(COLOR_RED "Failed to start test: %s\n" COLOR_RESET, module->name);
        return -1;
    }
    
    if (status == 0) {
	printf(COLOR_GREEN "\n✓ %s: PASSED\n" COLOR_RESET, module->name);
	return 0;
    } else {
	printf(COLOR_RED "\n✗ %s: FAILED (exit code: %d)\n" COLOR_RESET,
	       module->name, status);
	return status;
    }
    
    return -1;
}

int main() {
    int total_modules = 0;
    int passed_modules = 0;
    int failed_modules = 0;
    int i;
    
    printf(COLOR_YELLOW "\n");
    printf("╔════════════════════════════════════════╗\n");
    printf("║     Clib4 Test Suite Runner           ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    /* Run all test modules */
    for (i = 0; test_modules[i].name != NULL; i++) {
        int result = run_test_module(&test_modules[i]);
        total_modules++;
        
        if (result == 0) {
            passed_modules++;
        } else if (result > 0) {
            failed_modules++;
        }
        /* result < 0 means module not found or crashed, don't count as failed test */
    }
    
    /* Print summary */
    printf(COLOR_YELLOW "\n");
    printf("========================================\n");
    printf("Overall Test Summary\n");
    printf("========================================\n" COLOR_RESET);
    printf("Total test modules: %d\n", total_modules);
    printf(COLOR_GREEN "Passed modules: %d\n" COLOR_RESET, passed_modules);
    
    if (failed_modules > 0) {
        printf(COLOR_RED "Failed modules: %d\n" COLOR_RESET, failed_modules);
    } else {
        printf("Failed modules: 0\n");
    }
    
    if (passed_modules == total_modules) {
        printf(COLOR_GREEN "\n🎉 All tests passed!\n" COLOR_RESET);
        return 0;
    } else {
        printf(COLOR_RED "\n❌ Some tests failed!\n" COLOR_RESET);
        return 1;
    }
}
