#pragma once
#include <cmath>
#include <type_traits>

class RadPerSecond;

// 度毎秒
class DegPerSecond
{
public:
    float value;

    DegPerSecond() = default;
    explicit constexpr DegPerSecond(float val) : value(val) {}

    constexpr DegPerSecond operator+() const { return DegPerSecond(value); }
    constexpr DegPerSecond operator-() const { return DegPerSecond(-value); }

    // RadPerSecondへの変換
    constexpr operator RadPerSecond() const;
};

// ラジアン毎秒
class RadPerSecond
{
public:
    float value;

    RadPerSecond() = default;
    explicit constexpr RadPerSecond(float val) : value(val) {}

    constexpr RadPerSecond operator+() const { return RadPerSecond(value); }
    constexpr RadPerSecond operator-() const { return RadPerSecond(-value); }

    // DegPerSecondへの変換
    constexpr operator DegPerSecond() const;
};

// --- DegPerSecondの実装 ---

constexpr DegPerSecond::operator RadPerSecond() const { return RadPerSecond(value * M_PI / 180.0f); }

constexpr bool operator==(const DegPerSecond &lhs, const DegPerSecond &rhs) { return lhs.value == rhs.value; }
constexpr bool operator!=(const DegPerSecond &lhs, const DegPerSecond &rhs) { return lhs.value != rhs.value; }
constexpr bool operator<(const DegPerSecond &lhs, const DegPerSecond &rhs) { return lhs.value < rhs.value; }
constexpr bool operator>(const DegPerSecond &lhs, const DegPerSecond &rhs) { return lhs.value > rhs.value; }
constexpr bool operator<=(const DegPerSecond &lhs, const DegPerSecond &rhs) { return lhs.value <= rhs.value; }
constexpr bool operator>=(const DegPerSecond &lhs, const DegPerSecond &rhs) { return lhs.value >= rhs.value; }

constexpr DegPerSecond operator+(const DegPerSecond &lhs, const DegPerSecond &rhs) { return DegPerSecond(lhs.value + rhs.value); }
constexpr DegPerSecond operator-(const DegPerSecond &lhs, const DegPerSecond &rhs) { return DegPerSecond(lhs.value - rhs.value); }
constexpr DegPerSecond operator*(const DegPerSecond &lhs, float rhs) { return DegPerSecond(lhs.value * rhs); }
constexpr DegPerSecond operator*(float lhs, const DegPerSecond &rhs) { return DegPerSecond(lhs * rhs.value); }
constexpr DegPerSecond operator/(const DegPerSecond &lhs, float rhs) { return DegPerSecond(lhs.value / rhs); }

constexpr DegPerSecond operator"" _deg_s(long double val) { return DegPerSecond(static_cast<float>(val)); }
constexpr DegPerSecond operator"" _deg_s(unsigned long long val) { return DegPerSecond(static_cast<float>(val)); }

// --- RadPerSecondの実装 ---

constexpr RadPerSecond::operator DegPerSecond() const { return DegPerSecond(value * 180.0f / M_PI); }

constexpr bool operator==(const RadPerSecond &lhs, const RadPerSecond &rhs) { return lhs.value == rhs.value; }
constexpr bool operator!=(const RadPerSecond &lhs, const RadPerSecond &rhs) { return lhs.value != rhs.value; }
constexpr bool operator<(const RadPerSecond &lhs, const RadPerSecond &rhs) { return lhs.value < rhs.value; }
constexpr bool operator>(const RadPerSecond &lhs, const RadPerSecond &rhs) { return lhs.value > rhs.value; }
constexpr bool operator<=(const RadPerSecond &lhs, const RadPerSecond &rhs) { return lhs.value <= rhs.value; }
constexpr bool operator>=(const RadPerSecond &lhs, const RadPerSecond &rhs) { return lhs.value >= rhs.value; }

constexpr RadPerSecond operator+(const RadPerSecond &lhs, const RadPerSecond &rhs) { return RadPerSecond(lhs.value + rhs.value); }
constexpr RadPerSecond operator-(const RadPerSecond &lhs, const RadPerSecond &rhs) { return RadPerSecond(lhs.value - rhs.value); }
constexpr RadPerSecond operator*(const RadPerSecond &lhs, float rhs) { return RadPerSecond(lhs.value * rhs); }
constexpr RadPerSecond operator*(float lhs, const RadPerSecond &rhs) { return RadPerSecond(lhs * rhs.value); }
constexpr RadPerSecond operator/(const RadPerSecond &lhs, float rhs) { return RadPerSecond(lhs.value / rhs); }

constexpr RadPerSecond operator"" _rad_s(long double val) { return RadPerSecond(static_cast<float>(val)); }
constexpr RadPerSecond operator"" _rad_s(unsigned long long val) { return RadPerSecond(static_cast<float>(val)); }

// 異なる単位を持つ角速度同士の演算子

template <typename T>
constexpr bool is_angular_velocity_unit = std::is_same<T, DegPerSecond>::value ||
                                          std::is_same<T, RadPerSecond>::value;

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs == static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return lhs != static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, bool>
operator<(const T &lhs, const U &rhs) { return lhs < static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, bool>
operator>(const T &lhs, const U &rhs) { return lhs > static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, bool>
operator<=(const T &lhs, const U &rhs) { return lhs <= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, bool>
operator>=(const T &lhs, const U &rhs) { return lhs >= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_velocity_unit<T> && is_angular_velocity_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
