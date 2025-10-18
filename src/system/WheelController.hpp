#pragma once
#include "WheelConfig.hpp"
#include "WheelVector.hpp"
#include "DutyController.hpp"
#include "units/units.hpp"

// N: 駆動輪の数
template <int N>
class WheelController
{
public:
    WheelController(array<MotorWheel, N> &motor_wheels, PIDGain &pid_gain, MeterPerSecond max_speed, float max_duty = 1.0f)
        : pid_controller(pid_gain), max_speed(max_speed), max_duty(max_duty)
    {
        for (int i = 0; i < N; i++)
        {
            MeasuringWheel &measuring_wheel = motor_wheels[i].measuring_wheel;
            DCMotor &dc_motor = motor_wheels[i].dc_motor;
            PIDGain pid_gain = motor_wheels[i].pid_gain;

            wheel_vectors[i] = getWheelVector(measuring_wheel.positions);
            dc_motors[i] = &dc_motor;
            duty_controllers[i] = std::make_unique<DutyController>(measuring_wheel.encoder, pid_gain);
        }
    }

    void updateMotors(Position error)
    {
        array<float, N> duty = getTargetMotorDuty(error);
        for (int i = 0; i < N; i++)
        {
            dc_motors[i]->setDuty(duty[i]);
        }
    }

    // 一定周期で実行
    void updateCurrentRps()
    {
        for (int i = 0; i < N; i++)
        {
            this->duty_controllers[i]->updateCurrentRps();
        }
    }

private:
    PIDController<Position> pid_controller;
    array<WheelVector, N> wheel_vectors;
    array<DCMotor *, N> dc_motors;
    // array<DutyController, N>にした場合、理由は不明だが(DutyControllerのメンバ変数であるMutexがコピーできないため?)、
    // 配列初期化時にデフォルトコンストラクタ、配列代入時にコピーコンストラクタが必要になる。
    // ここでコピーコンストラクタを実装してしまうと他の場所でもDutyControllerをコピーできるようになってしまう。
    // Encoderのポインターが別の場所でコピーされて面倒くさいことになるのも嫌なのでunique_ptrを使って実装を回避した。
    array<std::unique_ptr<DutyController>, N> duty_controllers;
    MeterPerSecond max_speed;
    float max_duty;

    array<float, N> getTargetMotorDuty(Position error)
    {
        Velocity target_body_velocity = getTargetBodyVelocity(error);
        array<MeterPerSecond, N> target_motor_velocity = bodyVelocityToMotorSpeeds(target_body_velocity);

        array<float, N> motor_duty;
        float max_motor_duty = max_duty;

        for (int i = 0; i < N; i++)
        {
            // setTargetSpeedの引数はrpsなので変換する必要があるかも
            duty_controllers[i]->setTargetRps(target_motor_velocity[i].value);
            motor_duty[i] = duty_controllers[i]->calculateDuty();

            // max_dutyを超えるduty比がある場合、その最大値で全てのduty比を割って比率を保つ。
            if (fabs(motor_duty[i]) > max_motor_duty)
            {
                max_motor_duty = fabs(motor_duty[i]);
            }
        }

        for (int i = 0; i < N; i++)
        {
            motor_duty[i] = motor_duty[i] / max_motor_duty; // 速度を減衰
        }

        return motor_duty;
    }

    Velocity getTargetBodyVelocity(Position error)
    {
        Position result = pid_controller.calculate(error);
        // 返ってくるのは操作量 = 速度なので型変換
        return Velocity{
            MeterPerSecond(result.x.value),
            MeterPerSecond(result.y.value),
            RadPerSecond(result.theta.value),
        };
    }

    array<MeterPerSecond, N> bodyVelocityToMotorSpeeds(const Velocity velocity)
    {
        array<MeterPerSecond, N> speeds;
        float dec_ratio = 1.0; // 速度の減衰比

        for (int i = 0; i < N; i++)
        {
            speeds[i] = MeterPerSecond(getWheelSpeedRelative(velocity, wheel_vectors[i])); // 車輪の速度を計算
            if (fabs(speeds[i].value) > max_speed.value)
            {
                dec_ratio = fmin(dec_ratio, max_speed.value / fabs(speeds[i].value)); // 速度が最大速度を超えた場合、減衰比を更新
            }
        }

        for (int i = 0; i < N; i++)
        {
            speeds[i] = dec_ratio * speeds[i]; // 速度を減衰
        }

        return speeds;
    }

    // 車輪の速度を計算する
    inline float getWheelSpeedRelative(const Velocity velocity, const WheelVector wheel_vector)
    {
        return wheel_vector.x * velocity.x.value + wheel_vector.y * velocity.y.value + wheel_vector.theta * velocity.theta.value;
    }
};
