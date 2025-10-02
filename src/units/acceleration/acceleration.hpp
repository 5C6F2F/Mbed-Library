#pragma once

class MeterPerSecondSquared;

// メートル毎秒二乗
class MeterPerSecondSquared
{
public:
    float value;

    MeterPerSecondSquared() = default;
    explicit constexpr MeterPerSecondSquared(float val) : value(val) {}

    constexpr MeterPerSecondSquared operator+() const { return MeterPerSecondSquared(value); }
    constexpr MeterPerSecondSquared operator-() const { return MeterPerSecondSquared(-value); }
};

// --- MeterPerSecondSquaredの実装 ---

constexpr bool operator==(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return lhs.value == rhs.value; }
constexpr bool operator!=(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return lhs.value != rhs.value; }
constexpr bool operator<(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return lhs.value < rhs.value; }
constexpr bool operator>(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return lhs.value > rhs.value; }
constexpr bool operator<=(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return lhs.value <= rhs.value; }
constexpr bool operator>=(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return lhs.value >= rhs.value; }

constexpr MeterPerSecondSquared operator+(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return MeterPerSecondSquared(lhs.value + rhs.value); }
constexpr MeterPerSecondSquared operator-(const MeterPerSecondSquared &lhs, const MeterPerSecondSquared &rhs) { return MeterPerSecondSquared(lhs.value - rhs.value); }
constexpr MeterPerSecondSquared operator*(const MeterPerSecondSquared &lhs, float rhs) { return MeterPerSecondSquared(lhs.value * rhs); }
constexpr MeterPerSecondSquared operator*(float lhs, const MeterPerSecondSquared &rhs) { return MeterPerSecondSquared(lhs * rhs.value); }
constexpr MeterPerSecondSquared operator/(const MeterPerSecondSquared &lhs, float rhs) { return MeterPerSecondSquared(lhs.value / rhs); }
