#pragma once
#include <mbed.hpp>

/**
 * @brief SICK WTT12L-B1531 赤外線物体検知センサードライバクラス
 *
 * 赤外線を利用して、センサー前方に物体が存在するかどうかを検知する。
 * 検知結果はデジタル信号として出力される。
 * 物体検知時は出力がON（HIGH）となる。
 */
class TimeOfFlightSensor
{
public:
    /**
     * @brief コンストラクタ
     *
     * @param out センサーの出力ピン
     */
    TimeOfFlightSensor(PinName q1, PinName q2) : _sensorPin1(q1), _sensorPin2(q2) {}

    /**
     * @brief 物体の検知状態を取得
     *
     * @return true 物体が検知された場合
     * @return false 物体が検知されなかった場合
     */
    bool isDetecting_1()
    {
        return _sensorPin1.read();
    }

    /**
     * @brief 物体の検知状態を取得
     *
     * @return true 物体が検知された場合
     * @return false 物体が検知されなかった場合
     */
    bool isDetecting_2()
    {
        return _sensorPin2.read();
    }

private:
    DigitalIn _sensorPin1;
    DigitalIn _sensorPin2;
};
