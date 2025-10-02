#pragma once
#include <mbed.hpp>

/**
 * @brief OMRON SS-10GL13 マイクロスイッチドライバクラス
 *
 * R2ロボット用リミットスイッチドライバ
 * - OMRON SS-10GL13 (Simulated Roller Lever)専用最適化
 * - 動作力: 0.49N (50gf)
 * - SPDT接点 (SPST-NOとして使用)
 * - 250VAC 10.1A定格
 * - 機械的耐久性: 30,000,000回
 *
 * @author R2開発チーム
 * @date 2025-08-11
 */
class LimitSwitch
{
public:
    /**
     * @brief コンストラクタ
     * @param pin SS-10GL13のCOM端子を接続するMbedピン
     */
    LimitSwitch(PinName pin) : digitalIn(pin), lastState(false), currentState(false), lastEdgeState(false)
    {
        debounceTimer.start();
    }

    /**
     * @brief 初期化処理
     *
     * SS-10GL13の特性に最適化された設定:
     * - プルアップ抵抗有効 (NO接点→GND接続構成)
     * - デバウンス時間: 物理特性に合わせて30ms
     * - 初期状態の安定化処理
     */
    void init()
    {
        // SS-10GL13専用設定
        // COM → Mbedピン, NC → GND の配線構成
        // 非押下時: NC-COM間導通でLOW (0V)
        // 押下時: NC-COM間切断、プルアップでHIGH (3.3V)
        digitalIn.mode(PullUp);

        // SS-10GL13の安定化待機
        // 機械的応答と電気的安定性を確保
        ThisThread::sleep_for(STABILIZE_TIME);

        // 初期状態を安定的に取得
        bool initialRawState = digitalIn.read();
        currentState = initialRawState;
        lastState = initialRawState;
        lastEdgeState = initialRawState;

        // デバウンスタイマーリセット
        debounceTimer.reset();
    }

    /**
     * @brief スイッチ押下状態取得
     *
     * SS-10GL13のSimulated Roller Leverが壁に接触している状態を検知
     *
     * @return true: レバーが押されている(壁接触), false: 非接触
     */
    bool isPressed()
    {
        // 生入力値取得
        bool rawState = digitalIn.read();

        // SS-10GL13専用デバウンス処理
        return debounce(rawState);
    }

    /**
     * @brief エッジ検出機能
     *
     * 押下された瞬間を検知 (立ち上がりエッジ)
     * 連続で押されている間は最初の1回のみtrueを返す
     *
     * @return true: 押下された瞬間, false: その他
     */
    bool isPressedEdge()
    {
        bool currentPressed = isPressed();

        // 立ち上がりエッジ検出 (false → true)
        bool edge = currentPressed && !lastEdgeState;

        // エッジ状態更新
        lastEdgeState = currentPressed;

        return edge;
    }

    /**
     * @brief 生の入力値取得 (デバッグ用)
     * @return デバウンス処理前の生の入力値
     */
    bool getRawInput()
    {
        // デバッグ用: 生の入力値
        return digitalIn.read();
    }

private:
    DigitalIn digitalIn; // デジタル入力ピン
    bool lastState;      // 前回のデバウンス後状態
    bool currentState;   // 現在のデバウンス後状態
    bool lastEdgeState;  // エッジ検出用前回状態
    Timer debounceTimer; // デバウンス用タイマー

    // SS-10GL13に最適化された定数
    static constexpr chrono::milliseconds DEBOUNCE_TIME = 30ms;   // デバウンス時間
    static constexpr chrono::milliseconds STABILIZE_TIME = 100ms; // 初期化時安定化時間

    /**
     * @brief SS-10GL13専用デバウンス処理
     *
     * SS-10GL13の物理特性 (動作力0.49N, 機械的応答時間) に
     * 最適化されたデバウンスアルゴリズム
     *
     * @param rawState 生の入力状態
     * @return デバウンス処理後の安定した状態
     */
    bool debounce(bool rawState)
    {
        // SS-10GL13の物理特性に最適化されたデバウンスアルゴリズム

        // 状態変化検出
        if (rawState != lastState)
        {
            // 新しい状態変化を検出 → タイマーリセット
            debounceTimer.reset();
            lastState = rawState;
        }

        // SS-10GL13に最適化された30msデバウンス時間
        // この時間はSS-10GL13のSimulated Roller Leverの
        // 機械的応答時間と接点バウンス特性を考慮
        if (debounceTimer.elapsed_time() > DEBOUNCE_TIME)
        {
            // 十分な時間が経過 → 状態を確定
            if (rawState != currentState)
            {
                currentState = rawState;
            }
        }

        return currentState;
    }
};
