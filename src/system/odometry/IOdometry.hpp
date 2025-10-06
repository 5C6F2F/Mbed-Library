#include "units/units.hpp"
#include "system/WheelVector.hpp"
#include <Dense.h>

// Odometryの抽象クラス
// Imu, LimitSwitch, TOFなどで拡張した具象クラスをつくってね。
template <int N>
class IOdometry
{
public:
    virtual Position getCurrentPosition() = 0;
    virtual void setCurrentPosition(Position current_position) = 0;
    virtual void updatePosition() = 0;

protected:
    static array<WheelVectorInv, N> getWheelVectorInv(const array<WheelPositions, N> &wheel_position)
    {
        Eigen::Matrix<float, N, 3> wheel_matrix; // 車輪のベクトルを格納する行列
        for (int i = 0; i < N; i++)
        {
            WheelVector wheel_vector = getWheelVector(wheel_position[i]);
            wheel_matrix(i, 0) = wheel_vector.x;
            wheel_matrix(i, 1) = wheel_vector.y;
            wheel_matrix(i, 2) = wheel_vector.theta;
        }

        Eigen::Matrix<float, 3, N> wheel_matrix_inv; // 逆行列 (N>=4のときMoore-Penroseの疑似逆行列)
        // コンパイル時にif文を処理してEigenのstatic_assertを回避。c++17以降。
        if constexpr (N == 3)
        {
            wheel_matrix_inv = wheel_matrix.inverse();
        }
        else
        {
            wheel_matrix_inv = wheel_matrix.transpose() * (wheel_matrix * wheel_matrix.transpose()).inverse();
        }

        array<WheelVectorInv, N> wheel_vectors_inv; // 車輪のベクトルの逆行列
        for (int i = 0; i < N; i++)
        {
            wheel_vectors_inv[i].x = wheel_matrix_inv(0, i);
            wheel_vectors_inv[i].y = wheel_matrix_inv(1, i);
            wheel_vectors_inv[i].theta = wheel_matrix_inv(2, i);
        }

        return wheel_vectors_inv;
    }
};
