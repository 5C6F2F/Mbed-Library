#pragma once
#include <mbed.hpp>
#include <cmath>

class DCMotor
{
public:
    DCMotor(PinName pwm_pin, PinName dir_pin, bool is_clockwise = true, float pwm_freq = 16000 /* Hz */)
        : pwm(pwm_pin), dir(dir_pin), is_clockwise(is_clockwise), last_duty(0.0f)
    {
        pwm.period(1.0 / pwm_freq); // PWM周期を設定
        stop();                     // デューティ比を初期化
    }

    // duty比を設定
    void setDuty(float duty)
    {
        last_duty = duty;

        if (!is_clockwise)
        {
            duty = -duty; // 逆回転の場合はデューティ比を反転
        }

        pwm.write(fabs(duty));                                       // デューティ比を設定
        dir.write(duty > 0 ? FORWARD_DIR_STATE : REVERSE_DIR_STATE); // 回転方向を設定
    }

    // duty比を取得
    float getDuty() const
    {
        return last_duty;
    }

    void stop()
    {
        setDuty(0.0);
    }

private:
    PwmOut pwm;
    DigitalOut dir;
    // 回転方向
    bool is_clockwise;
    float last_duty;

    // モータードライバーの仕様に合わせて定義（例：0が正転、1が逆転）
    static constexpr int FORWARD_DIR_STATE = 0;
    static constexpr int REVERSE_DIR_STATE = 1;
};
