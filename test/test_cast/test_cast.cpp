#include <unity.h>
#include <FixedPoint.h>
#if defined(USBCON)
#include <Arduino.h>
#endif

void setUp()    {}
void tearDown() {}

// ── F32_16 → F32_8 (right shift by 8) ────────────────────────────────────────

void test_cast_f32_16_to_f32_8_positive() {
    // 1.5 in F32_16: raw=98304 → >>8 = 384 → F32_8(1.5)
    TEST_ASSERT_EQUAL_INT32(384, (F32_16(1.5f).cast<F32_8>()).raw());
}

void test_cast_f32_16_to_f32_8_two() {
    // 2.0: raw=131072 → >>8 = 512
    TEST_ASSERT_EQUAL_INT32(512, (F32_16(2.0f).cast<F32_8>()).raw());
}

void test_cast_f32_16_to_f32_8_negative() {
    // -1.5: raw=-98304 → >>8 = -384
    TEST_ASSERT_EQUAL_INT32(-384, (F32_16(-1.5f).cast<F32_8>()).raw());
}

void test_cast_f32_16_to_f32_8_zero() {
    TEST_ASSERT_EQUAL_INT32(0, (F32_16(0.0f).cast<F32_8>()).raw());
}

void test_cast_f32_16_to_f32_8_half() {
    // 0.5: raw=32768 → >>8 = 128
    TEST_ASSERT_EQUAL_INT32(128, (F32_16(0.5f).cast<F32_8>()).raw());
}

// ── F32_8 → F32_16 (left shift by 8) ─────────────────────────────────────────
// UWide = uint64_t, so no sign-bit truncation at intermediate step.

void test_cast_f32_8_to_f32_16_positive() {
    // 1.5 in F32_8: raw=384 → <<8 = 98304 → F32_16(1.5)
    TEST_ASSERT_EQUAL_INT32(98304, (F32_8(1.5f).cast<F32_16>()).raw());
}

void test_cast_f32_8_to_f32_16_negative() {
    // -1.5 in F32_8: raw=-384
    // UWide=uint64_t → (uint64_t)(-384LL) << 8 = bit-pattern -98304 in int64_t
    TEST_ASSERT_EQUAL_INT32(-98304, (F32_8(-1.5f).cast<F32_16>()).raw());
}

void test_cast_f32_8_to_f32_16_quarter() {
    // 0.25: raw=64 → <<8 = 16384
    TEST_ASSERT_EQUAL_INT32(16384, (F32_8(0.25f).cast<F32_16>()).raw());
}

void test_cast_f32_8_to_f32_16_zero() {
    TEST_ASSERT_EQUAL_INT32(0, (F32_8(0.0f).cast<F32_16>()).raw());
}

// ── F32_16 → F32_24 (left shift by 8: 24-16=8) ───────────────────────────────

void test_cast_f32_16_to_f32_24_positive() {
    // 1.5 in F32_16: raw=98304 → <<8 = 25165824
    TEST_ASSERT_EQUAL_INT32(25165824, (F32_16(1.5f).cast<F32_24>()).raw());
}

void test_cast_f32_16_to_f32_24_negative() {
    // -1.5: raw=-98304 → <<8 bit pattern = -25165824
    TEST_ASSERT_EQUAL_INT32(-25165824, (F32_16(-1.5f).cast<F32_24>()).raw());
}

void test_cast_f32_16_to_f32_24_half() {
    // 0.5: raw=32768 → <<8 = 8388608
    TEST_ASSERT_EQUAL_INT32(8388608, (F32_16(0.5f).cast<F32_24>()).raw());
}

// ── F32_24 → F32_16 (right shift by 8) ───────────────────────────────────────

void test_cast_f32_24_to_f32_16_positive() {
    // 1.5 in F32_24: raw=25165824 → >>8 = 98304
    TEST_ASSERT_EQUAL_INT32(98304, (F32_24(1.5f).cast<F32_16>()).raw());
}

void test_cast_f32_24_to_f32_16_negative() {
    TEST_ASSERT_EQUAL_INT32(-98304, (F32_24(-1.5f).cast<F32_16>()).raw());
}

// ── Identity cast: same type ─────────────────────────────────────────────────

