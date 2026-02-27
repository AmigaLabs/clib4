/*
 * Test suite for string.h functions
 */

#include "test_framework.h"
#include <string.h>
#include <stdio.h>

/* Test strlen */
static const char *test_strlen(void) {
    TEST_ASSERT_EQUAL("strlen of empty string", 0, strlen(""));
    TEST_ASSERT_EQUAL("strlen of 'hello'", 5, strlen("hello"));
    TEST_ASSERT_EQUAL("strlen of 'test string'", 11, strlen("test string"));
    return NULL;
}

/* Test strcpy */
static const char *test_strcpy(void) {
    char dest[50];
    
    strcpy(dest, "hello");
    TEST_ASSERT_STR_EQUAL("strcpy 'hello'", "hello", dest);
    
    strcpy(dest, "");
    TEST_ASSERT_STR_EQUAL("strcpy empty string", "", dest);
    
    strcpy(dest, "a longer test string");
    TEST_ASSERT_STR_EQUAL("strcpy longer string", "a longer test string", dest);
    
    return NULL;
}

/* Test strncpy */
static const char *test_strncpy(void) {
    char dest[50];
    
    memset(dest, 'X', sizeof(dest));
    strncpy(dest, "hello", 3);
    TEST_ASSERT("strncpy 3 chars", strncmp(dest, "helXX", 5) == 0);
    
    memset(dest, 'X', sizeof(dest));
    strncpy(dest, "hi", 10);
    TEST_ASSERT("strncpy with zeros", dest[0] == 'h' && dest[1] == 'i' && dest[2] == '\0');
    
    return NULL;
}

/* Test strcmp */
static const char *test_strcmp(void) {
    TEST_ASSERT_EQUAL("strcmp equal strings", 0, strcmp("hello", "hello"));
    TEST_ASSERT("strcmp different strings", strcmp("hello", "world") != 0);
    TEST_ASSERT("strcmp first less than second", strcmp("abc", "abd") < 0);
    TEST_ASSERT("strcmp first greater than second", strcmp("abd", "abc") > 0);
    TEST_ASSERT_EQUAL("strcmp empty strings", 0, strcmp("", ""));
    return NULL;
}

/* Test strncmp */
static const char *test_strncmp(void) {
    TEST_ASSERT_EQUAL("strncmp equal strings", 0, strncmp("hello", "hello", 5));
    TEST_ASSERT_EQUAL("strncmp first 3 chars equal", 0, strncmp("hello", "helium", 3));
    TEST_ASSERT("strncmp first 5 chars different", strncmp("hello", "helium", 5) != 0);
    TEST_ASSERT_EQUAL("strncmp with n=0", 0, strncmp("abc", "xyz", 0));
    return NULL;
}

/* Test strcat */
static const char *test_strcat(void) {
    char dest[50];
    
    strcpy(dest, "hello");
    strcat(dest, " world");
    TEST_ASSERT_STR_EQUAL("strcat 'hello' + ' world'", "hello world", dest);
    
    strcpy(dest, "");
    strcat(dest, "test");
    TEST_ASSERT_STR_EQUAL("strcat empty + 'test'", "test", dest);
    
    strcpy(dest, "test");
    strcat(dest, "");
    TEST_ASSERT_STR_EQUAL("strcat 'test' + empty", "test", dest);
    
    return NULL;
}

/* Test strncat */
static const char *test_strncat(void) {
    char dest[50];
    
    strcpy(dest, "hello");
    strncat(dest, " world", 3);
    TEST_ASSERT_STR_EQUAL("strncat 3 chars", "hello wo", dest);
    
    strcpy(dest, "test");
    strncat(dest, "12345", 10);
    TEST_ASSERT_STR_EQUAL("strncat all chars", "test12345", dest);
    
    return NULL;
}

/* Test strchr */
static const char *test_strchr(void) {
    const char *str = "hello world";
    char *result;
    
    result = strchr(str, 'o');
    TEST_ASSERT_NOT_NULL("strchr find 'o'", result);
    TEST_ASSERT_EQUAL("strchr 'o' at position 4", 4, result - str);
    
    result = strchr(str, 'z');
    TEST_ASSERT_NULL("strchr not find 'z'", result);
    
    result = strchr(str, '\0');
    TEST_ASSERT_NOT_NULL("strchr find null terminator", result);
    TEST_ASSERT_EQUAL("strchr null at end", strlen(str), result - str);
    
    return NULL;
}

