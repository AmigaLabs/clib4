/*
 * Test suite for stdio.h functions
 */

#include "test_framework.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Test sprintf */
static const char *test_sprintf(void) {
    char buffer[100];
    int result;
    
    result = sprintf(buffer, "hello");
    TEST_ASSERT_EQUAL("sprintf 'hello' returns 5", 5, result);
    TEST_ASSERT_STR_EQUAL("sprintf 'hello'", "hello", buffer);
    
    result = sprintf(buffer, "%d", 123);
    TEST_ASSERT_STR_EQUAL("sprintf '%d' with 123", "123", buffer);
    
    result = sprintf(buffer, "%s %d", "test", 42);
    TEST_ASSERT_STR_EQUAL("sprintf format string", "test 42", buffer);
    
    result = sprintf(buffer, "%x", 255);
    TEST_ASSERT_STR_EQUAL("sprintf hex", "ff", buffer);
    
    return NULL;
}

/* Test snprintf */
static const char *test_snprintf(void) {
    char buffer[10];
    int result;
    
    result = snprintf(buffer, 10, "hello");
    TEST_ASSERT_EQUAL("snprintf 'hello' returns 5", 5, result);
    TEST_ASSERT_STR_EQUAL("snprintf 'hello'", "hello", buffer);
    
    result = snprintf(buffer, 5, "hello world");
    TEST_ASSERT_STR_EQUAL("snprintf truncated", "hell", buffer);
    TEST_ASSERT("snprintf returns full length", result > 5);
    
    result = snprintf(buffer, 10, "%d", 12345);
    TEST_ASSERT_STR_EQUAL("snprintf number", "12345", buffer);
    
    return NULL;
}

/* Test sscanf */
static const char *test_sscanf(void) {
    int i;
    float f;
    char str[50];
    int result;
    
    result = sscanf("123", "%d", &i);
    TEST_ASSERT_EQUAL("sscanf '123' returns 1", 1, result);
    TEST_ASSERT_EQUAL("sscanf '123' value", 123, i);
    
    result = sscanf("42.5", "%f", &f);
    TEST_ASSERT_EQUAL("sscanf '42.5' returns 1", 1, result);
    TEST_ASSERT("sscanf '42.5' value", f > 42.4 && f < 42.6);
    
    result = sscanf("hello 123", "%s %d", str, &i);
    TEST_ASSERT_EQUAL("sscanf 'hello 123' returns 2", 2, result);
    TEST_ASSERT_STR_EQUAL("sscanf string part", "hello", str);
    TEST_ASSERT_EQUAL("sscanf int part", 123, i);
    
    return NULL;
}

/* Test file operations */
static const char *test_file_operations(void) {
    FILE *fp;
    char buffer[100];
#ifdef __AMIGA__
    const char *test_file = "T:test_clib4.txt";
#else
    const char *test_file = "/tmp/test_clib4.txt";
#endif

    /* Test fopen for writing */
    fp = fopen(test_file, "w");
    TEST_ASSERT_NOT_NULL("fopen for writing", fp);
    
    /* Test fprintf */
    fprintf(fp, "test line 1\n");
    fprintf(fp, "test line 2\n");
    
    /* Test fclose */
    TEST_ASSERT_EQUAL("fclose", 0, fclose(fp));
    
    /* Test fopen for reading */
    fp = fopen(test_file, "r");
    TEST_ASSERT_NOT_NULL("fopen for reading", fp);
    
    /* Test fgets */
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        TEST_ASSERT_STR_EQUAL("fgets line 1", "test line 1\n", buffer);
    } else {
        TEST_ASSERT("fgets line 1 failed", 0);
    }
    
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        TEST_ASSERT_STR_EQUAL("fgets line 2", "test line 2\n", buffer);
    } else {
        TEST_ASSERT("fgets line 2 failed", 0);
    }
    
    /* Test feof */
    fgets(buffer, sizeof(buffer), fp); /* Read past end */
    TEST_ASSERT("feof after reading all", feof(fp));
    
    fclose(fp);
    
    /* Cleanup */
    unlink(test_file);
    
    return NULL;
}

