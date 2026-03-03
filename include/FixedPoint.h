#pragma once
#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#  define FP_FORCE_INLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
#  define FP_FORCE_INLINE __forceinline
#else
#  define FP_FORCE_INLINE inline
#endif

#if __cplusplus >= 201703L
#  define FP_IF_CONSTEXPR if constexpr
#else
#  define FP_IF_CONSTEXPR if
#endif

namespace fp_detail
{
    template<typename T> struct wider;
    template<> struct wider<int8_t>   { using type = int16_t;  };
    template<> struct wider<int16_t>  { using type = int32_t;  };
    template<> struct wider<int32_t>  { using type = int64_t;  };
    template<> struct wider<int64_t>  { using type = int64_t;  };
    template<> struct wider<uint8_t>  { using type = uint16_t; };
    template<> struct wider<uint16_t> { using type = uint32_t; };
    template<> struct wider<uint32_t> { using type = uint64_t; };
    template<> struct wider<uint64_t> { using type = uint64_t; };

    template<typename T>
    using wider_t = typename wider<T>::type;

    template<typename T> struct unsigned_of             { using type = T;        };
    template<> struct unsigned_of<int8_t>               { using type = uint8_t;  };
    template<> struct unsigned_of<int16_t>              { using type = uint16_t; };
    template<> struct unsigned_of<int32_t>              { using type = uint32_t; };
    template<> struct unsigned_of<int64_t>              { using type = uint64_t; };
    template<typename T> using unsigned_t = typename unsigned_of<T>::type;

    constexpr bool is_pow2(intmax_t v) noexcept
    {
        return v > 1 && (v & (v - 1)) == 0;
    }

    constexpr int log2i(intmax_t v) noexcept
    {
        return v <= 1 ? 0 : 1 + log2i(v >> 1);
    }

}

namespace fp 
{ 
    struct raw_tag_t {};
    constexpr raw_tag_t raw_tag{};
}

template<typename T, intmax_t DENOM>
class FixedPoint {

    static_assert(fp_detail::is_pow2(DENOM), "Denominator must be a power of 2");

    using Wide = fp_detail::wider_t<T>;
    static constexpr int kShift = fp_detail::log2i(DENOM);

    T m_value;

public:

    using StorageType = T;
    static constexpr intmax_t Denominator = DENOM;

    constexpr FixedPoint() noexcept : m_value(0)
    {}

    explicit constexpr FixedPoint(float f) noexcept
        : m_value(static_cast<T>(f * static_cast<float>(DENOM))) 
    {}

    explicit constexpr FixedPoint(double d) noexcept
        : m_value(static_cast<T>(d * static_cast<double>(DENOM)))
    {}

    constexpr FixedPoint(fp::raw_tag_t, T raw) noexcept 
        : m_value(raw)
    {}

    FixedPoint(const FixedPoint&)            = default;
    FixedPoint(FixedPoint&&)                 = default;
    FixedPoint& operator=(const FixedPoint&) = default;
    FixedPoint& operator=(FixedPoint&&)      = default;

    FP_FORCE_INLINE FixedPoint& operator+=(const FixedPoint& b) noexcept
    {
        m_value += b.m_value;
        return *this;
    }

    FP_FORCE_INLINE FixedPoint& operator-=(const FixedPoint& b) noexcept
    {
        m_value -= b.m_value;
        return *this;
    }

    FP_FORCE_INLINE FixedPoint& operator*=(const FixedPoint& b) noexcept
    {
        m_value = static_cast<T>((Wide(m_value) * Wide(b.m_value)) >> kShift);
        return *this;
    }

    FP_FORCE_INLINE FixedPoint& operator/=(const FixedPoint& b) noexcept
    {
        m_value = static_cast<T>((Wide(m_value) << kShift) / Wide(b.m_value));
        return *this;
    }

    FP_FORCE_INLINE FixedPoint& operator*=(T scalar) noexcept { m_value *= scalar; return *this; }
    FP_FORCE_INLINE FixedPoint& operator/=(T scalar) noexcept { m_value /= scalar; return *this; }
    FP_FORCE_INLINE FixedPoint& operator>>=(int bits) noexcept { m_value >>= bits; return *this; }
    FP_FORCE_INLINE FixedPoint& operator<<=(int bits) noexcept { m_value <<= bits; return *this; }

