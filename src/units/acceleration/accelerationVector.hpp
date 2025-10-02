#pragma once
#include "acceleration.hpp"
#include "angularAcceleration.hpp"
#include "../position/position.hpp"

template <
    typename AccUnit,
    typename AngleAccUnit,
    typename EnableIf = std::enable_if_t<std::is_same<AccUnit, MeterPerSecondSquared>::value && is_angular_acceleration_unit<AngleAccUnit>>>
class AccelerationVectorT
{
public:
    AccUnit x;
    AccUnit y;
    AngleAccUnit theta;

    AccelerationVectorT() = default;
    constexpr AccelerationVectorT(AccUnit ax_val, AccUnit ay_val, AngleAccUnit alpha_val)
        : x(ax_val), y(ay_val), theta(alpha_val) {}

    // 他の単位の加速度ベクトルへの変換
    template <
        typename OtherAccUnit,
        typename OtherAngleAccUnit,
        typename OtherEnableIf = std::enable_if_t<std::is_same<OtherAccUnit, MeterPerSecondSquared>::value && is_angular_acceleration_unit<OtherAngleAccUnit>>>
    constexpr operator AccelerationVectorT<OtherAccUnit, OtherAngleAccUnit>() const
    {
        return AccelerationVectorT<OtherAccUnit, OtherAngleAccUnit>(
            static_cast<OtherAccUnit>(x),
            static_cast<OtherAccUnit>(y),
            static_cast<OtherAngleAccUnit>(theta));
    }
};

template <typename AccUnit, typename AngleAccUnit>
constexpr AccelerationVectorT<AccUnit, AngleAccUnit> operator+(const AccelerationVectorT<AccUnit, AngleAccUnit> &lhs, const AccelerationVectorT<AccUnit, AngleAccUnit> &rhs)
{
    return AccelerationVectorT<AccUnit, AngleAccUnit>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.theta + rhs.theta);
}

template <typename AccUnit, typename AngleAccUnit>
constexpr AccelerationVectorT<AccUnit, AngleAccUnit> operator-(const AccelerationVectorT<AccUnit, AngleAccUnit> &lhs, const AccelerationVectorT<AccUnit, AngleAccUnit> &rhs)
{
    return AccelerationVectorT<AccUnit, AngleAccUnit>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.theta - rhs.theta);
}

// 型エイリアス
using AccelerationVector = AccelerationVectorT<MeterPerSecondSquared, RadPerSecondSquared>;
using AccelerationVector_m_s2_deg_s2 = AccelerationVectorT<MeterPerSecondSquared, DegPerSecondSquared>;


template <typename T>
constexpr bool is_acceleration_vector_unit = std::is_same<T, AccelerationVector>::value ||
                                             std::is_same<T, AccelerationVector_m_s2_deg_s2>::value;

template <typename T, typename U>
constexpr std::enable_if_t<is_acceleration_vector_unit<T> && is_acceleration_vector_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs.x == static_cast<T>(rhs).x && lhs.y == static_cast<T>(rhs).y && lhs.theta == static_cast<T>(rhs).theta; }

template <typename T, typename U>
constexpr std::enable_if_t<is_acceleration_vector_unit<T> && is_acceleration_vector_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return !(lhs == rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_acceleration_vector_unit<T> && is_acceleration_vector_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_acceleration_vector_unit<T> && is_acceleration_vector_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
