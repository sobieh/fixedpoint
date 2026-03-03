#include <unity.h>
#include <FixedPoint.h>
#if defined(USBCON)
#include <Arduino.h>
#endif

void setUp()    {}
void tearDown() {}

// ── Addition ──────────────────────────────────────────────────────────────────

void test_add_pos_plus_pos() {
    // 1.0 + 2.0 = 3.0 → raw 65536 + 131072 = 196608
    TEST_ASSERT_EQUAL_INT32(196608, (F32_16(1.0f) + F32_16(2.0f)).raw());
}

void test_add_fractional() {
    // 1.5 + 1.5 = 3.0 → raw 98304 + 98304 = 196608
    TEST_ASSERT_EQUAL_INT32(196608, (F32_16(1.5f) + F32_16(1.5f)).raw());
}

void test_add_neg_plus_pos() {
    // -1.0 + 2.0 = 1.0 → -65536 + 131072 = 65536
    TEST_ASSERT_EQUAL_INT32(65536, (F32_16(-1.0f) + F32_16(2.0f)).raw());
}

void test_add_neg_plus_neg() {
    // -1.0 + -1.0 = -2.0 → -65536 + -65536 = -131072
    TEST_ASSERT_EQUAL_INT32(-131072, (F32_16(-1.0f) + F32_16(-1.0f)).raw());
}

void test_add_identity_zero() {
    // x + 0 = x
    TEST_ASSERT_EQUAL_INT32(98304, (F32_16(1.5f) + F32_16(0.0f)).raw());
}

void test_add_commutativity() {
    // a + b == b + a at raw level
    F32_16 a(1.5f), b(2.0f);
    TEST_ASSERT_EQUAL_INT32((a + b).raw(), (b + a).raw());
}

void test_add_f16_8() {
    // 1.5 + 1.0 = 2.5 → raw 384 + 256 = 640  (denom=256)
    TEST_ASSERT_EQUAL_INT16(640, (F16_8(1.5f) + F16_8(1.0f)).raw());
}

// ── Subtraction ───────────────────────────────────────────────────────────────

void test_sub_pos_minus_pos() {
    // 3.0 - 1.0 = 2.0 → 196608 - 65536 = 131072
    TEST_ASSERT_EQUAL_INT32(131072, (F32_16(3.0f) - F32_16(1.0f)).raw());
}

void test_sub_goes_negative() {
    // 1.0 - 3.0 = -2.0 → 65536 - 196608 = -131072
    TEST_ASSERT_EQUAL_INT32(-131072, (F32_16(1.0f) - F32_16(3.0f)).raw());
}

void test_sub_neg_minus_neg_cancel() {
    // -1.0 - (-1.0) = 0
    TEST_ASSERT_EQUAL_INT32(0, (F32_16(-1.0f) - F32_16(-1.0f)).raw());
}

void test_sub_identity_zero() {
    // x - 0 = x
    TEST_ASSERT_EQUAL_INT32(98304, (F32_16(1.5f) - F32_16(0.0f)).raw());
}

void test_sub_self_is_zero() {
    F32_16 a(3.5f);
    TEST_ASSERT_EQUAL_INT32(0, (a - a).raw());
}

// ── Multiplication (FP × FP) ──────────────────────────────────────────────────
// Formula: (wide_a * wide_b) >> 16
// All intermediate values confirmed to fit in int64_t.

void test_mul_integer_by_integer() {
    // 2.0 * 3.0 = 6.0 → (131072 * 196608) >> 16 = 393216
    TEST_ASSERT_EQUAL_INT32(393216, (F32_16(2.0f) * F32_16(3.0f)).raw());
}

void test_mul_fraction_by_fraction() {
    // 0.5 * 0.5 = 0.25 → (32768 * 32768) >> 16 = 16384
    TEST_ASSERT_EQUAL_INT32(16384, (F32_16(0.5f) * F32_16(0.5f)).raw());
}

