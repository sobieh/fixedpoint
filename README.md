# FixedPoint

A single-header, zero-dependency fixed-point arithmetic library for embedded C++.  
Denominators must be powers of two, so every multiply and divide compiles to a shift — no hardware FPU or integer division required.

- **Header-only** — drop `include/FixedPoint.h` into your project
- **C++11 minimum** — `if constexpr` used automatically on C++17
- **No heap, no exceptions, no RTTI**
- **`constexpr` throughout** — constants folded at compile time
- **PlatformIO library** — works on native, AVR, STM32, and any Arduino-compatible target

## Quick start

```cpp
#include <FixedPoint.h>

F32_16 a(3.14f);   // int32_t, denominator 65536 (Q15.16)
F32_16 b(2.0f);

F32_16 c = a * b;           // multiply: (raw_a * raw_b) >> 16
F32_16 d = a + b;           // add:  raw_a + raw_b
F32_16 e = a >> 1;          // halve: single SAR

printf("%.5f\n", c.as_float());

// Raw construction — no conversion cost
F32_16 v(fp::raw_tag, 98304);   // 1.5 exactly

// Integer scalar — no fixed-point overhead
F32_16 x = a * 3;

// Constexpr — zero runtime cost
constexpr F32_16 half(0.5f);
static_assert(half.raw() == 32768, "");
```

## Operations

| Expression | Description |
|---|---|
| `FixedPoint(float\|double)` | Convert from float/double |
| `FixedPoint(fp::raw_tag, T)` | Construct directly from raw integer bits |
| `a + b`, `a - b` | Addition / subtraction |
| `a * b`, `a / b` | Multiply (`>> kShift`) / divide (`<< kShift / b`) |
| `a * scalar`, `a / scalar` | Integer scalar multiply/divide (no shift) |
| `-a` | Unary negation |
| `a >> n`, `a << n` | Raw bit shift |
| `==`, `!=`, `<`, `>`, `<=`, `>=` | Comparison |
| `a.integer()` | Integer part (arithmetic right shift) |
| `a.fraction()` | Fractional raw bits (bitmask, always ≥ 0) |
| `a.raw()` | Underlying integer value |
| `a.as_float()` | Convert to `float` (multiply by reciprocal constant) |
| `a.as_double()` | Convert to `double` |
| `a.abs()` / `abs(a)` | Absolute value |
| `a.cast<FP2>()` | Lossless format conversion (shift only) |

All operators are `[[nodiscard]]` and `noexcept`. Arithmetic compound-assign operators return `*this`.  
`integer()` uses arithmetic right shift (floor semantics), so `integer()` and `fraction()` always satisfy:

$$\text{value} = \text{integer()} + \frac{\text{fraction()}}{\text{DENOM}}$$

for all positive and negative values.

## Predefined types

### Signed

| Typedef | Storage | Denominator | Fractional bits | Signed range | Step |
|---|---|---|---|---|---|
| `F8_2` | `int8_t` | 8 | 3 | −16 … 15.875 | 0.125 |
| `F8_4` | `int8_t` | 16 | 4 | −8 … 7.9375 | 0.0625 |
| `F16_4` | `int16_t` | 16 | 4 | −2048 … 2047.9375 | 0.0625 |
| `F16_8` | `int16_t` | 256 | 8 | −128 … 127.996 | ~0.0039 |
| `F16_12` | `int16_t` | 4096 | 12 | −8 … 7.9998 | ~0.000244 |
| `F32_4` | `int32_t` | 16 | 4 | −134 M … 134 M | 0.0625 |
| `F32_8` | `int32_t` | 256 | 8 | −8.4 M … 8.4 M | ~0.0039 |
| `F32_16` | `int32_t` | 65536 | 16 | −32768 … 32767.99998 | ~1.5e-5 |
| `F32_24` | `int32_t` | 16777216 | 24 | −128 … 128 | ~6.0e-8 |

### Unsigned

| Typedef | Storage | Denominator | Fractional bits | Range | Step |
|---|---|---|---|---|---|
| `UF8_2` | `uint8_t` | 8 | 3 | 0 … 31.875 | 0.125 |
| `UF8_4` | `uint8_t` | 16 | 4 | 0 … 15.9375 | 0.0625 |
| `UF16_4` | `uint16_t` | 16 | 4 | 0 … 4095.9375 | 0.0625 |
| `UF16_8` | `uint16_t` | 256 | 8 | 0 … 255.996 | ~0.0039 |
| `UF16_12` | `uint16_t` | 4096 | 12 | 0 … 15.9998 | ~0.000244 |
| `UF32_4` | `uint32_t` | 16 | 4 | 0 … 268 M | 0.0625 |
| `UF32_8` | `uint32_t` | 256 | 8 | 0 … 16.8 M | ~0.0039 |
| `UF32_16` | `uint32_t` | 65536 | 16 | 0 … 65535.99998 | ~1.5e-5 |
| `UF32_24` | `uint32_t` | 16777216 | 24 | 0 … 255.9999999 | ~6.0e-8 |

You can also instantiate `FixedPoint<T, DENOM>` directly with any power-of-2 denominator up to `int64_t`/`uint64_t`:

```cpp
using MyQ = FixedPoint<int32_t, 1024>;   // Q21.10
```

## Cross-format cast

`cast<FP2>()` converts between any two `FixedPoint` types in a single shift — no intermediate float:

```cpp
F32_8  q8(3.14f);
F32_16 q16 = q8.cast<F32_16>();   // << 8
F16_8  n   = q16.cast<F16_8>();   // >> 8, narrowing storage
```

The direction of the shift (left or right) is resolved at compile time.

## Custom type

```cpp
// 10-bit fixed-point PID coefficient: range ±2, resolution ~0.002
using PidCoeff = FixedPoint<int16_t, 512>;

PidCoeff kp(1.25f);
PidCoeff ki(0.0625f);
```

## Building & testing

Tests use [Unity](https://github.com/ThrowTheSwitch/Unity) via PlatformIO.

```bash
# Native (host machine)
pio test -e native

# Arduino Mega 2560
pio test -e mega2560

# STM32F411 BlackPill via ST-Link (CDC USB output)
pio test -e blackpill_f411
```

The STM32 environment enables CDC USB Serial (`-D USBCON`). Each test suite waits for the COM port to enumerate before calling `UNITY_BEGIN()`, so no output is lost on cold boot. Set `test_port` in `platformio.ini` if PlatformIO does not auto-detect the port:

```ini
[env:blackpill_f411]
test_port = COM5        ; Windows
; test_port = /dev/ttyACM0  ; Linux
```

## Installation

**PlatformIO** — add to `platformio.ini`:

```ini
lib_deps =
    https://github.com/sobieh/fixedpoint.git
```

**Manual** — copy `include/FixedPoint.h` anywhere on your include path. No other files required.

## Requirements

| Requirement | Minimum |
|---|---|
| C++ standard | C++11 |
| Compiler | GCC, Clang, MSVC |
| Dependencies | `<stdint.h>` only |

## License

GPL-3.0-or-later — see repository for full text.
