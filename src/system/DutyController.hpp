#pragma once
#include "PIDController.hpp"
#include "driver/Encoder.hpp"

class DutyController
{
public:
    DutyController(Encoder &encoder, PIDGain pid_gain)
        : encoder(encoder), pid_controller(pid_gain), target_rps(0.0f), current_rps(0.0f), last_rotation(0.0f), last_duty(0.0f) {}

    // 各車輪の速度比率を乱す可能性があるため、デューティ比の上限・下限の制限は上位クラスで行う。
    float calculateDuty()
    {
        float current_rps = getCurrentRps();

        float output = pid_controller.calculate(target_rps - current_rps);
        float duty = last_duty + output / pid_controller.getFrequency();

        // 最大速度に達した際の積分値リセットは後で考える

        last_duty = duty;

        return duty;
    }

    void setTargetRps(float target_rps)
    {
        this->target_rps = target_rps;
    }

    float getCurrentRps()
    {
        mutex.lock();
        int current_rps = this->current_rps;
        mutex.unlock();

        return current_rps;
    }

    void updateCurrentRps()
    {
        float current_rotation = encoder.getRotations();
        float new_current_rps = (current_rotation - last_rotation) * pid_controller.getFrequency();

        mutex.lock();
        last_rotation = current_rotation;
        current_rps = new_current_rps;
        mutex.unlock();
    }

private:
    Mutex mutex;
    Encoder &encoder;
    PIDController<float> pid_controller;
    float target_rps;
    float current_rps;
    float last_rotation;
    float last_duty;
};
