#pragma once
#include <mbed.hpp>
#include "units/units.hpp"

// 参考: https://keiorogiken.wordpress.com/2017/12/08/ロータリーエンコーダの話/
// A相がinterrupt_in, B相がdigital_in
class Encoder
{
public:
    Encoder(PinName interrupt_in_pin, PinName digital_in_pin, int resolution = 2048, bool is_clockwise = true, bool is_dual = false)
        : digital_in(digital_in_pin), interrupt_in(interrupt_in_pin), is_clockwise(is_clockwise), count(0)
    {
        converted_resolution = resolution * (is_dual ? 2 : 1);

        thread.start(callback(this, &Encoder::thread_handler));

        // clang-format off
        interrupt_in.rise([this]() { event_flags.set(ENCODER_RISE_FLAG); });
        // clang-format on

        if (is_dual)
        {
            // clang-format off
            interrupt_in.fall([this]() { event_flags.set(ENCODER_FALL_FLAG); });
            // clang-format on
        }
    }

    // カウント数を取得
    int getCount()
    {
        mutex.lock();
        int count = this->count;
        mutex.unlock();
        return count;
    }

    // 角度を取得
    Radian getAngles()
    {
        return Radian(getRotations() * 2 * M_PI);
    }

    // 回転数を取得
    float getRotations()
    {
        return (float)getCount() / converted_resolution;
    }

    // カウント数を回転数に変換
    float countToRotations(int count)
    {
        return (float)count / converted_resolution;
    }

    // カウント数を角度に変換
    Radian countToAngles(int count)
    {
        return Radian(countToRotations(count) * 2 * M_PI);
    }

    // 回転数をカウント数に変換
    int rotationsToCount(float rotations)
    {
        return (int)(converted_resolution * rotations);
    }

    void reset()
    {
        mutex.lock();
        count = 0;
        mutex.unlock();
    }

    // カウント数を加算 (シミュレーション用)
    void addCount(int count)
    {
        mutex.lock();
        this->count += count;
        mutex.unlock();
    }

private:
    DigitalIn digital_in;
    InterruptIn interrupt_in;
    // 1回転あたりのカウント数
    int converted_resolution;
    bool is_clockwise;
    Mutex mutex;
    EventFlags event_flags;
    Thread thread;
    int count;

    static const uint32_t ENCODER_RISE_FLAG = (1 << 0);
    static const uint32_t ENCODER_FALL_FLAG = (1 << 1);

    void thread_handler()
    {
        // is_clockwise:  A相の立ち上がり時にB相がON  = 1 -> count++
        // !is_clockwise: A相の立ち上がり時にB相がOFF = 0 -> count--
        const int rise_sgn = is_clockwise ? 1 : 0;
        const int fall_sgn = is_clockwise ? 0 : 1;

        while (true)
        {
            uint32_t flags = event_flags.wait_any(ENCODER_RISE_FLAG | ENCODER_FALL_FLAG);

            if ((flags & ENCODER_RISE_FLAG) != 0)
            {
                updateCount(rise_sgn);
            }
            if ((flags & ENCODER_FALL_FLAG) != 0)
            {
                updateCount(fall_sgn);
            }
        }
    }

    void updateCount(int sgn)
    {
        mutex.lock();
        if (digital_in.read() == sgn)
        {
            count++;
        }
        else
        {
            count--;
        }
        mutex.unlock();
    }
};
