/*
 * Test suite for math.h functions
 */

#define _USE_MATH_DEFINES
#include "test_framework.h"
#include <math.h>
#include <float.h>

#define EPSILON 0.0001

/* Define M_PI and M_E if not defined */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

/* Helper to compare floating point numbers */
static int float_equal(double a, double b) {
    return fabs(a - b) < EPSILON;
}

/* Test sqrt */
static const char *test_sqrt(void) {
    TEST_ASSERT("sqrt(4) = 2", float_equal(sqrt(4.0), 2.0));
    TEST_ASSERT("sqrt(9) = 3", float_equal(sqrt(9.0), 3.0));
    TEST_ASSERT("sqrt(0) = 0", float_equal(sqrt(0.0), 0.0));
    TEST_ASSERT("sqrt(0.25) = 0.5", float_equal(sqrt(0.25), 0.5));
    return NULL;
}

/* Test pow */
static const char *test_pow(void) {
    TEST_ASSERT("pow(2, 3) = 8", float_equal(pow(2.0, 3.0), 8.0));
    TEST_ASSERT("pow(5, 2) = 25", float_equal(pow(5.0, 2.0), 25.0));
    TEST_ASSERT("pow(2, 0) = 1", float_equal(pow(2.0, 0.0), 1.0));
    TEST_ASSERT("pow(10, -1) = 0.1", float_equal(pow(10.0, -1.0), 0.1));
    return NULL;
}

/* Test fabs */
static const char *test_fabs(void) {
    TEST_ASSERT("fabs(5.5) = 5.5", float_equal(fabs(5.5), 5.5));
    TEST_ASSERT("fabs(-5.5) = 5.5", float_equal(fabs(-5.5), 5.5));
    TEST_ASSERT("fabs(0) = 0", float_equal(fabs(0.0), 0.0));
    return NULL;
}

/* Test ceil */
static const char *test_ceil(void) {
    TEST_ASSERT("ceil(3.2) = 4", float_equal(ceil(3.2), 4.0));
    TEST_ASSERT("ceil(-3.2) = -3", float_equal(ceil(-3.2), -3.0));
    TEST_ASSERT("ceil(5.0) = 5", float_equal(ceil(5.0), 5.0));
    TEST_ASSERT("ceil(0.1) = 1", float_equal(ceil(0.1), 1.0));
    return NULL;
}

/* Test floor */
static const char *test_floor(void) {
    TEST_ASSERT("floor(3.9) = 3", float_equal(floor(3.9), 3.0));
    TEST_ASSERT("floor(-3.2) = -4", float_equal(floor(-3.2), -4.0));
    TEST_ASSERT("floor(5.0) = 5", float_equal(floor(5.0), 5.0));
    TEST_ASSERT("floor(0.9) = 0", float_equal(floor(0.9), 0.0));
    return NULL;
}

/* Test sin */
static const char *test_sin(void) {
    TEST_ASSERT("sin(0) = 0", float_equal(sin(0.0), 0.0));
    TEST_ASSERT("sin(PI/2) = 1", float_equal(sin(M_PI/2), 1.0));
    TEST_ASSERT("sin(PI) = 0", fabs(sin(M_PI)) < EPSILON);
    TEST_ASSERT("sin(-PI/2) = -1", float_equal(sin(-M_PI/2), -1.0));
    return NULL;
}

/* Test cos */
static const char *test_cos(void) {
    TEST_ASSERT("cos(0) = 1", float_equal(cos(0.0), 1.0));
    TEST_ASSERT("cos(PI/2) = 0", fabs(cos(M_PI/2)) < EPSILON);
    TEST_ASSERT("cos(PI) = -1", float_equal(cos(M_PI), -1.0));
    TEST_ASSERT("cos(2*PI) = 1", float_equal(cos(2*M_PI), 1.0));
    return NULL;
}

/* Test tan */
static const char *test_tan(void) {
    TEST_ASSERT("tan(0) = 0", float_equal(tan(0.0), 0.0));
    TEST_ASSERT("tan(PI/4) ≈ 1", fabs(tan(M_PI/4) - 1.0) < 0.01);
    TEST_ASSERT("tan(-PI/4) ≈ -1", fabs(tan(-M_PI/4) + 1.0) < 0.01);
    return NULL;
}

/* Test asin */
static const char *test_asin(void) {
    TEST_ASSERT("asin(0) = 0", float_equal(asin(0.0), 0.0));
    TEST_ASSERT("asin(1) = PI/2", float_equal(asin(1.0), M_PI/2));
    TEST_ASSERT("asin(-1) = -PI/2", float_equal(asin(-1.0), -M_PI/2));
    TEST_ASSERT("asin(0.5) = PI/6", fabs(asin(0.5) - M_PI/6) < 0.01);
    return NULL;
}

