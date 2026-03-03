#include <unity.h>
#include <FixedPoint.h>
#if defined(USBCON)
#include <Arduino.h>
#endif

void setUp()    {}
void tearDown() {}

// ── raw() ─────────────────────────────────────────────────────────────────────

void test_raw_positive() {
    TEST_ASSERT_EQUAL_INT32(98304, F32_16(1.5f).raw());
}

void test_raw_negative() {
    TEST_ASSERT_EQUAL_INT32(-98304, F32_16(-1.5f).raw());
}

void test_raw_zero() {
    TEST_ASSERT_EQUAL_INT32(0, F32_16(0.0f).raw());
}

void test_raw_tag_roundtrip() {
    const int32_t val = 123456;
    TEST_ASSERT_EQUAL_INT32(val, F32_16(fp::raw_tag, val).raw());
}

// ── integer() — positive values ──────────────────────────────────────────────

void test_integer_exact_integer() {
    TEST_ASSERT_EQUAL_INT32(1, F32_16(1.0f).integer());
}

void test_integer_with_fraction() {
    // 3.5 → integer = 3  (raw 229376 >> 16 = 3)
    TEST_ASSERT_EQUAL_INT32(3, F32_16(3.5f).integer());
}

void test_integer_pure_fraction_is_zero() {
    // 0.5 → integer = 0  (raw 32768 >> 16 = 0)
    TEST_ASSERT_EQUAL_INT32(0, F32_16(0.5f).integer());
}

void test_integer_zero() {
    TEST_ASSERT_EQUAL_INT32(0, F32_16(0.0f).integer());
}

void test_integer_power_of_two() {
    TEST_ASSERT_EQUAL_INT32(4, F32_16(4.0f).integer());
}

// ── integer() — negative values (floor semantics) ────────────────────────────

void test_integer_neg_exact() {
    // -1.0 → integer = -1  (raw -65536 >> 16 = -1)
    TEST_ASSERT_EQUAL_INT32(-1, F32_16(-1.0f).integer());
}

void test_integer_neg_with_fraction_floors() {
    // -1.5 → integer = -2  (floor, NOT -1)
    TEST_ASSERT_EQUAL_INT32(-2, F32_16(-1.5f).integer());
}

void test_integer_neg_two() {
    // -2.0 → integer = -2
    TEST_ASSERT_EQUAL_INT32(-2, F32_16(-2.0f).integer());
}

void test_integer_f16_8_positive() {
    TEST_ASSERT_EQUAL_INT16(1, F16_8(1.5f).integer());
}

void test_integer_f16_8_negative_floors() {
    // -1.5 with F16_8: raw = -384, -384 >> 8 = -2
    TEST_ASSERT_EQUAL_INT16(-2, F16_8(-1.5f).integer());
}

// ── fraction() — positive values ─────────────────────────────────────────────

void test_fraction_pure_fraction() {
    // 0.5 → fraction = 32768  (half of 65536)
    TEST_ASSERT_EQUAL_INT32(32768, F32_16(0.5f).fraction());
}

void test_fraction_of_exact_integer_is_zero() {
    // 3.0 → fraction = 0
    TEST_ASSERT_EQUAL_INT32(0, F32_16(3.0f).fraction());
}

void test_fraction_of_mixed() {
    // 3.5 → fraction = 32768
    TEST_ASSERT_EQUAL_INT32(32768, F32_16(3.5f).fraction());
}

void test_fraction_zero() {
    TEST_ASSERT_EQUAL_INT32(0, F32_16(0.0f).fraction());
}

void test_fraction_quarter() {
    // 0.25 → fraction = 16384
    TEST_ASSERT_EQUAL_INT32(16384, F32_16(0.25f).fraction());
}

void test_fraction_eighth() {
    // 0.125 → fraction = 8192
    TEST_ASSERT_EQUAL_INT32(8192, F32_16(0.125f).fraction());
}

// ── fraction() — negative values (& mask semantics) ──────────────────────────
// Because integer() floors, fraction() stays in [0, DENOM-1].
// The pair (integer, fraction) always reconstructs the original value exactly:
//   value == integer() + (fraction() / Denominator)