void test_mul_neg_by_pos() {
    // -2.0 * 3.0 = -6.0 → (-131072 * 196608) >> 16 = -393216
    TEST_ASSERT_EQUAL_INT32(-393216, (F32_16(-2.0f) * F32_16(3.0f)).raw());
}

void test_mul_neg_by_neg_is_pos() {
    // -2.0 * -3.0 = 6.0 → same magnitude as positive case
    TEST_ASSERT_EQUAL_INT32(393216, (F32_16(-2.0f) * F32_16(-3.0f)).raw());
}

void test_mul_by_zero() {
    TEST_ASSERT_EQUAL_INT32(0, (F32_16(1.5f) * F32_16(0.0f)).raw());
}

void test_mul_by_one_identity() {
    // x * 1.0 = x
    TEST_ASSERT_EQUAL_INT32(98304, (F32_16(1.5f) * F32_16(1.0f)).raw());
}

void test_mul_mixed_fraction() {
    // 1.5 * 2.0 = 3.0
    TEST_ASSERT_EQUAL_INT32(196608, (F32_16(1.5f) * F32_16(2.0f)).raw());
}

void test_mul_quarter_by_four() {
    // 0.25 * 4.0 = 1.0
    TEST_ASSERT_EQUAL_INT32(65536, (F32_16(0.25f) * F32_16(4.0f)).raw());
}

void test_mul_sub_one_fractions() {
    // 0.5 * 0.25 = 0.125 → (32768 * 16384) >> 16 = 8192
    TEST_ASSERT_EQUAL_INT32(8192, (F32_16(0.5f) * F32_16(0.25f)).raw());
}

void test_mul_commutativity() {
    // a * b == b * a
    F32_16 a(1.5f), b(2.0f);
    TEST_ASSERT_EQUAL_INT32((a * b).raw(), (b * a).raw());
}

void test_mul_f16_8() {
    // 3.0 * 2.0 = 6.0 → (768 * 512) >> 8 = 1536  (denom=256)
    TEST_ASSERT_EQUAL_INT16(1536, (F16_8(3.0f) * F16_8(2.0f)).raw());
}

void test_mul_f8_4() {
    // 1.5 * 2.0 = 3.0 → (24 * 32) >> 4 = 48  (denom=16)
    TEST_ASSERT_EQUAL_INT8(48, (F8_4(1.5f) * F8_4(2.0f)).raw());
}

// ── Division (FP ÷ FP) ───────────────────────────────────────────────────────
// Formula: (wide_a << 16) / wide_b

void test_div_integer_by_integer() {
    // 6.0 / 2.0 = 3.0 → (393216 << 16) / 131072 = 196608
    TEST_ASSERT_EQUAL_INT32(196608, (F32_16(6.0f) / F32_16(2.0f)).raw());
}

void test_div_result_is_fraction() {
    // 1.0 / 4.0 = 0.25 → (65536 << 16) / 262144 = 16384
    TEST_ASSERT_EQUAL_INT32(16384, (F32_16(1.0f) / F32_16(4.0f)).raw());
}

void test_div_neg_by_pos() {
    // -6.0 / 2.0 = -3.0
    TEST_ASSERT_EQUAL_INT32(-196608, (F32_16(-6.0f) / F32_16(2.0f)).raw());
}

void test_div_neg_by_neg_is_pos() {
    // -6.0 / -2.0 = 3.0
    TEST_ASSERT_EQUAL_INT32(196608, (F32_16(-6.0f) / F32_16(-2.0f)).raw());
}

void test_div_by_one_identity() {
    // x / 1.0 = x
    TEST_ASSERT_EQUAL_INT32(98304, (F32_16(1.5f) / F32_16(1.0f)).raw());
}

void test_div_result_one_and_half() {
    // 3.0 / 2.0 = 1.5
    TEST_ASSERT_EQUAL_INT32(98304, (F32_16(3.0f) / F32_16(2.0f)).raw());
}