/* Test acos */
static const char *test_acos(void) {
    TEST_ASSERT("acos(1) = 0", float_equal(acos(1.0), 0.0));
    TEST_ASSERT("acos(0) = PI/2", float_equal(acos(0.0), M_PI/2));
    TEST_ASSERT("acos(-1) = PI", float_equal(acos(-1.0), M_PI));
    return NULL;
}

/* Test atan */
static const char *test_atan(void) {
    TEST_ASSERT("atan(0) = 0", float_equal(atan(0.0), 0.0));
    TEST_ASSERT("atan(1) = PI/4", float_equal(atan(1.0), M_PI/4));
    TEST_ASSERT("atan(-1) = -PI/4", float_equal(atan(-1.0), -M_PI/4));
    return NULL;
}

/* Test atan2 */
static const char *test_atan2(void) {
    TEST_ASSERT("atan2(0, 1) = 0", float_equal(atan2(0.0, 1.0), 0.0));
    TEST_ASSERT("atan2(1, 1) = PI/4", float_equal(atan2(1.0, 1.0), M_PI/4));
    TEST_ASSERT("atan2(1, 0) = PI/2", float_equal(atan2(1.0, 0.0), M_PI/2));
    TEST_ASSERT("atan2(0, -1) = PI", float_equal(atan2(0.0, -1.0), M_PI));
    return NULL;
}

/* Test exp */
static const char *test_exp(void) {
    TEST_ASSERT("exp(0) = 1", float_equal(exp(0.0), 1.0));
    TEST_ASSERT("exp(1) = e", float_equal(exp(1.0), M_E));
    TEST_ASSERT("exp(2) = e^2", float_equal(exp(2.0), M_E * M_E));
    return NULL;
}

/* Test log */
static const char *test_log(void) {
    TEST_ASSERT("log(1) = 0", float_equal(log(1.0), 0.0));
    TEST_ASSERT("log(e) = 1", float_equal(log(M_E), 1.0));
    TEST_ASSERT("log(e^2) = 2", float_equal(log(M_E * M_E), 2.0));
    return NULL;
}

/* Test log10 */
static const char *test_log10(void) {
    TEST_ASSERT("log10(1) = 0", float_equal(log10(1.0), 0.0));
    TEST_ASSERT("log10(10) = 1", float_equal(log10(10.0), 1.0));
    TEST_ASSERT("log10(100) = 2", float_equal(log10(100.0), 2.0));
    TEST_ASSERT("log10(1000) = 3", float_equal(log10(1000.0), 3.0));
    return NULL;
}

/* Test fmod */
static const char *test_fmod(void) {
    TEST_ASSERT("fmod(5.5, 2.0) = 1.5", float_equal(fmod(5.5, 2.0), 1.5));
    TEST_ASSERT("fmod(18.5, 4.2) ≈ 1.7", fabs(fmod(18.5, 4.2) - 1.7) < 0.1);
    TEST_ASSERT("fmod(5.0, 2.0) = 1.0", float_equal(fmod(5.0, 2.0), 1.0));
    return NULL;
}

/* Test modf */
static const char *test_modf(void) {
    double intpart;
    double fracpart;
    
    fracpart = modf(3.14159, &intpart);
    TEST_ASSERT("modf(3.14159) int part = 3", float_equal(intpart, 3.0));
    TEST_ASSERT("modf(3.14159) frac part ≈ 0.14159", fabs(fracpart - 0.14159) < 0.001);
    
    fracpart = modf(-2.5, &intpart);
    TEST_ASSERT("modf(-2.5) int part = -2", float_equal(intpart, -2.0));
    TEST_ASSERT("modf(-2.5) frac part = -0.5", float_equal(fracpart, -0.5));
    
    return NULL;
}

/* Test frexp */
static const char *test_frexp(void) {
    int exp;
    double mantissa;
    
    mantissa = frexp(8.0, &exp);
    TEST_ASSERT("frexp(8.0) exponent = 4", exp == 4);
    TEST_ASSERT("frexp(8.0) mantissa = 0.5", float_equal(mantissa, 0.5));
    
    mantissa = frexp(1.0, &exp);
    TEST_ASSERT("frexp(1.0) exponent = 1", exp == 1);
    TEST_ASSERT("frexp(1.0) mantissa = 0.5", float_equal(mantissa, 0.5));
    
    return NULL;
}