void test_fraction_neg_with_frac_part() {
    // -1.5: integer=-2, fraction=32768  →  -2 + 32768/65536 = -1.5 ✓
    TEST_ASSERT_EQUAL_INT32(32768, F32_16(-1.5f).fraction());
}

void test_fraction_neg_exact_integer_is_zero() {
    // -1.0: integer=-1, fraction=0  →  -1 + 0/65536 = -1.0 ✓
    TEST_ASSERT_EQUAL_INT32(0, F32_16(-1.0f).fraction());
}

void test_fraction_neg_two_is_zero() {
    TEST_ASSERT_EQUAL_INT32(0, F32_16(-2.0f).fraction());
}

void test_fraction_f16_8_positive() {
    // 1.5, denom=256: fraction = 128
    TEST_ASSERT_EQUAL_INT16(128, F16_8(1.5f).fraction());
}

void test_fraction_f16_8_negative() {
    // -1.5, denom=256: integer=-2, fraction=128
    TEST_ASSERT_EQUAL_INT16(128, F16_8(-1.5f).fraction());
}

// ── Reconstruction invariant: integer + fraction/DENOM == original ────────────

void test_reconstruction_positive() {
    F32_16 v(3.5f);
    float reconstructed = (float)v.integer() + (float)v.fraction() / (float)F32_16::Denominator;
    TEST_ASSERT_FLOAT_WITHIN(1.0f / F32_16::Denominator, 3.5f, reconstructed);
}

void test_reconstruction_negative() {
    F32_16 v(-1.5f);
    float reconstructed = (float)v.integer() + (float)v.fraction() / (float)F32_16::Denominator;
    TEST_ASSERT_FLOAT_WITHIN(1.0f / F32_16::Denominator, -1.5f, reconstructed);
}

void test_reconstruction_pure_fractional() {
    F32_16 v(0.25f);
    float reconstructed = (float)v.integer() + (float)v.fraction() / (float)F32_16::Denominator;
    TEST_ASSERT_FLOAT_WITHIN(1.0f / F32_16::Denominator, 0.25f, reconstructed);
}

// ── as_float() ───────────────────────────────────────────────────────────────
// Uses multiply-by-reciprocal internally (1/DENOM computed at compile time).
// All tested values are exact in IEEE-754 single, so zero tolerance is valid.

void test_as_float_one() {
    TEST_ASSERT_EQUAL_FLOAT(1.0f, F32_16(1.0f).as_float());
}

void test_as_float_one_and_half() {
    TEST_ASSERT_EQUAL_FLOAT(1.5f, F32_16(1.5f).as_float());
}

void test_as_float_negative() {
    TEST_ASSERT_EQUAL_FLOAT(-1.5f, F32_16(-1.5f).as_float());
}

void test_as_float_zero() {
    TEST_ASSERT_EQUAL_FLOAT(0.0f, F32_16(0.0f).as_float());
}

void test_as_float_quarter() {
    TEST_ASSERT_EQUAL_FLOAT(0.25f, F32_16(0.25f).as_float());
}

void test_as_float_raw_roundtrip() {
    float original = 3.5f;
    F32_16 v(original);
    TEST_ASSERT_EQUAL_FLOAT(original, v.as_float());
}

// ── as_double() ──────────────────────────────────────────────────────────────

void test_as_double_one() {
    TEST_ASSERT_DOUBLE_WITHIN(1.0 / F32_16::Denominator, 1.0, F32_16(1.0f).as_double());
}

void test_as_double_one_and_half() {
    TEST_ASSERT_DOUBLE_WITHIN(1.0 / F32_16::Denominator, 1.5, F32_16(1.5f).as_double());
}

void test_as_double_negative() {
    TEST_ASSERT_DOUBLE_WITHIN(1.0 / F32_16::Denominator, -1.5, F32_16(-1.5f).as_double());
}

// ── Bit shifts on raw storage ─────────────────────────────────────────────────
// >> and << operate on the raw value: halving/doubling the represented value.

