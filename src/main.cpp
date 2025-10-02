#include "pins.hpp"
#include "WheelConfig.hpp"
#include "units/units.hpp"
#include "system/PositionController.hpp"
#include "system/odometry/WheelOdometry.hpp"

// #include "driver/Imu.hpp"
// #include "system/odometry/ImuWheelOdometry.hpp"

// #include "control/SectionController.hpp"
// #include "control/sections/section1.hpp"

int main()
{
    constexpr int frequency = 1;
    constexpr chrono::microseconds wait_time = 1s / frequency;

    Encoder front_encoder(InterruptInPins::OMNI_ENCODER1_A, DigitalInPins::OMNI_ENCODER1_B);
    Encoder rear_left_encoder(InterruptInPins::OMNI_ENCODER2_A, DigitalInPins::OMNI_ENCODER2_B);
    Encoder rear_right_encoder(InterruptInPins::OMNI_ENCODER3_A, DigitalInPins::OMNI_ENCODER3_B);
    Encoder measuring_x_encoder(InterruptInPins::MEASURING_ENCODER1_A, DigitalInPins::MEASURING_ENCODER1_B);
    Encoder measuring_y_encoder(InterruptInPins::MEASURING_ENCODER2_A, DigitalInPins::MEASURING_ENCODER2_B);

    DCMotor front_motor(PwmOutPins::OMNI_MOTOR1_PWM, DigitalOutPins::OMNI_MOTOR1_DIR);
    DCMotor rear_left_motor(PwmOutPins::OMNI_MOTOR2_PWM, DigitalOutPins::OMNI_MOTOR2_DIR);
    DCMotor rear_right_motor(PwmOutPins::OMNI_MOTOR3_PWM, DigitalOutPins::OMNI_MOTOR3_DIR);

    PIDGain motor_pid_gain = PIDGain({0.7f, 0.0f, 0.0f, frequency});

    Wheels wheels = {
        .front = {
            .measuring_wheel = {
                .positions = WheelSettings::front,
                .encoder = front_encoder,
            },
            .dc_motor = front_motor,
            .pid_gain = motor_pid_gain,
        },
        .rear_left = {
            .measuring_wheel = {
                .positions = WheelSettings::rear_left,
                .encoder = rear_left_encoder,
            },
            .dc_motor = rear_left_motor,
            .pid_gain = motor_pid_gain,
        },
        .rear_right = {
            .measuring_wheel = {
                .positions = WheelSettings::rear_right,
                .encoder = rear_right_encoder,
            },
            .dc_motor = rear_right_motor,
            .pid_gain = motor_pid_gain,
        },
        .measuring_x = {
            .positions = WheelSettings::measuring_x,
            .encoder = measuring_x_encoder,
        },
        .measuring_y = {
            .positions = WheelSettings::measuring_y,
            .encoder = measuring_y_encoder,
        },
    };

    PIDGain position_pid_gain = PIDGain({0.1f, 0.0f, 0.0f, frequency});
    constexpr MeterPerSecond max_speed = 10_m_s;

    array<MeasuringWheel, 5> measuring_wheels = {
        wheels.front.measuring_wheel,
        wheels.rear_left.measuring_wheel,
        wheels.rear_right.measuring_wheel,
        wheels.measuring_x,
        wheels.measuring_y,
    };
    array<MotorWheel, 3> motor_wheels = {
        wheels.front,
        wheels.rear_left,
        wheels.rear_right,
    };

    // メモリのスタック領域に入り切らないのでunique_ptrを使ってヒープ領域に配置。
    auto odometry = std::make_unique<WheelOdometry<5>>(measuring_wheels);

    // Imu imu(PinsForSensor::IMU_SDA, PinsForSensor::IMU_SCL);
    // imu.init();
    // auto odometry = std::make_unique<ImuWheelOdometry<5>>(measuring_wheels, imu);

    auto position_controller = std::make_unique<PositionController<5, 3>>(std::move(odometry), motor_wheels, position_pid_gain, max_speed);
    position_controller->setTargetPosition({10_m, 0_m, 0_deg});

    while (true)
    {
        Position pos = position_controller->getCurrentPosition();

        printf("position: %f, %f, %f\n", pos.x.value, pos.y.value, pos.theta.value);
        printf(" encoder: %d, %d, %d\n", front_encoder.getCount(), rear_left_encoder.getCount(), rear_right_encoder.getCount());
        printf("    duty: %f, %f, %f\n", front_motor.getDuty(), rear_left_motor.getDuty(), rear_right_motor.getDuty());
        printf("\n");

        wait_us(wait_time.count());
    }

    // Servo servo(PA_0, 1s, 1s);
    // Section1 section1(position_controller, servo, {10_m, 0_m, 90_deg});
    // array<ISection *, 1> sections = {&section1};
    // SectionController<1> section_controller(sections);

    // section_controller.start();

    // while (section_controller.isRunning())
    // {
    //     wait_us(wait_time.count());
    // }
}
