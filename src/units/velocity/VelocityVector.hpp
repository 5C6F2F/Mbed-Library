#pragma once
#include "velocity.hpp"
#include "angularVelocity.hpp"
#include "../position/position.hpp"

template <
    typename VelUnit,
    typename AngleVelUnit,
    typename EnableIf = std::enable_if_t<is_velocity_unit<VelUnit> && is_angular_velocity_unit<AngleVelUnit>>>
class VelocityVectorT
{
public:
    VelUnit x;
    VelUnit y;
    AngleVelUnit theta;

    VelocityVectorT() = default;
    constexpr VelocityVectorT(VelUnit vx_val, VelUnit vy_val, AngleVelUnit omega_val)
        : x(vx_val), y(vy_val), theta(omega_val) {}

    // 他の単位の速度ベクトルへの変換
    template <
        typename OtherVelUnit,
        typename OtherAngleUnit,
        typename OtherEnableIf = std::enable_if_t<is_velocity_unit<OtherVelUnit> && is_angular_velocity_unit<OtherAngleUnit>>>
    constexpr operator VelocityVectorT<OtherVelUnit, OtherAngleUnit>() const
    {
        return VelocityVectorT<OtherVelUnit, OtherAngleUnit>(
            static_cast<OtherVelUnit>(x),
            static_cast<OtherVelUnit>(y),
            static_cast<OtherAngleUnit>(theta));
    }
};

template <typename VelUnit, typename AngleVelUnit>
constexpr VelocityVectorT<VelUnit, AngleVelUnit> operator+(const VelocityVectorT<VelUnit, AngleVelUnit> &lhs, const VelocityVectorT<VelUnit, AngleVelUnit> &rhs)
{
    return VelocityVectorT<VelUnit, AngleVelUnit>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.theta + rhs.theta);
}

template <typename VelUnit, typename AngleVelUnit>
constexpr VelocityVectorT<VelUnit, AngleVelUnit> operator-(const VelocityVectorT<VelUnit, AngleVelUnit> &lhs, const VelocityVectorT<VelUnit, AngleVelUnit> &rhs)
{
    return VelocityVectorT<VelUnit, AngleVelUnit>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.theta - rhs.theta);
}

// 型エイリアス
using Velocity = VelocityVectorT<MeterPerSecond, RadPerSecond>;
using Velocity_m_s_deg_s = VelocityVectorT<MeterPerSecond, DegPerSecond>;
using Velocity_km_h_rad_s = VelocityVectorT<KilometerPerHour, RadPerSecond>;
using Velocity_km_h_deg_s = VelocityVectorT<KilometerPerHour, DegPerSecond>;

template <typename T>
constexpr bool is_velocity_vector_unit = std::is_same<T, Velocity>::value ||
                                         std::is_same<T, Velocity_m_s_deg_s>::value ||
                                         std::is_same<T, Velocity_km_h_rad_s>::value ||
                                         std::is_same<T, Velocity_km_h_deg_s>::value;

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_vector_unit<T> && is_velocity_vector_unit<U>, bool>
operator==(const T &lhs, const U &rhs) { return lhs.x == static_cast<T>(rhs).x && lhs.y == static_cast<T>(rhs).y && lhs.theta == static_cast<T>(rhs).theta; }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_vector_unit<T> && is_velocity_vector_unit<U>, bool>
operator!=(const T &lhs, const U &rhs) { return !(lhs == rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_vector_unit<T> && is_velocity_vector_unit<U>, T>
operator+(const T &lhs, const U &rhs) { return lhs + static_cast<T>(rhs); }

template <typename T, typename U>
constexpr std::enable_if_t<is_velocity_vector_unit<T> && is_velocity_vector_unit<U>, T>
operator-(const T &lhs, const U &rhs) { return lhs - static_cast<T>(rhs); }
