#pragma once
#include "WheelConfig.hpp"
#include "system/wheelVector.hpp"
#include "driver/Encoder.hpp"
#include <Dense.h>

template <int N>
class WheelOdometry : public IOdometry<N>
{
    static_assert(N > 2, "N must be greater than 2.");

public:
    WheelOdometry(array<MeasuringWheel, N> &measuring_wheels) : position(0_m, 0_m, 0_rad)
    {
        array<WheelPositions, N> wheel_positions;
        for (int i = 0; i < N; i++)
        {
            wheel_positions[i] = measuring_wheels[i].positions;
            encoders[i] = &measuring_wheels[i].encoder;
        }

        wheel_vectors_inv = this->getWheelVectorInv(wheel_positions);
        setCurrentPosition({0_m, 0_m, 0_deg});
        last_encoder_counts.fill(0);
    }

    Position getCurrentPosition() override
    {
        mutex.lock();
        Position position = this->position;
        mutex.unlock();

        return position;
    }

    void setCurrentPosition(Position current_position) override
    {
        mutex.lock();
        position = current_position;
        mutex.unlock();
    }

    void updatePosition() override
    {
        array<int, N> encoder_counts;
        for (int i = 0; i < N; i++)
        {
            encoder_counts[i] = encoders[i]->getCount();
        }

        float delta_x = 0.0;
        float delta_y = 0.0;
        float delta_theta = 0.0;

        for (int i = 0; i < N; i++)
        {
            float encoder_delta = encoders[i]->countToRotations(encoder_counts[i] - last_encoder_counts[i]);
            last_encoder_counts[i] = encoder_counts[i]; // 最後のエンコーダーのカウントを更新

            delta_x += encoder_delta * wheel_vectors_inv[i].x;
            delta_y += encoder_delta * wheel_vectors_inv[i].y;
            delta_theta += encoder_delta * wheel_vectors_inv[i].theta;
        }

        // ロボット座標系からフィールド座標系への変換を含む位置と姿勢の更新
        float theta = position.theta.value;
        float delta_x_abs = delta_x * cos(theta + delta_theta / 2) - delta_y * sin(theta + delta_theta / 2); // x方向の移動量 (フィールド座標系)
        float delta_y_abs = delta_x * sin(theta + delta_theta / 2) + delta_y * cos(theta + delta_theta / 2); // y方向の移動量 (フィールド座標系)

        mutex.lock();
        position.x += Meter(delta_x_abs);
        position.y += Meter(delta_y_abs);
        position.theta += Radian(delta_theta);
        mutex.unlock();
    }

private:
    Mutex mutex;
    array<Encoder *, N> encoders;
    array<int, N> last_encoder_counts;
    array<WheelVectorInv, N> wheel_vectors_inv;
    Position position;
};
