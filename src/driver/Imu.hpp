#pragma once
#include <mbed.hpp>

/**
 * @brief 3軸データを格納する構造体
 */
struct Vector3
{
    float x;
    float y;
    float z;

    Vector3() : x(0.0), y(0.0), z(0.0) {}
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

/**
 * @brief BNO055 IMUセンサードライバクラス
 *
 * 9軸フュージョンモード（NDOFモード）を使用して、姿勢角、角速度、
 * 線形加速度、キャリブレーション状態を提供する。
 * 100Hzでのデータ更新により、リアルタイム性を確保する。
 */
class Imu
{
public:
    /**
     * @brief コンストラクタ
     * @param sda I2CのSDAピン
     * @param scl I2CのSCLピン
     */
    Imu(PinName sda, PinName scl)
        : i2c_(sda, scl), yaw_(0.0), pitch_(0.0), roll_(0.0), angular_velocity_(), linear_acceleration_(), calibration_status_(0), yaw_offset_(0.0)
    {
        // I2C通信速度を400kHzに設定
        i2c_.frequency(400000);
    }

    /**
     * @brief デストラクタ
     */
    ~Imu()
    {
        // Tickerを停止
        data_update_ticker_.detach();
    }

    /**
     * @brief BNO055の初期化とデータ更新処理を開始する
     * @return true: 初期化成功, false: 初期化失敗
     */
    bool init()
    {
        // BNO055との通信確認（チップIDの確認）
        uint8_t chip_id = readByte(BNO055_CHIP_ID_ADDR);
        if (chip_id != BNO055_ID)
        {
            printf("BNO055 not found. Chip ID: 0x%02X (expected: 0x%02X)\n", chip_id, BNO055_ID);
            return false;
        }

        printf("BNO055 detected. Chip ID: 0x%02X\n", chip_id);

        // コンフィグモードに設定
        if (!setOperationMode(OPERATION_MODE_CONFIG))
        {
            printf("Failed to set config mode\n");
            return false;
        }

        // システムリセット
        writeByte(BNO055_SYS_TRIGGER_ADDR, 0x20);
        ThisThread::sleep_for(650ms); // リセット待機

        // 再度チップIDを確認
        chip_id = readByte(BNO055_CHIP_ID_ADDR);
        if (chip_id != BNO055_ID)
        {
            printf("BNO055 communication failed after reset\n");
            return false;
        }

        // パワーモードをNORMALに設定
        if (!writeByte(BNO055_PWR_MODE_ADDR, 0x00))
        {
            printf("Failed to set power mode\n");
            return false;
        }
        ThisThread::sleep_for(10ms);

        // 軸マッピングを設定
        if (!setupAxisMapping())
        {
            printf("Failed to setup axis mapping\n");
            return false;
        }

        // NDOFモードに設定
        if (!setOperationMode(OPERATION_MODE_NDOF))
        {
            printf("Failed to set NDOF mode\n");
            return false;
        }

        // データ更新用スレッドを開始
        data_update_thread_.start(callback(this, &Imu::updateData));
        // TickerはISRとしてデータ更新のフラグを立てるように設定
        data_update_ticker_.attach(callback(this, &Imu::updateDataFlagSet), chrono::microseconds(UPDATE_INTERVAL_US));

        printf("BNO055 initialization completed\n");
        return true;
    }

    /**
     * @brief 機体のヨー角（Z軸周りの回転角度）を取得する
     * @return ヨー角 [deg] (-180.0 ~ 180.0)
     */
    float getYaw()
    {
        data_mutex_.lock();
        float yaw = yaw_ - yaw_offset_;
        data_mutex_.unlock();

        // -180度から180度の範囲に正規化
        while (yaw > 180.0)
            yaw -= 360.0;
        while (yaw <= -180.0)
            yaw += 360.0;

        return yaw;
    }

    /**
     * @brief 機体のピッチ角（Y軸周りの回転角度）を取得する
     * @return ピッチ角 [deg] (-90.0 ~ 90.0)
     */
    float getPitch()
    {
        data_mutex_.lock();
        float pitch = pitch_;
        data_mutex_.unlock();
        return pitch;
    }

    /**
     * @brief 機体のロール角（X軸周りの回転角度）を取得する
     * @return ロール角 [deg] (-180.0 ~ 180.0)
     */
    float getRoll()
    {
        data_mutex_.lock();
        float roll = roll_;
        data_mutex_.unlock();
        return roll;
    }

    /**
     * @brief 機体の角速度（3軸）を取得する
     * @return 角速度 [deg/s] (x:ロール速度, y:ピッチ速度, z:ヨー速度)
     */
    Vector3 getAngularVelocity()
    {
        data_mutex_.lock();
        Vector3 angular_velocity = angular_velocity_;
        data_mutex_.unlock();
        return angular_velocity;
    }

