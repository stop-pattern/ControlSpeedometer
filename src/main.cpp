#include <Arduino.h>

#include "BarGraph.h"
#include "Settings.h"
#include "WiFiManager.h"

namespace {
BarGraph barGraph;       ///< バーグラフ制御インスタンス
WiFiManager wifiManager; ///< WiFi管理インスタンス

int speed = 0;
} // namespace

/**
 * @brief システム初期化処理
 *
 * 各機能を初期化する
 * 初期化順序：シリアル通信 -> WiFi
 */
void setup() {
    Serial.begin(115200);
    Serial.println();
    log_d("setup start");

    barGraph.begin();
    wifiManager.begin();

    log_d("setup end");
    if (settings::debug) {
        log_d("Debug mode is enabled");
        barGraph.changeMode(BarGraphMode::MODE_DEMO);
    }
}

/**
 * @brief メインループ処理
 *
 * 処理を定期的に実行する
 * WiFi接続状態の確認を行い、必要に応じて再接続を試みる
 */
void loop() {

    barGraph.loop();
    wifiManager.loop();

    delay(100);
}
