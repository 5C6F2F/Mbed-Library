#pragma once
#include <cmath>
#include <type_traits>

// 相互に参照するため、前方宣言
class Radian;

// 度数法
class Degree
{
public:
    float value;

    Degree() = default;
    // doubleからの暗黙的な型変換を防ぐためexplicitを指定
    explicit constexpr Degree(float val) : value(val) {}

    constexpr Degree operator+() const;
    constexpr Degree operator-() const;

    // Radian型への変換演算子
    constexpr operator Radian() const;
};

// 弧度法
class Radian
{
public:
    float value;

    Radian() = default;
    explicit constexpr Radian(float val) : value(val) {}

    constexpr Radian operator+() const;
    constexpr Radian operator-() const;

    // Degree型への変換演算子
    constexpr operator Degree() const;
};

// --- Degreeの実装 ---

constexpr Degree::operator Radian() const { return Radian(value * M_PI / 180.0); }
constexpr Degree Degree::operator+() const { return Degree(value); }
constexpr Degree Degree::operator-() const { return Degree(-value); }

// リテラルの実装

// ### example
// Degree deg = 3.14_deg;
constexpr Degree operator"" _deg(long double val)
{
    return Degree(static_cast<float>(val));
}

// ### example
// Degree rad = 3_deg;
constexpr Degree operator"" _deg(unsigned long long val)
{
    return Degree(static_cast<float>(val));
}

// --- Radianの実装 ---

constexpr Radian::operator Degree() const { return Degree(value * 180.0 / M_PI); }
constexpr Radian Radian::operator+() const { return Radian(value); }
constexpr Radian Radian::operator-() const { return Radian(-value); }

// ### example
// Radian rad = 3.14_rad;
constexpr Radian operator"" _rad(long double val)
{
    return Radian(static_cast<float>(val));
}

// ### example
// Radian rad = 3_rad;
constexpr Radian operator"" _rad(unsigned long long val)
{
    return Radian(static_cast<float>(val));
}

template <typename T>
constexpr bool is_angle_unit = std::is_same<T, Degree>::value ||
                               std::is_same<T, Radian>::value;

// 同じ単位を持つ距離同士の演算子を定義
// std::enable_if_tによって、Tが角度単位である場合にテンプレートが有効になる

// 比較

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, bool>
operator==(const T &lhs, const T &rhs) { return lhs.value == rhs.value; }
template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, bool>
operator!=(const T &lhs, const T &rhs) { return lhs.value != rhs.value; }
template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, bool>
operator<(const T &lhs, const T &rhs) { return lhs.value < rhs.value; }
template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, bool>
operator>(const T &lhs, const T &rhs) { return lhs.value > rhs.value; }
template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, bool>
operator<=(const T &lhs, const T &rhs) { return lhs.value <= rhs.value; }
template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, bool>
operator>=(const T &lhs, const T &rhs) { return lhs.value >= rhs.value; }

// 加算

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T>
operator+(const T &lhs, const T &rhs) { return T(lhs.value + rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T &>
operator+=(T &lhs, const T &rhs) {
    lhs.value += rhs.value;
    return lhs;
}

// 減算

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T>
operator-(const T &lhs, const T &rhs) { return T(lhs.value - rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T &>
operator-=(T &lhs, const T &rhs) {
    lhs.value -= rhs.value;
    return lhs;
}

// 乗算

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T>
operator*(const T &lhs, float rhs) { return T(lhs.value * rhs); }

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T>
operator*(float lhs, const T &rhs) { return T(lhs * rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T &>
operator*=(T &lhs, float rhs)
{
    lhs.value *= rhs;
    return lhs;
}

// 除算

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T>
operator/(const T &lhs, float rhs) { return T(lhs.value / rhs); }

template <typename T>
constexpr std::enable_if_t<is_angle_unit<T>, T &>
operator/=(T &lhs, float rhs)
{
    lhs.value /= rhs;
    return lhs;
}

// 異なる単位を持つ距離同士の演算子を定義
// std::enable_if_tによって、TとUの両方が角度単位である場合にテンプレートが有効になる

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs == static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return lhs != static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, bool>
operator<(const T &lhs, const U &rhs) { return lhs < static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, bool>
operator>(const T &lhs, const U &rhs) { return lhs > static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, bool>
operator<=(const T &lhs, const U &rhs) { return lhs <= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, bool>
operator>=(const T &lhs, const U &rhs) { return lhs >= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angle_unit<T> && is_angle_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