/* Test strrchr */
static const char *test_strrchr(void) {
    const char *str = "hello world";
    char *result;
    
    result = strrchr(str, 'o');
    TEST_ASSERT_NOT_NULL("strrchr find 'o'", result);
    TEST_ASSERT_EQUAL("strrchr 'o' at position 7", 7, result - str);
    
    result = strrchr(str, 'h');
    TEST_ASSERT_NOT_NULL("strrchr find 'h'", result);
    TEST_ASSERT_EQUAL("strrchr 'h' at position 0", 0, result - str);
    
    result = strrchr(str, 'z');
    TEST_ASSERT_NULL("strrchr not find 'z'", result);
    
    return NULL;
}

/* Test strstr */
static const char *test_strstr(void) {
    const char *haystack = "hello world";
    char *result;
    
    result = strstr(haystack, "world");
    TEST_ASSERT_NOT_NULL("strstr find 'world'", result);
    TEST_ASSERT_STR_EQUAL("strstr found substring", "world", result);
    
    result = strstr(haystack, "hello");
    TEST_ASSERT_NOT_NULL("strstr find 'hello' at start", result);
    TEST_ASSERT_EQUAL("strstr at start", 0, result - haystack);
    
    result = strstr(haystack, "xyz");
    TEST_ASSERT_NULL("strstr not find 'xyz'", result);
    
    result = strstr(haystack, "");
    TEST_ASSERT_NOT_NULL("strstr find empty string", result);
    
    return NULL;
}

/* Test strdup */
static const char *test_strdup(void) {
    char *result;
    
    result = strdup("hello");
    TEST_ASSERT_NOT_NULL("strdup returns non-NULL", result);
    if (result != NULL) {
        TEST_ASSERT_STR_EQUAL("strdup copies string", "hello", result);
        free(result);
    }
    
    result = strdup("");
    TEST_ASSERT_NOT_NULL("strdup empty string returns non-NULL", result);
    if (result != NULL) {
        TEST_ASSERT_STR_EQUAL("strdup empty string", "", result);
        free(result);
    }
    
    return NULL;
}

/* Test strnlen */
static const char *test_strnlen(void) {
    TEST_ASSERT_EQUAL("strnlen 'hello' max 10", 5, strnlen("hello", 10));
    TEST_ASSERT_EQUAL("strnlen 'hello' max 3", 3, strnlen("hello", 3));
    TEST_ASSERT_EQUAL("strnlen empty max 10", 0, strnlen("", 10));
    TEST_ASSERT_EQUAL("strnlen max 0", 0, strnlen("hello", 0));
    return NULL;
}

/* Test memcpy */
static const char *test_memcpy(void) {
    char src[] = "hello world";
    char dest[50];
    
    memcpy(dest, src, 11);
    TEST_ASSERT_MEM_EQUAL("memcpy 11 bytes", src, dest, 11);
    
    memcpy(dest, "test", 4);
    TEST_ASSERT_MEM_EQUAL("memcpy 4 bytes", "test", dest, 4);
    
    return NULL;
}

/* Test memmove */
static const char *test_memmove(void) {
    char buffer[] = "hello world";
    
    /* Overlapping regions */
    memmove(buffer + 2, buffer, 5);
    TEST_ASSERT("memmove overlapping forward", strncmp(buffer, "hehello", 7) == 0);
    
    strcpy(buffer, "hello world");
    memmove(buffer, buffer + 6, 5);
    TEST_ASSERT("memmove overlapping backward", strncmp(buffer, "world", 5) == 0);
    
    return NULL;
}

/* Test memset */
static const char *test_memset(void) {
    char buffer[20];
    
    memset(buffer, 'A', 10);
    TEST_ASSERT("memset with 'A'", buffer[0] == 'A' && buffer[9] == 'A');
    
    memset(buffer, 0, 20);
    TEST_ASSERT("memset with 0", buffer[0] == 0 && buffer[19] == 0);
    
    return NULL;
}