void test_right_shift_by_1_halves_value() {
    // 4.0 >> 1 = 2.0  (raw 262144 >> 1 = 131072)
    TEST_ASSERT_EQUAL_INT32(131072, (F32_16(4.0f) >> 1).raw());
}

void test_right_shift_by_2_quarters_value() {
    // 4.0 >> 2 = 1.0  (raw 262144 >> 2 = 65536)
    TEST_ASSERT_EQUAL_INT32(65536, (F32_16(4.0f) >> 2).raw());
}

void test_left_shift_by_1_doubles_value() {
    // 1.0 << 1 = 2.0  (raw 65536 << 1 = 131072)
    TEST_ASSERT_EQUAL_INT32(131072, (F32_16(1.0f) << 1).raw());
}

void test_left_shift_by_2_quadruples_value() {
    // 1.0 << 2 = 4.0  (raw 65536 << 2 = 262144)
    TEST_ASSERT_EQUAL_INT32(262144, (F32_16(1.0f) << 2).raw());
}

void test_right_shift_fractional() {
    // 1.5 >> 1 = 0.75  (raw 98304 >> 1 = 49152)
    TEST_ASSERT_EQUAL_INT32(49152, (F32_16(1.5f) >> 1).raw());
}

void test_right_shift_negative_is_arithmetic() {
    // -4.0 >> 1 = -2.0  (arithmetic shift: raw -262144 >> 1 = -131072)
    TEST_ASSERT_EQUAL_INT32(-131072, (F32_16(-4.0f) >> 1).raw());
}

void test_shift_compound_assign_right() {
    F32_16 a(4.0f);
    a >>= 1;
    TEST_ASSERT_EQUAL_INT32(131072, a.raw());
}

void test_shift_compound_assign_left() {
    F32_16 a(1.0f);
    a <<= 2;
    TEST_ASSERT_EQUAL_INT32(262144, a.raw());
}

void test_shift_right_then_left_roundtrip() {
    // For exact values: (x >> n) << n == x  (no bits are lost)
    F32_16 original(4.0f);
    F32_16 result = (original >> 2) << 2;
    TEST_ASSERT_EQUAL_INT32(original.raw(), result.raw());
}

void test_shift_matches_scalar_div() {
    // x >> 1  must give the same raw as  x / 2  for even raw values
    F32_16 x(4.0f);
    TEST_ASSERT_EQUAL_INT32((x / (int32_t)2).raw(), (x >> 1).raw());
}

