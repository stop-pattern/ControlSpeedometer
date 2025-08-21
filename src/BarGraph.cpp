
#include "BarGraph.h"
#include "settings.h"
#include <Arduino.h>
#include <driver/ledc.h>

/**
 * @brief BarGraphクラスのコンストラクタ
 */
BarGraph::BarGraph() {}

/**
 * @brief バーグラフシステムの初期化
 *
 * GPIOピンをOUTPUTモードに設定し、LEDC（LED制御）の初期化を行う
 */
void BarGraph::begin() {
    pinMode(settings::pin_ledG, OUTPUT);
    pinMode(settings::pin_ledR, OUTPUT);
    pinMode(settings::pin_out, OUTPUT);

    // 緑LED用LEDC設定
    ledcSetup(LEDC_CHANNEL_1, 1000, LEDC_TIMER_12_BIT);
    ledcAttachPin(settings::pin_ledG, LEDC_CHANNEL_1);
    ledcWrite(LEDC_CHANNEL_1, 0);

    // PFM出力用LEDC設定
    ledcSetup(LEDC_CHANNEL_0, 1000, LEDC_TIMER_12_BIT);
    ledcAttachPin(settings::pin_out, LEDC_CHANNEL_0);
    ledcWrite(LEDC_CHANNEL_0, 0);
}

/**
 * @brief メインループ処理
 *
 * 定期的に呼び出され、バーグラフの状態を更新する
 * 周期は100ms以下を推奨（300msを十分に表現できる程度）
 */
void BarGraph::loop() {
    // シリアル入力がある場合はシリアル制御モードに遷移
    if (Serial.available()) {
        this->mode = BarGraphMode::MODE_SERIAL;
        log_i("switch to MODE_SERIAL");
    }

    switch (mode) {
    // シリアル制御モードの処理
    case BarGraphMode::MODE_SERIAL:
        if (Serial.available()) {
            String str = Serial.readStringUntil('\n');
            if (str.length() == 0) {
                log_i("serial input: empty");
                break;
            } else if (str.length() > 3) {
                log_d("serial input: too long");
                str = str.substring(0, 3); // 最大3文字まで
            } else
                this->update(str.toInt());
            log_d("serial input: %s", str.c_str());
            this->lastUpdateTime = millis();
            break;
        }

        if (millis() - this->lastUpdateTime > settings::threshold) {
            // シリアル入力がない場合はデモモードに遷移
            this->mode = BarGraphMode::MODE_DEMO;
            log_i("switch to MODE_DEMO");
            break;
        }

        break;

    // デモモードの処理
    case BarGraphMode::MODE_DEMO: {
        // 0-180km/hの間を往復するように動く
        // 0,180での待機時間を含む周期的な動作
        // -> 0 -> (+speedStep) -> 180 -> (waitTime) ->
        // -> 180 -> (-speedStep) -> 0 -> (waitTime) ->

        int newSpeed = this->getDemoSpeed();

        // 速度が変化した場合のみ更新
        if (this->speed != newSpeed) {
            this->speed = newSpeed;
            this->update(this->speed);
        }

        break;
    }

    // カスタムモードの処理
    case BarGraphMode::MODE_WEB:
        break;

    default:

    // カスタムモードの処理
    case BarGraphMode::MODE_CUSTOM:
        // todo:ユーザー定義の動作を実装する

    // 0km/h固定モードの処理
    case BarGraphMode::MODE_0:
        this->update(0);
        break;
    }
}

/**
 * @brief PFM信号を指定周波数で出力
 *
 * @param freq 出力周波数[Hz]
 */
void BarGraph::pfmWrite(uint32_t freq) {
    ledcSetup(LEDC_CHANNEL_0, freq, LEDC_TIMER_12_BIT);
    ledcAttachPin(settings::pin_out, LEDC_CHANNEL_0); // pin_outに接続を明示
    ledcWrite(LEDC_CHANNEL_0, 2048);                  // 50%
    log_d("set freq: %d", freq);
}

/**
 * @brief 速度値から周波数を計算してPFM出力
 *
 * 速度値を0-180の範囲に制限し、周波数に変換
 * 速度値を線形変換して周波数に変換し、PFM出力を行う
 * 変換式: freq = speed * 6.472 + 5.070
 *
 * @param speed 速度値
 */
void BarGraph::speedWrite(int16_t speed) {
    // 速度値を0-180の範囲に制限
    if (speed < 0) {
        speed = 0;
    } else if (speed > 180) {
        speed = 180;
    }

    pfmWrite(speed * 6.472 + 5.070);
    log_d("set speed: %d", speed);
}

/**
 * @brief バーグラフの値を更新
 *
 * 入力値に基づいてPFM出力とLED表示を更新する
 * 速度が90を超える場合は赤LED、90以下の場合は緑LEDを点灯
 *
 * @param value 更新する値（0-180の範囲を想定）
 */
void BarGraph::update(int value) {
    // valueを速度に変換して出力
    speed = value;
    speedWrite(speed);
}

/**
 * @brief バーグラフの表示モードを変更
 *
 * @param mode 更新するモード
 */
void BarGraph::changeMode(BarGraphMode mode) {
    this->mode = mode;
    this->update(0); // モード変更時は0km/hにする
    log_d("change mode: %d", static_cast<int>(mode));
}

/**
 * @brief デモモードの速度を取得
 *
 * @return デモモードの速度
 */
int BarGraph::getDemoSpeed() {
    // 0→180または180→0にかかる時間を計算
    // ステップ数 = maxSpeed / speedStep、各ステップでupdateInterval待機
    constexpr int waitTime = 6000; // 待機時間（ms）
    constexpr int stepCount = settings::maxSpeed / settings::speedStep;
    constexpr int rampTime = stepCount * settings::updateInterval;
    constexpr int span = rampTime * 2 + waitTime * 2; // 全周期時間（120s）

    unsigned long cycleTime = millis() % span;
    int newSpeed = 0;

    if (cycleTime < rampTime) {
        // 前半: 速度を上げる（0→180）
        float progress =
            static_cast<float>(cycleTime) / static_cast<float>(rampTime);
        newSpeed = static_cast<int>(settings::maxSpeed * progress);
        log_d("ramp up: progress=%.3f, speed=%d", progress, newSpeed);
    } else if (cycleTime < (rampTime + waitTime)) {
        // 最高速度での待機
        newSpeed = settings::maxSpeed;
        log_d("wait at max speed: %d", newSpeed);
    } else if (cycleTime < (rampTime * 2 + waitTime)) {
        // 後半: 速度を下げる（180→0）
        unsigned long downTime = cycleTime - rampTime - waitTime;
        float progress =
            static_cast<float>(downTime) / static_cast<float>(rampTime);
        newSpeed = static_cast<int>(settings::maxSpeed * (1.0f - progress));
        log_d("ramp down: progress=%.3f, speed=%d", progress, newSpeed);
    } else {
        // 最低速度での待機
        newSpeed = 0;
        log_d("wait at min speed: %d", newSpeed);
    }

    log_i("demo speed: %d (cycle: %lu/%d)", newSpeed, cycleTime, span);
    return newSpeed;
}

/**
 * @brief バーグラフの速度を取得
 *
 * @return 現在の速度
 */
int BarGraph::getSpeed() { return this->speed; }
