#pragma once
#include "odometry/IOdometry.hpp"
#include "WheelController.hpp"
#include "PIDController.hpp"

template <int N, int M>
class PositionController
{
public:
    PositionController(std::unique_ptr<IOdometry<N>> odometry, array<MotorWheel, M> &motor_wheels, PIDGain pid_gain, MeterPerSecond max_speed, chrono::microseconds odometry_update_interval = 5ms)
        : odometry(std::move(odometry)), wheel_controller(motor_wheels, pid_gain, max_speed), target_position(0_m, 0_m, 0_rad)
    {
        odometry_ticker.attach(callback(this, &PositionController::updatePositionFlagSet), odometry_update_interval);
        odometry_thread.start(callback(this, &PositionController::updatePosition));
        wheel_controller_ticker.attach(callback(this, &PositionController::updateMotorsFlagSet), 1s / pid_gain.frequency);
        wheel_controller_thread.start(callback(this, &PositionController::updateMotors));
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
        return odometry->getCurrentPosition();
    }

private:
    Ticker odometry_ticker;
    Thread odometry_thread;
    EventFlags odometry_flag;
    static constexpr uint32_t ODOMETRY_UPDATE_SIGNAL = 1;
    std::unique_ptr<IOdometry<N>> odometry;

    Ticker wheel_controller_ticker;
    Thread wheel_controller_thread;
    EventFlags wheel_controller_flag;
    static constexpr uint32_t WHEEL_CONTROLLER_UPDATE_SIGNAL = 1;
    WheelController<M> wheel_controller;

    Mutex mutex;
    Position target_position;

    Position getError()
    {
        mutex.lock();
        Position target_position = this->target_position;
        mutex.unlock();
        Position current_position = odometry->getCurrentPosition();

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

            odometry->updatePosition();
        }
    }

    void updateMotorsFlagSet()
    {
        wheel_controller_flag.set(WHEEL_CONTROLLER_UPDATE_SIGNAL);
    }

    void updateMotors()
    {
        while (true)
        {
            wheel_controller_flag.wait_any(WHEEL_CONTROLLER_UPDATE_SIGNAL);

            wheel_controller.updateMotors(getError());
        }
    }
};