    /**
     * @brief 機体の線形加速度（3軸、重力除去済み）を取得する
     * @return 線形加速度 [m/s^2]
     */
    Vector3 getLinearAcceleration()
    {
        data_mutex_.lock();
        Vector3 linear_acceleration = linear_acceleration_;
        data_mutex_.unlock();
        return linear_acceleration;
    }

    /**
     * @brief センサが完全にキャリブレーションされているかを確認する
     * @return true: キャリブレーション完了, false: キャリブレーション未完了
     */
    bool isCalibrated()
    {
        data_mutex_.lock();
        uint8_t status = calibration_status_;
        data_mutex_.unlock();

        // 各センサーのキャリブレーションレベルを抽出
        uint8_t sys_cal = (status >> 6) & 0x03; // システム
        uint8_t gyr_cal = (status >> 4) & 0x03; // ジャイロ
        uint8_t acc_cal = (status >> 2) & 0x03; // 加速度
        uint8_t mag_cal = status & 0x03;        // 磁気

        // 全てのキャリブレーションレベルが3（最大値）の場合にtrueを返す
        return (sys_cal == 3) && (gyr_cal == 3) && (acc_cal == 3) && (mag_cal == 3);
    }

    /**
     * @brief 現在のヨー角を0度とするオフセットを設定する
     */
    void resetYaw()
    {
        data_mutex_.lock();
        yaw_offset_ = yaw_;
        data_mutex_.unlock();
        printf("Yaw offset reset to: %.2f degrees\n", yaw_offset_);
    }

private:
    // BNO055レジスタアドレス定義
    static constexpr uint8_t BNO055_ADDRESS = 0x28 << 1; // I2Cアドレス（7bitを8bitに変換）
    static constexpr uint8_t BNO055_CHIP_ID_ADDR = 0x00;
    static constexpr uint8_t BNO055_OPR_MODE_ADDR = 0x3D;
    static constexpr uint8_t BNO055_PWR_MODE_ADDR = 0x3E;
    static constexpr uint8_t BNO055_SYS_TRIGGER_ADDR = 0x3F;
    static constexpr uint8_t BNO055_AXIS_MAP_CONFIG_ADDR = 0x41;
    static constexpr uint8_t BNO055_AXIS_MAP_SIGN_ADDR = 0x42;
    // データレジスタアドレス
    static constexpr uint8_t BNO055_EULER_H_LSB_ADDR = 0x1A;
    static constexpr uint8_t BNO055_GYRO_DATA_X_LSB_ADDR = 0x14;
    static constexpr uint8_t BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR = 0x28;
    static constexpr uint8_t BNO055_CALIB_STAT_ADDR = 0x35;
    // 動作モード定義
    static constexpr uint8_t OPERATION_MODE_CONFIG = 0x00;
    static constexpr uint8_t OPERATION_MODE_NDOF = 0x0C;
    // 期待されるチップID
    static constexpr uint8_t BNO055_ID = 0xA0;

    static constexpr int UPDATE_FREQUENCY_HZ = 100; // データ更新周期
    static constexpr int UPDATE_INTERVAL_US = 1000000 / UPDATE_FREQUENCY_HZ;

    I2C i2c_; // I2C通信オブジェクト

    Mutex data_mutex_;                             // 排他制御用ミューテックス
    Ticker data_update_ticker_;                    // データ更新用タイマー
    Thread data_update_thread_;                    // データ更新用スレッド
    EventFlags data_update_flag_;                  // データ更新用フラグ
    static constexpr uint32_t UPDATE_SIGNAL = 0x1; // データ更新用フラグ

    // センサーデータを保持するメンバ変数
    float yaw_;                  // ヨー角 [deg]
    float pitch_;                // ピッチ角 [deg]
    float roll_;                 // ロール角 [deg]
    Vector3 angular_velocity_;    // 角速度 [deg/s]
    Vector3 linear_acceleration_; // 線形加速度 [m/s^2]
    uint8_t calibration_status_;  // キャリブレーション状態

    float yaw_offset_; // ヨー角のオフセット値

    /**
     * @brief レジスタから1バイト読み取り
     * @param reg_addr レジスタアドレス
     * @return 読み取った値
     */
    uint8_t readByte(uint8_t reg_addr)
    {
        char cmd[1] = {reg_addr};
        char data[1];

        if (i2c_.write(BNO055_ADDRESS, cmd, 1, true) != 0)
        {
            return 0;
        }

        if (i2c_.read(BNO055_ADDRESS, data, 1) != 0)
        {
            return 0;
        }

        return data[0];
    }

    /**
     * @brief レジスタに1バイト書き込み
     * @param reg_addr レジスタアドレス
     * @param data 書き込むデータ
     * @return true: 成功, false: 失敗
     */
    bool writeByte(uint8_t reg_addr, uint8_t data)
    {
        char cmd[2] = {reg_addr, data};
        return i2c_.write(BNO055_ADDRESS, cmd, 2) == 0;
    }