/* Test memcmp */
static const char *test_memcmp(void) {
    char buf1[] = "hello";
    char buf2[] = "hello";
    char buf3[] = "world";
    
    TEST_ASSERT_EQUAL("memcmp equal", 0, memcmp(buf1, buf2, 5));
    TEST_ASSERT("memcmp different", memcmp(buf1, buf3, 5) != 0);
    TEST_ASSERT("memcmp first less", memcmp("abc", "abd", 3) < 0);
    TEST_ASSERT("memcmp first greater", memcmp("abd", "abc", 3) > 0);
    
    return NULL;
}

/* Test memchr */
static const char *test_memchr(void) {
    char buffer[] = "hello world";
    void *result;
    
    result = memchr(buffer, 'o', 11);
    TEST_ASSERT_NOT_NULL("memchr find 'o'", result);
    TEST_ASSERT_EQUAL("memchr 'o' position", 4, (char*)result - buffer);
    
    result = memchr(buffer, 'z', 11);
    TEST_ASSERT_NULL("memchr not find 'z'", result);
    
    result = memchr(buffer, '\0', 12);
    TEST_ASSERT_NOT_NULL("memchr find null", result);
    
    return NULL;
}

/* Test strtok */
static const char *test_strtok(void) {
    char str[] = "hello,world,test";
    char *token;
    
    token = strtok(str, ",");
    TEST_ASSERT_NOT_NULL("strtok first token", token);
    TEST_ASSERT_STR_EQUAL("strtok first is 'hello'", "hello", token);
    
    token = strtok(NULL, ",");
    TEST_ASSERT_NOT_NULL("strtok second token", token);
    TEST_ASSERT_STR_EQUAL("strtok second is 'world'", "world", token);
    
    token = strtok(NULL, ",");
    TEST_ASSERT_NOT_NULL("strtok third token", token);
    TEST_ASSERT_STR_EQUAL("strtok third is 'test'", "test", token);
    
    token = strtok(NULL, ",");
    TEST_ASSERT_NULL("strtok no more tokens", token);
    
    return NULL;
}

/* Test strspn */
static const char *test_strspn(void) {
    TEST_ASSERT_EQUAL("strspn matching chars", 5, strspn("12345abc", "12345"));
    TEST_ASSERT_EQUAL("strspn no matching", 0, strspn("abc", "123"));
    TEST_ASSERT_EQUAL("strspn all matching", 3, strspn("abc", "abcdefg"));
    return NULL;
}

/* Test strcspn */
static const char *test_strcspn(void) {
    TEST_ASSERT_EQUAL("strcspn find rejection", 5, strcspn("hello world", " "));
    TEST_ASSERT_EQUAL("strcspn no rejection", 5, strcspn("hello", "xyz"));
    TEST_ASSERT_EQUAL("strcspn immediate rejection", 0, strcspn("hello", "h"));
    return NULL;
}

/* Test strpbrk */
static const char *test_strpbrk(void) {
    const char *str = "hello world";
    char *result;
    
    result = strpbrk(str, " ");
    TEST_ASSERT_NOT_NULL("strpbrk find space", result);
    TEST_ASSERT_EQUAL("strpbrk space position", 5, result - str);
    
    result = strpbrk(str, "or");
    TEST_ASSERT_NOT_NULL("strpbrk find 'o' or 'r'", result);
    TEST_ASSERT_EQUAL("strpbrk 'o' position", 4, result - str);
    
    result = strpbrk(str, "xyz");
    TEST_ASSERT_NULL("strpbrk not find", result);
    
    return NULL;
}

/* Main test runner */
int main(void) {
    BEGIN_TEST_SUITE("string.h");
    
    RUN_TEST(test_strlen);
    RUN_TEST(test_strcpy);
    RUN_TEST(test_strncpy);
    RUN_TEST(test_strcmp);
    RUN_TEST(test_strncmp);
    RUN_TEST(test_strcat);
    RUN_TEST(test_strncat);
    RUN_TEST(test_strchr);
    RUN_TEST(test_strrchr);
    RUN_TEST(test_strstr);
    RUN_TEST(test_strdup);
    RUN_TEST(test_strnlen);
    RUN_TEST(test_memcpy);
    RUN_TEST(test_memmove);
    RUN_TEST(test_memset);
    RUN_TEST(test_memcmp);
    RUN_TEST(test_memchr);
    RUN_TEST(test_strtok);
    RUN_TEST(test_strspn);
    RUN_TEST(test_strcspn);
    RUN_TEST(test_strpbrk);
    
    END_TEST_SUITE();
    
    return tests_failed;
}
