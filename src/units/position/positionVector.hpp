#pragma once
#include "angle.hpp"
#include "position.hpp"

template <typename DistUnit, typename AngleVelUnit>
class PositionVectorT
{
    // コンパイル時に型をチェックし、条件を満たさない場合は指定したメッセージを出力
    static_assert(is_distance_unit<DistUnit>, "DistUnit must be a distance unit (e.g., Meter, Centimeter).");
    static_assert(is_angle_unit<AngleVelUnit>, "AngleVelUnit must be an angle unit (e.g., Radian, Degree).");

public:
    DistUnit x;
    DistUnit y;
    AngleVelUnit theta;

    PositionVectorT() = default;
    constexpr PositionVectorT(DistUnit x_val, DistUnit y_val, AngleVelUnit theta_val)
        : x(x_val), y(y_val), theta(theta_val) {}

    // 他の単位の位置ベクトルへの変換
    template <
        typename OtherDistUnit,
        typename OtherAngleUnit,
        typename OtherEnableIf = std::enable_if_t<is_distance_unit<OtherDistUnit> && is_angle_unit<OtherAngleUnit>>>
    operator PositionVectorT<OtherDistUnit, OtherAngleUnit>() const
    {
        return PositionVectorT<OtherDistUnit, OtherAngleUnit>(
            static_cast<OtherDistUnit>(x),
            static_cast<OtherDistUnit>(y),
            static_cast<OtherAngleUnit>(theta));
    }
};

// 型エイリアス

using Position = PositionVectorT<Meter, Radian>;
using Position_m_deg = PositionVectorT<Meter, Degree>;
using Position_cm_rad = PositionVectorT<Centimeter, Radian>;
using Position_cm_deg = PositionVectorT<Centimeter, Degree>;
using Position_mm_rad = PositionVectorT<Millimeter, Radian>;
using Position_mm_deg = PositionVectorT<Millimeter, Degree>;
using Position_km_rad = PositionVectorT<Kilometer, Radian>;
using Position_km_deg = PositionVectorT<Kilometer, Degree>;

template <typename T>
constexpr bool is_position_unit = std::is_same<T, Position>::value ||
                                  std::is_same<T, Position_m_deg>::value ||
                                  std::is_same<T, Position_cm_deg>::value ||
                                  std::is_same<T, Position_cm_rad>::value ||
                                  std::is_same<T, Position_mm_rad>::value ||
                                  std::is_same<T, Position_mm_deg>::value ||
                                  std::is_same<T, Position_km_deg>::value ||
                                  std::is_same<T, Position_km_deg>::value ||
                                  std::is_same<T, Position_km_deg>::value;

// 加算

template <typename T>
constexpr std::enable_if_t<is_position_unit<T>, T>
operator+(const T &lhs, const T &rhs)
{
    return T(lhs.x + rhs.x, lhs.y + rhs.y, lhs.theta + rhs.theta);
}

template <typename T>
constexpr std::enable_if_t<is_position_unit<T>, T &>
operator+=(T &lhs, const T &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.theta += rhs.theta;
    return lhs;
}

// 減算

template <typename T>
constexpr std::enable_if_t<is_position_unit<T>, T>
operator-(const T &lhs, const T &rhs)
{
    return T(lhs.x - rhs.x, lhs.y - rhs.y, lhs.theta - rhs.theta);
}

// 乗算

template <typename T>
constexpr std::enable_if_t<is_position_unit<T>, T>
operator*(const T &lhs, const float &rhs)
{
    return T(lhs.x * rhs, lhs.y * rhs, lhs.theta * rhs);
}

// 異なる単位を持つ速度同士の演算子を定義
// std::enable_if_tによって、TとUの両方が位置ベクトルである場合にテンプレートが有効になる

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs == static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return lhs != static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, bool>
operator<(const T &lhs, const U &rhs) { return lhs < static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, bool>
operator>(const T &lhs, const U &rhs) { return lhs > static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, bool>
operator<=(const T &lhs, const U &rhs) { return lhs <= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, bool>
operator>=(const T &lhs, const U &rhs) { return lhs >= static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_position_unit<T> && is_position_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