    /**
     * @brief レジスタから複数バイト読み取り
     * @param reg_addr 開始レジスタアドレス
     * @param buffer 読み取りデータを格納するバッファ
     * @param length 読み取りバイト数
     * @return true: 成功, false: 失敗
     */
    bool readBytes(uint8_t reg_addr, uint8_t *buffer, int length)
    {
        char cmd[1] = {reg_addr};

        if (i2c_.write(BNO055_ADDRESS, cmd, 1, true) != 0)
        {
            return false;
        }

        return i2c_.read(BNO055_ADDRESS, (char *)buffer, length) == 0;
    }

    /**
     * @brief 16bit符号付き整数に変換
     * @param lsb 下位バイト
     * @param msb 上位バイト
     * @return 16bit符号付き整数
     */
    int16_t combineBytes(uint8_t lsb, uint8_t msb)
    {
        return (int16_t)((msb << 8) | lsb);
    }

    /**
     * @brief データ更新処理（Ticker割り込みから呼び出し）
     */
    void updateDataFlagSet()
    {
        data_update_flag_.set(UPDATE_SIGNAL);
    }

    /**
     * @brief データ更新処理
     */
    void updateData()
    {
        while (true)
        {
            data_update_flag_.wait_any(UPDATE_SIGNAL);

            uint8_t buffer[18]; // オイラー角(6) + 角速度(6) + 線形加速度(6)のデータ

            // オイラー角データを読み取り (6バイト)
            if (readBytes(BNO055_EULER_H_LSB_ADDR, buffer, 6))
            {
                int16_t yaw_raw = combineBytes(buffer[0], buffer[1]);
                int16_t roll_raw = combineBytes(buffer[2], buffer[3]);
                int16_t pitch_raw = combineBytes(buffer[4], buffer[5]);

                data_mutex_.lock();
                yaw_ = yaw_raw / 16.0; // LSB = 1/16度
                roll_ = roll_raw / 16.0;
                pitch_ = pitch_raw / 16.0;
                data_mutex_.unlock();
            }

            // 角速度データを読み取り (6バイト)
            if (readBytes(BNO055_GYRO_DATA_X_LSB_ADDR, buffer, 6))
            {
                int16_t x_raw = combineBytes(buffer[0], buffer[1]);
                int16_t y_raw = combineBytes(buffer[2], buffer[3]);
                int16_t z_raw = combineBytes(buffer[4], buffer[5]);

                data_mutex_.lock();
                angular_velocity_.x = x_raw / 16.0; // LSB = 1/16 deg/s
                angular_velocity_.y = y_raw / 16.0;
                angular_velocity_.z = z_raw / 16.0;
                data_mutex_.unlock();
            }

            // 線形加速度データを読み取り (6バイト)
            if (readBytes(BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR, buffer, 6))
            {
                int16_t x_raw = combineBytes(buffer[0], buffer[1]);
                int16_t y_raw = combineBytes(buffer[2], buffer[3]);
                int16_t z_raw = combineBytes(buffer[4], buffer[5]);

                data_mutex_.lock();
                linear_acceleration_.x = x_raw / 100.0; // LSB = 1m/s² = 100 LSB
                linear_acceleration_.y = y_raw / 100.0;
                linear_acceleration_.z = z_raw / 100.0;
                data_mutex_.unlock();
            }

            // キャリブレーション状態を読み取り
            data_mutex_.lock();
            calibration_status_ = readByte(BNO055_CALIB_STAT_ADDR);
            data_mutex_.unlock();
        }
    }

    /**
     * @brief BNO055の動作モードを設定
     * @param mode 動作モード
     * @return true: 成功, false: 失敗
     */
    bool setOperationMode(uint8_t mode)
    {
        if (!writeByte(BNO055_OPR_MODE_ADDR, mode))
        {
            return false;
        }

        // モード変更待機時間
        if (mode == OPERATION_MODE_CONFIG)
        {
            ThisThread::sleep_for(19ms); // コンフィグモードへの変更は19ms
        }
        else
        {
            ThisThread::sleep_for(7ms); // その他のモードは7ms
        }

        return true;
    }

    /**
     * @brief 軸マッピングを設定（座標系の統一）
     * @return true: 成功, false: 失敗
     */
    bool setupAxisMapping()
    {
        // デフォルトの軸マッピングを使用（必要に応じて変更）
        // X軸:前方, Y軸:左方, Z軸:上方

        // 軸マッピング設定 (デフォルト: X=X, Y=Y, Z=Z)
        if (!writeByte(BNO055_AXIS_MAP_CONFIG_ADDR, 0x24))
        {
            return false;
        }

        // 軸の方向設定 (デフォルト: 全て正方向)
        if (!writeByte(BNO055_AXIS_MAP_SIGN_ADDR, 0x00))
        {
            return false;
        }

        return true;
    }
};
