# Clib4 Test Suite

This is a comprehensive test suite for the clib4 C standard library. The test suite covers the most commonly used functions from the standard C library headers.

## Overview

The test suite uses a lightweight, self-contained testing framework inspired by minunit. It provides clear, colorful output and comprehensive coverage of library functions.

## Test Coverage

The test suite includes tests for the following modules:

### 1. String Functions (`test_string.c`)
Tests for `string.h` functions including:
- String length and copying: `strlen`, `strcpy`, `strncpy`
- String comparison: `strcmp`, `strncmp`
- String concatenation: `strcat`, `strncat`
- String searching: `strchr`, `strrchr`, `strstr`, `strpbrk`
- String tokenization: `strtok`, `strspn`, `strcspn`
- Memory operations: `memcpy`, `memmove`, `memset`, `memcmp`, `memchr`
- String utilities: `strdup`, `strnlen`

### 2. Standard Library Functions (`test_stdlib.c`)
Tests for `stdlib.h` functions including:
- String conversion: `atoi`, `atol`, `atof`, `strtol`, `strtoul`
- Memory allocation: `malloc`, `calloc`, `realloc`, `free`
- Mathematical operations: `abs`, `labs`, `div`, `ldiv`
- Random numbers: `rand`, `srand`
- Searching and sorting: `qsort`, `bsearch`
- Environment: `getenv`, `putenv`
- System: `system`

### 3. Standard I/O Functions (`test_stdio.c`)
Tests for `stdio.h` functions including:
- Formatted output: `sprintf`, `snprintf`, `fprintf`
- Formatted input: `sscanf`
- File operations: `fopen`, `fclose`, `fread`, `fwrite`
- File positioning: `fseek`, `ftell`, `rewind`
- Character I/O: `fgetc`, `fputc`, `getc`, `putc`, `ungetc`
- Line I/O: `fgets`, `fputs`
- File management: `remove`, `rename`
- Buffer control: `fflush`
- EOF detection: `feof`

### 4. Math Functions (`test_math.c`)
Tests for `math.h` functions including:
- Basic operations: `sqrt`, `pow`, `fabs`, `fmod`
- Rounding: `ceil`, `floor`, `round`, `trunc`
- Trigonometric: `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`
- Hyperbolic: `sinh`, `cosh`, `tanh`
- Exponential/logarithmic: `exp`, `log`, `log10`
- Float manipulation: `modf`, `frexp`, `ldexp`
- Classification: `isnan`, `isinf`, `isfinite`

### 5. Time Functions (`test_time.c`)
Tests for `time.h` functions including:
- Time retrieval: `time`, `clock`
- Time conversion: `localtime`, `gmtime`, `mktime`
- Time formatting: `asctime`, `ctime`, `strftime`
- Time arithmetic: `difftime`
- High-resolution time: `nanosleep` (if available)

### 6. Shared Memory & mmap Functions (`test_shm.c`)
Tests for SysV shared memory (`sys/shm.h`) and memory mapping (`sys/mman.h`) functions, with SQLite WAL compatibility testing:
- **SysV SHM lifecycle**: `shmget`, `shmat`, `shmdt`, `shmctl`
- **IPC_PRIVATE unique segments**: verifies each call creates a distinct segment
- **Named key sharing**: same key returns same shmid, `IPC_EXCL` correctly fails
- **Data persistence**: data survives detach/reattach cycles
- **IPC_STAT**: `shmctl` reports correct segment size
- **Deferred deletion**: `IPC_RMID` while attached, freed on last `shmdt`
- **Error handling**: `shmctl` with NULL buffer (EFAULT), `shmdt` with invalid address (EIDRM), invalid shmid
- **Large segments**: 1 MB allocation with boundary verification
- **Multiple segments**: 5 concurrent IPC_PRIVATE segments with unique data
- **Anonymous mmap**: `MAP_ANONYMOUS` zero-initialized memory, read/write
- **File-backed mmap**: `MAP_PRIVATE` read of file content
- **mmap with offset**: mapping from a non-zero file offset
- **MAP_SHARED write-back**: modify mapped region, `msync`, verify file updated
- **Multiple mmap regions**: 4 concurrent mappings with independent data
- **Edge cases**: `mmap` with zero length returns `MAP_FAILED` + EINVAL
- **SQLite WAL pattern**: simulates WAL-index shm region (32 KB `MAP_SHARED` mmap with `msync`)
- **SysV SHM for WAL**: simulates WAL lock page via `shmget`/`shmat` with reader/writer flags

