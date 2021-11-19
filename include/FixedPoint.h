#pragma once
#include <stdint.h>


template<typename T, typename IMMT, IMMT DENOMINATOR>
class FixedPoint
{
public:

    using StorageType = T;
    using ImmType = IMMT;
    static constexpr ImmType Denominator = DENOMINATOR;

    FixedPoint() : m_value(0)
    {
    }

    FixedPoint(float f)
    {
        m_value = T(f * Denominator);
    }

    explicit FixedPoint(T raw_value, int)
    {
        m_value = raw_value;
    }

    FixedPoint(const FixedPoint& f) = default;
    FixedPoint(FixedPoint&& f) = default;
    FixedPoint& operator=(const FixedPoint& b) = default;
    FixedPoint& operator=(FixedPoint&& b) = default;

    FixedPoint& operator+=(const FixedPoint& b) { m_value += b.m_value; return *this; }
    const FixedPoint operator+(const FixedPoint& b) const { FixedPoint result = *this; result += b; return result; }

    FixedPoint& operator-=(const FixedPoint& b) { m_value -= b.m_value; return *this; }
    const FixedPoint operator-(const FixedPoint& b) const { FixedPoint result = *this; result -= b; return result; }

    FixedPoint& operator*=(const FixedPoint& b)
    {
        m_value = T((IMMT(m_value) * IMMT(b.m_value)) / Denominator);
        return *this;
    }

    FixedPoint& operator/=(const FixedPoint& b) 
    {
        m_value = T((IMMT(m_value) * Denominator) / IMMT(b.m_value));
        return *this; 
    }

    const FixedPoint operator*(const FixedPoint& b) const { FixedPoint result = *this; result *= b; return result; }
    const FixedPoint operator/(const FixedPoint& b) const { FixedPoint result = *this; result /= b; return result; }
 
    bool operator == (const FixedPoint& b) const { return m_value == b.m_value; }
    bool operator != (const FixedPoint& b) const { return m_value != b.m_value; }
    bool operator < (const FixedPoint& b) const { return m_value < b.m_value; }
    bool operator > (const FixedPoint& b) const { return m_value > b.m_value; }
    bool operator <= (const FixedPoint& b) const { return m_value <= b.m_value; }
    bool operator >= (const FixedPoint& b) const { return m_value >= b.m_value; }

    T integer() const
    {
        return T(m_value / Denominator);
    }

    T fraction() const
    {
        const T frac = T(m_value % Denominator);
        return frac < 0 ? frac * -1 : frac;
    }

    float as_float() const
    {
        return float(m_value) / Denominator;
    }

    template <typename T2>
    inline T2 cast()
    {
        return T2(T2::StorageType((IMMT(m_value) * T2::Denominator) / Denominator), 0);
    }

protected:

    T m_value;
};


typedef FixedPoint<int8_t,  int32_t, 10>        F8x1;
typedef FixedPoint<int16_t, int32_t, 10>       F16x1;
typedef FixedPoint<int16_t, int32_t, 100>      F16x2;
typedef FixedPoint<int16_t, int32_t, 1000>     F16x3;
typedef FixedPoint<int32_t, int32_t, 10>       F32x1;
typedef FixedPoint<int32_t, int32_t, 100>      F32x2;
typedef FixedPoint<int32_t, int32_t, 1000>     F32x3;
typedef FixedPoint<int32_t, int32_t, 10000>    F32x4;

typedef FixedPoint<uint8_t,  uint32_t, 10>     UF8x1;
typedef FixedPoint<uint16_t, uint32_t, 10>    UF16x1;
typedef FixedPoint<uint16_t, uint32_t, 100>   UF16x2;
typedef FixedPoint<uint16_t, uint32_t, 1000>  UF16x3;
typedef FixedPoint<uint32_t, uint32_t, 10>    UF32x1;
typedef FixedPoint<uint32_t, uint32_t, 100>   UF32x2;
typedef FixedPoint<uint32_t, uint32_t, 1000>  UF32x3;
typedef FixedPoint<uint32_t, uint32_t, 10000> UF32x4;
