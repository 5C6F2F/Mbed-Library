#pragma once
#include <type_traits>
#include "../position/position.hpp"

class MeterPerSecond;
class KilometerPerHour;

// メートル毎秒
class MeterPerSecond
{
public:
    float value;

    MeterPerSecond() = default;
    explicit constexpr MeterPerSecond(float val) : value(val) {}

    constexpr MeterPerSecond operator+() const { return MeterPerSecond(value); }
    constexpr MeterPerSecond operator-() const { return MeterPerSecond(-value); }

    // 他の単位への変換演算子
    constexpr operator KilometerPerHour() const;
};

// キロメートル毎時
class KilometerPerHour
{
public:
    float value;

    KilometerPerHour() = default;
    explicit constexpr KilometerPerHour(float val) : value(val) {}

    constexpr KilometerPerHour operator+() const { return KilometerPerHour(value); }
    constexpr KilometerPerHour operator-() const { return KilometerPerHour(-value); }

    constexpr operator MeterPerSecond() const;
};

// --- MeterPerSecondの実装 ---

constexpr MeterPerSecond::operator KilometerPerHour() const { return KilometerPerHour(value * 3600.0f / 1000.0f); }

constexpr MeterPerSecond operator"" _m_s(long double val) { return MeterPerSecond(static_cast<float>(val)); }
constexpr MeterPerSecond operator"" _m_s(unsigned long long val) { return MeterPerSecond(static_cast<float>(val)); }

// --- KilometerPerHourの実装 ---

constexpr KilometerPerHour::operator MeterPerSecond() const { return MeterPerSecond(value * 1000.0f / 3600.0f); }

constexpr KilometerPerHour operator"" _km_h(long double val) { return KilometerPerHour(static_cast<float>(val)); }
constexpr KilometerPerHour operator"" _km_h(unsigned long long val) { return KilometerPerHour(static_cast<float>(val)); }

template <typename T>
constexpr bool is_velocity_unit = std::is_same<T, MeterPerSecond>::value ||
                                  std::is_same<T, KilometerPerHour>::value;

// 同じ単位を持つ距離同士の演算子を定義
// std::enable_if_tによって、Tが距離単位である場合にテンプレートが有効になる

// 比較

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, bool>
operator==(const T &lhs, const T &rhs) { return lhs.value == rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, bool>
operator!=(const T &lhs, const T &rhs) { return lhs.value != rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, bool>
operator<(const T &lhs, const T &rhs) { return lhs.value < rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, bool>
operator>(const T &lhs, const T &rhs) { return lhs.value > rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, bool>
operator<=(const T &lhs, const T &rhs) { return lhs.value <= rhs.value; }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, bool>
operator>=(const T &lhs, const T &rhs) { return lhs.value >= rhs.value; }

// 加算

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T>
operator+(const T &lhs, const T &rhs) { return T(lhs.value + rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T &>
operator+=(T &lhs, const T &rhs)
{
    lhs.value += rhs.value;
    return lhs;
}

// 減算

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T>
operator-(const T &lhs, const T &rhs) { return T(lhs.value - rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T &>
operator-=(T &lhs, const T &rhs)
{
    lhs.value -= rhs.value;
    return lhs;
}

// 乗算

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T>
operator*(const T &lhs, float rhs) { return T(lhs.value * rhs); }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T>
operator*(float lhs, const T &rhs) { return T(lhs * rhs.value); }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T &>
operator*=(T &lhs, float rhs)
{
    lhs.value *= rhs;
    return lhs;
}

// 除算

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T>
operator/(const T &lhs, float rhs) { return T(lhs.value / rhs); }

template <typename T>
constexpr std::enable_if_t<is_velocity_unit<T>, T &>
operator/=(T &lhs, float rhs)
{
    lhs.value /= rhs;
    return lhs;
}

// 異なる単位を持つ速度同士の演算子を定義
// std::enable_if_tによって、TとUの両方が速度単位である場合にテンプレートが有効になる

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs == static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return lhs != static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, bool>
operator<(const T &lhs, const U &rhs) { return lhs < static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, bool>
operator>(const T &lhs, const U &rhs) { return lhs > static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, bool>
operator<=(const T &lhs, const U &rhs) { return lhs <= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, bool>
operator>=(const T &lhs, const U &rhs) { return lhs >= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_unit<T> && is_velocity_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