/* Test fread/fwrite */
static const char *test_fread_fwrite(void) {
    FILE *fp;
    char write_buffer[] = "binary data test";
    char read_buffer[50];
    size_t result;
#ifdef __AMIGA__
    const char *test_file = "T:test_clib4_bin.txt";
#else
    const char *test_file = "/tmp/test_clib4_bin.txt";
#endif

    /* Write binary data */
    fp = fopen(test_file, "wb");
    TEST_ASSERT_NOT_NULL("fopen wb", fp);
    
    result = fwrite(write_buffer, 1, strlen(write_buffer), fp);
    TEST_ASSERT_EQUAL("fwrite returns correct count", strlen(write_buffer), result);
    fclose(fp);
    
    /* Read binary data */
    fp = fopen(test_file, "rb");
    TEST_ASSERT_NOT_NULL("fopen rb", fp);
    
    memset(read_buffer, 0, sizeof(read_buffer));
    result = fread(read_buffer, 1, strlen(write_buffer), fp);
    TEST_ASSERT_EQUAL("fread returns correct count", strlen(write_buffer), result);
    TEST_ASSERT_STR_EQUAL("fread data matches", write_buffer, read_buffer);
    
    fclose(fp);
    unlink(test_file);
    
    return NULL;
}

/* Test fseek/ftell */
static const char *test_fseek_ftell(void) {
    FILE *fp;
#ifdef __AMIGA__
    const char *test_file = "T:test_clib4_seek.txt";
#else
    const char *test_file = "/tmp/test_clib4_seek.txt";
#endif
    long pos;
    char c;
    
    /* Create a test file */
    fp = fopen(test_file, "w");
    fprintf(fp, "0123456789");
    fclose(fp);
    
    /* Test seeking */
    fp = fopen(test_file, "r");
    TEST_ASSERT_NOT_NULL("fopen for seeking", fp);
    
    /* Test SEEK_SET */
    TEST_ASSERT_EQUAL("fseek SEEK_SET", 0, fseek(fp, 5, SEEK_SET));
    pos = ftell(fp);
    TEST_ASSERT_EQUAL("ftell after SEEK_SET", 5, pos);
    
    /* Read character at position 5 */
    c = fgetc(fp);
    TEST_ASSERT_EQUAL("fgetc at position 5", '5', c);
    
    /* Test SEEK_CUR */
    TEST_ASSERT_EQUAL("fseek SEEK_CUR", 0, fseek(fp, 2, SEEK_CUR));
    pos = ftell(fp);
    TEST_ASSERT_EQUAL("ftell after SEEK_CUR", 8, pos);
    
    /* Test SEEK_END */
    TEST_ASSERT_EQUAL("fseek SEEK_END", 0, fseek(fp, 0, SEEK_END));
    pos = ftell(fp);
    TEST_ASSERT_EQUAL("ftell at end", 10, pos);
    
    fclose(fp);
    unlink(test_file);
    
    return NULL;
}

/* Test putc/getc */
static const char *test_putc_getc(void) {
    FILE *fp;
#ifdef __AMIGA__
    const char *test_file = "T:test_clib4_putc.txt";
#else
    const char *test_file = "/tmp/test_clib4_putc.txt";
#endif
    int c;
    
    /* Write using putc */
    fp = fopen(test_file, "w");
    TEST_ASSERT_NOT_NULL("fopen for putc", fp);
    
    putc('A', fp);
    putc('B', fp);
    putc('C', fp);
    fclose(fp);
    
    /* Read using getc */
    fp = fopen(test_file, "r");
    TEST_ASSERT_NOT_NULL("fopen for getc", fp);
    
    c = getc(fp);
    TEST_ASSERT_EQUAL("getc reads 'A'", 'A', c);
    
    c = getc(fp);
    TEST_ASSERT_EQUAL("getc reads 'B'", 'B', c);
    
    c = getc(fp);
    TEST_ASSERT_EQUAL("getc reads 'C'", 'C', c);
    
    c = getc(fp);
    TEST_ASSERT_EQUAL("getc returns EOF", EOF, c);
    
    fclose(fp);
    unlink(test_file);
    
    return NULL;
}