void test_shift_matches_scalar_mul() {
    F32_16 x(1.0f);
    TEST_ASSERT_EQUAL_INT32((x * (int32_t)4).raw(), (x << 2).raw());
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
    RUN_TEST(test_raw_positive);
    RUN_TEST(test_raw_negative);
    RUN_TEST(test_raw_zero);
    RUN_TEST(test_raw_tag_roundtrip);
    RUN_TEST(test_integer_exact_integer);
    RUN_TEST(test_integer_with_fraction);
    RUN_TEST(test_integer_pure_fraction_is_zero);
    RUN_TEST(test_integer_zero);
    RUN_TEST(test_integer_power_of_two);
    RUN_TEST(test_integer_neg_exact);
    RUN_TEST(test_integer_neg_with_fraction_floors);
    RUN_TEST(test_integer_neg_two);
    RUN_TEST(test_integer_f16_8_positive);
    RUN_TEST(test_integer_f16_8_negative_floors);
    RUN_TEST(test_fraction_pure_fraction);
    RUN_TEST(test_fraction_of_exact_integer_is_zero);
    RUN_TEST(test_fraction_of_mixed);
    RUN_TEST(test_fraction_zero);
    RUN_TEST(test_fraction_quarter);
    RUN_TEST(test_fraction_eighth);
    RUN_TEST(test_fraction_neg_with_frac_part);
    RUN_TEST(test_fraction_neg_exact_integer_is_zero);
    RUN_TEST(test_fraction_neg_two_is_zero);
    RUN_TEST(test_fraction_f16_8_positive);
    RUN_TEST(test_fraction_f16_8_negative);
    RUN_TEST(test_reconstruction_positive);
    RUN_TEST(test_reconstruction_negative);
    RUN_TEST(test_reconstruction_pure_fractional);
    RUN_TEST(test_as_float_one);
    RUN_TEST(test_as_float_one_and_half);
    RUN_TEST(test_as_float_negative);
    RUN_TEST(test_as_float_zero);
    RUN_TEST(test_as_float_quarter);
    RUN_TEST(test_as_float_raw_roundtrip);
    RUN_TEST(test_as_double_one);
    RUN_TEST(test_as_double_one_and_half);
    RUN_TEST(test_as_double_negative);
    RUN_TEST(test_right_shift_by_1_halves_value);
    RUN_TEST(test_right_shift_by_2_quarters_value);
    RUN_TEST(test_left_shift_by_1_doubles_value);
    RUN_TEST(test_left_shift_by_2_quadruples_value);
    RUN_TEST(test_right_shift_fractional);
    RUN_TEST(test_right_shift_negative_is_arithmetic);
    RUN_TEST(test_shift_compound_assign_right);
    RUN_TEST(test_shift_compound_assign_left);
    RUN_TEST(test_shift_right_then_left_roundtrip);
    RUN_TEST(test_shift_matches_scalar_div);
    RUN_TEST(test_shift_matches_scalar_mul);
    UNITY_END();
}
extern "C" void loop() {}
#else
int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_raw_positive);
    RUN_TEST(test_raw_negative);
    RUN_TEST(test_raw_zero);
    RUN_TEST(test_raw_tag_roundtrip);
    RUN_TEST(test_integer_exact_integer);
    RUN_TEST(test_integer_with_fraction);
    RUN_TEST(test_integer_pure_fraction_is_zero);
    RUN_TEST(test_integer_zero);
    RUN_TEST(test_integer_power_of_two);
    RUN_TEST(test_integer_neg_exact);
    RUN_TEST(test_integer_neg_with_fraction_floors);
    RUN_TEST(test_integer_neg_two);
    RUN_TEST(test_integer_f16_8_positive);
    RUN_TEST(test_integer_f16_8_negative_floors);
    RUN_TEST(test_fraction_pure_fraction);
    RUN_TEST(test_fraction_of_exact_integer_is_zero);
    RUN_TEST(test_fraction_of_mixed);
    RUN_TEST(test_fraction_zero);
    RUN_TEST(test_fraction_quarter);
    RUN_TEST(test_fraction_eighth);
    RUN_TEST(test_fraction_neg_with_frac_part);
    RUN_TEST(test_fraction_neg_exact_integer_is_zero);
    RUN_TEST(test_fraction_neg_two_is_zero);
    RUN_TEST(test_fraction_f16_8_positive);
    RUN_TEST(test_fraction_f16_8_negative);
    RUN_TEST(test_reconstruction_positive);
    RUN_TEST(test_reconstruction_negative);
    RUN_TEST(test_reconstruction_pure_fractional);
    RUN_TEST(test_as_float_one);
    RUN_TEST(test_as_float_one_and_half);
    RUN_TEST(test_as_float_negative);
    RUN_TEST(test_as_float_zero);
    RUN_TEST(test_as_float_quarter);
    RUN_TEST(test_as_float_raw_roundtrip);
    RUN_TEST(test_as_double_one);
    RUN_TEST(test_as_double_one_and_half);
    RUN_TEST(test_as_double_negative);
    RUN_TEST(test_right_shift_by_1_halves_value);
    RUN_TEST(test_right_shift_by_2_quarters_value);
    RUN_TEST(test_left_shift_by_1_doubles_value);
    RUN_TEST(test_left_shift_by_2_quadruples_value);
    RUN_TEST(test_right_shift_fractional);
    RUN_TEST(test_right_shift_negative_is_arithmetic);
    RUN_TEST(test_shift_compound_assign_right);
    RUN_TEST(test_shift_compound_assign_left);
    RUN_TEST(test_shift_right_then_left_roundtrip);
    RUN_TEST(test_shift_matches_scalar_div);
    RUN_TEST(test_shift_matches_scalar_mul);
    return UNITY_END();
}
#endif
