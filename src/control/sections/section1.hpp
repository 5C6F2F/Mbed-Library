#pragma once
#include "control/ISection.hpp"
#include "system/PositionController.hpp"
#include "driver/ServoMotor.hpp"

class Section1 : public ISection
{
public:
    Section1(std::unique_ptr<PositionController<5, 3>> &position_controller, Servo &servo, Position target_position)
        : position_controller(position_controller), servo(servo), target_position(target_position) {};

    void action() override
    {
        position_controller->setTargetPosition(target_position);

        Timer servo_timer;
        bool is_servo_up = false;

        servo.setAngles(0_deg);
        servo_timer.start();

        while (!isTargetPositionReached())
        {
            if (servo_timer.elapsed_time() >= 1s)
            {
                // サーボを上下
                if (is_servo_up)
                {
                    servo.setAngles(0_deg);
                }
                else
                {
                    servo.setAngles(45_deg);
                }

                is_servo_up = !is_servo_up;
                servo_timer.reset();
            }

            ThisThread::sleep_for(10ms);
        }
    }

private:
    bool isTargetPositionReached()
    {
        Position current_position = position_controller->getCurrentPosition();
        Position error = target_position - current_position;

        return (fabs(error.x.value) < 0.1 &&
                fabs(error.y.value) < 0.1 &&
                fabs(error.theta.value) < 0.1);
    }

    std::unique_ptr<PositionController<5, 3>> &position_controller;
    Servo &servo;
    Position target_position;
};