/* Test ungetc */
static const char *test_ungetc(void) {
    FILE *fp;
#ifdef __AMIGA__
    const char *test_file = "T:test_clib4_ungetc.txt";
#else
    const char *test_file = "/tmp/test_clib4_ungetc.txt";
#endif
    int c;
    
    /* Create test file */
    fp = fopen(test_file, "w");
    fprintf(fp, "ABC");
    fclose(fp);
    
    /* Test ungetc */
    fp = fopen(test_file, "r");
    TEST_ASSERT_NOT_NULL("fopen for ungetc", fp);
    
    c = getc(fp); /* Read 'A' */
    TEST_ASSERT_EQUAL("read 'A'", 'A', c);
    
    /* Push back a different character */
    c = ungetc('X', fp);
    TEST_ASSERT_EQUAL("ungetc returns pushed char", 'X', c);
    
    /* Next read should get 'X' */
    c = getc(fp);
    TEST_ASSERT_EQUAL("getc after ungetc gets pushed char", 'X', c);
    
    /* Next read should get 'B' */
    c = getc(fp);
    TEST_ASSERT_EQUAL("next getc gets 'B'", 'B', c);
    
    fclose(fp);
    unlink(test_file);
    
    return NULL;
}

/* Test fflush */
static const char *test_fflush(void) {
    FILE *fp;
#ifdef __AMIGA__
    const char *test_file = "T:test_clib4_flush.txt";
#else
    const char *test_file = "/tmp/test_clib4_flush.txt";
#endif
    
    fp = fopen(test_file, "w");
    TEST_ASSERT_NOT_NULL("fopen for fflush", fp);
    
    fprintf(fp, "test");
    TEST_ASSERT_EQUAL("fflush", 0, fflush(fp));
    
    fclose(fp);
    unlink(test_file);
    
    return NULL;
}

/* Test remove */
static const char *test_remove(void) {
    FILE *fp;
#ifdef __AMIGA__
    const char *test_file = "T:test_clib4_remove.txt";
#else
    const char *test_file = "/tmp/test_clib4_remove.txt";
#endif
    
    /* Create a file */
    fp = fopen(test_file, "w");
    fprintf(fp, "test");
    fclose(fp);
    
    /* Remove it */
    TEST_ASSERT_EQUAL("remove file", 0, remove(test_file));
    
    /* Try to open it - should fail */
    fp = fopen(test_file, "r");
    TEST_ASSERT_NULL("fopen removed file returns NULL", fp);
    
    return NULL;
}

/* Test rename */
static const char *test_rename(void) {
    FILE *fp;
#ifdef __AMIGA__
    const char *old_file = "T:test_clib4_old.txt";
    const char *new_file = "T:test_clib4_new.txt";
#else
    const char *old_file = "/tmp/test_clib4_old.txt";
    const char *new_file = "/tmp/test_clib4_new.txt";
#endif
    
    /* Create a file */
    fp = fopen(old_file, "w");
    fprintf(fp, "test");
    fclose(fp);
    
    /* Rename it */
    TEST_ASSERT_EQUAL("rename file", 0, rename(old_file, new_file));
    
    /* Old file should not exist */
    fp = fopen(old_file, "r");
    TEST_ASSERT_NULL("old file doesn't exist", fp);
    
    /* New file should exist */
    fp = fopen(new_file, "r");
    TEST_ASSERT_NOT_NULL("new file exists", fp);
    if (fp) fclose(fp);
    
    /* Cleanup */
    unlink(new_file);
    
    return NULL;
}

/* Main test runner */
int main(void) {
    BEGIN_TEST_SUITE("stdio.h");
    
    RUN_TEST(test_sprintf);
    RUN_TEST(test_snprintf);
    RUN_TEST(test_sscanf);
    RUN_TEST(test_file_operations);
    RUN_TEST(test_fread_fwrite);
    RUN_TEST(test_fseek_ftell);
    RUN_TEST(test_putc_getc);
    RUN_TEST(test_ungetc);
    RUN_TEST(test_fflush);
    RUN_TEST(test_remove);
    RUN_TEST(test_rename);
    
    END_TEST_SUITE();
    
    return tests_failed;
}