/* Test ldexp */
static const char *test_ldexp(void) {
    TEST_ASSERT("ldexp(0.5, 4) = 8", float_equal(ldexp(0.5, 4), 8.0));
    TEST_ASSERT("ldexp(1.0, 0) = 1", float_equal(ldexp(1.0, 0), 1.0));
    TEST_ASSERT("ldexp(2.0, 3) = 16", float_equal(ldexp(2.0, 3), 16.0));
    return NULL;
}

/* Test sinh */
static const char *test_sinh(void) {
    TEST_ASSERT("sinh(0) = 0", float_equal(sinh(0.0), 0.0));
    TEST_ASSERT("sinh(1) ≈ 1.175", fabs(sinh(1.0) - 1.175) < 0.01);
    TEST_ASSERT("sinh(-1) ≈ -1.175", fabs(sinh(-1.0) + 1.175) < 0.01);
    return NULL;
}

/* Test cosh */
static const char *test_cosh(void) {
    TEST_ASSERT("cosh(0) = 1", float_equal(cosh(0.0), 1.0));
    TEST_ASSERT("cosh(1) ≈ 1.543", fabs(cosh(1.0) - 1.543) < 0.01);
    return NULL;
}

/* Test tanh */
static const char *test_tanh(void) {
    TEST_ASSERT("tanh(0) = 0", float_equal(tanh(0.0), 0.0));
    TEST_ASSERT("tanh(1) ≈ 0.762", fabs(tanh(1.0) - 0.762) < 0.01);
    return NULL;
}

/* Test round */
static const char *test_round(void) {
    TEST_ASSERT("round(3.4) = 3", float_equal(round(3.4), 3.0));
    TEST_ASSERT("round(3.5) = 4", float_equal(round(3.5), 4.0));
    TEST_ASSERT("round(3.6) = 4", float_equal(round(3.6), 4.0));
    TEST_ASSERT("round(-3.5) = -4", float_equal(round(-3.5), -4.0));
    return NULL;
}

/* Test trunc */
static const char *test_trunc(void) {
    TEST_ASSERT("trunc(3.9) = 3", float_equal(trunc(3.9), 3.0));
    TEST_ASSERT("trunc(-3.9) = -3", float_equal(trunc(-3.9), -3.0));
    TEST_ASSERT("trunc(5.0) = 5", float_equal(trunc(5.0), 5.0));
    return NULL;
}

/* Test isnan */
static const char *test_isnan(void) {
    TEST_ASSERT("!isnan(1.0)", !isnan(1.0));
    TEST_ASSERT("!isnan(0.0)", !isnan(0.0));
    TEST_ASSERT("isnan(0.0/0.0)", isnan(0.0/0.0));
    return NULL;
}

/* Test isinf */
static const char *test_isinf(void) {
    TEST_ASSERT("!isinf(1.0)", !isinf(1.0));
    TEST_ASSERT("!isinf(0.0)", !isinf(0.0));
    TEST_ASSERT("isinf(1.0/0.0)", isinf(1.0/0.0));
    return NULL;
}

/* Test isfinite */
static const char *test_isfinite(void) {
    TEST_ASSERT("isfinite(1.0)", isfinite(1.0));
    TEST_ASSERT("isfinite(0.0)", isfinite(0.0));
    TEST_ASSERT("!isfinite(1.0/0.0)", !isfinite(1.0/0.0));
    return NULL;
}

/* Main test runner */
int main(void) {
    BEGIN_TEST_SUITE("math.h");
    
    RUN_TEST(test_sqrt);
    RUN_TEST(test_pow);
    RUN_TEST(test_fabs);
    RUN_TEST(test_ceil);
    RUN_TEST(test_floor);
    RUN_TEST(test_sin);
    RUN_TEST(test_cos);
    RUN_TEST(test_tan);
    RUN_TEST(test_asin);
    RUN_TEST(test_acos);
    RUN_TEST(test_atan);
    RUN_TEST(test_atan2);
    RUN_TEST(test_exp);
    RUN_TEST(test_log);
    RUN_TEST(test_log10);
    RUN_TEST(test_fmod);
    RUN_TEST(test_modf);
    RUN_TEST(test_frexp);
    RUN_TEST(test_ldexp);
    RUN_TEST(test_sinh);
    RUN_TEST(test_cosh);
    RUN_TEST(test_tanh);
    RUN_TEST(test_round);
    RUN_TEST(test_trunc);
    RUN_TEST(test_isnan);
    RUN_TEST(test_isinf);
    RUN_TEST(test_isfinite);
    
    END_TEST_SUITE();
    
    return tests_failed;
}
