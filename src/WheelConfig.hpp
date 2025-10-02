#pragma once
#include "units/units.hpp"
#include "system/PIDController.hpp"
#include "driver/Encoder.hpp"
#include "driver/DCMotor.hpp"

struct WheelPositions
{
    Position position;
    // ホイール半径
    Meter radius;
};

struct MeasuringWheel
{
    WheelPositions positions;
    Encoder &encoder;
};

struct MotorWheel
{
    MeasuringWheel measuring_wheel;
    DCMotor &dc_motor;
    PIDGain pid_gain;
};

struct Wheels
{
    MotorWheel front;
    MotorWheel rear_left;
    MotorWheel rear_right;
    MeasuringWheel measuring_x;
    MeasuringWheel measuring_y;
};

namespace WheelSettings
{
    constexpr Meter TREAD_RAD = 210_mm;
    constexpr Meter WHEEL_RAD = 30_mm;

    constexpr WheelPositions front{
        .position = {0_mm, TREAD_RAD, 0_deg},
        .radius = WHEEL_RAD,
    };
    constexpr WheelPositions rear_left{
        .position = {TREAD_RAD * -M_SQRT3 / 2.0f, -TREAD_RAD / 2.0f, Radian(M_PI * 2.0f / 3.0f)},
        .radius = WHEEL_RAD,
    };
    constexpr WheelPositions rear_right{
        .position = {TREAD_RAD * +M_SQRT3 / 2.0f, -TREAD_RAD / 2.0f, Radian(M_PI * 4.0f / 3.0f)},
        .radius = WHEEL_RAD,
    };
    constexpr WheelPositions measuring_x{
        .position = {0_mm, 0_mm, 0_deg},
        .radius = WHEEL_RAD,
    };
    constexpr WheelPositions measuring_y{
        .position = {0_mm, 0_mm, 90_deg},
        .radius = WHEEL_RAD,
    };
}
