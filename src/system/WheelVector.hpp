#pragma once
#include "WheelConfig.hpp"

struct WheelVector
{
    float x;     // x方向の速度に対する車輪の速度の寄与
    float y;     // y方向の速度に対する車輪の速度の寄与
    float theta; // 角速度に対する車輪の速度の寄与
};

struct WheelVectorInv
{
    float x;
    float y;
    float theta;
};

// 車輪の位置から車輪のベクトル(vx, vy, omegaそれぞれの係数)を計算する
// 車輪の速度ベクトルが(vx, vy) + omega * (-wheel_y, wheel_x)で、これの車輪の方向(cos(theta), sin(theta))との内積を取り、そのvx, vy, omegaの係数を求める
constexpr WheelVector getWheelVector(const WheelPositions wheel_position)
{
    Position wheel_pos = wheel_position.position;
    Meter wheel_radius = wheel_position.radius;

    float wheel_circumference = 2 * (float)M_PI * wheel_radius.value;
    float x = cos(wheel_pos.theta.value) / wheel_circumference;
    float y = sin(wheel_pos.theta.value) / wheel_circumference;
    float theta = (wheel_pos.x.value * y - wheel_pos.y.value * x);

    return WheelVector{
        .x = x,
        .y = y,
        .theta = theta,
    };
}
