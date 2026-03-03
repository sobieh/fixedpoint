#include "../include/FixedPoint.h"
#include <stdio.h>

static void demo_arithmetic() {
    printf("\n--- F32_16 arithmetic (int32_t / 65536) ---\n");

    F32_16 a(3.14f);
    F32_16 b(2.0f);

    printf("a         = %.6f\n", a.as_float());
    printf("b         = %.6f\n", b.as_float());
    printf("a + b     = %.6f\n", (a + b).as_float());
    printf("a - b     = %.6f\n", (a - b).as_float());
    printf("a * b     = %.6f\n", (a * b).as_float());
    printf("a / b     = %.6f\n", (a / b).as_float());
    printf("a * 3     = %.6f\n", (a * 3).as_float());
    printf("a / 2     = %.6f\n", (a / 2).as_float());
    printf("-a        = %.6f\n", (-a).as_float());
    printf("a >> 1    = %.6f\n", (a >> 1).as_float());
    printf("integer() = %d\n",   (int)a.integer());
    printf("fraction()= %d\n",   (int)a.fraction());
    printf("raw()     = %d\n",   (int)a.raw());

    F32_16 neg(-3.75f);
    printf("abs(-3.75)= %.6f\n", abs(neg).as_float());

    F32_16 raw(fp::raw_tag, 196608);
    printf("raw 196608= %.6f\n", raw.as_float());
}

static void demo_formats() {
    printf("\n--- Various F formats ---\n");

    F16_8  sensor(23.5f);
    printf("F16_8  23.5  = %.4f\n", sensor.as_float());

    F32_24 coeff(0.123456f);
    printf("F32_24 0.123456 = %.8f\n", coeff.as_float());

    F32_8  wide(100.0f);
    printf("F32_8  100.0 = %.4f\n", wide.as_float());

    F16_8  narrow(1.5f);
    printf("F16_8  1.5   = %.4f\n", narrow.as_float());
}

static void demo_cast() {
    printf("\n--- Cross-format cast ---\n");

    F32_8  F8(3.14f);
    F32_16 F16 = F8.cast<F32_16>();
    printf("F32_8  -> F32_16 : %.6f\n", F16.as_float());

    F32_8  back = F16.cast<F32_8>();
    printf("F32_16 -> F32_8  : %.6f\n", back.as_float());

    F16_8  narrow(1.5f);
    F32_16 wide = narrow.cast<F32_16>();
    printf("F16_8  -> F32_16 : %.6f\n", wide.as_float());
}

static void demo_constexpr() {
    printf("\n--- constexpr constants ---\n");

    constexpr F32_16 pi(3.14159f);
    constexpr F32_16 half(0.5f);
    constexpr F32_16 zero;

    static_assert(zero.raw() == 0, "zero must be zero");
    (void)pi; (void)half;

    printf("constexpr pi   = %.5f\n", pi.as_float());
    printf("constexpr half = %.5f\n", half.as_float());
}

void setup() {
    demo_arithmetic();
    demo_formats();
    demo_cast();
    demo_constexpr();
}

void loop() {}

int main() { setup(); }