### 7. mprotect & mmap Page-Alignment (`test_mmap.c`)
Tests for `mprotect()` (POSIX.1-2001) and the page-alignment guarantee of `mmap()`:
- **Error paths**: `mprotect(NULL)` → EINVAL, non-page-aligned addr → EINVAL, invalid `prot` flags → EINVAL
- **Zero length**: `mprotect(addr, 0, prot)` is a no-op returning 0
- **All valid `PROT_*` values**: `PROT_NONE`, `PROT_READ`, `PROT_WRITE`, `PROT_EXEC` and all combinations
- **Anonymous mmap regions**: `mprotect` updates tracking header; data survives round-trip
- **`PROT_NONE` round-trip**: apply PROT_NONE then restore PROT_READ|PROT_WRITE with data integrity check
- **`mprotect` → `msync` → `munmap` chain**: on a `MAP_SHARED` file-backed mapping
- **Page-alignment guarantee**: `mmap()` always returns a pointer aligned to `PAGE_SIZE` (4096)
- **Multiple mappings**: 4 concurrent `mmap()` calls all return page-aligned pointers
- **MMU write-enforcement note**: writing to a `PROT_NONE` page causes a real hardware DSI exception on AmigaOS 4 (confirmed by crash log); the in-process write test is intentionally omitted because `siglongjmp` recovery from a hardware DSI is not safe

### 8. Memory Locking Functions (`test_mlock.c`)
Tests for `mlock()`, `mlock2()`, `munlock()`, `mlockall()`, `munlockall()` (POSIX.1-2001 + Linux):
- **`mlock()` error paths**: `len=0` → success no-op; `addr+len` overflow → EINVAL
- **`munlock()` error paths**: `len=0` → success no-op; `addr+len` overflow → EINVAL
- **`mlock`/`munlock` success**: heap memory, page-aligned memory, stack buffer, anonymous `mmap` region, multiple concurrent regions
- **`mlock2()` error paths**: unknown flags → EINVAL; `len=0` → no-op; `addr+len` overflow → EINVAL
- **`mlock2()` success**: `flags=0` (identical to `mlock()`), `MLOCK_ONFAULT` (same behaviour on AmigaOS 4 — no non-resident pages exist)
- **`mlockall()` error paths**: unknown flags → EINVAL; `MCL_ONFAULT` alone (without `MCL_CURRENT`/`MCL_FUTURE`) → EINVAL
- **`mlockall()` success**: `MCL_CURRENT`, `MCL_FUTURE`, `MCL_CURRENT|MCL_FUTURE`, `MCL_CURRENT|MCL_ONFAULT`
- **`munlockall()`**: always succeeds
- **Constant values**: `MCL_CURRENT=1`, `MCL_FUTURE=2`, `MCL_ONFAULT=4`, `MLOCK_ONFAULT=1`

**AmigaOS 4 notes**: AmigaOS 4 has no virtual memory or swap subsystem — all allocated memory is always physically resident in RAM. `mlock()`/`mlock2()`/`munlock()` delegate to `IExec->LockMem()`/`IExec->UnlockMem()`, which pin memory for hardware DMA stability. `mlockall()` and `munlockall()` are validated no-ops.

## Building the Tests

### Prerequisites

- GCC compiler (or compatible C compiler)
- Standard C library development files
- Make utility

### Build All Tests

```bash
cd tests
make
```

This will compile all test modules and the test runner.

### Build Individual Test Modules

```bash
make test_string   # Build string tests only
make test_stdlib   # Build stdlib tests only
make test_stdio    # Build stdio tests only
make test_math     # Build math tests only
make test_time     # Build time tests only
make test_shm      # Build shm tests only
make test_mmap     # Build mprotect/mmap tests only
make test_mlock    # Build mlock/mlock2/mlockall tests only
```

## Running the Tests

### Run All Tests

```bash
make run
```

This will run all test modules through the test runner, which provides a comprehensive summary.

### Run Individual Test Modules

```bash
make run-string   # Run string tests only
make run-stdlib   # Run stdlib tests only
make run-stdio    # Run stdio tests only
make run-math     # Run math tests only
make run-time     # Run time tests only
make run-mmap     # Run mprotect/mmap tests only
make run-mlock    # Run mlock/mlock2/mlockall tests only
```

Or run the test executables directly:

