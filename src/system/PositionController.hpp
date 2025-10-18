#pragma once
#include "odometry/IOdometry.hpp"
#include "WheelController.hpp"
#include "PIDController.hpp"

template <int N, int M>
class PositionController
{
public:
    PositionController(IOdometry<N> &odometry, array<MotorWheel, M> &motor_wheels, PIDGain &pid_gain, MeterPerSecond max_speed, chrono::microseconds odometry_update_interval = 5ms)
        : odometry(odometry), wheel_controller(motor_wheels, pid_gain, max_speed), target_position(0_m, 0_m, 0_rad)
    {
        odometry_ticker.attach(callback(this, &PositionController::updatePositionFlagSet), odometry_update_interval);
        odometry_thread.start(callback(this, &PositionController::updatePosition));
    };

    void setCurrentPosition(Position current_position)
    {
        odometry->setCurrentPosition(current_position);
    }

    void setTargetPosition(Position target_position)
    {
        mutex.lock();
        this->target_position = target_position;
        mutex.unlock();
    }

    Position getCurrentPosition()
    {
        return odometry.getCurrentPosition();
    }

    // 一定周期で実行
    void updateMotors()
    {
        wheel_controller.updateCurrentRps();
        wheel_controller.updateMotors(getError());
    }

private:
    Ticker odometry_ticker;
    Thread odometry_thread;
    EventFlags odometry_flag;
    static constexpr uint32_t ODOMETRY_UPDATE_SIGNAL = 1;
    IOdometry<N> &odometry;
    WheelController<M> wheel_controller;

    Mutex mutex;
    Position target_position;

    Position getError()
    {
        mutex.lock();
        Position target_position = this->target_position;
        mutex.unlock();

        Position current_position = odometry.getCurrentPosition();

        return target_position - current_position;
    }

    void updatePositionFlagSet()
    {
        odometry_flag.set(ODOMETRY_UPDATE_SIGNAL);
    }

    void updatePosition()
    {
        while (true)
        {
            odometry_flag.wait_any(ODOMETRY_UPDATE_SIGNAL);

            odometry.updatePosition();
        }
    }
};
