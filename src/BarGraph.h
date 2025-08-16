// 型定義とArduinoライブラリ
#pragma once
#include <Arduino.h>
#include <driver/ledc.h>

/**
 * @brief バーグラフの表示モード
 */
enum class BarGraphMode {
    MODE_0,      ///< 0km/h固定
    MODE_SERIAL, ///< シリアル制御モード
    MODE_DEMO,   ///< デモモード
    MODE_WEB,    ///< Webモード
    MODE_TEST,   ///< テストモード
    MODE_CUSTOM, ///< カスタムモード
    MODE_MAX
};

/**
 * @class BarGraph
 * @brief バーグラフ制御クラス
 *
 * PFM信号出力によるバーグラフ制御とLED状態表示を管理するクラス
 */
class BarGraph {
  public:
    /**
     * @brief コンストラクタ
     */
    BarGraph();

    /**
     * @brief バーグラフシステムの初期化
     *
     * GPIOピンの設定とLEDC設定を行う
     */
    void begin();

    /**
     * @brief メインループ処理
     *
     * 定期的に呼び出され、バーグラフの状態を更新する
     */
    void loop();

    /**
     * @brief バーグラフの値を更新
     *
     * @param value 更新する値（0-180の範囲を想定）
     */
    void update(int value);

    /**
     * @brief バーグラフの表示モードを変更
     *
     * @param mode 更新するモード
     */
    void changeMode(BarGraphMode mode);

    /**
     * @brief バーグラフの速度を取得
     *
     * @return 現在の速度
     */
    int getSpeed();

  private:
    /**
     * @brief PFM信号を出力
     *
     * @param freq 出力周波数[Hz]
     */
    void pfmWrite(uint32_t freq);

    /**
     * @brief 速度値から周波数を計算してPFM出力
     *
     * @param speed 速度値
     */
    void speedWrite(int16_t speed);

    /**
     * @brief デモモードの速度を取得
     *
     * @return デモモードの速度
     */
    int getDemoSpeed();

    /**
     * @brief バーグラフの表示モード
     */
    BarGraphMode mode = BarGraphMode::MODE_0;

    /**
     * @brief 速度
     */
    int speed = 0;

    /**
     * @brief 最後の更新時刻
     */
    ulong lastUpdateTime = 0;
};