void test_div_self_is_one() {
    // a / a == 1.0
    F32_16 a(3.5f);
    TEST_ASSERT_EQUAL_INT32(65536, (a / a).raw());
}

// ── Scalar integer multiply / divide ─────────────────────────────────────────
// No intermediate promotion — raw * scalar or raw / scalar directly.

void test_scalar_mul_pos() {
    // F32_16(2.0f) * 3 → raw 131072 * 3 = 393216
    TEST_ASSERT_EQUAL_INT32(393216, (F32_16(2.0f) * (int32_t)3).raw());
}

void test_scalar_mul_commutative() {
    // 3 * F32_16(2.0f) == F32_16(2.0f) * 3
    TEST_ASSERT_EQUAL_INT32(393216, ((int32_t)3 * F32_16(2.0f)).raw());
}

void test_scalar_mul_negative() {
    // F32_16(2.0f) * -2 → raw 131072 * -2 = -262144
    TEST_ASSERT_EQUAL_INT32(-262144, (F32_16(2.0f) * (int32_t)-2).raw());
}

void test_scalar_mul_zero() {
    TEST_ASSERT_EQUAL_INT32(0, (F32_16(1.5f) * (int32_t)0).raw());
}

void test_scalar_div_halve() {
    // F32_16(6.0f) / 2 → raw 393216 / 2 = 196608
    TEST_ASSERT_EQUAL_INT32(196608, (F32_16(6.0f) / (int32_t)2).raw());
}

void test_scalar_div_quarter() {
    // F32_16(0.5f) / 2 → raw 32768 / 2 = 16384 = 0.25
    TEST_ASSERT_EQUAL_INT32(16384, (F32_16(0.5f) / (int32_t)2).raw());
}

void test_scalar_mul_matches_fp_mul_for_integer_factor() {
    // x * 4  must equal  x * F32_16(4.0f)
    F32_16 x(1.5f);
    TEST_ASSERT_EQUAL_INT32((x * F32_16(4.0f)).raw(), (x * (int32_t)4).raw());
}

void test_scalar_div_matches_fp_div_for_power_of_two() {
    F32_16 x(4.0f);
    TEST_ASSERT_EQUAL_INT32((x / F32_16(2.0f)).raw(), (x / (int32_t)2).raw());
}

// ── Unary minus ───────────────────────────────────────────────────────────────

void test_unary_minus_positive() {
    TEST_ASSERT_EQUAL_INT32(-98304, (-F32_16(1.5f)).raw());
}

void test_unary_minus_negative() {
    TEST_ASSERT_EQUAL_INT32(98304, (-F32_16(-1.5f)).raw());
}

void test_unary_minus_zero() {
    TEST_ASSERT_EQUAL_INT32(0, (-F32_16(0.0f)).raw());
}

void test_unary_minus_double_negation() {
    // --x == x
    F32_16 x(3.5f);
    TEST_ASSERT_EQUAL_INT32(x.raw(), (-(-x)).raw());
}

// ── abs() ────────────────────────────────────────────────────────────────────

void test_abs_positive_unchanged() {
    TEST_ASSERT_EQUAL_INT32(229376, abs(F32_16(3.5f)).raw());
}

void test_abs_negative_becomes_positive() {
    TEST_ASSERT_EQUAL_INT32(229376, abs(F32_16(-3.5f)).raw());
}

void test_abs_zero() {
    TEST_ASSERT_EQUAL_INT32(0, abs(F32_16(0.0f)).raw());
}

void test_abs_member_matches_free_function() {
    F32_16 x(-1.5f);
    TEST_ASSERT_EQUAL_INT32(x.abs().raw(), abs(x).raw());
}

void test_abs_negative_one() {
    TEST_ASSERT_EQUAL_INT32(65536, abs(F32_16(-1.0f)).raw());
}

// ── Compound assignment ───────────────────────────────────────────────────────