```bash
./test_string
./test_stdlib
./test_stdio
./test_math
./test_time
./test_mmap
./test_mlock
```

### Run the Test Runner

```bash
./test_runner
```

The test runner executes all test modules and provides an overall summary of results.

## Test Output

The tests produce colorful, easy-to-read output:

- ✓ **Green** indicates passed tests
- ✗ **Red** indicates failed tests
- **Yellow** highlights test suite headers and summaries

Example output:
```
Running test suite: string.h
  ✓ PASS: strlen of empty string
  ✓ PASS: strlen of 'hello'
  ✓ PASS: strcpy 'hello'
  ...

Test suite summary:
  Total tests: 52
  Passed: 52
  Failed: 0
```

## Test Framework

The test suite uses a custom lightweight framework defined in `test_framework.h`. It provides:

### Assertion Macros

- `TEST_ASSERT(message, test)` - Assert that a condition is true
- `TEST_ASSERT_EQUAL(message, expected, actual)` - Assert equality of integers
- `TEST_ASSERT_STR_EQUAL(message, expected, actual)` - Assert string equality
- `TEST_ASSERT_MEM_EQUAL(message, expected, actual, size)` - Assert memory equality
- `TEST_ASSERT_NULL(message, ptr)` - Assert that pointer is NULL
- `TEST_ASSERT_NOT_NULL(message, ptr)` - Assert that pointer is not NULL

### Test Management

- `RUN_TEST(test)` - Run a test function
- `BEGIN_TEST_SUITE(name)` - Start a test suite
- `END_TEST_SUITE()` - End a test suite and print summary

## Adding New Tests

To add new tests to the suite:

1. Create a new test file (e.g., `test_newmodule.c`)
2. Include the test framework: `#include "test_framework.h"`
3. Write test functions that return `const char *` (NULL on success, error message on failure)
4. Create a main function that calls `BEGIN_TEST_SUITE`, runs tests with `RUN_TEST`, and calls `END_TEST_SUITE`
5. Add the new test to the Makefile
6. Update the test runner to include the new module

### Example Test Function

```c
static const char *test_example(void) {
    TEST_ASSERT_EQUAL("1 + 1 equals 2", 2, 1 + 1);
    TEST_ASSERT("true is true", 1);
    return NULL;  // Success
}
```

## Cleaning Up

To remove all compiled test programs:

```bash
make clean
```

This will remove:
- All test executables
- Object files
- Temporary test files in `/tmp/`

## Integration with CI/CD

The test suite is designed to be easily integrated into continuous integration pipelines:

```bash
cd tests
make clean
make all
make run
```

The test runner returns:
- Exit code `0` if all tests pass
- Exit code `1` if any tests fail

## Requirements

- C99 or later standard
- POSIX-compliant system (for some tests)
- Math library (link with `-lm`)

## Notes

- Some tests create temporary files in `/tmp/` (Linux) or `T:` (AmigaOS) for I/O testing
- Tests are designed to be independent and can run in any order
- The test suite focuses on functional correctness, not performance
- Some platform-specific functions may not be tested on all systems
- `test_mmap` skips the PROT_NONE write-enforcement probe on AmigaOS 4 because a hardware DSI exception cannot be safely recovered via `siglongjmp`

## Manual / Exploratory Test Programs

The `test_programs/` directory contains standalone programs that demonstrate and manually verify library functionality. They are not part of the automated test suite.

### `test_programs/memory/mmap2.c`
Demonstrates file-backed `MAP_SHARED` memory mapping with `mprotect`-compatible usage:
- Opens (or creates) a binary file and sizes it with `ftruncate`
- Maps the file with `PROT_READ | PROT_WRITE` + `MAP_SHARED`
- Writes a struct directly through the mapped pointer
- Flushes to disk with `msync(MS_SYNC)`
- Verifies the full `mmap` → write → `msync` → `munmap` lifecycle

Build and run on AmigaOS 4:
```bash
ppc-amigaos-gcc -mcrt=clib4 test_programs/memory/mmap2.c -o mmap2
./mmap2
```

## Contributing

When adding new library functions to clib4, please:

1. Add corresponding tests to the appropriate test module
2. Ensure tests cover normal cases, edge cases, and error conditions
3. Update this README if adding a new test module
4. Verify all tests pass before submitting changes

## License

This test suite is part of the clib4 project and is distributed under the same license (BSD 3-Clause).

## Authors

Created for the clib4 project to ensure library quality and correctness.
