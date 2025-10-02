#pragma once
#include <type_traits>

class Millimeter;
class Centimeter;
class Meter;
class Kilometer;

// ミリメートル
class Millimeter
{
public:
    float value;

    Millimeter() = default;
    explicit constexpr Millimeter(float val) : value(val) {}

    constexpr Millimeter operator+() const { return Millimeter(value); }
    constexpr Millimeter operator-() const { return Millimeter(-value); }

    // 他の単位への変換演算子
    constexpr operator Centimeter() const;
    constexpr operator Meter() const;
    constexpr operator Kilometer() const;
};

// センチメートル
class Centimeter
{
public:
    float value;

    Centimeter() = default;
    explicit constexpr Centimeter(float val) : value(val) {}

    constexpr Centimeter operator+() const { return Centimeter(value); }
    constexpr Centimeter operator-() const { return Centimeter(-value); }

    constexpr operator Millimeter() const;
    constexpr operator Meter() const;
    constexpr operator Kilometer() const;
};

// メートル
class Meter
{
public:
    float value;

    Meter() = default;
    explicit constexpr Meter(float val) : value(val) {}

    constexpr Meter operator+() const { return Meter(value); }
    constexpr Meter operator-() const { return Meter(-value); }

    constexpr operator Millimeter() const;
    constexpr operator Centimeter() const;
    constexpr operator Kilometer() const;
};

// キロメートル
class Kilometer
{
public:
    float value;

    Kilometer() = default;
    explicit constexpr Kilometer(float val) : value(val) {}

    constexpr Kilometer operator+() const { return Kilometer(value); }
    constexpr Kilometer operator-() const { return Kilometer(-value); }

    constexpr operator Millimeter() const;
    constexpr operator Centimeter() const;
    constexpr operator Meter() const;
};

// --- Millimeterの実装 ---

constexpr Millimeter::operator Centimeter() const { return Centimeter(value / 10.0f); }
constexpr Millimeter::operator Meter() const { return Meter(value / 1000.0f); }
constexpr Millimeter::operator Kilometer() const { return Kilometer(value / 1000000.0f); }

constexpr Millimeter operator"" _mm(long double val) { return Millimeter(static_cast<float>(val)); }
constexpr Millimeter operator"" _mm(unsigned long long val) { return Millimeter(static_cast<float>(val)); }

// --- Centimeterの実装 ---

constexpr Centimeter::operator Millimeter() const { return Millimeter(value * 10.0f); }
constexpr Centimeter::operator Meter() const { return Meter(value / 100.0f); }
constexpr Centimeter::operator Kilometer() const { return Kilometer(value / 100000.0f); }

constexpr Centimeter operator"" _cm(long double val) { return Centimeter(static_cast<float>(val)); }
constexpr Centimeter operator"" _cm(unsigned long long val) { return Centimeter(static_cast<float>(val)); }

// --- Meterの実装 ---

constexpr Meter::operator Millimeter() const { return Millimeter(value * 1000.0f); }
constexpr Meter::operator Centimeter() const { return Centimeter(value * 100.0f); }
constexpr Meter::operator Kilometer() const { return Kilometer(value / 1000.0f); }

constexpr Meter operator"" _m(long double val) { return Meter(static_cast<float>(val)); }
constexpr Meter operator"" _m(unsigned long long val) { return Meter(static_cast<float>(val)); }

// --- Kilometerの実装 ---

constexpr Kilometer::operator Millimeter() const { return Millimeter(value * 1000000.0f); }
constexpr Kilometer::operator Centimeter() const { return Centimeter(value * 100000.0f); }
constexpr Kilometer::operator Meter() const { return Meter(value * 1000.0f); }

constexpr Kilometer operator"" _km(long double val) { return Kilometer(static_cast<float>(val)); }
constexpr Kilometer operator"" _km(unsigned long long val) { return Kilometer(static_cast<float>(val)); }

template <typename T>
constexpr bool is_distance_unit = std::is_same<T, Meter>::value ||
                                  std::is_same<T, Millimeter>::value ||
                                  std::is_same<T, Centimeter>::value ||
                                  std::is_same<T, Kilometer>::value;

// 同じ単位を持つ距離同士の演算子を定義
// std::enable_if_tによって、Tが距離単位である場合にテンプレートが有効になる

// 比較

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, bool>
operator==(const T &lhs, const T &rhs) { return lhs.value == rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, bool>
operator!=(const T &lhs, const T &rhs) { return lhs.value != rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, bool>
operator<(const T &lhs, const T &rhs) { return lhs.value < rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, bool>
operator>(const T &lhs, const T &rhs) { return lhs.value > rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, bool>
operator<=(const T &lhs, const T &rhs) { return lhs.value <= rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, bool>
operator>=(const T &lhs, const T &rhs) { return lhs.value >= rhs.value; }

// 加算

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T>
operator+(const T &lhs, const T &rhs) { return T(lhs.value + rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T &>
operator+=(T &lhs, const T &rhs)
{
    lhs.value += rhs.value;
    return lhs;
}

// 減算

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T>
operator-(const T &lhs, const T &rhs) { return T(lhs.value - rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T &>
operator-=(T &lhs, const T &rhs)
{
    lhs.value -= rhs.value;
    return lhs;
}

// 乗算

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T>
operator*(const T &lhs, float rhs) { return T(lhs.value * rhs); }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T>
operator*(float lhs, const T &rhs) { return T(lhs * rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T &>
operator*=(T &lhs, float rhs)
{
    lhs.value *= rhs;
    return lhs;
}

// 除算

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T>
operator/(const T &lhs, float rhs) { return T(lhs.value / rhs); }

template <typename T>
constexpr std::enable_if_t<is_distance_unit<T>, T &>
operator/=(T &lhs, float rhs)
{
    lhs.value /= rhs;
    return lhs;
}

// 異なる単位を持つ距離同士の演算子を定義
// std::enable_if_tによって、TとUの両方が距離単位である場合にテンプレートが有効になる
// 1_m + 1_cm == 1010_mmなど

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs == static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return lhs != static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, bool>
operator<(const T &lhs, const U &rhs) { return lhs < static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, bool>
operator>(const T &lhs, const U &rhs) { return lhs > static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, bool>
operator<=(const T &lhs, const U &rhs) { return lhs <= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, bool>
operator>=(const T &lhs, const U &rhs) { return lhs >= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_distance_unit<T> && is_distance_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
