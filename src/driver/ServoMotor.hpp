#pragma once
#include <mbed.hpp>
#include <cmath>
#include "units/units.hpp"

class Servo
{
public:
    PwmOut pwm;
    chrono::microseconds min_pulse_width;
    chrono::microseconds max_pulse_width;

    Servo(PinName pwm_pin, chrono::microseconds min_pulse_width, chrono::microseconds max_pulse_width, chrono::milliseconds pwm_period = 20ms)
        : pwm(pwm_pin), min_pulse_width(min_pulse_width), max_pulse_width(max_pulse_width)
    {
        pwm.period_ms(pwm_period.count());
    }

    // @param pulse_width chrono::microseconds 目標のパルス幅[us]。
    void setPulseWidth(chrono::microseconds pulse_width)
    {
        assert(min_pulse_width <= pulse_width && pulse_width <= max_pulse_width);
        pwm.pulsewidth_us(pulse_width.count());
    }

    // @param angles Radian 目標の角度
    void setAngles(Radian angles)
    {
        assert(-90_deg <= angles && angles <= 90_deg);

        chrono::microseconds pulse_range = max_pulse_width - min_pulse_width;
        float offset_pulse_width_float = Degree(angles + 90_deg).value * (float)pulse_range.count() / 180.f;
        int offset_pulse_width = static_cast<int>(std::round(offset_pulse_width_float));
        chrono::microseconds pulse_width = min_pulse_width + chrono::microseconds(offset_pulse_width);

        setPulseWidth(pulse_width);
    }
};
