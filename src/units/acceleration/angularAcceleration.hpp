#pragma once
#include <cmath>
#include <type_traits>

class RadPerSecondSquared;

// 度毎秒毎秒
class DegPerSecondSquared
{
public:
    float value;

    DegPerSecondSquared() = default;
    explicit constexpr DegPerSecondSquared(float val) : value(val) {}

    constexpr DegPerSecondSquared operator+() const { return DegPerSecondSquared(value); }
    constexpr DegPerSecondSquared operator-() const { return DegPerSecondSquared(-value); }

    constexpr operator RadPerSecondSquared() const;
};

// ラジアン毎秒毎秒
class RadPerSecondSquared
{
public:
    float value;

    RadPerSecondSquared() = default;
    explicit constexpr RadPerSecondSquared(float val) : value(val) {}

    constexpr RadPerSecondSquared operator+() const { return RadPerSecondSquared(value); }
    constexpr RadPerSecondSquared operator-() const { return RadPerSecondSquared(-value); }

    constexpr operator DegPerSecondSquared() const;
};

// --- DegPerSecondSquaredの実装 ---

constexpr DegPerSecondSquared::operator RadPerSecondSquared() const { return RadPerSecondSquared(value * M_PI / 180.0f); }

constexpr bool operator==(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return lhs.value == rhs.value; }
constexpr bool operator!=(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return lhs.value != rhs.value; }
constexpr bool operator<(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return lhs.value < rhs.value; }
constexpr bool operator>(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return lhs.value > rhs.value; }
constexpr bool operator<=(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return lhs.value <= rhs.value; }
constexpr bool operator>=(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return lhs.value >= rhs.value; }

constexpr DegPerSecondSquared operator+(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return DegPerSecondSquared(lhs.value + rhs.value); }
constexpr DegPerSecondSquared operator-(const DegPerSecondSquared &lhs, const DegPerSecondSquared &rhs) { return DegPerSecondSquared(lhs.value - rhs.value); }
constexpr DegPerSecondSquared operator*(const DegPerSecondSquared &lhs, float rhs) { return DegPerSecondSquared(lhs.value * rhs); }
constexpr DegPerSecondSquared operator*(float lhs, const DegPerSecondSquared &rhs) { return DegPerSecondSquared(lhs * rhs.value); }
constexpr DegPerSecondSquared operator/(const DegPerSecondSquared &lhs, float rhs) { return DegPerSecondSquared(lhs.value / rhs); }

// --- RadPerSecondSquaredの実装 ---

constexpr RadPerSecondSquared::operator DegPerSecondSquared() const { return DegPerSecondSquared(value * 180.0f / M_PI); }

constexpr bool operator==(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return lhs.value == rhs.value; }
constexpr bool operator!=(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return lhs.value != rhs.value; }
constexpr bool operator<(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return lhs.value < rhs.value; }
constexpr bool operator>(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return lhs.value > rhs.value; }
constexpr bool operator<=(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return lhs.value <= rhs.value; }
constexpr bool operator>=(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return lhs.value >= rhs.value; }

constexpr RadPerSecondSquared operator+(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return RadPerSecondSquared(lhs.value + rhs.value); }
constexpr RadPerSecondSquared operator-(const RadPerSecondSquared &lhs, const RadPerSecondSquared &rhs) { return RadPerSecondSquared(lhs.value - rhs.value); }
constexpr RadPerSecondSquared operator*(const RadPerSecondSquared &lhs, float rhs) { return RadPerSecondSquared(lhs.value * rhs); }
constexpr RadPerSecondSquared operator*(float lhs, const RadPerSecondSquared &rhs) { return RadPerSecondSquared(lhs * rhs.value); }
constexpr RadPerSecondSquared operator/(const RadPerSecondSquared &lhs, float rhs) { return RadPerSecondSquared(lhs.value / rhs); }

// 異なる単位を持つ角加速度同士の演算子

template <typename T>
constexpr bool is_angular_acceleration_unit = std::is_same<T, DegPerSecondSquared>::value ||
                                              std::is_same<T, RadPerSecondSquared>::value;

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs == static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return lhs != static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, bool>
operator<(const T &lhs, const U &rhs) { return lhs < static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, bool>
operator>(const T &lhs, const U &rhs) { return lhs > static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, bool>
operator<=(const T &lhs, const U &rhs) { return lhs <= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, bool>
operator>=(const T &lhs, const U &rhs) { return lhs >= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_angular_acceleration_unit<T> && is_angular_acceleration_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