void test_compound_add_assign() {
    F32_16 a(2.0f);
    a += F32_16(1.5f);
    TEST_ASSERT_EQUAL_INT32(229376, a.raw());   // 3.5
}

void test_compound_sub_assign() {
    F32_16 a(3.0f);
    a -= F32_16(1.5f);
    TEST_ASSERT_EQUAL_INT32(98304, a.raw());    // 1.5
}

void test_compound_mul_assign() {
    F32_16 a(2.0f);
    a *= F32_16(3.0f);
    TEST_ASSERT_EQUAL_INT32(393216, a.raw());   // 6.0
}

void test_compound_div_assign() {
    F32_16 a(6.0f);
    a /= F32_16(2.0f);
    TEST_ASSERT_EQUAL_INT32(196608, a.raw());   // 3.0
}

void test_compound_scalar_mul_assign() {
    F32_16 a(2.0f);
    a *= (int32_t)3;
    TEST_ASSERT_EQUAL_INT32(393216, a.raw());   // 6.0
}

void test_compound_scalar_div_assign() {
    F32_16 a(6.0f);
    a /= (int32_t)2;
    TEST_ASSERT_EQUAL_INT32(196608, a.raw());   // 3.0
}

void test_compound_returns_self_reference() {
    // Chaining: a += b += c must work correctly
    F32_16 a(1.0f), b(1.0f), c(1.0f);
    a += (b += c);
    TEST_ASSERT_EQUAL_INT32(131072, b.raw());   // b = 2.0
    TEST_ASSERT_EQUAL_INT32(196608, a.raw());   // a = 3.0
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
    RUN_TEST(test_add_pos_plus_pos);
    RUN_TEST(test_add_fractional);
    RUN_TEST(test_add_neg_plus_pos);
    RUN_TEST(test_add_neg_plus_neg);
    RUN_TEST(test_add_identity_zero);
    RUN_TEST(test_add_commutativity);
    RUN_TEST(test_add_f16_8);
    RUN_TEST(test_sub_pos_minus_pos);
    RUN_TEST(test_sub_goes_negative);
    RUN_TEST(test_sub_neg_minus_neg_cancel);
    RUN_TEST(test_sub_identity_zero);
    RUN_TEST(test_sub_self_is_zero);
    RUN_TEST(test_mul_integer_by_integer);
    RUN_TEST(test_mul_fraction_by_fraction);
    RUN_TEST(test_mul_neg_by_pos);
    RUN_TEST(test_mul_neg_by_neg_is_pos);
    RUN_TEST(test_mul_by_zero);
    RUN_TEST(test_mul_by_one_identity);
    RUN_TEST(test_mul_mixed_fraction);
    RUN_TEST(test_mul_quarter_by_four);
    RUN_TEST(test_mul_sub_one_fractions);
    RUN_TEST(test_mul_commutativity);
    RUN_TEST(test_mul_f16_8);
    RUN_TEST(test_mul_f8_4);
    RUN_TEST(test_div_integer_by_integer);
    RUN_TEST(test_div_result_is_fraction);
    RUN_TEST(test_div_neg_by_pos);
    RUN_TEST(test_div_neg_by_neg_is_pos);
    RUN_TEST(test_div_by_one_identity);
    RUN_TEST(test_div_result_one_and_half);
    RUN_TEST(test_div_self_is_one);
    RUN_TEST(test_scalar_mul_pos);
    RUN_TEST(test_scalar_mul_commutative);
    RUN_TEST(test_scalar_mul_negative);
    RUN_TEST(test_scalar_mul_zero);
    RUN_TEST(test_scalar_div_halve);
    RUN_TEST(test_scalar_div_quarter);
    RUN_TEST(test_scalar_mul_matches_fp_mul_for_integer_factor);
    RUN_TEST(test_scalar_div_matches_fp_div_for_power_of_two);
    RUN_TEST(test_unary_minus_positive);
    RUN_TEST(test_unary_minus_negative);
    RUN_TEST(test_unary_minus_zero);
    RUN_TEST(test_unary_minus_double_negation);
    RUN_TEST(test_abs_positive_unchanged);
    RUN_TEST(test_abs_negative_becomes_positive);
    RUN_TEST(test_abs_zero);
    RUN_TEST(test_abs_member_matches_free_function);
    RUN_TEST(test_abs_negative_one);
    RUN_TEST(test_compound_add_assign);
    RUN_TEST(test_compound_sub_assign);
    RUN_TEST(test_compound_mul_assign);
    RUN_TEST(test_compound_div_assign);
    RUN_TEST(test_compound_scalar_mul_assign);
    RUN_TEST(test_compound_scalar_div_assign);
    RUN_TEST(test_compound_returns_self_reference);
    UNITY_END();
}
extern "C" void loop() {}
#else
int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_add_pos_plus_pos);
    RUN_TEST(test_add_fractional);
    RUN_TEST(test_add_neg_plus_pos);
    RUN_TEST(test_add_neg_plus_neg);
    RUN_TEST(test_add_identity_zero);
    RUN_TEST(test_add_commutativity);
    RUN_TEST(test_add_f16_8);
    RUN_TEST(test_sub_pos_minus_pos);
    RUN_TEST(test_sub_goes_negative);
    RUN_TEST(test_sub_neg_minus_neg_cancel);
    RUN_TEST(test_sub_identity_zero);
    RUN_TEST(test_sub_self_is_zero);
    RUN_TEST(test_mul_integer_by_integer);
    RUN_TEST(test_mul_fraction_by_fraction);
    RUN_TEST(test_mul_neg_by_pos);
    RUN_TEST(test_mul_neg_by_neg_is_pos);
    RUN_TEST(test_mul_by_zero);
    RUN_TEST(test_mul_by_one_identity);
    RUN_TEST(test_mul_mixed_fraction);
    RUN_TEST(test_mul_quarter_by_four);
    RUN_TEST(test_mul_sub_one_fractions);
    RUN_TEST(test_mul_commutativity);
    RUN_TEST(test_mul_f16_8);
    RUN_TEST(test_mul_f8_4);
    RUN_TEST(test_div_integer_by_integer);
    RUN_TEST(test_div_result_is_fraction);
    RUN_TEST(test_div_neg_by_pos);
    RUN_TEST(test_div_neg_by_neg_is_pos);
    RUN_TEST(test_div_by_one_identity);
    RUN_TEST(test_div_result_one_and_half);
    RUN_TEST(test_div_self_is_one);
    RUN_TEST(test_scalar_mul_pos);
    RUN_TEST(test_scalar_mul_commutative);
    RUN_TEST(test_scalar_mul_negative);
    RUN_TEST(test_scalar_mul_zero);
    RUN_TEST(test_scalar_div_halve);
    RUN_TEST(test_scalar_div_quarter);
    RUN_TEST(test_scalar_mul_matches_fp_mul_for_integer_factor);
    RUN_TEST(test_scalar_div_matches_fp_div_for_power_of_two);
    RUN_TEST(test_unary_minus_positive);
    RUN_TEST(test_unary_minus_negative);
    RUN_TEST(test_unary_minus_zero);
    RUN_TEST(test_unary_minus_double_negation);
    RUN_TEST(test_abs_positive_unchanged);
    RUN_TEST(test_abs_negative_becomes_positive);
    RUN_TEST(test_abs_zero);
    RUN_TEST(test_abs_member_matches_free_function);
    RUN_TEST(test_abs_negative_one);
    RUN_TEST(test_compound_add_assign);
    RUN_TEST(test_compound_sub_assign);
    RUN_TEST(test_compound_mul_assign);
    RUN_TEST(test_compound_div_assign);
    RUN_TEST(test_compound_scalar_mul_assign);
    RUN_TEST(test_compound_scalar_div_assign);
    RUN_TEST(test_compound_returns_self_reference);
    return UNITY_END();
}
#endif
