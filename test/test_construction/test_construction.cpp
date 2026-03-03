#include <unity.h>
#include <FixedPoint.h>
#if defined(USBCON)
#include <Arduino.h>
#endif

void setUp()    {}
void tearDown() {}

// ── Default construction ──────────────────────────────────────────────────────

void test_default_ctor_is_zero_f32_16() {
    F32_16 v;
    TEST_ASSERT_EQUAL_INT32(0, v.raw());
}

void test_default_ctor_is_zero_f16_8() {
    F16_8 v;
    TEST_ASSERT_EQUAL_INT16(0, v.raw());
}

void test_default_ctor_is_zero_f8_4() {
    F8_4 v;
    TEST_ASSERT_EQUAL_INT8(0, v.raw());
}

void test_default_ctor_is_zero_uf32_16() {
    UF32_16 v;
    TEST_ASSERT_EQUAL_UINT32(0u, v.raw());
}

// ── Float constructor ─────────────────────────────────────────────────────────
// All values chosen as exact binary fractions: no float-rounding noise.

void test_float_ctor_positive_one() {
    TEST_ASSERT_EQUAL_INT32(65536, F32_16(1.0f).raw());
}

void test_float_ctor_zero() {
    TEST_ASSERT_EQUAL_INT32(0, F32_16(0.0f).raw());
}

void test_float_ctor_negative_one() {
    TEST_ASSERT_EQUAL_INT32(-65536, F32_16(-1.0f).raw());
}

void test_float_ctor_half() {
    TEST_ASSERT_EQUAL_INT32(32768, F32_16(0.5f).raw());
}

void test_float_ctor_one_and_half() {
    TEST_ASSERT_EQUAL_INT32(98304, F32_16(1.5f).raw());
}

void test_float_ctor_negative_one_and_half() {
    TEST_ASSERT_EQUAL_INT32(-98304, F32_16(-1.5f).raw());
}

void test_float_ctor_quarter() {
    TEST_ASSERT_EQUAL_INT32(16384, F32_16(0.25f).raw());
}

void test_float_ctor_eighth() {
    TEST_ASSERT_EQUAL_INT32(8192, F32_16(0.125f).raw());
}

void test_float_ctor_f32_8() {
    TEST_ASSERT_EQUAL_INT32(384, F32_8(1.5f).raw());   // 1.5 * 256 = 384
}

void test_float_ctor_f16_8() {
    TEST_ASSERT_EQUAL_INT16(384, F16_8(1.5f).raw());
}

void test_float_ctor_f8_4() {
    TEST_ASSERT_EQUAL_INT8(24, F8_4(1.5f).raw());      // 1.5 * 16 = 24
}

void test_float_ctor_unsigned_f32_16() {
    TEST_ASSERT_EQUAL_UINT32(98304u, UF32_16(1.5f).raw());
}

void test_float_ctor_unsigned_zero() {
    TEST_ASSERT_EQUAL_UINT32(0u, UF32_16(0.0f).raw());
}

// ── Double constructor ────────────────────────────────────────────────────────

void test_double_ctor_one() {
    TEST_ASSERT_EQUAL_INT32(65536, F32_16(1.0).raw());
}

void test_double_ctor_negative_one_and_half() {
    TEST_ASSERT_EQUAL_INT32(-98304, F32_16(-1.5).raw());
}

// ── raw_tag constructor ───────────────────────────────────────────────────────

void test_raw_tag_positive() {
    F32_16 v(fp::raw_tag, 98304);
    TEST_ASSERT_EQUAL_INT32(98304, v.raw());
}

void test_raw_tag_negative() {
    F32_16 v(fp::raw_tag, -65536);
    TEST_ASSERT_EQUAL_INT32(-65536, v.raw());
}

void test_raw_tag_zero() {
    F32_16 v(fp::raw_tag, 0);
    TEST_ASSERT_EQUAL_INT32(0, v.raw());
}

void test_raw_tag_f16_8() {
    F16_8 v(fp::raw_tag, 384);
    TEST_ASSERT_EQUAL_INT16(384, v.raw());
}

// ── Copy constructor & copy assignment ───────────────────────────────────────

void test_copy_ctor_preserves_raw() {
    F32_16 src(fp::raw_tag, 229376);   // 3.5
    F32_16 dst(src);
    TEST_ASSERT_EQUAL_INT32(229376, dst.raw());
}

void test_copy_assign_preserves_raw() {
    F32_16 src(fp::raw_tag, -98304);   // -1.5
    F32_16 dst;
    dst = src;
    TEST_ASSERT_EQUAL_INT32(-98304, dst.raw());
}

void test_copy_is_independent() {
    F32_16 a(1.0f);
    F32_16 b(a);
    b += F32_16(1.0f);
    TEST_ASSERT_EQUAL_INT32(65536,  a.raw());   // a unchanged
    TEST_ASSERT_EQUAL_INT32(131072, b.raw());   // b incremented
}

