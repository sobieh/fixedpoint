#include "../include/FixedPoint.h"

void setup()
{
    // 32bit x 2 decimal places of precision
    // value range is -21474836.99 to 21474836.99
    F32x2 a(1.0f);
    F32x2 b(0.5f);
    F32x2 c = a + b;

    c += 0.01f;
    float f = c.as_float();

    char buffer[32] = { 0 };
    sprintf(buffer, "%d.%d", c.integer(), c.fraction());

    // 32bit x 3 decimal places of precision
    // value range is -2147483.999 to 2147483.999
    F32x2 big(100000.123f);

    // 8bit x 1 decimal place of precision
    // value range is -12.9 to 12.9
    F8x1 tiny(10.1f);

    // 16bit x 1 decimal place of precision
    // value range is -3276.9 to 3276.9
    F16x1 small(1000.1f);

    // 16bit x 2 decimal place of precision
    // value range is -327.99 to 327.99
    F16x2 small(300.12f);
}

void loop()
{

}