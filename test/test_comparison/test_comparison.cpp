#include <unity.h>
#include <FixedPoint.h>
#if defined(USBCON)
#include <Arduino.h>
#endif

void setUp()    {}
void tearDown() {}

// ── Equality ─────────────────────────────────────────────────────────────────

void test_equal_same_value() {
    TEST_ASSERT_TRUE(F32_16(1.5f) == F32_16(1.5f));
}

void test_equal_both_zero() {
    TEST_ASSERT_TRUE(F32_16(0.0f) == F32_16(0.0f));
}

void test_equal_both_negative() {
    TEST_ASSERT_TRUE(F32_16(-2.0f) == F32_16(-2.0f));
}

void test_equal_different_values_is_false() {
    TEST_ASSERT_FALSE(F32_16(1.0f) == F32_16(2.0f));
}

void test_equal_pos_and_neg_is_false() {
    TEST_ASSERT_FALSE(F32_16(1.0f) == F32_16(-1.0f));
}

// ── Inequality ────────────────────────────────────────────────────────────────

void test_not_equal_different_values() {
    TEST_ASSERT_TRUE(F32_16(1.0f) != F32_16(2.0f));
}

void test_not_equal_same_value_is_false() {
    TEST_ASSERT_FALSE(F32_16(1.5f) != F32_16(1.5f));
}

void test_not_equal_pos_and_neg() {
    TEST_ASSERT_TRUE(F32_16(1.0f) != F32_16(-1.0f));
}

// ── Less than ────────────────────────────────────────────────────────────────

void test_less_than_positive_pair() {
    TEST_ASSERT_TRUE(F32_16(1.0f) < F32_16(2.0f));
}

void test_less_than_negative_pair() {
    TEST_ASSERT_TRUE(F32_16(-2.0f) < F32_16(-1.0f));
}

void test_less_than_neg_vs_pos() {
    TEST_ASSERT_TRUE(F32_16(-1.0f) < F32_16(1.0f));
}

void test_less_than_zero_vs_pos() {
    TEST_ASSERT_TRUE(F32_16(0.0f) < F32_16(0.5f));
}

void test_less_than_equal_is_false() {
    TEST_ASSERT_FALSE(F32_16(1.5f) < F32_16(1.5f));
}

void test_less_than_greater_is_false() {
    TEST_ASSERT_FALSE(F32_16(2.0f) < F32_16(1.0f));
}

// ── Greater than ─────────────────────────────────────────────────────────────

void test_greater_than_positive_pair() {
    TEST_ASSERT_TRUE(F32_16(2.0f) > F32_16(1.0f));
}

void test_greater_than_neg_pair() {
    TEST_ASSERT_TRUE(F32_16(-1.0f) > F32_16(-2.0f));
}

void test_greater_than_pos_vs_neg() {
    TEST_ASSERT_TRUE(F32_16(1.0f) > F32_16(-1.0f));
}

void test_greater_than_equal_is_false() {
    TEST_ASSERT_FALSE(F32_16(1.5f) > F32_16(1.5f));
}

void test_greater_than_lesser_is_false() {
    TEST_ASSERT_FALSE(F32_16(1.0f) > F32_16(2.0f));
}

// ── Less than or equal ───────────────────────────────────────────────────────

void test_leq_less_case() {
    TEST_ASSERT_TRUE(F32_16(1.0f) <= F32_16(2.0f));
}

void test_leq_equal_case() {
    TEST_ASSERT_TRUE(F32_16(1.5f) <= F32_16(1.5f));
}

void test_leq_zero_and_zero() {
    TEST_ASSERT_TRUE(F32_16(0.0f) <= F32_16(0.0f));
}

void test_leq_greater_is_false() {
    TEST_ASSERT_FALSE(F32_16(2.0f) <= F32_16(1.0f));
}

void test_leq_neg_vs_pos() {
    TEST_ASSERT_TRUE(F32_16(-1.0f) <= F32_16(1.0f));
}

// ── Greater than or equal ────────────────────────────────────────────────────

void test_geq_greater_case() {
    TEST_ASSERT_TRUE(F32_16(2.0f) >= F32_16(1.0f));
}

void test_geq_equal_case() {
    TEST_ASSERT_TRUE(F32_16(1.5f) >= F32_16(1.5f));
}

void test_geq_zero_and_zero() {
    TEST_ASSERT_TRUE(F32_16(0.0f) >= F32_16(0.0f));
}

void test_geq_lesser_is_false() {
    TEST_ASSERT_FALSE(F32_16(1.0f) >= F32_16(2.0f));
}

void test_geq_neg_vs_pos_is_false() {
    TEST_ASSERT_FALSE(F32_16(-1.0f) >= F32_16(1.0f));
}

// ── Ordering consistency: a < b implies !(b < a) ────────────────────────────

void test_strict_order_antisymmetric() {
    F32_16 a(1.0f), b(2.0f);
    TEST_ASSERT_TRUE(a < b);
    TEST_ASSERT_FALSE(b < a);
}

void test_strict_order_neg_antisymmetric() {
    F32_16 a(-2.0f), b(-1.0f);
    TEST_ASSERT_TRUE(a < b);
    TEST_ASSERT_FALSE(b < a);
}

// ── After arithmetic, comparison still correct ───────────────────────────────

void test_compare_after_add() {
    F32_16 a(1.0f);
    F32_16 b(2.0f);
    a += F32_16(1.0f);   // a = 2.0
    TEST_ASSERT_TRUE(a == b);
}