void test_cast_identity_f32_16() {
    TEST_ASSERT_EQUAL_INT32(98304, (F32_16(1.5f).cast<F32_16>()).raw());
}

void test_cast_identity_f32_8() {
    TEST_ASSERT_EQUAL_INT32(384, (F32_8(1.5f).cast<F32_8>()).raw());
}

void test_cast_identity_negative() {
    TEST_ASSERT_EQUAL_INT32(-98304, (F32_16(-1.5f).cast<F32_16>()).raw());
}

// ── Unsigned casts ───────────────────────────────────────────────────────────

void test_cast_uf32_8_to_uf32_16() {
    // UF32_8(1.5f): raw=384 → <<8 = 98304
    TEST_ASSERT_EQUAL_UINT32(98304u, (UF32_8(1.5f).cast<UF32_16>()).raw());
}

void test_cast_uf32_16_to_uf32_8() {
    // UF32_16(1.5f): raw=98304 → >>8 = 384
    TEST_ASSERT_EQUAL_UINT32(384u, (UF32_16(1.5f).cast<UF32_8>()).raw());
}

void test_cast_uf32_8_to_uf32_16_quarter() {
    // 0.25: raw=64 → <<8 = 16384
    TEST_ASSERT_EQUAL_UINT32(16384u, (UF32_8(0.25f).cast<UF32_16>()).raw());
}

// ── Round-trip: upcast then downcast preserves value for exact values ─────────

void test_roundtrip_f32_8_via_f32_16() {
    F32_8 original(1.5f);
    F32_8 roundtrip = original.cast<F32_16>().cast<F32_8>();
    TEST_ASSERT_EQUAL_INT32(original.raw(), roundtrip.raw());
}

void test_roundtrip_f32_8_via_f32_16_negative() {
    F32_8 original(-1.5f);
    F32_8 roundtrip = original.cast<F32_16>().cast<F32_8>();
    TEST_ASSERT_EQUAL_INT32(original.raw(), roundtrip.raw());
}

void test_roundtrip_f32_16_via_f32_24() {
    F32_16 original(1.5f);
    F32_16 roundtrip = original.cast<F32_24>().cast<F32_16>();
    TEST_ASSERT_EQUAL_INT32(original.raw(), roundtrip.raw());
}

void test_roundtrip_uf32_8_via_uf32_16() {
    UF32_8 original(1.5f);
    UF32_8 roundtrip = original.cast<UF32_16>().cast<UF32_8>();
    TEST_ASSERT_EQUAL_UINT32(original.raw(), roundtrip.raw());
}

// ── Cast preserves float value (float-level sanity check) ────────────────────

void test_cast_value_preserved_as_float_f32_16_to_f32_8() {
    float original = 1.5f;
    F32_8 result = F32_16(original).cast<F32_8>();
    TEST_ASSERT_EQUAL_FLOAT(original, result.as_float());
}

void test_cast_value_preserved_as_float_f32_8_to_f32_16() {
    float original = 0.25f;
    F32_16 result = F32_8(original).cast<F32_16>();
    TEST_ASSERT_EQUAL_FLOAT(original, result.as_float());
}

void test_cast_value_preserved_as_float_f32_16_to_f32_24() {
    float original = 1.5f;
    F32_24 result = F32_16(original).cast<F32_24>();
    TEST_ASSERT_FLOAT_WITHIN(1.0f / F32_24::Denominator, original, result.as_float());
}

// ── F32_8 ↔ F32_4 ────────────────────────────────────────────────────────────

void test_cast_f32_8_to_f32_4_positive() {
    // F32_8(1.5f): raw=384, shift = log2(16)-log2(256) = 4-8 = -4 → >>4 = 24
    TEST_ASSERT_EQUAL_INT32(24, (F32_8(1.5f).cast<F32_4>()).raw());
}