// ── constexpr construction (verified at compile time) ────────────────────────

void test_constexpr_construction() {
    constexpr F32_16 zero;
    constexpr F32_16 one(1.0f);
    constexpr F32_16 neg(fp::raw_tag, -65536);

    static_assert(zero.raw() == 0,      "constexpr zero must be 0");
    static_assert(one.raw()  == 65536,  "constexpr 1.0f must be 65536");
    static_assert(neg.raw()  == -65536, "constexpr raw -65536 must hold");

    TEST_ASSERT_EQUAL_INT32(0,      zero.raw());
    TEST_ASSERT_EQUAL_INT32(65536,  one.raw());
    TEST_ASSERT_EQUAL_INT32(-65536, neg.raw());
}

// ── Denominator and StorageType static members ───────────────────────────────

void test_static_denominator_f32_16() {
    TEST_ASSERT_EQUAL_INT32(65536, (int32_t)F32_16::Denominator);
}

void test_static_denominator_f16_8() {
    TEST_ASSERT_EQUAL_INT32(256, (int32_t)F16_8::Denominator);
}

void test_static_denominator_f8_4() {
    TEST_ASSERT_EQUAL_INT32(16, (int32_t)F8_4::Denominator);
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
    RUN_TEST(test_default_ctor_is_zero_f32_16);
    RUN_TEST(test_default_ctor_is_zero_f16_8);
    RUN_TEST(test_default_ctor_is_zero_f8_4);
    RUN_TEST(test_default_ctor_is_zero_uf32_16);
    RUN_TEST(test_float_ctor_positive_one);
    RUN_TEST(test_float_ctor_zero);
    RUN_TEST(test_float_ctor_negative_one);
    RUN_TEST(test_float_ctor_half);
    RUN_TEST(test_float_ctor_one_and_half);
    RUN_TEST(test_float_ctor_negative_one_and_half);
    RUN_TEST(test_float_ctor_quarter);
    RUN_TEST(test_float_ctor_eighth);
    RUN_TEST(test_float_ctor_f32_8);
    RUN_TEST(test_float_ctor_f16_8);
    RUN_TEST(test_float_ctor_f8_4);
    RUN_TEST(test_float_ctor_unsigned_f32_16);
    RUN_TEST(test_float_ctor_unsigned_zero);
    RUN_TEST(test_double_ctor_one);
    RUN_TEST(test_double_ctor_negative_one_and_half);
    RUN_TEST(test_raw_tag_positive);
    RUN_TEST(test_raw_tag_negative);
    RUN_TEST(test_raw_tag_zero);
    RUN_TEST(test_raw_tag_f16_8);
    RUN_TEST(test_copy_ctor_preserves_raw);
    RUN_TEST(test_copy_assign_preserves_raw);
    RUN_TEST(test_copy_is_independent);
    RUN_TEST(test_constexpr_construction);
    RUN_TEST(test_static_denominator_f32_16);
    RUN_TEST(test_static_denominator_f16_8);
    RUN_TEST(test_static_denominator_f8_4);
    UNITY_END();
}
extern "C" void loop() {}
#else
int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_default_ctor_is_zero_f32_16);
    RUN_TEST(test_default_ctor_is_zero_f16_8);
    RUN_TEST(test_default_ctor_is_zero_f8_4);
    RUN_TEST(test_default_ctor_is_zero_uf32_16);
    RUN_TEST(test_float_ctor_positive_one);
    RUN_TEST(test_float_ctor_zero);
    RUN_TEST(test_float_ctor_negative_one);
    RUN_TEST(test_float_ctor_half);
    RUN_TEST(test_float_ctor_one_and_half);
    RUN_TEST(test_float_ctor_negative_one_and_half);
    RUN_TEST(test_float_ctor_quarter);
    RUN_TEST(test_float_ctor_eighth);
    RUN_TEST(test_float_ctor_f32_8);
    RUN_TEST(test_float_ctor_f16_8);
    RUN_TEST(test_float_ctor_f8_4);
    RUN_TEST(test_float_ctor_unsigned_f32_16);
    RUN_TEST(test_float_ctor_unsigned_zero);
    RUN_TEST(test_double_ctor_one);
    RUN_TEST(test_double_ctor_negative_one_and_half);
    RUN_TEST(test_raw_tag_positive);
    RUN_TEST(test_raw_tag_negative);
    RUN_TEST(test_raw_tag_zero);
    RUN_TEST(test_raw_tag_f16_8);
    RUN_TEST(test_copy_ctor_preserves_raw);
    RUN_TEST(test_copy_assign_preserves_raw);
    RUN_TEST(test_copy_is_independent);
    RUN_TEST(test_constexpr_construction);
    RUN_TEST(test_static_denominator_f32_16);
    RUN_TEST(test_static_denominator_f16_8);
    RUN_TEST(test_static_denominator_f8_4);
    return UNITY_END();
}
#endif