    [[nodiscard]] FP_FORCE_INLINE constexpr FixedPoint operator-() const noexcept { return FixedPoint(fp::raw_tag, static_cast<T>(-m_value)); }

    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator+(FixedPoint a, const FixedPoint& b) noexcept { return a += b; }
    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator-(FixedPoint a, const FixedPoint& b) noexcept { return a -= b; }
    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator*(FixedPoint a, const FixedPoint& b) noexcept { return a *= b; }
    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator/(FixedPoint a, const FixedPoint& b) noexcept { return a /= b; }

    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator*(FixedPoint a, T s) noexcept { return a *= s; }
    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator*(T s, FixedPoint a) noexcept { return a *= s; }
    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator/(FixedPoint a, T s) noexcept { return a /= s; }

    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator>>(FixedPoint a, int b) noexcept { return a >>= b; }
    [[nodiscard]] FP_FORCE_INLINE friend FixedPoint operator<<(FixedPoint a, int b) noexcept { return a <<= b; }

    [[nodiscard]] FP_FORCE_INLINE constexpr bool operator==(const FixedPoint& b) const noexcept { return m_value == b.m_value; }
    [[nodiscard]] FP_FORCE_INLINE constexpr bool operator!=(const FixedPoint& b) const noexcept { return m_value != b.m_value; }
    [[nodiscard]] FP_FORCE_INLINE constexpr bool operator< (const FixedPoint& b) const noexcept { return m_value <  b.m_value; }
    [[nodiscard]] FP_FORCE_INLINE constexpr bool operator> (const FixedPoint& b) const noexcept { return m_value >  b.m_value; }
    [[nodiscard]] FP_FORCE_INLINE constexpr bool operator<=(const FixedPoint& b) const noexcept { return m_value <= b.m_value; }
    [[nodiscard]] FP_FORCE_INLINE constexpr bool operator>=(const FixedPoint& b) const noexcept { return m_value >= b.m_value; }

    [[nodiscard]] FP_FORCE_INLINE constexpr T integer() const noexcept 
    {
        return static_cast<T>(m_value >> kShift);
    }

    [[nodiscard]] FP_FORCE_INLINE constexpr T fraction() const noexcept
    {
        return static_cast<T>(Wide(m_value) & Wide(DENOM - 1));
    }

    [[nodiscard]] FP_FORCE_INLINE constexpr T raw() const noexcept
    {
        return m_value;
    }

    [[nodiscard]] FP_FORCE_INLINE float as_float() const noexcept
    {
        static constexpr float kInv = 1.0f / static_cast<float>(DENOM);
        return static_cast<float>(m_value) * kInv;
    }

    [[nodiscard]] FP_FORCE_INLINE double as_double() const noexcept
    {
        static constexpr double kInv = 1.0 / static_cast<double>(DENOM);
        return static_cast<double>(m_value) * kInv;
    }

    template<typename FP2>
    [[nodiscard]] FP_FORCE_INLINE constexpr FP2 cast() const noexcept
    {
        using DstStorage = typename FP2::StorageType;
        using UWide = fp_detail::unsigned_t<Wide>;
        constexpr intmax_t D2 = FP2::Denominator;
        constexpr int shift = fp_detail::log2i(D2) - kShift;

        FP_IF_CONSTEXPR (shift >= 0)
            return FP2(fp::raw_tag, static_cast<DstStorage>(static_cast<Wide>(static_cast<UWide>(m_value) << shift)));
        else
            return FP2(fp::raw_tag, static_cast<DstStorage>(Wide(m_value) >> (-shift)));
    }

    [[nodiscard]] FP_FORCE_INLINE constexpr FixedPoint abs() const noexcept
    {
        return m_value < 0 ? FixedPoint(fp::raw_tag, static_cast<T>(-m_value)) : *this;
    }
};

template<typename T, intmax_t D>
[[nodiscard]] FP_FORCE_INLINE constexpr FixedPoint<T,D> abs(const FixedPoint<T,D>& x) noexcept
{
    return x.abs();
}

typedef FixedPoint<int8_t,   8>        F8_2;
typedef FixedPoint<int8_t,   16>       F8_4;
typedef FixedPoint<int16_t,  16>       F16_4;
typedef FixedPoint<int16_t,  256>      F16_8;
typedef FixedPoint<int16_t,  4096>     F16_12;
typedef FixedPoint<int32_t,  16>       F32_4;
typedef FixedPoint<int32_t,  256>      F32_8;
typedef FixedPoint<int32_t,  65536>    F32_16;
typedef FixedPoint<int32_t,  16777216> F32_24;

typedef FixedPoint<uint8_t,   8>        UF8_2;
typedef FixedPoint<uint8_t,   16>       UF8_4;
typedef FixedPoint<uint16_t,  16>       UF16_4;
typedef FixedPoint<uint16_t,  256>      UF16_8;
typedef FixedPoint<uint16_t,  4096>     UF16_12;
typedef FixedPoint<uint32_t,  16>       UF32_4;
typedef FixedPoint<uint32_t,  256>      UF32_8;
typedef FixedPoint<uint32_t,  65536>    UF32_16;
typedef FixedPoint<uint32_t,  16777216> UF32_24;