void test_cast_f32_4_to_f32_8_positive() {
    // F32_4(1.5f): raw=24, shift=4 → <<4 = 384
    TEST_ASSERT_EQUAL_INT32(384, (F32_4(1.5f).cast<F32_8>()).raw());
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
    RUN_TEST(test_cast_f32_16_to_f32_8_positive);
    RUN_TEST(test_cast_f32_16_to_f32_8_two);
    RUN_TEST(test_cast_f32_16_to_f32_8_negative);
    RUN_TEST(test_cast_f32_16_to_f32_8_zero);
    RUN_TEST(test_cast_f32_16_to_f32_8_half);
    RUN_TEST(test_cast_f32_8_to_f32_16_positive);
    RUN_TEST(test_cast_f32_8_to_f32_16_negative);
    RUN_TEST(test_cast_f32_8_to_f32_16_quarter);
    RUN_TEST(test_cast_f32_8_to_f32_16_zero);
    RUN_TEST(test_cast_f32_16_to_f32_24_positive);
    RUN_TEST(test_cast_f32_16_to_f32_24_negative);
    RUN_TEST(test_cast_f32_16_to_f32_24_half);
    RUN_TEST(test_cast_f32_24_to_f32_16_positive);
    RUN_TEST(test_cast_f32_24_to_f32_16_negative);
    RUN_TEST(test_cast_identity_f32_16);
    RUN_TEST(test_cast_identity_f32_8);
    RUN_TEST(test_cast_identity_negative);
    RUN_TEST(test_cast_uf32_8_to_uf32_16);
    RUN_TEST(test_cast_uf32_16_to_uf32_8);
    RUN_TEST(test_cast_uf32_8_to_uf32_16_quarter);
    RUN_TEST(test_roundtrip_f32_8_via_f32_16);
    RUN_TEST(test_roundtrip_f32_8_via_f32_16_negative);
    RUN_TEST(test_roundtrip_f32_16_via_f32_24);
    RUN_TEST(test_roundtrip_uf32_8_via_uf32_16);
    RUN_TEST(test_cast_value_preserved_as_float_f32_16_to_f32_8);
    RUN_TEST(test_cast_value_preserved_as_float_f32_8_to_f32_16);
    RUN_TEST(test_cast_value_preserved_as_float_f32_16_to_f32_24);
    RUN_TEST(test_cast_f32_8_to_f32_4_positive);
    RUN_TEST(test_cast_f32_4_to_f32_8_positive);
    UNITY_END();
}
extern "C" void loop() {}
#else
int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_cast_f32_16_to_f32_8_positive);
    RUN_TEST(test_cast_f32_16_to_f32_8_two);
    RUN_TEST(test_cast_f32_16_to_f32_8_negative);
    RUN_TEST(test_cast_f32_16_to_f32_8_zero);
    RUN_TEST(test_cast_f32_16_to_f32_8_half);
    RUN_TEST(test_cast_f32_8_to_f32_16_positive);
    RUN_TEST(test_cast_f32_8_to_f32_16_negative);
    RUN_TEST(test_cast_f32_8_to_f32_16_quarter);
    RUN_TEST(test_cast_f32_8_to_f32_16_zero);
    RUN_TEST(test_cast_f32_16_to_f32_24_positive);
    RUN_TEST(test_cast_f32_16_to_f32_24_negative);
    RUN_TEST(test_cast_f32_16_to_f32_24_half);
    RUN_TEST(test_cast_f32_24_to_f32_16_positive);
    RUN_TEST(test_cast_f32_24_to_f32_16_negative);
    RUN_TEST(test_cast_identity_f32_16);
    RUN_TEST(test_cast_identity_f32_8);
    RUN_TEST(test_cast_identity_negative);
    RUN_TEST(test_cast_uf32_8_to_uf32_16);
    RUN_TEST(test_cast_uf32_16_to_uf32_8);
    RUN_TEST(test_cast_uf32_8_to_uf32_16_quarter);
    RUN_TEST(test_roundtrip_f32_8_via_f32_16);
    RUN_TEST(test_roundtrip_f32_8_via_f32_16_negative);
    RUN_TEST(test_roundtrip_f32_16_via_f32_24);
    RUN_TEST(test_roundtrip_uf32_8_via_uf32_16);
    RUN_TEST(test_cast_value_preserved_as_float_f32_16_to_f32_8);
    RUN_TEST(test_cast_value_preserved_as_float_f32_8_to_f32_16);
    RUN_TEST(test_cast_value_preserved_as_float_f32_16_to_f32_24);
    RUN_TEST(test_cast_f32_8_to_f32_4_positive);
    RUN_TEST(test_cast_f32_4_to_f32_8_positive);
    return UNITY_END();
}
#endif