void test_compare_after_mul() {
    F32_16 a(1.5f);
    F32_16 b(3.0f);
    a *= F32_16(2.0f);   // a = 3.0
    TEST_ASSERT_TRUE(a == b);
    TEST_ASSERT_FALSE(a != b);
}

// ── Unsigned type comparisons ────────────────────────────────────────────────

void test_uf32_equal() {
    TEST_ASSERT_TRUE(UF32_16(1.5f) == UF32_16(1.5f));
}

void test_uf32_less_than() {
    TEST_ASSERT_TRUE(UF32_16(0.5f) < UF32_16(1.5f));
}

void test_uf32_greater_than() {
    TEST_ASSERT_TRUE(UF32_16(2.0f) > UF32_16(1.0f));
}

// ── Narrow type comparisons ──────────────────────────────────────────────────

void test_f16_8_less_than() {
    TEST_ASSERT_TRUE(F16_8(1.0f) < F16_8(2.0f));
}

void test_f16_8_equal() {
    TEST_ASSERT_TRUE(F16_8(1.5f) == F16_8(1.5f));
}

// ─────────────────────────────────────────────────────────────────────────────

#if defined(ARDUINO) || defined(__AVR__) || defined(__arm__) || defined(__ARM_ARCH)
extern "C" void setup() {
#if defined(USBCON)
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    delay(1000);
#endif
    UNITY_BEGIN();
    RUN_TEST(test_equal_same_value);
    RUN_TEST(test_equal_both_zero);
    RUN_TEST(test_equal_both_negative);
    RUN_TEST(test_equal_different_values_is_false);
    RUN_TEST(test_equal_pos_and_neg_is_false);
    RUN_TEST(test_not_equal_different_values);
    RUN_TEST(test_not_equal_same_value_is_false);
    RUN_TEST(test_not_equal_pos_and_neg);
    RUN_TEST(test_less_than_positive_pair);
    RUN_TEST(test_less_than_negative_pair);
    RUN_TEST(test_less_than_neg_vs_pos);
    RUN_TEST(test_less_than_zero_vs_pos);
    RUN_TEST(test_less_than_equal_is_false);
    RUN_TEST(test_less_than_greater_is_false);
    RUN_TEST(test_greater_than_positive_pair);
    RUN_TEST(test_greater_than_neg_pair);
    RUN_TEST(test_greater_than_pos_vs_neg);
    RUN_TEST(test_greater_than_equal_is_false);
    RUN_TEST(test_greater_than_lesser_is_false);
    RUN_TEST(test_leq_less_case);
    RUN_TEST(test_leq_equal_case);
    RUN_TEST(test_leq_zero_and_zero);
    RUN_TEST(test_leq_greater_is_false);
    RUN_TEST(test_leq_neg_vs_pos);
    RUN_TEST(test_geq_greater_case);
    RUN_TEST(test_geq_equal_case);
    RUN_TEST(test_geq_zero_and_zero);
    RUN_TEST(test_geq_lesser_is_false);
    RUN_TEST(test_geq_neg_vs_pos_is_false);
    RUN_TEST(test_strict_order_antisymmetric);
    RUN_TEST(test_strict_order_neg_antisymmetric);
    RUN_TEST(test_compare_after_add);
    RUN_TEST(test_compare_after_mul);
    RUN_TEST(test_uf32_equal);
    RUN_TEST(test_uf32_less_than);
    RUN_TEST(test_uf32_greater_than);
    RUN_TEST(test_f16_8_less_than);
    RUN_TEST(test_f16_8_equal);
    UNITY_END();
}
extern "C" void loop() {}
#else
int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_equal_same_value);
    RUN_TEST(test_equal_both_zero);
    RUN_TEST(test_equal_both_negative);
    RUN_TEST(test_equal_different_values_is_false);
    RUN_TEST(test_equal_pos_and_neg_is_false);
    RUN_TEST(test_not_equal_different_values);
    RUN_TEST(test_not_equal_same_value_is_false);
    RUN_TEST(test_not_equal_pos_and_neg);
    RUN_TEST(test_less_than_positive_pair);
    RUN_TEST(test_less_than_negative_pair);
    RUN_TEST(test_less_than_neg_vs_pos);
    RUN_TEST(test_less_than_zero_vs_pos);
    RUN_TEST(test_less_than_equal_is_false);
    RUN_TEST(test_less_than_greater_is_false);
    RUN_TEST(test_greater_than_positive_pair);
    RUN_TEST(test_greater_than_neg_pair);
    RUN_TEST(test_greater_than_pos_vs_neg);
    RUN_TEST(test_greater_than_equal_is_false);
    RUN_TEST(test_greater_than_lesser_is_false);
    RUN_TEST(test_leq_less_case);
    RUN_TEST(test_leq_equal_case);
    RUN_TEST(test_leq_zero_and_zero);
    RUN_TEST(test_leq_greater_is_false);
    RUN_TEST(test_leq_neg_vs_pos);
    RUN_TEST(test_geq_greater_case);
    RUN_TEST(test_geq_equal_case);
    RUN_TEST(test_geq_zero_and_zero);
    RUN_TEST(test_geq_lesser_is_false);
    RUN_TEST(test_geq_neg_vs_pos_is_false);
    RUN_TEST(test_strict_order_antisymmetric);
    RUN_TEST(test_strict_order_neg_antisymmetric);
    RUN_TEST(test_compare_after_add);
    RUN_TEST(test_compare_after_mul);
    RUN_TEST(test_uf32_equal);
    RUN_TEST(test_uf32_less_than);
    RUN_TEST(test_uf32_greater_than);
    RUN_TEST(test_f16_8_less_than);
    RUN_TEST(test_f16_8_equal);
    return UNITY_END();
}
#endif
